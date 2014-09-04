/// @file MfFilter.h
/// @brief Header file for the Mafalda ROOT format pixel filter.

#ifndef MfFilter_h
#define MfFilter_h 1

// Standard include statements.
#include <sstream>
#include <fstream>
#include <algorithm>

// ROOT include statements.
#include "TString.h"
#include "TFile.h"
#include "TTree.h"

// Local include statements.
#include "Frames.h"
#include "Utils.h"

using namespace std;

/// @brief A class that filters pixels for the Mafalda format data.
///
/// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
/// @date May 2014
///
/// This class owes a debt to J. Idarraga's Mafalda code:
/// * [Mafalda](https://twiki.cern.ch/twiki/bin/view/Main/MAFalda).
class MfFilter {

 public:

  /// @brief Constructor.
  ///
  /// @param [in] datasetpath The full path of the ROOT file.
  /// @param [in] filterpath The full path of the pixel filter file.
  /// @param [in] dbg Run in debug mode.
  explicit MfFilter(
    TString datasetpath,
    TString filterpath,
    TString filtername,
    Bool_t dbg = false);

  /// @brief Destructor.
  ~MfFilter();

 protected:
  
  /// @brief Copy constructor.
  ///
  /// @param [in] cc MfFilter to copy.
  MfFilter(const MfFilter& cc);

  /// @brief Copy assignment operator.
  ///
  /// @param [in] cc MfFilter to copy.
  MfFilter & operator=(const MfFilter& cc);

 public:

  /// @brief Closes the current ntuple file.
  void closeNtuple();

  /// @brief Pointer to the current ntuple file.
  TFile * m_pNt;

  /// @brief Pointer to the TTree.
  TTree * m_pTr;

  /// @brief Pointer to the new TTree.
  TTree * m_pTrNew;

  /// @brief Pointer to the current frame container.
  FrameStruct * m_pFrame; 

  /// @brief Pointer to the TTree frame branch.
  TBranch * m_pBr;

  /// @brief The current frame number.
  Long_t m_currentFrameNumber; 

}; //end of MfFilter class definition.

#endif
