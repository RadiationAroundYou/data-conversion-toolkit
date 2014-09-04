/// @file Cl2Mf-converter.cpp
/// @brief Code for the Cl2Mf-converter executable: Calibration -> MAFalda

// Standard includes.
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

// ROOT includes.
#include "TString.h"
#include "TFile.h"
#include "TH1D.h"

// toolkit includes.
#include "Cl2MfConverter.h"

using namespace std;

// Forward declaration of helper functions.
void checkParameters(int, char**, TString *);


/// @brief Cl2Mf-converter: Converts Calibration data to the MAFalda format.
///
/// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
/// @date January 2014
///
/// @param[in] argc Input argument numbers.
/// @param[in] argv Input argument values.
int main(int argc, char ** argv) {

  bool dbg = false; //true;

  // Check the input arguments.
  TString tempScratchDir = "";
  checkParameters(argc, argv, &tempScratchDir);

  cout << "" << endl
       << "==============================" << endl
       << " CERN@school: Cl2Mf-converter " << endl
       << "==============================" << endl
       << "*" << endl;


  cout << "* Cluster log file name:       '" << argv[1] << "'" << endl;
  cout << "* XML metadata file name:      '" << argv[2] << "'" << endl;
  cout << "* Output directory:            '" << argv[3] << "'" << endl;
  cout << "* Frames per ROOT file:         " << argv[4]        << endl;

  // Determine the number of frames per ROOT file to write.  
  Int_t num_frames_per_root_file = atoi(argv[4]);

  // Determine the number of frames to read.
  Int_t num_frames_to_read(0);
  if (argc > 5) num_frames_to_read = atoi(argv[5]);
  if (num_frames_to_read == -1 || argc <= 5) {
    cout << "* Reading all frames." << endl;
  }
  if (num_frames_to_read > 0) {
    cout << "* No. of frames to read:        " << num_frames_to_read << endl;
  }
  cout << endl;

  // Instantiate the converter object
  Cl2MfConverter converter(argv[1], argv[2], argv[3], num_frames_per_root_file, num_frames_to_read, dbg);

  return 0;

}

/// @brief Checks the validity of the input arguments.
///
/// @param[in] argc Input argument numbers.
/// @param[in] argv Input argument values.
/// @param[in] tempScratchDir Temporary scratch directory.
///
/// @todo Check the output directory exists and is valid.
void checkParameters(int argc, char ** argv, TString * tempScratchDir){

  if(argc < 5) {
    cout
      << endl
      << "ERROR: insufficient input arguments!" << endl
      << endl
      << "Usage: " << endl
      << endl
      << "./Cl2Mf-converter "
      << "[Cluster log file] "
      << "[XML config file] "
      << "[output directory] "
      << "[number of frames/ROOT file] "
      << "{no. of frames to read}" << endl
      << endl;
    exit(1);
  }

}//end of checkParameters helper function.
