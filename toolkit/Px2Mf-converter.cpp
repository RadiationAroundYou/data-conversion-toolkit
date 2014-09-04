/// @file Px2Mf-converter.cpp
/// @brief Code for the Px2Mf-converter executable: Pixelman -> MAFalda

// Standard include statements.
#include <stdlib.h>
#include <iostream>

// ROOT include statements.
#include "TString.h"

// toolkit include statements.
#include "ListHandler.h"
#include "WriteToNtuple.h"
#include "Frames.h"

using namespace std;

void checkParameters(int, char**, TString *);

/// @brief Px2Mf-converter: Converts Pixelman data to the MAFalda format.
///
/// @param[in] argc Input argument numbers.
/// @param[in] argv Input argument values.
int main(int argc, char ** argv) {

  cout
    <<                                     endl
    << "==============================" << endl
    << " CERN@school: Px2Mf-converter " << endl
    << "==============================" << endl;


  // Check the input arguments
  TString tempScratchDir("");
  checkParameters(argc, argv, &tempScratchDir);

  // Create the ntuple and the FramesHandler
  //-----------------------------------------

  // Get the dataset path from the input arguments.
  TString dataset = argv[2];

  // Instantiate the WriteToNtuple object.
  WriteToNtuple * MPXnTuple = new WriteToNtuple(dataset, tempScratchDir);

  // Instantiate the FramesHandler object.
  FramesHandler frames(dataset);

  // Determine if the user required any frames to be skipped.
  long int skipFrames = 0;
  if(argc == 5) skipFrames = atoi(argv[4]);

  // Process the list of data files
  //--------------------------------

  // Instantiate the ListHandler object.
  ListHandler handlerL(argv[1]);

  vector<string> listOfFiles;
  vector<string> listOfDSCFiles;
  vector<string> listOfIDXFiles;

  vector<string>::iterator itrF;

  handlerL.getListToLoop(tempScratchDir, listOfFiles, listOfDSCFiles, listOfIDXFiles);

  cout
    << "*"                                                     << endl
    << "* Data files:"                                         << endl
    << "*--> Number of DSC files:   " << listOfDSCFiles.size() << endl
    << "*--> Number of frame files: " << listOfFiles.size()    << endl
    << "*--> Number of IDX files:   " << listOfIDXFiles.size() << endl
    << "*"                                                     << endl;

  // Error handling
  //----------------
  // If there are no DSC files:
  if ( listOfDSCFiles.empty() ) {
    cout
      << "ERROR: * Unable to find the DSC files in the directory." << endl
      << "ERROR: * Exiting."                                       << endl;
    delete MPXnTuple;
    return 1;
  }

  // If there are no frame files:
  if ( listOfFiles.empty() ) {
    cout
      << "ERROR: * Unable to find some files in the specified " << endl
      << "ERROR: * directory, or the directory does not exist." << endl;
    delete MPXnTuple;
    return 1;
  }

  // Ensure that the frames list and DSC list match: // DEPRECATED?
  //if(listOfDSCFiles.size() != 0) {
  //  listOfDSCFiles = handlerL.OrderFilesPairMatch(listOfFiles, listOfDSCFiles);
  //}

  // If the number of frame files does not match the number of DSC files:
  if( listOfFiles.size() != listOfDSCFiles.size() ) {
    cout
      << "ERROR: * The number of DSC and frame files differs."   << endl
      << "ERROR: * You probably have an extra file that doesn't" << endl
      << "ERROR: * correspond to the data taking."               << endl
      << "ERROR: * Exiting."                                     << endl;
    exit(1);
  }

  // Process the dataset
  //---------------------

  Long_t filesItr = 0; // The number of the current file.

  std::string oneFileName    = "";
  std::string oneDSCFileName = "";
  std::string oneIDXFileName = "";

  int ftype;

  // Loop over the list of files.
  for (; filesItr<(Int_t)listOfFiles.size(); filesItr++) {

    // Skip over frames if the user has specified this.
    if (filesItr < skipFrames) continue;

    // Get the file names of the frame file and the DSC file.
    oneFileName    = listOfFiles[filesItr];
    oneDSCFileName = listOfDSCFiles[filesItr];
    
    // If there is an index (idx) file, get that too.
    if (listOfIDXFiles.size() > 0) {
      oneIDXFileName = listOfIDXFiles[filesItr];
    }

    //frames.getAFrameMatrix((TString) oneFileName, (TString) oneDSCFileName);

    // Read the frame(s) from the data files
    //---------------------------------------

    // Get the number of frames read from the current set of file names.
    int nread = frames.readOneFrame((TString) oneFileName, (TString) oneDSCFileName, &ftype);

    // Write whatever was read from the data in this set of file names.
    if(nread == 1) {                // Single frame.
      MPXnTuple->fillVars(&frames); // Also rewinds the metadata for the next frame.
    } else if (nread > 1) {         // Multiple frame (with idx files).
      frames.ProcessMultiframe(oneFileName, oneDSCFileName, oneIDXFileName, MPXnTuple, ftype);
      // Rewind the frame at the very end in this case,
      // as the metadata wasn't rewound before.
      frames.RewindAll();
    }//end of number of frames read check.

  }//end of loop over the list of files.

  // Close the ntuple and update the user on progress.
  MPXnTuple->closeNtuple();

  cout
    << "*"                                                               << endl
    << "* Conversion finished."                                          << endl
    << "* The output file is '" << MPXnTuple->GetNtupleFileName() << "'" << endl;


  // Erase any temporary files
  //---------------------------

  // The frame files list.
  std::string command0 = "/bin/bash -c 'if [ -e ";
  if(tempScratchDir.Length() > 0) { command0 += tempScratchDir.Data(); command0 += "/"; }
  command0 += "listOfFiles.txt ] ; then rm -f ";
  if(tempScratchDir.Length() > 0) { command0 += tempScratchDir.Data(); command0 += "/"; }
  command0 += "listOfFiles.txt ; fi'";

  // The DSC files list.
  std::string command02 = "/bin/bash -c 'if [ -e ";
  if(tempScratchDir.Length() > 0) { command02 += tempScratchDir.Data(); command02 += "/"; }
  command02 += "listOfFiles.dsc.txt ] ; then rm -f ";
  if(tempScratchDir.Length() > 0) { command02 += tempScratchDir.Data(); command02 += "/"; }
  command02 += "listOfFiles.dsc.txt ; fi'";

  // The index (idx) files list.
  std::string command03 = "/bin/bash -c 'if [ -e ";
  if(tempScratchDir.Length() > 0) { command03 += tempScratchDir.Data(); command03 += "/"; }
  command03 += "listOfFiles.idx.txt ] ; then rm -f ";
  if(tempScratchDir.Length() > 0) { command03 += tempScratchDir.Data(); command03 += "/"; }
  command03 += "listOfFiles.idx.txt ; fi'";

  // Execute the commands.
  system(command0.c_str());
  system(command02.c_str());
  system(command03.c_str());

  
  // That's it!
  return 0;
}//end of main function.


/// @brief Helper method for checking the validity of the input arguments.
///
/// @param [in] argc The number of input arguments from the command line.
/// @param [in] argv The input arguments from the commend line.
/// @param [in,out] tempScratchDir Reference to the output path string.
void checkParameters(int argc, char ** argv, TString * tempScratchDir){

  // If an insufficient number of arguments is supplied:
  if(argc < 3) {
    cout
      << "INFO: * Px2Mf-converter - usage:" << endl
      << "INFO: * "
      << argv[0] << " pathToData outputFileName "
      << "{tempScratchDir} {skip}"                                  << endl
      << "INFO:"                                                    << endl
      << "INFO: *--> [inputPath]  : The path to the folder "        << endl
      << "INFO:                     containing the data."           << endl
      << "INFO:"                                                    << endl
      << "INFO: *--> [runID]      : The run ID, which should be in" << endl
      << "INFO:                     the standard format:          " << endl
      << "INFO:                     WXX-YZZZZ_yyyy-mm-dd-HHMMSS   " << endl
      << "INFO:                     Do not use special characters " << endl
      << "INFO:                     or spaces."                     << endl
      << "INFO:"                                                    << endl
      << "INFO: *--> [outputPath] : The path to the output        " << endl
      << "INFO:                     directory."                     << endl
      << "INFO:"                                                    << endl;
//      << "INFO: *--> skip         : Number of frames to skip."      << endl
//      << "INFO:                     Optional."                      << endl;

    exit(1);
  }//end of insuffucient arguments check.

  // Assign the dataset path from the first input argument.
  TString dataSet = argv[2];

  // If the dataset path is blank or contains a space.
  if (dataSet == "" || dataSet.Contains(' '))
    {
      cout
        << "ERROR: * Bad format in the dataSet:"                     << endl
        << "ERROR: *--> dataSetNumber:"                              << endl
        << "ERROR: *----> Example: MediPix_SPS_TOTmode_25-06-2007"   << endl
        << "ERROR: * Please do not use special character or spaces." << endl;
      exit(1);
    }

  // If the user decided to use a different output directory.
  // (Optional argument.)
  if(argc > 2) {
    *tempScratchDir += argv[3];
    // If the supplied output path is blank, add the dot and slash.
    if (tempScratchDir->Length() == 0) {
      *tempScratchDir = "./";
    }
    cout 
      << "*"                                                << endl
      << "* The output path is '" << *tempScratchDir << "'" << endl
      << "*"                                                << endl;  
  }//end of output path input argument check.

}//end of checkParameters helper method.
