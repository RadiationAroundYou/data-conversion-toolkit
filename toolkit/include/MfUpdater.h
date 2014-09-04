/// @file MfUpdater.h
/// @brief Header file for the Mafalda ROOT format metadata updater.

#ifndef MfUpdater_h
#define MfUpdater_h 1

// Standard include statements.
#include <sstream>
#include <fstream>
#include <algorithm>

// Getting http requests.
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

// Parsing JSON.
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"

// ROOT include statements.
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
//#include "TH1.h"

// Local include statements.
#include "AddMetadata.h"
#include "Frames.h"
#include "Utils.h"
//#include "BlobFinder.h"

using namespace std;

using namespace rapidjson;

//class FramesHandler;
//class FrameStruct;

/// @brief A class that manages updating metadata for the ROOT format.
///
/// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
/// @date May 2014
///
/// This class owes a debt to J. Idarraga's Mafalda code:
/// * [Mafalda](https://twiki.cern.ch/twiki/bin/view/Main/MAFalda).
class MfUpdater {

 public:

  /// @brief Constructor.
  ///
  /// @param [in] datasetpath The full path of the ROOT file.
  /// @param [in] datasetmetadata The full path of the meta-data XML file.
  /// @param [in] dbg Run in debug mode.
  explicit MfUpdater(
    TString datasetpath,
    TString datasetmetadata,
    Bool_t dbg = false);

  /// @brief Destructor.
  ~MfUpdater();

 protected:
  
  /// @brief Copy constructor.
  ///
  /// @param [in] cc MfUpdater to copy.
  MfUpdater(const MfUpdater& cc);

  /// @brief Copy assignment operator.
  ///
  /// @param [in] cc MfUpdater to copy.
  MfUpdater & operator=(const MfUpdater& cc);

 public:

  /// @brief Closes the current ntuple file.
  void closeNtuple();

  /// @brief The meta data for the calibration data.
  AddMetadata * m_pAddMetadata; 

  /// @brief Pointer to the current ntuple file.
  TFile * m_pNt;

  /// @brief Pointer to the TTree.
  TTree * m_pTr;

  TTree * m_pTrNew;

  /// @brief Pointer to the current frame container.
  FrameStruct * m_pFrame; 

  TBranch * m_pBr;

  /// @brief The current frame number.
  Long_t m_currentFrameNumber; 

}; //end of MfUpdater class definition.

#endif
