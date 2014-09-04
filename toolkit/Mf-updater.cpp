/// @file Mf-updater.cpp
/// @brief Code for the Mf-updater executable: MAFalda metadata updater.

// Standard includes.
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

// ROOT includes.
#include "TString.h"

// Toolkit includes.
#include "MfUpdater.h"

using namespace std;

// Forward declaration of helper functions.
void checkParameters(int, char**, TString *);


/// @brief Mf-updaterer: Updates metadata (MAFalda format).
///
/// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
/// @date May 2014
///
/// @param[in] argc Input argument numbers.
/// @param[in] argv Input argument values.
int main(int argc, char ** argv) {

  bool dbg = true;

  // Check the input arguments.
  TString tempScratchDir = "";
  checkParameters(argc, argv, &tempScratchDir);

  cout << "" << endl
       << "=========================" << endl
       << " CERN@school: Mf-updater " << endl
       << "=========================" << endl
       << "*" << endl;


  cout << "* ROOT file name:      :       '" << argv[1] << "'" << endl;
  cout << "* XML metadata file name:      '" << argv[2] << "'" << endl;

  // Instantiate the converter object
  MfUpdater updater(argv[1], argv[2], dbg);

  updater.closeNtuple();

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

  if(argc < 3) {
    cout
      << endl
      << "ERROR: insufficient input arguments!" << endl
      << endl
      << "Usage: " << endl
      << endl
      << "./Mf-updater "
      << "[inputPath] "
      << "[metadataXmlPath] "
      << endl;
    exit(1);
  }

}//end of checkParameters helper function.
