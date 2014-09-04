/// @file Frames.cc
/// @brief Implementation of the frame container classes.

#include "Frames.h"

using namespace std;

// ROOT dictionary macros.

/// @brief Class implementation flag for the ROOT dictionary.
ClassImp(FrameContainer)

/// @brief Class implementation flag for the ROOT dictionary.
ClassImp(FrameStruct)

//
// FrameContainer Constructor
//
FrameContainer::FrameContainer()
:
  m_nEntriesPad(0),
  m_nHitsInPad(0),
  m_nChargeInPad(0),
  m_isMCData(false)
{}

//
// FrameContainer::FillOneElement
//
void FrameContainer::FillOneElement(
  Int_t x,
  Int_t y,
  Int_t w,
  Int_t C)
{

  // Calculate X, the combined (x,y) coordinate of the pixel.
  // X,Y,C --> X,C : yi*width + xi
  Int_t X = y*w + x;

  // Count the number of hit pixels.
  // (Unless counted before - but this shouldn't happen...?)
  if (m_frameXC.find(X) == m_frameXC.end()) {
    m_nEntriesPad++;
  } else {
    //cout << "DEBUG: MULTIPLE HIT PIXEL??? "<<x<<", "<<y<<", "<<C<<endl;
  }//end of "hit before" check.

  // Store the ToT count by the X coordinate.
  m_frameXC[X] += C; 

  // Count the number of hits.
  m_nHitsInPad++;

  // Increase the total ToT counts counter.
  m_nChargeInPad += C;

}//end of the FillOneElement method (x, y, width, X).

//
// FillOneElement (X and C supplied)
//
void FrameContainer::FillOneElement(Int_t X, Int_t C) {

  // X has already been supplied, so no further calculation is needed.
  m_frameXC[X] += C;

  // Count the number of hit pixels.
  // (Unless counted before - but this shouldn't happen...?)
  if (m_frameXC.find(X) == m_frameXC.end()) {
    m_nEntriesPad++;
  } else {
    //cout << "DEBUG: MULTIPLE HIT PIXEL??? "<<x<<", "<<y<<", "<<C<<endl;
  }//end of "hit before" check.

  // Store the ToT count by the X coordinate.
  m_frameXC[X] += C; 

  // Count the number of hits.
  m_nHitsInPad++;

  // Increase the total ToT counts counter.
  m_nChargeInPad += C;

}//end of FillOneElement method (X and C supplied).

//
// FrameContainer::FillOneElement (including energy values)
//
void FrameContainer::FillOneElement(
  Int_t xi,
  Int_t yi,
  Int_t width,
  Int_t counts,
  Double_t truthE,
  Double_t E)
{

	// Fill pixel withouth MC info first
	FillOneElement(xi, yi, width, counts);

	// X,Y,C --> X,C : yi*width + xi
	Int_t X = yi*width + xi;

	m_frameXC_TruthE[X] += truthE; // Truth energy
	m_frameXC_E[X] += E;           // Energy with detector effects

}

//
// FrameContainer::SetLVL1
//
void FrameContainer::SetLVL1(
  Int_t x,
  Int_t y,
  Int_t w,
  Int_t lvl1)
{

  // Calculate X, the combined (x,y) coordinate of the pixel.
  // X,Y,C --> X,C : yi*width + xi
  Int_t X = y*w + x;

  // Add the level 1 trigger to the pixel map.
  m_lvl1[X] += lvl1;

}//end of SetLVL1 method.

//
// FrameContainer::CleanUpMatrix
//
void FrameContainer::CleanUpMatrix() {

  // Clear the pixel maps.
  m_frameXC.clear();
  m_lvl1.clear();
  m_frameXC_TruthE.clear();
  m_frameXC_E.clear();

}//end of CleanUpMatrix method.

//
// FrameContainer::ResetCountersPad
//
void FrameContainer::ResetCountersPad() {

  // Reset the counters to zero.
  m_nEntriesPad  = 0;
  m_nHitsInPad   = 0;
  m_nChargeInPad = 0;

  // Reset the simulated data flag to false.
  m_isMCData = false;

}//end of ResetCountersPad method.

//
// FrameStruct constructor.
//
FrameStruct::FrameStruct(TString datasetid) 
:
  FrameContainer()
{

  // Clear the pixel maps.
  this->CleanUpMatrix();

  // Reset the pixel counters.
  this->ResetCountersPad();

  // Reset the metadata.
  this->RewindMetaDataValues();

  // Metadata not reset by RewindMetaDataValues().
  fFrameId = -1;
  fMPXDataSetNumber = datasetid;

}//end of FrameStruct constructor.

//
// FrameStruct::RewindMetaDataValues 
//
void FrameStruct::RewindMetaDataValues() {

  // Payload information
  //---------------------
  fWidth   =  0;
  fHeight  =  0;
  fFormat  =  0;
  fFrameId = -1;
  fMPXDataSetNumber = "";
  //
  // Pixel-dependent payload information.
  m_occupancy = 0;
  m_occupancyPc = 0.0;
  m_DoseRate           = -1.0;
  m_DoseEquivalentRate = -1.0;

  // Acquisition information
  //-------------------------
  fAcq_mode = 0;
  fCounters.clear();
  fHw_timer = 0;
  fAuto_erase_interval = 0.0;
  fAuto_erase_interval_counter = 0;
  fTrigger_time = 0.0;
  fCoincidence_mode = 0;
  fCoincidence_delay = 0.0;
  fCoinc_live_time = 0.0;
  fPixelman_version = "";

  // Geospatial information
  //------------------------
  fLatitude  = 0.0;
  fLongitude = 0.0;
  fAltitude  = 0.0;

  // Temporal information
  //----------------------
  fAcq_time = 0.0;
  fStart_time = 0.0;
  fStart_timeS = "";

  // Detector settings
  //-------------------
  fPolarity      = -1;
  fHV            =  0.0;
  fDACs.clear();
  fMpx_clock     =  0.0;
  fTimepix_clock =  0.0;
  fBS_active = false;

  // Detector information
  //----------------------
  fChipboardID     = "";
  fCustom_name     = "";
  fFirmware        = "";
  fInterface       = "";
  fMpx_type = -1;
  fApplied_filters = "";
  //
  m_x         = 0.0;
  m_y         = 0.0;
  m_z         = 0.0;
  m_omega_x   = 0.0;
  m_omega_y   = 0.0;
  m_omega_z   = 0.0;
  m_roll_orf  = 0.0;
  m_pitch_orf = 0.0;
  m_yaw_orf   = 0.0;

  // Source information
  //--------------------
  m_primaryVertex_x.clear();
  m_primaryVertex_y.clear();
  m_primaryVertex_z.clear();

}//end of FrameStruct::RewindMetaDataValues method.

//
// FrameStruct::SetDACs
//
Bool_t FrameStruct::SetDACs(vector<Int_t> dacs) {

  if (dacs.size()==14) {
    fDACs = dacs;
    return true;
  } else {
    cout << "ERROR: Wrong number of DAC values supplied!" << endl;
    return false;
  }//end of DACs size check.

}//end of SetDACs method.

//
// FrameStruct::SetPrimaryVertex
//
void FrameStruct::SetPrimaryVertex(Double_t vx, Double_t vy, Double_t vz){

  // Add the x, y, and z of the primary vertex to the frame information.
  m_primaryVertex_x.push_back(vx);
  m_primaryVertex_y.push_back(vy);
  m_primaryVertex_z.push_back(vz);

}//end of SetPrimaryVertex method.

//
// FrameStruct::UpdateOccupancy
//
void FrameStruct::UpdateOccupancy() {

  // Set the value of the occupancy from the size of the pixel map.
  m_occupancy = this->GetEntriesPad();

  // Calculate the percentage occupancy.
  m_occupancyPc = (100. * Double_t(m_occupancy)) / 
                  (Double_t(fWidth) * Double_t(fHeight));
 
}//end of UpdateOccupancy method.

void FrameStruct::CalculateDoseRates() {

  if (GetPixelEnergies().size()==0) {
    // The energies have not been calculated/provided.
    m_DoseRate           = -1.0;
    m_DoseEquivalentRate = -1.0;
  } else {
    // Calculate the dose rates from the pixel counts and calibration
    // information.
    // The code for this should be in REMAnalysis.
  }//end of the pixel energies check.

}//end of CalculateDoseRates method.

//
// FrameStruct::GetOccupancy
//
Int_t FrameStruct::GetOccupancy() {

  this->UpdateOccupancy();

  // Return the value.
  return m_occupancy;
}

//
// FrameStruct::GetOccupancyPc
//
Double_t FrameStruct::GetOccupancyPc() {

  this->UpdateOccupancy();

  // Return the value.
  return m_occupancyPc;
}

//
// FrameStruct::FillMetaData
//
void FrameStruct::FillMetaData(TString METAString, Int_t metaCode) {

  TString tempEntry = "";

  // Temporary values to be used.
  int temp_i;
  double temp_d;

  switch (metaCode) {
  case ACQ_MODE_CODE:
    fAcq_mode = METAString.Atoi();
    break;
  case ACQ_TIME_CODE:
    fAcq_time = METAString.Atof();
    break;
  case CHIPBOARDID_CODE:
    fChipboardID = METAString;
    break;
  case DACS_CODE:
    for(Int_t i = 0 ; i < METAString.Length() ; i++) {
      if (METAString[i] != ' ') {
        tempEntry += METAString[i];
      }
      else if (METAString[i] == ' ' && tempEntry.Length() > 0) {
        //if(fDACsSize < MAX_DAQ_INT){
          //cout << tempEntry << std::endl;
          //fDACs[fDACsSize] = tempEntry.Atoi();
          //fDACsSize++;
        fDACs.push_back((Int_t)tempEntry.Atoi());
        //}
        tempEntry = "";
      }
    }//end of loop over the DAC metadata string.
    tempEntry = "";
    break;
  case HW_TIMER_CODE:
    fHw_timer = METAString.Atoi();
    break;
  case INTERFACE_CODE:
    fInterface = METAString;
    break;
  case POLARITY_CODE:
    fPolarity = METAString.Atoi();
    break;
  case START_TIME_CODE:
    fStart_time = METAString.Atof();
    break;
  case START_TIME_S_CODE:
    fStart_timeS = METAString;
    break;
  case MPX_CLOCK_CODE:
    fMpx_clock = METAString.Atof();
    break;
  case HV_CODE:
    fHV = METAString.Atof();
    break;
  case TIMEPIX_CLOCK_CODE:
    // Note - TW 2014-05-12
    // The Pixelman software stores the Timepix clock as a byte, apparently.
    // The values of which are 0-3, corresponding to
    // 10MHz, 20MHz, 40MHz, 80MHz respectively.
    // Note - TW 2014-05-15
    // Ah - Pixelman 2.2.2 stores it in MHz anyway. Stick to the original plan!
    temp_i = METAString.Atoi();
    temp_d = 10.0; // MHz
    switch (temp_i) {
      case 0:
        temp_d = 10.0;
        //fTimepix_clock = 0x00;
        break;
      case 1:
        temp_d = 20.0;
        //fTimepix_clock = 0x01;
        break;
      case 2:
        temp_d = 40.0;
        //fTimepix_clock = 0x02;
        break;
      case 3:
        temp_d = 80.0;
        //fTimepix_clock = 0x03;
        break;
      default:
        cout
          << "WARNING: * Couldn't determine the Timepix clock : "     << endl
	  << "WARNING: *--> The METAString is '" << METAString << "'" << endl;
        break;
    };//end of Timepix clock code switch.
    fTimepix_clock = temp_d;
    //fTimepix_clock = 0x00;
    break;
  case TIMEPIX_CLOCKINMHZ_CODE:
    fTimepix_clock = METAString.Atof();
    break;
  case FIRMWARE_VERSION_CODE: // The firmware version.
    fFirmware = METAString;
    break;
  case PIXELMAN_VERSION_CODE: // The Pixelman version.
    fPixelman_version = METAString;
    break;
  case MPX_TYPE_CODE:         // The Medipix type code.
    fMpx_type = METAString.Atoi();
    break;
  default:
    cout
      << "WARNING: * Couldn't parse an item of metadata:" << endl
      << "WARNING: *--> " << METAString << endl;
    break;
  }

  /// @todo Decide how to give real dataset IDs to data.
  /// * PGP signature?
  /// * md5 of the output file?
  /// * User decided?
  /// * Connect to a database?

  fMPXDataSetNumber = "1234ABCD";

}// end of FrameStruct::FillMetaData method.

//
// FramesHandler constructor
//
FramesHandler::FramesHandler(TString dataset) {

  // Instantiate one frame.
  // This frame will be  overwritten many times,
  // but is instantiated only once.
  m_aFrame = new FrameStruct(dataset);
  m_nFrames = 0;

  getAFrameMatrix_flag = false;
  getAFrameHist_flag = false;

  m_metaBit = -1;
  m_metaCode = -1;
  m_ParseAndHold = true;

  nFrames256x256 = 0;
  nFramesXYC = 0;

}//end of the FramesHandler constructor.

FramesHandler::~FramesHandler(){

	delete m_aFrame;
}

/* JI - orig
TH2I * FramesHandler::getHistFrame(Int_t frameId, Int_t * frameMatrix){

	TString title = "frame ", hname = "frame_";
	title += frameId; hname += frameId;
	TH2I * histFrame = new TH2I(hname, title,
			MAX_FRAME_ROW, 0, MAX_FRAME_ROW-1,
			MAX_FRAME_COL, 0, MAX_FRAME_COL-1
	);

	Int_t iCntr = 0;
	Int_t jCntr = 0;
	for(Int_t i = 0 ; i < MAX_FRAME_ROW*MAX_FRAME_COL ; i++)
	{
		histFrame->Fill(jCntr, iCntr, frameMatrix[i]);
		iCntr++;
		if(iCntr >= MAX_FRAME_ROW)
		{
			iCntr = 0;
			jCntr++;
		}
		//std::cout << frameMatrix[i][j] << " ";
	}

	return histFrame;
}
 */

//
// FramesHandler::IdentifyTypeOfInput2
//
Int_t FramesHandler::IdentifyTypeOfInput2(
  TString fullDSCFileName,
  Int_t & width,
  Int_t & height,
  int & nFrames
  )
{

  // Create a file stream for the DSC file.
  fstream filestr;
  filestr.open(fullDSCFileName, fstream::in);

  Char_t temp[2048];
  string tempS = "";

  // Extract the very first line which is of the form
  // B000000X or A0000000X where B:=binary, A:=ASCII
  // and X is the number of frames associated to this
  // description (DSC) file.
  //
  // Get the line of the DSC file.
  if (filestr.good()) {
    filestr.getline(temp, 2048);
  }

  // Find the encoding of the payload data.
  int encoding = -1;
  if        (temp[0] == 'A') { // ASCII
    encoding = FSAVE_ASCII;
  } else if (temp[0] == 'B') { // Binary
    encoding = FSAVE_BINARY;
  }

  // If no encoding information found, there is no data
  // associated with this DSC file. Return the error code 0.
  if (encoding == -1) return 0; 

  // Extract number of frames to process.
  temp[0] = '0'; // replace the character and check the number of files
  nFrames = atoi(temp);
  //cout
  //  << "INFO : No. of frames for this DSC file = " << nFrames << endl;

  string infoS;
  string fSave = "";
  size_t found = std::string::npos;

  // Loop over the DSC file.
  while (filestr.good())// && lineCntr < 1)
  {

    tempS = temp;

    // Skip one line and extract the info in the third line
    filestr.getline(temp, 2048); // 2nd line
    filestr.getline(temp, 2048); // 3rd line
    tempS = temp;

    // At this point we need to analyse this type of string.
    // Type=i16 [X,Y,C] width=1024 height=512
    infoS = WIDTH_STRING;
    found = tempS.find(infoS);

    // Extract the frame width.
    int i = (int)found + (int)infoS.length();
    while ( tempS[i] != 0x20 && ( tempS[i] != 0xd && tempS[i] != '\0' ) )
    {
      if( tempS[i] != 0x20 ) {
        fSave.append(1, tempS[i]);
      }
      i++;
    }

    // Store the extracted frame width.
    width = atoi(fSave.c_str());
    m_width = width;

    fSave.clear();

    // Extract the frame height.
    infoS = HEIGHT_STRING;
    found = tempS.find(infoS);
    //tempS[i] != 0xd &&
    i = (int) found + (int) infoS.length();
    while ( tempS[i] != 0x20 && ( tempS[i] != 0xd && tempS[i] != '\0' ) )
    {
      if ( tempS[i] != 0x20 ) {
        fSave.append( 1, tempS[i] );
      }
      i++;
    }

    // Store the extracted frame height.
    height = atoi(fSave.c_str());
    m_height = height;

    // Now extract the frame format.
    // X,Y,C:
    infoS = NEW_STRING_TYPEI16_XYC;
    if ( (found = tempS.find(infoS)) != std::string::npos) {
      return encoding | FSAVE_I16 | FSAVE_SPARSEXY;
    }

    infoS = NEW_STRING_TYPEU32_XYC;
    if ( (found = tempS.find(infoS)) != std::string::npos) {
      return encoding | FSAVE_U32 | FSAVE_SPARSEXY;
    }

    // Matrix:
    infoS = NEW_STRING_TYPEI16_MATRIX;
    if ( (found = tempS.find(infoS)) != std::string::npos) {
      return encoding | FSAVE_I16;
    }

    infoS = NEW_STRING_TYPEU32_MATRIX;
    if ( (found = tempS.find(infoS)) != std::string::npos) {
      return encoding | FSAVE_U32;
    }

    // X,C
    infoS = NEW_STRING_TYPEI16_XC;
    if ( (found = tempS.find(infoS)) != std::string::npos) {
      return encoding | FSAVE_I16 | FSAVE_SPARSEX;
    }

    infoS = NEW_STRING_TYPEU32_XC;
    if ( (found = tempS.find(infoS)) != std::string::npos) {
      return encoding | FSAVE_I16 | FSAVE_SPARSEX;
    }

    break;

  }//end of loop over the DSC file.

  // Otherwise, return the number of frames to process.
  // 0 is an error.
  return 0;

}//end of FramesHandler::IdentifyTypeOfInput2 method.

/*

DEPRECATED

//
// FramesHandler::IdentifyTypeOfInput
//
Int_t FramesHandler::IdentifyTypeOfInput(
  TString fullDSCFileName,
  Int_t & width,
  Int_t & height,
  int & nframes
  )
{

  // Open the DSC file.
  fstream filestr;
  filestr.open(fullDSCFileName, fstream::in);

  Int_t lineCntr = 0;
  Char_t temp[2048];
  std::string tempS = "";
  std::string typeS = FRAME_TYPE_STRING;
  std::string binTypeS = BINARY_FRAME_TYPE_STRING;
  std::string fSave = "";
  size_t found = std::string::npos;

  // Loop through the DSC file.
  while (filestr.good())// && lineCntr < 1)
  {

    filestr.getline(temp, 2048);
    tempS = temp;

    found = tempS.find(typeS);
    if (found != std::string::npos) {

      // The frame format line has been found.
      // Now establish the format BITS
      for (int i = (int)typeS.length(); i < (int)tempS.length(); i++) {
        if (tempS[i] != ' ') {
          fSave.append(1,tempS[i]);
        }
        else break;
      }
    }

    // Check if the frame is in binary format.
    found = tempS.find(binTypeS);

    if (found != std::string::npos) {
      return FSAVE_BINARY;
    }

    lineCntr++;
  }//end of loop over the DSC file.

  // Close the file stream.
  filestr.close();

  return atoi(fSave.c_str());

}//end of FramesHandler::IdentifyTypeOfInput method.
*/

//
// FramesHandler:: RewindAll method
//
void FramesHandler::RewindAll(bool rewind_metadata) {

  // Clean the pixel matrix - set every pixel to 0.
  m_aFrame->CleanUpMatrix();

  // Reset the counters m_nEntriesPad, m_nHitsInPad, m_nChargeInPad.
  m_aFrame->ResetCountersPad();

  // Reset metadata if needed. Not necessary when processing multiframe.
  if (rewind_metadata) m_aFrame->RewindMetaDataValues();

}//end of FramesHandler::RewindAll method.

//
// FramesHandler::readOneFrame method.
//
int FramesHandler::readOneFrame(
  TString fullFileName,
  TString fullDSCFileName,
  int * ftype
  )
{

  // Reset all data in the frame.
  RewindAll();

  // Initially this will be 0 (it is initialized at -1)
  m_aFrame->IncreaseId();

  // Add to the frames counter.
  m_nFrames++;

  // Identify the type of file, 256x256 or XYC
  Int_t width  = 256; // default
  Int_t height = 256; // default

  int nFramesToRead = 1;
  //Int_t frameType = IdentifyTypeOfInput(fullDSCFileName, width, height, nFramesToRead);
  //if ( frameType == 0 ) {
  // try the newer layout
  int frameType = IdentifyTypeOfInput2(fullDSCFileName, width, height, nFramesToRead);
  //}
  *ftype = frameType;

  cout
    << "INFO : The frame type is             '" << frameType << "'" << endl
    << "INFO : The number of frames found is " << nFramesToRead     << endl;

  if ( frameType == 0 ) { // if still 0
    std::cout << "[ERROR] could not determine the frame type --> " << fullDSCFileName << std::endl;
    return -1; // -1 frames to read
  }

  // Store this info in class members
  m_width = width;
  m_height = height;

  // Set the payload format.
  m_aFrame->SetPayloadFormat(frameType);

  if (nFramesToRead > 1) { // Multiframe payload - this is the wrong method!
    return nFramesToRead;
  }

  // Open the frame payload file.
  TString typeS = "";
  fstream filestr;

  Int_t temp = 0;
  Long_t totalCounts = 0;
  Long_t pixelHits = 0;
  Int_t cntri = 0;
  Int_t cntrj = 0;
  Int_t wholePadCntr = 0;

  ////////////////////////////////////////////////////////////////////////////
  // ASCII formats
  ////////////////////////////////////////////////////////////////////////////
  if (frameType == (FSAVE_ASCII | FSAVE_I16) ||
      frameType == (FSAVE_ASCII | FSAVE_U32) ||
      frameType == (FSAVE_ASCII | FSAVE_DOUBLE)
     )
  {
    filestr.open(fullFileName, fstream::in);

    typeS = TYPE_256x256_STRING;
    cout
      << "INFO: * Reading single frame data:" << endl
      << "INFO: *--> Payload file name is '" << fullFileName    << "'" << endl
      << "INFO: *--> DSC file name is     '" << fullDSCFileName << "'" << endl
      << "INFO: *--> Type is '" << typeS << "' (" << frameType  << ")" << endl;

    int maxocc = m_width*m_height;

    // Loop over the payload data.
    while ( filestr.good() && wholePadCntr < maxocc ) {

      filestr >> temp;

      // See if the end of file has been reached.
      if ( filestr.eof() ) break;

      if(temp > 0) {

        // Only if this is a valid entry.
        // Since this is a  matrix even zeros will show here.
        totalCounts += temp;
        pixelHits++;
        m_aFrame->FillOneElement(cntri, cntrj, m_width, temp);
      }
      cntri++;

      if (cntri == m_width) {
        cntrj++;
        cntri = 0;
      }
      wholePadCntr++;
    }//end of loop over the payload file.
  }
  else if (frameType == (FSAVE_ASCII | FSAVE_I16    | FSAVE_SPARSEXY) ||
           frameType == (FSAVE_ASCII | FSAVE_U32    | FSAVE_SPARSEXY) ||
           frameType == (FSAVE_ASCII | FSAVE_DOUBLE | FSAVE_SPARSEXY)
          )
  {
    filestr.open(fullFileName, fstream::in);

    typeS = TYPE_XYC_STRING;
    cout
      << "INFO: * Reading single frame data:" << endl
      << "INFO: *--> Payload file name is '" << fullFileName    << "'" << endl
      << "INFO: *--> DSC file name is     '" << fullDSCFileName << "'" << endl
      << "INFO: *--> Type is '" << typeS << "' (" << frameType  << ")" << endl;

    std::map<Int_t, Int_t> frameMap;
    for (Int_t itr = 0; itr < MAX_FRAME_OCC; itr++) {
      frameMap[itr] = 0;
    }

    Int_t fillTime = 0;
    Int_t colr = 0, rowr = 0;

    // Loop over the payload file.
    while (filestr.good()) {

      filestr >> temp;

      // See if the end of file has been reached.
      if ( filestr.eof() ) break;

      if (fillTime%3 == 0) {
        // Got X.
        // There is a harmles extra read at this point. No risk.
			
        colr = temp;
        //std::cout << "colr: " << colr << std::endl;
        fillTime++;
      } else if (fillTime%3 == 1) { //got Y
        rowr = temp;
        //std::cout << "rowr: " << rowr << std::endl;
        fillTime++;
      } else if (fillTime%3 == 2) { // Got Counts

        //std::cout << "val: " << temp << std::endl;
        totalCounts += temp;
        pixelHits++;
        m_aFrame->FillOneElement(colr, rowr, m_width, temp);
        fillTime = 0;
      }

      cntri++;
      if (cntri == m_width) {
        cntrj++;
        cntri = 0;
      }

    }//end of loop over the payload file.

    // Now read the dictionary and fill the matrix.
    // There is no need for this anymore, so it's commented out.
    /*
    std::map<Int_t, Int_t>::iterator it;
    cntri = 0;
    cntrj = 0;

    for ( it=frameMap.begin() ; it != frameMap.end(); it++ ) {

      m_aFrame->FillOneElement(cntri, cntrj, m_width, (*it).second);
      cntri++;

      if (cntri == MAX_FRAME_COL) {
        cntrj++;
        cntri = 0;
      }

    }
    */

  }
  else if (frameType == (FSAVE_ASCII | FSAVE_I16    | FSAVE_SPARSEX) ||
           frameType == (FSAVE_ASCII | FSAVE_U32    | FSAVE_SPARSEX) ||
           frameType == (FSAVE_ASCII | FSAVE_DOUBLE | FSAVE_SPARSEX)
          )
  {
    filestr.open(fullFileName, fstream::in);

    typeS = TYPE_XC_STRING;
    cout
      << "INFO: * Reading single frame data:" << endl
      << "INFO: *--> Payload file name is '" << fullFileName    << "'" << endl
      << "INFO: *--> DSC file name is     '" << fullDSCFileName << "'" << endl
      << "INFO: *--> Type is '" << typeS << "' (" << frameType  << ")" << endl;

    std::map<Int_t, Int_t> frameMap;
    for (Int_t itr = 0 ; itr < MAX_FRAME_OCC ; itr++) {
      frameMap[itr] = 0;
    }

    Int_t fillTime = 0;
    Int_t colr = 0, rowr = 0;

    // Loop over the payload file.
    while (filestr.good()) {
      filestr >> temp;

      // See if the end of file has been reached.
      if ( filestr.eof() ) break;

      if (fillTime%2 == 0) {// Got the pixel X value 			
        colr = temp;
        //std::cout << "colr: " << colr << std::endl;
        fillTime++;
      } else if (fillTime%2 == 1) { // Got the pixel ToT counts.
        totalCounts += temp;
        pixelHits++;
        frameMap[colr] = temp;
        fillTime = 0;
      }

      cntri++;
      if (cntri == m_width){
        cntrj++;
        cntri = 0;
      }

    }//end of loop over the payload file.

    // Now read the map and fill the matrix.
    std::map<Int_t, Int_t>::iterator it;
    for (it=frameMap.begin(); it != frameMap.end(); it++ ) {
      m_aFrame->FillOneElement((*it).first, (*it).second);
    }

  }

  ////////////////////////////////////////////////////////////////////////////
  // Binary formats
  ////////////////////////////////////////////////////////////////////////////
  /// @todo Implement reading the binary matrix format for single frames.

  else if (
           ( frameType == (FSAVE_BINARY | FSAVE_I16 | FSAVE_SPARSEXY) )
          )
  {

    typeS = TYPE_XYC_BIN_STRING;
    cout
      << "INFO: * Reading single frame data:" << endl
      << "INFO: *--> Payload file name is '" << fullFileName    << "'" << endl
      << "INFO: *--> DSC file name is     '" << fullDSCFileName << "'" << endl
      << "INFO: *--> Type is '" << typeS << "' (" << frameType  << ")" << endl;

    //filestr.open(fullFileName, fstream::binary);
    filestr.open(fullFileName, istream::in);

    unsigned int bytesRead = 0;
    char tempByte[4];
    unsigned int x = 0x0, y = 0x0, counts = 0x0;

    // Loop over the payload file.
    while (filestr.good()) {

      // read X, 32 bits
      x = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);

      // See if the end of file has been reached.
      if ( filestr.eof() ) break;

      // Read x
      push_back_nbytes(&x, tempByte, 4);

      // Read y, 32 bits
      y = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);
      push_back_nbytes(&y, tempByte, 4);

      // Read ToT counts, 16 bits
      counts = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      push_back_nbytes(&counts, tempByte, 2);

      //printf("%x, %x --> %x\n", x, y, counts);
      //printf("%d, %d --> %d\n", x, y, counts);

      bytesRead += 10;

      m_aFrame->FillOneElement(x, y, m_width, counts);

    }

  }
  else if (
             ( frameType == (FSAVE_BINARY | FSAVE_U32 | FSAVE_SPARSEXY) )
          )
  {

    typeS = TYPE_XYC_BIN_STRING;
    cout
      << "INFO: * Reading single frame data:" << endl
      << "INFO: *--> Payload file name is '" << fullFileName    << "'" << endl
      << "INFO: *--> DSC file name is     '" << fullDSCFileName << "'" << endl
      << "INFO: *--> Type is '" << typeS << "' (" << frameType  << ")" << endl;

    //filestr.open(fullFileName, fstream::binary);
    filestr.open(fullFileName, istream::in);

    unsigned int bytesRead = 0;
    char tempByte[4];
    unsigned int x = 0x0, y = 0x0, counts = 0x0;

    // Loop over the payload file.
    while (filestr.good()) {

      // Read the pixel x, 32 bits.
      x = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);

      // See if the end of file has been reached.
      if ( filestr.eof() ) break;

      push_back_nbytes(&x, tempByte, 4);

      // read the pixel y, 32 bits.
      y = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);
      push_back_nbytes(&y, tempByte, 4);

      // Read the ToT counts, 32 bits.
      counts = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);
      push_back_nbytes(&counts, tempByte, 4);

      //printf("%x, %x --> %x\n", x, y, counts);
      //printf("%d, %d --> %d\n", x, y, counts);

      bytesRead += 12;

      // Fill the payload data in the frame container.
      m_aFrame->FillOneElement(x, y, m_width, counts);

    }
  }
  else if (
           ( frameType == (FSAVE_BINARY | FSAVE_I16 | FSAVE_SPARSEX ) )
          )
  {

    typeS = TYPE_XYC_BIN_STRING;
    cout
      << "INFO: * Reading single frame data:" << endl
      << "INFO: *--> Payload file name is '" << fullFileName    << "'" << endl
      << "INFO: *--> DSC file name is     '" << fullDSCFileName << "'" << endl
      << "INFO: *--> Type is '" << typeS << "' (" << frameType  << ")" << endl;

    //filestr.open(fullFileName, fstream::binary);
    filestr.open(fullFileName, istream::in);

    unsigned int bytesRead = 0;
    char tempByte[4];
    unsigned int x = 0x0, counts = 0x0;

    // Loop over the payload file.
    while (filestr.good()) {

      // Read the pixel X value, 32 bits.
      x = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);

      // See if the end of file has been reached.
      if ( filestr.eof() ) break;

      push_back_nbytes(&x, tempByte, 4);

      // Read the pixel ToT counts, 16 bits.
      counts = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      push_back_nbytes(&counts, tempByte, 2);

      //printf("%x, %x --> %x\n", x, y, counts);
      //printf("%d, %d --> %d\n", x, y, counts);

      bytesRead += 6;

      // Fill the frame container with the payload information.
      m_aFrame->FillOneElement(x, counts);

    }

  }
  else if (
           ( frameType == (FSAVE_BINARY | FSAVE_U32 | FSAVE_SPARSEX ) )
          )
  {

    typeS = TYPE_XYC_BIN_STRING;
    cout
      << "INFO: * Reading single frame data:" << endl
      << "INFO: *--> Payload file name is '" << fullFileName    << "'" << endl
      << "INFO: *--> DSC file name is     '" << fullDSCFileName << "'" << endl
      << "INFO: *--> Type is '" << typeS << "' (" << frameType  << ")" << endl;

    //filestr.open(fullFileName, fstream::binary);
    filestr.open(fullFileName, istream::in);

    unsigned int bytesRead = 0;
    char tempByte[4];
    unsigned int x = 0x0, counts = 0x0;

    // Loop over the payload file.
    while (filestr.good()) {

      // Read the pixel X value, 32 bits.
      x = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);

      // See if the end of file has been reached.
      if ( filestr.eof() ) break;

      push_back_nbytes(&x, tempByte, 4);

      // Read the pixel ToT counts value, 32 bits.
      counts = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);
      push_back_nbytes(&counts, tempByte, 4);

      //printf("%x, %x --> %x\n", x, y, counts);
      //printf("%d, %d --> %d\n", x, y, counts);

      bytesRead += 8;

      // Fill the frame container with the payload information.
      m_aFrame->FillOneElement(x, counts);

    }//end of loop over the payload file.

  }
  else {
    // Unknown case ... giving up here.
  }

  // Set the frame width and height.
  /// @todo Get the width and height information from the DSC file.
  SetnX(width);
  SetnY(height);

  // Set the payload format...
  m_aFrame->SetPayloadFormat(frameType);

  // Close the payload file stream.
  filestr.close();

  // Extract the metadata for the frame.
  fstream META_filestr;
  Int_t nLineMetaFile = 0;
  META_filestr.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );

  // Read the metadata.
  try {

    META_filestr.open(fullDSCFileName, fstream::in);

    Char_t * METAtemp;
    METAtemp = new char[META_DATA_LINE_SIZE];

    // Loop over the metadata file.
    while (META_filestr.good()) {

      META_filestr.getline(METAtemp, META_DATA_LINE_SIZE);

      // Parse the current line of metadata.
      if (m_ParseAndHold) parseMetaLine((TString)METAtemp, nLineMetaFile);

      if (nLineMetaFile++ == m_metaBit) {

        // Fill the frame container with the extracted metadata.
        m_aFrame->FillMetaData((TString)METAtemp, m_metaCode);
        m_ParseAndHold = true;

      }

    }//end of loop over the metadata file.
  }
  catch (ifstream::failure e) {
    //cout
    //  << "ERROR: * Exception opening/reading file '" << fullDSCFileName << "'" << endl
    //  << "ERROR: * The file probably doesn't not exist." << endl;
    //exit(1);
  }

  // Close the metadata file.
  META_filestr.close();

  return 1; // It's single frame, as expected.

}//end of FramesHandler::readOneFrame method.

//
// FramesHandler::push_back_nbytes (counts only)
//
void FramesHandler::push_back_nbytes(
  unsigned int * val,
  char         * bytes,
  int            nbytes)
{

  // The indices are in the following format:
  // 0 --> lower byte  0x......XX
  // 3 --> higher byte 0xXX......

  *val &= 0x00000000;
  unsigned int tempVal;

  for(int idx = nbytes-1 ; idx >= 0 ; idx--) {

    // Get the byte
    tempVal = 0x0;
    tempVal ^= bytes[idx];

    // Clean up to have info for only one byte
    tempVal &= 0x000000ff;

    // Switch it to the right place
    for(int sw = 0 ; sw < idx ; sw++) {
      tempVal = tempVal << 8;
    }
    // XOR the value
    *val ^= tempVal;

  }//end of loop over the nbytes.

}//end of push_back_nbytes methods.

//
// FramesHandler::push_back_nbytes
//
void FramesHandler::push_back_nbytes(
  long long * val,
  char      * bytes,
  int         nbytes)
{

  // The indices are in the following format:
  // 0 --> lower byte  0x......XX
  // 3 --> higher byte 0xXX......

  *val &= 0x0000000000000000; // 64 bits
  unsigned int tempVal;

  for(int idx = nbytes-1 ; idx >= 0 ; idx--) {

    // Get the byte.
    tempVal  = 0x0;
    tempVal ^= bytes[idx];

    // Clean up the value to leave only one byte.
    tempVal &= 0x000000ff;

    // Switch it to the right place.
    for (int sw = 0; sw < idx; sw++) {
      tempVal = tempVal << 8;
    }

    // XOR the value.
    *val ^= tempVal;

  }//end of loop over the number of bytes.

}//end of FramesHandler::push_back_nbytes (64 bit).

//
// FramesHandler::LoadFramePixel
//
Bool_t FramesHandler::LoadFramePixel(
  Int_t col,
  Int_t row,
  Int_t counts,
  Double_t truthE,
  Double_t E)
{

  // Fill the frame container.
  m_aFrame->FillOneElement(col, row, m_width, counts, truthE, E);

  // Report the success of the operation.
  return true;

}//end of FramesHandler::LoadFramePixel (incl. energy) method.

//
// FramesHandler::LoadFramePixel
//
Bool_t FramesHandler::LoadFramePixel(Int_t col, Int_t row, Int_t counts) {

  // Fill the frame container.
  m_aFrame->FillOneElement(col, row, m_width, counts);

  // Report the success of the operation.
  return true;

}

//
// FramesHandler::SetLVL1
//
void FramesHandler::SetLVL1(Int_t col, Int_t row, Int_t lvl1) {
  m_aFrame->SetLVL1(col, row, m_width, lvl1);
}

void FramesHandler::IncreaseCurrentFrameId(){
	m_aFrame->IncreaseId();
}

//
// FramesHandler::LoadFrameMetaData
//
Bool_t FramesHandler::LoadFrameMetaData(TString metastring , Int_t metacode){

  // Fill the item of metadata.
  m_aFrame->FillMetaData(metastring, metacode);

  // Report on the success of the operation.
  return true;

}

//
//
//
void FramesHandler::parseMetaLine(TString aMetaLine, Int_t currentLine){

  // It is probably a good idea to change this to
  // "switch case" because it may be a bit faster
  if (aMetaLine.Contains(ACQ_MODE_STRING, TString::kExact)){
    //std::cout << "-> 1" << std::endl;
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = ACQ_MODE_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(ACQ_TIME_STRING, TString::kExact)){
    //std::cout << "-> 2" << std::endl;
    m_metaBit    = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode   = ACQ_TIME_CODE;
  m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(CHIPBOARDID_STRING, TString::kExact)){
    //std::cout << "-> 3" << std::endl;
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = CHIPBOARDID_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(DACS_STRING, TString::kExact)){
    //std::cout << "-> 4" << std::endl;
    m_metaBit    = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode   = DACS_CODE;
  m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(HW_TIMER_STRING, TString::kExact)){
    //std::cout << "-> 5" << std::endl;
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = HW_TIMER_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(INTERFACE_STRING, TString::kExact)){
    //std::cout << "-> 6" << std::endl;
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = INTERFACE_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(POLARITY_STRING, TString::kExact)){
    //std::cout << "-> 7" << std::endl;
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = POLARITY_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(START_TIME_STRING, TString::kExact)){
    //std::cout << "-> 8" << std::endl;
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = START_TIME_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(START_TIME_S_STRING, TString::kExact)){
    //std::cout << "-> 9" << std::endl;
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = START_TIME_S_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(MPX_CLOCK_STRING, TString::kExact)){
    //std::cout << "-> 9" << std::endl;
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = MPX_CLOCK_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(HV_STRING, TString::kExact)){
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = HV_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(TIMEPIX_CLOCK_STRING,      TString::kExact)
          &&
        ! aMetaLine.Contains(TIMEPIX_CLOCKINMHZ_STRING, TString::kExact)
         )
  {
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = TIMEPIX_CLOCK_CODE ;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(TIMEPIX_CLOCKINMHZ_STRING, TString::kExact)){
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = TIMEPIX_CLOCKINMHZ_CODE ;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(FIRMWARE_VERSION_STRING, TString::kExact)){
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = FIRMWARE_VERSION_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(PIXELMAN_VERSION_STRING, TString::kExact)){
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = PIXELMAN_VERSION_CODE;
    m_ParseAndHold = false;
  }
  else if(aMetaLine.Contains(MPX_TYPE_STRING, TString::kExact)){
    m_metaBit      = currentLine + ACTUAL_VALUE_OFFSET;
    m_metaCode     = MPX_TYPE_CODE;
    m_ParseAndHold = false;
  }
  else{
    //metaBit = -1;
  }

}//end of parseMetaLine method.

/*
//
//
//
TH2I * FramesHandler::getAFrameHist(TString fullFileName, TString fullDSCFileName, TString){

  getAFrameMatrix_flag = false; getAFrameHist_flag = true;
  Bool_t frameTrigger = readOneFrame(fullFileName, fullDSCFileName);

  // check the integrity of the frame
  if (!frameSupervisor())
  {
    std::cout << "[ERROR] frame " << fullFileName << "\n";
    std::cout << "        does not seem to be a well formatted frame\n";
    std::cout << "        giving up." << std::endl;
    exit(1);
  }

  TH2I * frameHist = 0;
  TH2I * h_emptyFrame = 0;

  Int_t * matrix_i;
  matrix_i = m_aFrame->GetPixelsMatrix();
  if (matrix_i != 0)
  {
    frameHist = getHistFrame(m_aFrame->GetFrameId(), matrix_i);
  }
  else
  {
    std::cout << "[ERROR] frame matrix not initialized ... givin up" << std::endl;
    exit(1);
  }


  if    (frameTrigger) return frameHist;
  else                 return h_emptyFrame;
}
*/

//
// FramesHandler::getAFrameMatrix method.
//
Int_t ** FramesHandler::getAFrameMatrix(

  TString fullFileName,
  TString /*fullDSCFileName*/) {

  getAFrameMatrix_flag = true;
  getAFrameHist_flag   = false;

  //Bool_t frameTrigger = readOneFrame(fullFileName, fullDSCFileName);

  // Check the integrity of the frame.
  if (!frameSupervisor()) {
    cout << "ERROR: Frame '" << fullFileName << "'" << endl;
    cout << "ERROR: does not seem to be a well formatted frame." << endl;
    cout << "ERROR: giving up." << endl;
    exit(1);
  }

  //if (m_aFrame->GetPixelsMatrix() != 0) return m_aFrame->GetPixelsMatrix();

  // not used !
  return 0x0;

}//end of FramesHandler::getAFrameMatrix method.

//
// FramesHandler::XYtoX method.
//
Int_t FramesHandler::XYtoX(int x, int y, Int_t dimX) {
  return (y * dimX) + x;
}

//
// FramesHandler::ProcessMultiframe
//
Bool_t FramesHandler::ProcessMultiframe(
  TString datafile,
  TString dscfile,
  TString idxfile,
  WriteToNtuple * wte,
  int ftype
  )
{

  // The frame format/type is already known at this point, and is
  // supplied by ftype.

  // Open the payload file for reading.
  fstream filestr;
  filestr.open(datafile, fstream::in);

  // Open the index file for reading.
  fstream filestr_idx;
  filestr_idx.open(idxfile, fstream::in);

  cout
    << "INFO: * Processing multiple frames from:"         << endl
    << "INFO: *--> Payload file: '" << datafile << "'"    << endl
    << "INFO: *--> DSC file:     '" << dscfile  << "'"    << endl
    << "INFO: *--> index file:   '" << idxfile  << "'"    << endl
    << "INFO: * Payload format is type (" << ftype << ")" << endl;

  // With multiple frame payloads, the metadata
  // has to be processed at the same time.

  // Fetch the metadata
  //--------------------

  // Create a file stream for the metadata.
  fstream META_filestr;
  Int_t nLineMetaFile = 0;

  //META_filestr.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );

  // Prepare to read the metadata.
  META_filestr.open(dscfile.Data(), fstream::in);
  Char_t * METAtemp;
  METAtemp = new char[META_DATA_LINE_SIZE];

  // Loop over the metadata.
  while ( META_filestr.good() ) {

    // Get the next line of metadata.
    META_filestr.getline(METAtemp, META_DATA_LINE_SIZE);

    // Parse the line of metadata.
    if(m_ParseAndHold) parseMetaLine((TString)METAtemp, nLineMetaFile); // pass currentLine

    if (nLineMetaFile++ == m_metaBit) {

      m_aFrame->FillMetaData((TString)METAtemp, m_metaCode);
      m_ParseAndHold = true;

    }

  }//end of loop over the metadata file.

  // Close the metadata file.
  META_filestr.close();


  if (
      ftype == (FSAVE_ASCII | FSAVE_I16 | FSAVE_SPARSEX) ||
      ftype == (FSAVE_ASCII | FSAVE_U32 | FSAVE_SPARSEX)
     ) {

    std::map<int, int> frameMap;
    std::map<int, int>::iterator it;

    int fillTime = 0;
    int xi = 0;
    char temps[256];
    bool newframe = false;
    int cntr = 0;

    // Loop over the payload file.
    while (filestr.good()) {

      // Read the payload data
      filestr.getline(temps, 256);

      // Check for the end of file (EOF)
      if(filestr.eof()) break;     // EOF

      // Check for the next frame indicator.
      if (temps[0] == '#') { // new frame
        newframe = true;
      }

      if(!newframe) {
        // extract both numbers
        string tempval;
        for (int i = 0; i < 256; i++) {
          //printf("%x\n",temps[i]);
          if (temps[i] != 0x9 && (temps[i] != 0xa && temps[i] != 0xd) ) { // 0x9 horizontal tab
            tempval.append(1, temps[i]);
          } else { // finish
            if (fillTime%2 == 0) {        // X
              xi = atoi( tempval.c_str() );
              tempval.clear();
              fillTime++;
            } else if (fillTime%2 == 1) { // C
              frameMap[xi] = atoi( tempval.c_str() );
              tempval.clear();
              fillTime = 0;
            }

          }

          if (temps[i] == 0xa || temps[i] == 0xd) break; // end of this line
        }//end of loop over the 256 entries.

        //cout << xi << ", " << frameMap[xi] << endl;

      }//end of NOT new frame check.

      if (newframe) {

        //cout << "Filling frame " << cntr << endl;

        // Read the dictionary and fill the matrix.
        for (it=frameMap.begin(); it != frameMap.end(); it++ ) {
          // X,C
          //cout << (*it).first << ", " << (*it).second << endl;
          m_aFrame->FillOneElement((*it).first, (*it).second);
        }
        
        // Set the per-frame metadata 
        SetnX(m_width);
        SetnY(m_height);
        m_aFrame->SetId(cntr);

        // Clean map
        frameMap.clear();

        // Finally, fill the frame container with the extracted information.
        // The second parameter indicates that we don't need to reset the
        // metadata.
        wte->fillVars(this, false); 

        // Indicate that we're ready to read the next frame.
        newframe = false;
        cntr++;
      }//end of new frame check.

    }//end of loop over the payload file.

    // Close the payload data file.
    filestr.close();

  } else if (
              ftype == (FSAVE_ASCII | FSAVE_I16 | FSAVE_SPARSEXY) ||
              ftype == (FSAVE_ASCII | FSAVE_U32 | FSAVE_SPARSEXY)
            ) {

    cout << "DEBUG: Frame is ASCII x,y,C!!!!!!!" << endl;

    std::map<int, int> frameMap;
    std::map<int, int>::iterator it;

    int fillTime = 0;
    int xi = 0, yi = 0;
    char temps[256];
    bool newframe = false;
    int cntr = 0;

    // Loop over the payload data.
    while (filestr.good()) {

      // Read the data.
      filestr.getline(temps, 256);

      // Check for the EOF.
      if (filestr.eof()) break; // EOF reached.

      // Check for the next frame indicator.
      if (temps[0] == '#') { // A new frame.
        newframe = true;
      }

      if (!newframe) {

        // Extract both numbers.
        string tempval;
        for (int i = 0; i < 256; i++) {

          //printf("%x\n",temps[i]);
          if (temps[i] != 0x9 && (temps[i] != 0xa && temps[i] != 0xd) ) { // 0x9 horizontal tab
            tempval.append(1, temps[i]);
          } else { // Finish
            if (fillTime%3 == 0) {        // X
              xi = atoi( tempval.c_str() );
              tempval.clear();
              fillTime++;
            } else if (fillTime%3 == 1){  // Y
              yi = atoi( tempval.c_str() );
              tempval.clear();
              fillTime++;
            } else if (fillTime%3 == 2) { // C
              frameMap[ XYtoX(xi, yi, m_width) ] = atoi( tempval.c_str() );
              tempval.clear();
              fillTime = 0;
            }
          }

          if(temps[i] == 0xa || temps[i] == 0xd) break; // end of this line
        }//end of loop over the 256 entries(???).

        //cout << xi << ", " << yi << " : " << XYtoX(xi, yi, m_width) << endl;

      }

      if (newframe) {

        //cout << "Filling frame " << cntr << endl;

        // Now read the dictionary and fill the matrix.
        for (it=frameMap.begin(); it != frameMap.end(); it++ ) {
          // X,C
          //cout << (*it).first << ", " << (*it).second << endl;
          m_aFrame->FillOneElement((*it).first, (*it).second);
        }//end of loop over the frame map.
        
        // Set the per-frame metadata.
        SetnX(m_width);
        SetnY(m_height);
        m_aFrame->SetId(cntr);

        // Clean up the frame map.
        frameMap.clear();

        cout << "DEBUG: Calling fillVars!!!!!!!!!!!!" << endl;

        // Finally, fill the frame container with the extracted data.
        wte->fillVars(this, false); // Don't reset metadata.

        // Indicate that we're ready to read the next frame.
        newframe = false;
        cntr++;
      }//end of new frame check.

    }

    // Close the payload file stream.
    filestr.close();

  } else if (
             ftype == (FSAVE_BINARY | FSAVE_I16 | FSAVE_SPARSEXY)
             ||
             ftype == (FSAVE_BINARY | FSAVE_U32 | FSAVE_SPARSEXY)
            ) 
  {

    // 2 := I16, 4 := U32
    int nCountBytes = 2;
    if ( ftype == (FSAVE_BINARY | FSAVE_I16 | FSAVE_SPARSEXY) ) nCountBytes = 2;
    if ( ftype == (FSAVE_BINARY | FSAVE_U32 | FSAVE_SPARSEXY) ) nCountBytes = 4;

    // Information from the idx file (indexation).
    // All starting at 0
    long long dscPos = 0, dataPos = 0, sfPos = 0;
    long long pixelAppendCntr = 0;

    // For the payload data.
    long long bytesRead = 0;
    char tempByte[4];
    unsigned int x = 0x0, y = 0x0, counts = 0x0;

    // Get the index (idx) values.
    // This information indexes from the _second_ frame.
    GetIdxValues(&filestr_idx, &dscPos, &dataPos, &sfPos);

    // Clean up.
    getFrameStructObject()->CleanUpMatrix();
    getFrameStructObject()->ResetCountersPad();

    // Loop over the payload data.
    while ( filestr.good() ) {

      // read X, 32 bits
      x = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);

      // Check for the EOF.
      if (filestr.eof()) break;     // EOF

      push_back_nbytes(&x, tempByte, 4);

      // read Y, 32 bits
      y = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      filestr.get(tempByte[2]);
      filestr.get(tempByte[3]);
      push_back_nbytes(&y, tempByte, 4);

      // Read Counts, 16 bits or 32 bits
      counts = 0x0;
      filestr.get(tempByte[0]);
      filestr.get(tempByte[1]);
      if (nCountBytes == 4){ // U32
        filestr.get(tempByte[2]);
        filestr.get(tempByte[3]);
      }
      push_back_nbytes(&counts, tempByte, nCountBytes);

      //printf("%x, %x --> %x\n", x, y, counts);
      //printf("0x%x 0x%x, --> 0x%x | %d %d, --> %d\n", x, y, counts, x, y, counts);
      if (nCountBytes == 2) bytesRead += 10;
      if (nCountBytes == 4) bytesRead += 12;

      // Store this pixel information.
      if (bytesRead < dataPos) {

        getFrameStructObject()->FillOneElement(x, y, 256, counts);
        pixelAppendCntr++;

      } else { // Change Frame

        getFrameStructObject()->IncreaseId();
        getFrameStructObject()->SetnX(256);
        getFrameStructObject()->SetnY(256);
        wte->fillVars(this, false); // don't rewind medatada

        // Get new Idx values.
        GetIdxValues(&filestr_idx, &dscPos, &dataPos, &sfPos);
        // Don't rewind the bytes counter. dataPos matches this value.

        // Reset the pixel counter per frame.
        pixelAppendCntr = 0;

      }//end of bytesRead/dataPos check.

    }//end of loop over the payload data.

  }//end of payload format/type check.

  //cout
  // << "INFO: Multiframe ... " << cntr << " frames processed" << endl;

  // Report on the success of the operation.
  return true;

}//end of the FramesHandler::ProcessMultiframe method.

//
// FramesHandler::GetIdxValues method.
//
void FramesHandler::GetIdxValues(
  fstream * filestr_idx,
  long long * dscPos,
  long long * dataPos,
  long long * sfPos
  )
{
  // for idx
  char tempByte64[8]; // 64 bits

  if (filestr_idx->good()) {
    // The DSC position.
    for(int i = 0 ; i < 8 ; i++) {
      filestr_idx->get(tempByte64[i]);
    }
    push_back_nbytes(dscPos, tempByte64, 8);

    // The data position.
    for(int i = 0 ; i < 8 ; i++) {
      filestr_idx->get(tempByte64[i]);
    }
    push_back_nbytes(dataPos, tempByte64, 8);

    // The sf position.
    for(int i = 0 ; i < 8 ; i++) {
      filestr_idx->get(tempByte64[i]);
    }
    push_back_nbytes(sfPos, tempByte64, 8);

    //cout
    //  << "dscPos  = " << *dscPos  << ", "
    //  << "dataPos = " << *dataPos << ", "
    //  << "sfPos   = " << *sfPos   << endl;

  }//end of the idx file check.

}//end of FramesHandler::GetIdxValues method.


/*
void FramesHandler::push_back_nbytes(unsigned int * val, char * bytes, int nbytes) {

  // indexes go like this
  // 0 --> lower byte  0x......XX
  // 3 --> higher byte 0xXX......

 *val &= 0x00000000;
  unsigned int tempVal;

  for(int idx = nbytes-1 ; idx >= 0 ; idx--) {

    // Get the byte
    tempVal = 0x0;
    tempVal ^= bytes[idx];
    // Clean up to have info for only one byte
    tempVal &= 0x000000ff;
    // Switch it to the right place
    for(int sw = 0 ; sw < idx ; sw++){
      tempVal = tempVal << 8;
    }
    // XOR the value
    *val ^= tempVal;

  }

}
*/

//
// FramesHandler::frameSupervisor method
//
Bool_t FramesHandler::frameSupervisor() {

  if (m_aFrame->GetEntriesPad() != MAX_FRAME_OCC) {
    return false;
  } else {
    return true;
  }

}//end of FramesHandler::frameSupervisor method.
