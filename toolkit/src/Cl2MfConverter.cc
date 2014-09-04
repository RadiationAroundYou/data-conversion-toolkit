/// @file Cl2MfConverter.cc
/// @brief Implementation of the cal->ROOT data converter class.

#include "Cl2MfConverter.h"

//
// Cl2MfConverter constructor
//
Cl2MfConverter::Cl2MfConverter(
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
  m_currentNtupleFileNum(0),
  m_currentFrameNumber(1),
  m_clfis(datasetpath)
{

  // Get the info from the XML file
  //--------------------------------

  // Instantiate the calibration data container.
  m_pCalibMetadata = new CalibMetadata(datasetmetadata);
  if (dbg) {
    cout
      << "DEBUG: Created CalibMetadata object with XML file '"
      << m_pCalibMetadata->GetXmlPath() << endl;
    //m_pCalibMetadata->PrintContents();
  }//end of dbg check.

  // Add the data set label to the ROOT file name.
  m_currentNtupleFileName
    //<< "MPXNtuple_"
    << m_pCalibMetadata->GetMPXDataSetNumber() << "_"
    << setw(10) << setfill('0') << m_currentNtupleFileNum
    << ".root";

  m_outputPath = m_outputDir + "/" + m_currentNtupleFileName.str();

  // Determine the validation ROOT file name.
  m_valFileName
    << "MPXVal_"
    << m_pCalibMetadata->GetMPXDataSetNumber()
    << ".root";

  m_valPath = m_outputDir + "/" + m_valFileName.str();

  if (dbg) {
    cout
      << "DEBUG: First file name is      '"
      << m_currentNtupleFileName.str() << "'"                      << endl
      << "DEBUG: Output path is          '" << m_outputDir  << "'" << endl
      << "DEBUG: ROOT file path is       '" << m_outputPath << "'" << endl
      << "DEBUG: Validation file path is '"
      << m_valFileName.str() << "'"                                << endl;
  }

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

  // Create the first ROOT ntuple file.
  m_pNt = new TFile(m_outputPath, "RECREATE");

  // Create the ROOT TTree.
  m_pTr = new TTree("MPXTree","Medi/TimePix data");

  // Create the frame container.
  m_pFrame = new FrameStruct(m_pCalibMetadata->GetMPXDataSetNumber());

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
      if (dbg) {
        cout
          << "DEBUG: Current frame number is " << m_currentFrameNumber << endl;
      }//end of dbg check.
    }//end of end of frames check.

    // Check if the frames/ROOT file limit has been reached.
    // If the limit has been reached, close the ROOT file
    // and create a new one.
    if ((m_currentFrameNumber % frames_per_root_file == 0) && more_frames) {

      if (dbg) {
        cout
          << "DEBUG:" << endl
          << "DEBUG: Reached the frames_per_root_file limit."               << endl
          << "DEBUG: Current ROOT file number = " << m_currentNtupleFileNum << endl
          << "DEBUG: m_currentFrameNumber = " << m_currentFrameNumber       << endl;
      }

      // Close the current old ntuple file.
      closeNtuple();

      // Increment the ROOT file number.
      m_currentNtupleFileNum++;

      // Add the data set label to the ROOT file name.
      m_currentNtupleFileName.str("");
      m_currentNtupleFileName
        //<< "MPXNtuple_"
        << m_pCalibMetadata->GetMPXDataSetNumber() << "_"
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
        << "DEBUG: Exiting the loop over the frames." << endl
        << "DEBUG: " << endl;
    }

    // Increment the current frame number.
    m_currentFrameNumber++;

  }//end of loop over the frames...

  // Close the last ntuple file.
  closeNtuple();

  // Write the validation histograms to file.
  m_pVf->Write();
  m_pVf->Close();

}//end of Cl2MfConverter constructor.

//
// Cl2MfConverter destructor
//
Cl2MfConverter::~Cl2MfConverter() {

  // Tidy up.

  // Delete the pointer to the calibration data wrapper.
  if (m_pCalibMetadata) delete m_pCalibMetadata;

  if (m_bf) delete m_bf;

  // Close the cluster log file input stream.
  m_clfis.close();

}//end of the Cl2MfConverter destructor.

//
// processNextFrame method
//
Bool_t Cl2MfConverter::processNextFrame(Bool_t dbg) {

  if (dbg) {
    cout
      << "DEBUG: Cl2MfConverter::processNextFrame method called." << endl;
  }


  // Loop over the lines in the data file.
  Long_t num_lines(0);
  Long_t pixels_in_frame(0);
  Long_t clusters_in_frame(0);
  Bool_t reached_eof = false;
  Bool_t new_frame = false;
  string line;


  while (m_clfis.good()) {

    // Increment the line counter.
    num_lines++;
    m_hg_nLines_ex->Fill(0.5);

    // Read the next line of the cluster log file.
    getline(this->m_clfis,line);

    // Retrieve frame number, acquisition time and time stamp subroutine
    
    // Find "Frame n (....., ...s)" by checking if the input data
    // line has a '(' in it.
    if (line.find ('(') != string::npos) {

      m_hg_nHeaders_ex->Fill(0.5);

      // Extract the frame information from the "header".
      int l_bracket = line.find('(');
      int r_bracket = line.find(')');
      int comma     = line.find(',');

      // Extract the frame number from the header.	  
      int f_num_from_file = atoi(line.substr(6,l_bracket-1).c_str());

      // Set the frame ID from the frame number.
      m_pFrame->SetId(f_num_from_file);

      // Extract the start time from the header.
      string timestamp    = line.substr(l_bracket+1,(comma-l_bracket-3));
      int    timestampdot = timestamp.find('.');
      string timestampdec = timestamp.substr(timestampdot,timestamp.length()-timestampdot);
      double starttime = atof(timestamp.c_str());
      Utils::TimeHandler t(starttime);
      
      // Set the frame start time (numeric and string)
      m_pFrame->SetStartTime(starttime);
      m_pFrame->SetStartTimeS(t.GetPixelmanTime());

      // Store the start time for the validation file.
      if (m_currentFrameNumber==1) m_phg_times->SetBinContent(1,starttime);

      // Extract and the acquisition time from the header.
      string acqtime = line.substr(comma+2,(r_bracket-comma)-4);

      // Set the acquisition time.
      m_pFrame->SetAcqTime(atof(acqtime.c_str()));

      // Store the end time for the validation file.
      m_phg_times->SetBinContent(2,starttime+atof(acqtime.c_str()));
	  
      if (dbg) {
        //cout << "DEBUG: l_bracket = " << l_bracket << endl;
        //cout << "DEBUG: r_bracket = " << r_bracket << endl;
        //cout << "DEBUG: comma     = " << comma     << endl;
        cout << "DEBUG: frame number from counter   = '" << m_currentFrameNumber << "'" << endl;
        cout << "DEBUG: frame number from file      = '" << f_num_from_file      << "'" << endl;
        cout << "DEBUG: start time from file        = '" << timestamp            << "'" << endl;
        cout << "DEBUG: start time from file (str.) = '" << t.GetPixelmanTime()  << "'" << endl;
        cout << "DEBUG: acquisition time from file  = '" << acqtime              << "'" << endl;
      }
	  
      if (m_currentFrameNumber != f_num_from_file) {
        cout << "ERROR: Frame number mismatch:" << endl;
        cout << "ERROR: frame_number = "    << m_currentFrameNumber << endl;
        cout << "ERROR: f_num_from_file = " << f_num_from_file      << endl;
        exit(0);
      }
    } else if (line.empty() || line=="\r") { // blank line - end of the frame.

      // Increment the number of blanks (extraction) histogram counter.
      m_hg_nBlanks_ex->Fill(0.5);

      // Increment the number of frames (extraction) histogram.
      m_hg_nframes_ex->Fill(0.5);

      if (dbg) {
        cout << "DEBUG: Found a blank line." << endl;
        cout << "DEBUG: Pixels in the frame   = " << pixels_in_frame   << endl;
        cout << "DEBUG: Clusters in the frame = " << clusters_in_frame << endl;
      }

      // Payload 
      //---------
      // [Pixel counts added in the cluster processing.]
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
      m_pFrame->SetnX(m_pCalibMetadata->GetFrameWidth());
      m_pFrame->SetnY(m_pCalibMetadata->GetFrameHeight());
      m_pFrame->SetPayloadFormat(m_pCalibMetadata->GetPayloadFormat());
      // [Frame ID set from the frame number in the file.]
      m_pFrame->SetDataSet(m_pCalibMetadata->GetMPXDataSetNumber());
      m_pFrame->UpdateOccupancy();
      m_pFrame->CalculateDoseRates();

      // Acquisition information
      //-------------------------
      m_pFrame->SetAcqMode(m_pCalibMetadata->GetAcqMode());
      m_pFrame->SetCounters(m_pCalibMetadata->GetCounters());
      m_pFrame->SetHwTimerMode(m_pCalibMetadata->GetHwTimerMode());
      m_pFrame->SetAutoEraseInterval(m_pCalibMetadata->GetAutoEraseInterval());
      m_pFrame->SetAutoEraseIntervalCounter(
        m_pCalibMetadata->GetAutoEraseIntervalCounter());
      m_pFrame->SetLastTriggerTime(m_pCalibMetadata->GetLastTriggerTime());
      m_pFrame->SetCoincidenceMode(m_pCalibMetadata->GetCoincidenceMode());
      m_pFrame->SetCoincidenceDelayTime(m_pCalibMetadata->GetCoincidenceDelayTime());
      m_pFrame->SetCoincidenceLiveTime(m_pCalibMetadata->GetCoincidenceLiveTime());
      m_pFrame->SetPixelmanVersion(m_pCalibMetadata->GetPixelmanVersion());

      // Geospatial information
      //------------------------
      m_pFrame->SetLatitude( m_pCalibMetadata->GetLatitude() );
      m_pFrame->SetLongitude(m_pCalibMetadata->GetLongitude());
      m_pFrame->SetAltitude( m_pCalibMetadata->GetAltitude() );

      // Detector settings
      //-------------------
      m_pFrame->SetPolarity(m_pCalibMetadata->GetPolarity());
      m_pFrame->SetHV(      m_pCalibMetadata->GetHV());
      Bool_t dacsset = m_pFrame->SetDACs(m_pCalibMetadata->GetDACs());
      m_pFrame->SetMpxClock(m_pCalibMetadata->GetMpxClock());
      m_pFrame->SetTpxClock(m_pCalibMetadata->GetTpxClock());
      m_pFrame->SetBsActive(m_pCalibMetadata->GetBsActive());

      // Detector information
      //----------------------
      m_pFrame->SetChipboardID(m_pCalibMetadata->GetChipboardID());
      m_pFrame->SetCustomName( m_pCalibMetadata->GetCustomName() );
      m_pFrame->SetFirmware(   m_pCalibMetadata->GetFirmware()   );
      m_pFrame->SetInterface(  m_pCalibMetadata->GetInterface()  );
      m_pFrame->SetMpxType(    m_pCalibMetadata->GetMpxType()    );
      m_pFrame->SetAppFilters( m_pCalibMetadata->GetAppFilters() );
      //
      m_pFrame->SetDet_x(  m_pCalibMetadata->GetDet_x()  );
      m_pFrame->SetDet_y(  m_pCalibMetadata->GetDet_y()  );
      m_pFrame->SetDet_z(  m_pCalibMetadata->GetDet_z()  );
      m_pFrame->SetOmega_x(m_pCalibMetadata->GetOmega_x());
      m_pFrame->SetOmega_y(m_pCalibMetadata->GetOmega_y());
      m_pFrame->SetOmega_z(m_pCalibMetadata->GetOmega_z());
      m_pFrame->SetRoll(   m_pCalibMetadata->GetRoll()   );
      m_pFrame->SetPitch(  m_pCalibMetadata->GetPitch()  );
      m_pFrame->SetYaw(    m_pCalibMetadata->GetYaw()    );

      // Source information
      //--------------------
      m_pFrame->SetSourceId(m_pCalibMetadata->GetSourceId());
      // [No primary vertex information required - real data.]

      // Fill the tree with the frame information.
      m_pNt->cd();
      m_pTr->Fill();

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

    } else { // Cluster entry

      // Increment the cluster counter.
      clusters_in_frame++;

      // Increment the cluster histogram counter.
      m_hg_nClusters_ex->Fill(0.5);

      // Process the cluster.
      pixels_in_frame += processCluster(line, dbg);

      //total_clusters++;

    }//end of frame header check. 
    
    reached_eof = !m_clfis.good();

  }//end of while loop over the data file.

  // The code shouldn't reach this point.
  return false;

}//end of Cl2MfConverter::processNextFrame method.

//
// processCluster method.
//
Int_t Cl2MfConverter::processCluster(string clusterline, Bool_t dbg) {

  // Count the pixels in the line.
  Int_t n_px = count(clusterline.begin(), clusterline.end(), '[');

  // Fill the pixels per cluster histogram.
  m_hg_ppc_ex->Fill(double(n_px));

  //if (dbg) {
  //  cout
  //    << "DEBUG: Cluster line = '" << clusterline << "'" << endl
  //    << "DEBUG: Number of pixels = " << n_px << endl;
  //}

  // Loop over the clusters.
  Int_t sb_l = clusterline.find_first_of('[');
  Int_t sb_r = clusterline.find_first_of(']');
  for (Int_t i=0; i<n_px; i++) {

    string cl = clusterline.substr(sb_l+1, sb_r-1);

    //cout << "DEBUG: " << cl << endl;

    Int_t c_1 = cl.find_first_of(',');
    Int_t c_2 = cl.find_last_of(',');
    Int_t cll = cl.length();

    string xstr(cl.substr(0    ,c_1)); Int_t x = atoi(xstr.c_str());
    string ystr(cl.substr(c_1+1,c_2)); Int_t y = atoi(ystr.c_str());
    string Cstr(cl.substr(c_2+1,cll)); Int_t C = atoi(Cstr.c_str());

    //cout << "DEBUG: (x,y,C) = ("<<x<<", "<<y<<", "<<C<<")"<<endl;
    
    // Write the pixel information to frame.
    this->m_pFrame->FillOneElement(x, y, m_pCalibMetadata->GetFrameWidth(), C);

    // Remove the extracted cluster from the string.
    //cout << "clusterline before extraction = " << clusterline << endl;
    clusterline.replace(sb_l, sb_r+2, "");
    //cout << "clusterline after extraction  = " << clusterline << endl;
    sb_l = clusterline.find_first_of('[');
    sb_r = clusterline.find_first_of(']');

  }//end of loop over the clusters.

  return n_px;

}//end of processCluster method.

//
// closeNtuple
//
void Cl2MfConverter::closeNtuple() {

  // Write the TTree contents.
  m_pTr->Write();

  // Close the ntuple file.
  m_pNt->Close();

}//end of closeNtuple method.
