/// @file Mo2MfConverter.cc
/// @brief Implementation of the cal->ROOT data converter class.

#include "Mo2MfConverter.h"

//
// Mo2MfConverter constructor
//
Mo2MfConverter::Mo2MfConverter(
    TString datasetpath,
    TString datasetmetadata,
    TString outputdir,
    Int_t frames_per_root_file,
    Int_t num_frames_to_read,
    Bool_t dbg
)

 :
  m_outputDir(outputdir),
  //m_MPXDataSetNumber(dataSet),
  //m_ntupleFileBaseName(outputdir + "/"),
  m_currentNtupleFileNum(1),
  m_currentFrameNumber(1) //,
//  m_clfis(datasetpath)

{

  // Get the info from the XML file
  //--------------------------------

  // Instantiate the MoEDAL metadata container.
  m_pMoEDALMetadata = new MoEDALMetadata(datasetmetadata);
  if (dbg) {
    cout
      << "* DEBUG: Created MoEDALMetadata object with XML file '"
      << m_pMoEDALMetadata->GetXmlPath() << endl;
    m_pMoEDALMetadata->PrintContents();
  }//end of dbg check.

  // Add the data set label to the ROOT file name.
  m_currentNtupleFileName
    //<< "MPXNtuple_"
    << m_pMoEDALMetadata->GetMPXDataSetNumber() << "_"
    << setw(10) << setfill('0') << m_currentNtupleFileNum
    << ".root";

  m_outputPath = m_outputDir + "/" + m_currentNtupleFileName.str();

  // Open the source MoEDAL Timepix ROOT file.
  m_pMof = new TFile(datasetpath, "READ");

  // Get the TTree from the MoEDAL Timepix ROOT file.
  m_pMoTr = (TTree*)m_pMof->Get("dscData");

  // Set the frame start time TTree branch.
  m_pMoTr->SetBranchAddress("Start_time", &Start_time); 

  // Set the acquisition time from the TTree branch.
  m_pMoTr->SetBranchAddress("Acq_time", &Acq_time);

  // Set the pixel hits from the TTree branch.
  m_pMoTr->SetBranchAddress("hits", hits);

  // Get the total number of frames in the input ROOT file.
  m_totframes = m_pMoTr->GetEntriesFast();

  if (dbg) {
    cout
      << "* DEBUG: Number of frames found: " << m_totframes << endl;
    //m_pMoTr->Show();
  }


//  // Determine the validation ROOT file name.
//  m_valFileName
//    << "MPXVal_"
//    << m_pMoEDALMetadata->GetMPXDataSetNumber()
//    << ".root";
//
//  m_valPath = m_outputDir + "/" + m_valFileName.str();

  if (dbg) {
    cout
      << "* DEBUG: First file name is      '"
      << m_currentNtupleFileName.str() << "'"                      << endl
      << "* DEBUG: Output path is          '" << m_outputDir  << "'" << endl
      << "* DEBUG: ROOT file path is       '" << m_outputPath << "'" << endl;
//      << "DEBUG: Validation file path is '"
//      << m_valFileName.str() << "'"                                << endl;
  }

/*
  // Create the validation ROOT file.
  m_pVf = new TFile(m_valPath, "RECREATE");

  // Create the validation histograms.
  m_hg_nframes_ex   = new TH1D("nFrames_ex",   "",1,0.0,1.0);
  m_hg_nLines_ex    = new TH1D("nLines_ex",    "",1,0.0,1.0);
  m_hg_nHeaders_ex  = new TH1D("nHeaders_ex",  "",1,0.0,1.0);
  m_hg_nClusters_ex = new TH1D("nClusters_ex", "",1,0.0,1.0);
  m_hg_nBlanks_ex   = new TH1D("nBlanks_ex",   "",1,0.0,1.0);
  //
  m_hg_nPixelsPf_ex   = new TH1D("nPixelsPf_ex","",65536,0.0,65536.0);
  m_hg_nPixelsPf_fc   = new TH1D("nPixelsPf_fc","",65536,0.0,65536.0);
  //
  m_hg_nClustersPf_ex = new TH1D("nClustersPf_ex","",16384,0.,16384.0);
  m_hg_nClustersPf_bf = new TH1D("nClustersPf_bf","",16384,0.,16384.0);
  //
  m_hg_ppc_ex = new TH1D("ppc_ex","",11811,0.,11811.0);
  //
  m_phg_times = new TH1D("times","",2,0.0,2.0);

*/

  // Create the first ROOT ntuple file.
  m_pNt = new TFile(m_outputPath, "RECREATE");

  // Create the ROOT TTree.
  m_pTr = new TTree("MPXTree","Medi/TimePix data");

  // Create the frame container.
  m_pFrame = new FrameStruct(m_pMoEDALMetadata->GetMPXDataSetNumber());

  // Create the branch for the frame information.
  m_pTr->Branch("FramesData", "FrameStruct", &m_pFrame, 128000, 2);

  Bool_t more_frames = true;

  while (more_frames) {

    // Read in the frame information from the input stream
    // and write the frame information to the FrameStruct.
    if (!(this->processNextFrame(dbg))) break;
    
    // Check if the maximum number of frames has been reached.
    if (m_currentFrameNumber==num_frames_to_read) {
      more_frames = false;
      //if (dbg) {
      //  cout
      //    << "DEBUG: Current frame number is " << m_currentFrameNumber << endl;
      //}//end of dbg check.
    }//end of end of frames check.

    // Check if the frames/ROOT file limit has been reached.
    // If the limit has been reached, close the ROOT file
    // and create a new one.
    if ((m_currentFrameNumber % frames_per_root_file == 0) && (more_frames) ) {

      if (dbg) {
        cout
          << "* DEBUG:" << endl
          << "* DEBUG: Reached the frames_per_root_file limit."               << endl
          << "* DEBUG: Current ROOT file number = " << m_currentNtupleFileNum << endl
          << "* DEBUG: m_currentFrameNumber = " << m_currentFrameNumber       << endl
          << "* DEBUG:" << endl;
      }

      // Close the current old ntuple file.
      closeNtuple();

      // Increment the ROOT file number.
      m_currentNtupleFileNum++;

      // Add the data set label to the ROOT file name.
      m_currentNtupleFileName.str("");
      m_currentNtupleFileName
        //<< "MPXNtuple_"
        << m_pMoEDALMetadata->GetMPXDataSetNumber() << "_"
        << setw(10) << setfill('0') << m_currentNtupleFileNum
        << ".root";
      m_outputPath = m_outputDir + "/" + m_currentNtupleFileName.str();
      
      // Create a new ntuple file.
      m_pNt = new TFile(m_outputPath, "RECREATE");

      // Create the ROOT TTree.
      m_pTr = new TTree("MPXTree","Medi/TimePix data");

      // Create the branch for the frame information.
      m_pTr->Branch("FramesData", "FrameStruct", &m_pFrame, 128000, 2);

      // Update the user (debug mode).
      if (dbg) {
        cout
          << "DEBUG:" << endl
          << "DEBUG: Current ROOT file number = " << m_currentNtupleFileNum   << endl
          << "DEBUG: New ROOT file path = '" << m_currentNtupleFileName.str() << endl
          << "DEBUG:" << endl;
      }
      cout 
        << "INFO: ROOT file '" << m_currentNtupleFileName.str() 
        << "', frame "         << m_currentFrameNumber << endl;

    }//end of check of the frames/ROOT file limit.

    if (dbg && !more_frames) {
      cout
        << "* DEBUG: Exiting the loop over the frames." << endl
        << "* DEBUG: " << endl;
    }

    // Increment the current frame number.
    m_currentFrameNumber++;

    //if (m_currentFrameNumber%10==0) cout << m_currentFrameNumber << endl;

  }//end of loop over the frames...

  // Close the last ntuple file.
  closeNtuple();

//  // Write the validation histograms to file.
//  m_pVf->Write();
//  m_pVf->Close();

}//end of Mo2MfConverter constructor.

//
// Mo2MfConverter destructor
//
Mo2MfConverter::~Mo2MfConverter() {

  // Tidy up.

  // Delete the pointer to the calibration data wrapper.
  if (m_pMoEDALMetadata) delete m_pMoEDALMetadata;

//  if (m_bf) delete m_bf;

//  // Close the cluster log file input stream.
//  m_clfis.close();

  // Delete the pointer to the input ROOT file.
  if (m_pMof) delete m_pMof;

}//end of the Mo2MfConverter destructor.


//
// processNextFrame method
//
Bool_t Mo2MfConverter::processNextFrame(Bool_t dbg) {

  //if (dbg) {
  //  cout
  //    << "DEBUG: Mo2MfConverter::processNextFrame method called." << endl;
  //}

  if (m_currentFrameNumber > m_totframes) {

    return false;

  } else {

    // Process the frame.
 
    // Load in the current frame.
    m_pMoTr->GetEntry(m_currentFrameNumber-1);

    // Payload 
    //---------

    // Get the pixel information from the ROOT file.
    for (Int_t X = 0; X<65536; X++) {
      Int_t x = X%256;
      Int_t y = X/256;
      UShort_t C = hits[X];
      if (C > 0) { 
        //cout << "* DEBUG: Pixel X="<<X<<" ("<<x<<","<<y<<") = "<<C<<endl;
        // Write the pixel information to frame.
        m_pFrame->FillOneElement(x, y, m_pMoEDALMetadata->GetFrameWidth(), C);
      }
    }

    // [No Level 1 trigger.]
    // [No energy information.]
    // [No truth energy information.]

    // Add the whole-frame data.
    // [Hit pixel counter incremented in the cluster processing.]
    // [Hit counter incremented in the cluster processing.]
    // [Charge counter incremented in the cluster processing.]

    m_pFrame->SetFrameAsData();

    // Payload information
    //---------------------
    m_pFrame->SetnX(m_pMoEDALMetadata->GetFrameWidth());
    m_pFrame->SetnY(m_pMoEDALMetadata->GetFrameHeight());
    m_pFrame->SetPayloadFormat(m_pMoEDALMetadata->GetPayloadFormat());
    m_pFrame->SetId(m_currentFrameNumber);
    m_pFrame->SetDataSet(m_pMoEDALMetadata->GetMPXDataSetNumber());
    m_pFrame->UpdateOccupancy();
    m_pFrame->CalculateDoseRates();


    // Acquisition information
    //-------------------------
    m_pFrame->SetAcqMode(m_pMoEDALMetadata->GetAcqMode());
    m_pFrame->SetCounters(m_pMoEDALMetadata->GetCounters());
    m_pFrame->SetHwTimerMode(m_pMoEDALMetadata->GetHwTimerMode());
    m_pFrame->SetAutoEraseInterval(m_pMoEDALMetadata->GetAutoEraseInterval());
    m_pFrame->SetAutoEraseIntervalCounter(
    m_pMoEDALMetadata->GetAutoEraseIntervalCounter());
    m_pFrame->SetLastTriggerTime(m_pMoEDALMetadata->GetLastTriggerTime());
    m_pFrame->SetCoincidenceMode(m_pMoEDALMetadata->GetCoincidenceMode());
    m_pFrame->SetCoincidenceDelayTime(m_pMoEDALMetadata->GetCoincidenceDelayTime());
    m_pFrame->SetCoincidenceLiveTime(m_pMoEDALMetadata->GetCoincidenceLiveTime());
    m_pFrame->SetPixelmanVersion(m_pMoEDALMetadata->GetPixelmanVersion());

    // Geospatial information
    //------------------------
    m_pFrame->SetLatitude( m_pMoEDALMetadata->GetLatitude() );
    m_pFrame->SetLongitude(m_pMoEDALMetadata->GetLongitude());
    m_pFrame->SetAltitude( m_pMoEDALMetadata->GetAltitude() );
    //
    m_pFrame->SetOmega_x(m_pMoEDALMetadata->GetOmega_x());
    m_pFrame->SetOmega_y(m_pMoEDALMetadata->GetOmega_y());
    m_pFrame->SetOmega_z(m_pMoEDALMetadata->GetOmega_z());
    //
    m_pFrame->SetRoll(   m_pMoEDALMetadata->GetRoll()   );
    m_pFrame->SetPitch(  m_pMoEDALMetadata->GetPitch()  );
    m_pFrame->SetYaw(    m_pMoEDALMetadata->GetYaw()    );

    // Temporal information
    //----------------------

    // Set the frame start time.
    m_pFrame->SetStartTime(Start_time);

    // Create a TimeHandler object using the start time from the ROOT file.
    Utils::TimeHandler t(Start_time);

    // Set the start time string using the TimeHandler class.
    m_pFrame->SetStartTimeS(t.GetPixelmanTime());

    // Set the acquisition time.
    m_pFrame->SetAcqTime(Acq_time); 


    // Detector settings
    //-------------------
    m_pFrame->SetPolarity(m_pMoEDALMetadata->GetPolarity());
    m_pFrame->SetHV(      m_pMoEDALMetadata->GetHV());
    //
    Bool_t dacsset = m_pFrame->SetDACs(m_pMoEDALMetadata->GetDACs());
    //
    m_pFrame->SetMpxClock(m_pMoEDALMetadata->GetMpxClock());
    m_pFrame->SetTpxClock(m_pMoEDALMetadata->GetTpxClock());
    m_pFrame->SetBsActive(m_pMoEDALMetadata->GetBsActive());


    // Detector information
    //----------------------
    m_pFrame->SetChipboardID(m_pMoEDALMetadata->GetChipboardID());
    m_pFrame->SetCustomName( m_pMoEDALMetadata->GetCustomName() );
    m_pFrame->SetFirmware(   m_pMoEDALMetadata->GetFirmware()   );
    m_pFrame->SetInterface(  m_pMoEDALMetadata->GetInterface()  );
    m_pFrame->SetMpxType(    m_pMoEDALMetadata->GetMpxType()    );
    m_pFrame->SetAppFilters( m_pMoEDALMetadata->GetAppFilters() );
    //
    m_pFrame->SetDet_x(m_pMoEDALMetadata->GetDet_x());
    m_pFrame->SetDet_y(m_pMoEDALMetadata->GetDet_y());
    m_pFrame->SetDet_z(m_pMoEDALMetadata->GetDet_z());
    //
    m_pFrame->SetEulerA(m_pMoEDALMetadata->GetEulerA());
    m_pFrame->SetEulerB(m_pMoEDALMetadata->GetEulerB());
    m_pFrame->SetEulerC(m_pMoEDALMetadata->GetEulerC());


    // Source information
    //--------------------
    m_pFrame->SetSourceId(m_pMoEDALMetadata->GetSourceId());
    // [No primary vertex information required - real data.]

    // Fill the tree with the frame information.
    m_pNt->cd();
    m_pTr->Fill();

    // Flush out the frame information.
    m_pFrame->ResetCountersPad();
    m_pFrame->CleanUpMatrix();

    return true;

  }//end of check on the current frame number.


/*

      // Run the cluster validation
      //----------------------------
      m_bf = new BlobFinder(m_pFrame->GetPixelCounts(),
                            m_pFrame->GetFrameWidth(),
                            m_pFrame->GetFrameHeight());

      // Plot the number of pixels per frame (extracted).
      m_hg_nPixelsPf_ex->Fill(pixels_in_frame);

      // Plot the number of pixels per frame (frame container).
      m_hg_nPixelsPf_fc->Fill(m_pFrame->GetPixelCounts().size() + 0.5);

      // Plot the number of clusters per frame.
      m_hg_nClustersPf_bf->Fill(m_bf->getSize() + 0.5);
      //cout << "DEBUG: * Blobs in the BlobFinder = " << m_bf->getSize() << endl;

      //total_blanks++;
      //if (clusters_in_frame>0) hg_nc->Fill(double(clusters_in_frame));
      m_hg_nClustersPf_ex->Fill(clusters_in_frame);

      clusters_in_frame = 0;

      // Flush out the frame information.
      m_pFrame->ResetCountersPad();
      m_pFrame->CleanUpMatrix();

      // Return "true" if a blank line, but not the end of the file
      return m_clfis.good();
*/

}//end of Mo2MfConverter::processNextFrame method.


//
// closeNtuple
//
void Mo2MfConverter::closeNtuple() {

  // Write the TTree contents.
  m_pTr->Write();

  // Close the ntuple file.
  m_pNt->Close();

}//end of closeNtuple method.
