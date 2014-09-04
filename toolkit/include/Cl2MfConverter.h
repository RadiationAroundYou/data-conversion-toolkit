/// @file Cl2MfConverter.h
/// @brief Header file for the Cl2MfConverter class.

#ifndef Cl2MfConverter_h
#define Cl2MfConverter_h 1

// Standard include statements.
#include <sstream>
#include <fstream>
#include <algorithm>

// ROOT include statements.
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

// Local include statements.
#include "CalibMetadata.h"
#include "Frames.h"
#include "Utils.h"
#include "BlobFinder.h"

using namespace std;

//class FramesHandler;
//class FrameStruct;

/// @brief A class that manages converting calibration data to ROOT format.
///
/// @author T. Whyntie (CERN\@school - t.whyntie@qmul.ac.uk)
/// @date February 2014
///
/// This class owes a debt to J. Idarraga's Mafalda code:
/// * [Mafalda](https://twiki.cern.ch/twiki/bin/view/Main/MAFalda).
class Cl2MfConverter {

 public:

  /// @brief Constructor.
  ///
  /// @param [in] datasetpath The full path of the cluster log file.
  /// @param [in] datasetmetadata The full path of the meta-data XML file.
  /// @param [in] outputdir The full path of the output directory.
  /// @param [in] frames_per_root_file The number of frames per ROOT file.
  /// @param [in] num_frames_to_read The number of frames to read.
  /// @param [in] dbg Run in debug mode.
  explicit Cl2MfConverter(
    TString datasetpath,
    TString datasetmetadata,
    TString outputdir,
    Int_t   frames_per_root_file,
    Int_t   num_frames_to_read,
    Bool_t dbg = false);

  /// @brief Destructor.
  ~Cl2MfConverter();

 protected:
  
  /// @brief Copy constructor.
  ///
  /// @param [in] cc Cl2MfConverter to copy.
  Cl2MfConverter(const Cl2MfConverter& cc);

  /// @brief Copy assignment operator.
  ///
  /// @param [in] cc Cl2MfConverter to copy.
  Cl2MfConverter & operator=(const Cl2MfConverter& cc);

 public:

  /// @brief Get the ntuple file name (including path).
  ///
  /// @return The path to the current ntuple.
  TString GetCurrentNtupleFileName() { return m_currentNtupleFileName.str(); };

  //void fillVars(FramesHandler *, bool rmd = true);

  /// @brief Closes the current ntuple file.
  void closeNtuple();


 private:

  // Private methods

  /// @brief Process the next frame from the cluster log file.
  Bool_t processNextFrame(Bool_t dbg = false);

  /// @brief Process a cluster from a line of the cluster log file.
  ///
  /// @param [in] clusterline Line from the cluster log file.
  /// @param [in] dbg Debug mode?
  Int_t processCluster(string clusterline, Bool_t dbg = false);

  // Private members

  // Data set information
  //----------------------

  /// @brief The meta data for the calibration data.
  CalibMetadata * m_pCalibMetadata; 

  /// @brief The identifying label for the data set.
  TString m_MPXDataSetNumber;  
 
  /// @brief The input stream to the cluster log file.
  ifstream m_clfis; 

  // File information
  //------------------

  /// @brief The output directory.
  TString m_outputDir;

  /// @brief The current ntuple file number.
  Int_t m_currentNtupleFileNum;

  /// @brief The current ntuple file name.
  stringstream m_currentNtupleFileName; 

  /// @brief The current output path.
  TString m_outputPath;

  /// @brief Pointer to the current ntuple file.
  TFile * m_pNt;

  /// @brief Pointer to the TTree.
  TTree * m_pTr;

  /// @brief Pointer to the current frame container.
  FrameStruct * m_pFrame; 

  /// @brief The current frame number.
  Long_t m_currentFrameNumber; 

  // Clustering
  //------------
  
  ///  @brief A pointer to the BlobFinder (for cluster validation).
  BlobFinder * m_bf;

  // Validation
  //------------

  /// @brief A pointer to the validation ROOT file.
  TFile * m_pVf;

  /// @brief The validation ROOT file name.
  stringstream m_valFileName;

  /// @brief The path of the validation ROOT file.
  TString m_valPath;

  /// @brief Histogram counter for the number of frames extracted.
  TH1D * m_hg_nframes_ex;

  /// @brief Histogram counter for the number of lines in the cluster file.
  TH1D * m_hg_nLines_ex;

  /// @brief Histogram counter for the number of frame headers extracted.
  TH1D * m_hg_nHeaders_ex;

  /// @brief Histogram counter for the number of clusters extracted.
  TH1D * m_hg_nClusters_ex;

  /// @brief Histogram counter for the number of blank lines extracted.
  TH1D * m_hg_nBlanks_ex;

  /// @brief Histogram for the number of clusters per frame (extracted).
  TH1D * m_hg_nClustersPf_ex;

  /// @brief Histogram for the number of clusters per frame (BlobFinder).
  TH1D * m_hg_nClustersPf_bf;

  /// @brief Histogram for the number of pixels per frame (extracted).
  TH1D * m_hg_nPixelsPf_ex;

  /// @brief Histogram for the number of pixels per frame (frame container).
  TH1D * m_hg_nPixelsPf_fc;

  /// @brief Histogram for the pixels per cluster.
  TH1D * m_hg_ppc_ex;

  /// @brief Pseudo-histogram for the start and end times.
  TH1D * m_phg_times;

}; //end of Cl2MfConverter class definition.

#endif
