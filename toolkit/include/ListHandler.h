/// @file ListHandler.h
/// @brief Header file for the ListHandler class.

// Standard include statements.
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "algorithm"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

// ROOT include statements.
#include <TROOT.h>
#include <TString.h>

using namespace std;

#define FRAME_FILES 10 //!< Payload files.
#define DSC_FILES   20 //!< Detector settings (.dsc) files.
#define IDX_FILES   30 //!< Index files (for multi-frame payload files).


/// @brief Handles the lists of files in a dataset for a given directory.
///
/// @author J. Idarraga (principal author - idarraga@cern.ch)
/// @author T. Whyntie (editor for CERN\@school - t.whyntie@qmul.ac.uk)
/// @date 2006 (ed. 2013)
///
/// This class Handles the list of files in a given directory
/// that corresponds to frame files to be read.  It calculates
/// the number the number of files and returns a vector<string>
/// with the filenames.
class ListHandler {

 public:

  /// @brief The constructor.
  ///
  /// @param [in] datasetpath The path of the dataset directory.
  ListHandler(Char_t * datasetpath);

  /// @brief The destructor.
  ~ListHandler() {}; 

  /// @brief Get the lists of payload DSC, and index files.
  ///
  /// @param [in] tempScratchDir The dataset directory path.
  /// @param [in] files Reference to the vector of payload file names.
  /// @param [in] dscfiles Reference to the vector of DSC file names.
  /// @param [in] idxfiles Reference to the vector of index file names.
  /// @return The number of files in each list (vector).
  int getListToLoop (TString tempScratchDir,
                     vector<string> & ,
                     vector<string> & ,
                     vector<string> & );

  /// @brief Sorts the supplied list (passed by reference).
  ///
  /// @param [in] f Reference to the vector of file names to be sorted.
  void list_order(vector<string> & f);

  /// @brief Get the list of files of a given type.
  ///
  /// @param [in] tempScratchDir The dataset directory path.
  /// @param [in] typeF The type of file to get the list for.
  /// @return The list of files of the specified type.
  vector<string> getListToLoop(TString tempScratchDir,Int_t typeF=FRAME_FILES);

  /// @brief Orders the list of payload and DSC files so that they match.
  ///
  /// @param [in] listFrames Vector of payload file names.
  /// @param [in] listDSC Vector of DSC file names.
  /// @return The ordered list of DSC files.
  vector<string> OrderFilesPairMatch(
    vector<string> listFrames,
    vector<string> listDSC
  );

 private:
  /// @brief The number of files in the dataset directory.
  Long_t m_nFiles;  

  /// @brief The path of the dataset directory.
  string m_dirPath; 

};//end of ListHandler class definition.
