/// @file WriteToNtuple.h
/// @brief Header file for the ntuple writing classes.

#ifndef MediPixWriteToEntuple_h
#define MediPixWriteToEntuple_h 1

// Standard include statements.
#include <vector>
#include <iostream>

// ROOT include statements.
#include <TROOT.h>
#include <TChain.h>
#include <TString.h>
#include <TFile.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TMath.h>

// Local include statements.
#include "Frames.h"
#include "FramesConsts.h"

// Forward declarations.
class FramesHandler;
class FrameStruct;

/// @brief A class for handling the ntuple writing.
///
/// @author J. Idárraga (principle author - idarraga@cern.ch)
/// @author T. Whyntie (editor, CERN\@school - t.whyntie@qmul.ac.uk)
/// @date August 2006 (ed. January 2014)
class WriteToNtuple {

 public:

  /// @brief Constructor.
  WriteToNtuple(TString, TString);

  /// @brief Desctructor.
  ~WriteToNtuple();

  /// @brief Populate the frame container with the frame data.
  ///
  /// @param [in] frameHandlerObj Pointer to the frame data's container.
  /// @param [in] rewind_metadata Reset the frame data and metadata?
  void fillVars(FramesHandler * frameHandlerObj, bool rewind_metadata = true);

  /// @brief Closes the ntuple.
  void closeNtuple();

  /// @brief Returns the ntuple file name.
  TString GetNtupleFileName() { return m_ntupleFileName; };

private:
  
  /// @brief Pointer to the frame container.
  FrameStruct * m_frame;

  /// @brief Histogram - what is this for?
  TH2 * h1;

  /// @brief Pointer to the ntuple file.
  TFile * nt;

  /// @brief Pointer to the TTree containing the frame data.
  TTree * t2;

  /// @brief The dataset ID.
  TString m_MPXDataSetNumber;

  /// @brief The ntuple file name.
  TString m_ntupleFileName;

  //ClassDef(WriteToNtuple,1)

};

#endif
