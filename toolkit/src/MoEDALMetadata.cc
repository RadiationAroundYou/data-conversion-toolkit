/// @file MoEDALMetadata.cc
/// @brief Implementation of the calibration meta data container class.

#include "MoEDALMetadata.h"

//
// MoEDALMetadata constructor
//
MoEDALMetadata::MoEDALMetadata(
  TString xmlPath
  )

 :
  m_xmlPath(xmlPath),
  fMPXDataSetNumber(""),
  fAcq_mode(0),
  fHw_timer(0),
  fAuto_erase_interval(0.0),
  fAuto_erase_interval_counter(0),
  fCoincidence_mode(0x00),
  fCoincidence_delay(0x00),
  fCoinc_live_time(0.0),
  fPolarity(1),
  fHV(0.0),
  fMpx_clock(0.0),
  fTimepix_clock(0x00),
  fBS_active(false),
  fChipboardID(""),
//  fCustom_name(""),
//  fFirmware(""),
//  fInterface(""),
//  fMpx_type(0),
  fLatitude(0.0), fLongitude(0.0), fAltitude(0.0),
  m_omega_x(0.0),  m_omega_y(0.0),   m_omega_z(0.0),
  m_roll_orf(0.0), m_pitch_orf(0.0), m_yaw_orf(0.0)
//  m_x(0.0), m_y(0.0), m_z(0.0),

{

  // Set the counters variable.
  fCounters.clear();
  for (Int_t i=0;i<3;i++) fCounters.push_back(0);

  // Set up the DACs.
  fDACs.clear();
  for (Int_t i=0;i<14;i++) fDACs.push_back(0);

/*
  fApplied_filters = "";

  // Leave the primary vertex (PV) vectors empty - this
  // calibration data is "keepin' it real".
  m_primaryVertex_x.clear();
  m_primaryVertex_y.clear();
  m_primaryVertex_z.clear();

*/

  if (!ParseXmlFile()) { cout<<"ERROR: Invalid XML file."<<endl; exit(1);} 

} //end of constructor.

//
// MoEDALMetadata destructor
//
MoEDALMetadata::~MoEDALMetadata() {}

//
// MoEDALMetadata::ParseXmlFile
//
Bool_t MoEDALMetadata::ParseXmlFile() {

  // Get the metadata from the XML file.

  TXMLEngine * xml = new TXMLEngine;

  XMLDocPointer_t xmldoc = xml->ParseFile(m_xmlPath);
  
  if (xmldoc==0) {
    delete xml;
    //exit(1);
    return false;
  }

  // Get access to the main node.
  XMLNodePointer_t mainnode = xml->DocGetRootElement(xmldoc);

  XMLAttrPointer_t attr = xml->GetFirstAttr(mainnode);
  TString attrname = "", attrvalue = ""; 
  while (attr!=0) {
    attrname  = xml->GetAttrName(attr);
    attrvalue = xml->GetAttrValue(attr);
    if (attrname == "id") fMPXDataSetNumber = attrvalue;
    //printf("%*c attr: %s value: %s\n",3,' ', xml->GetAttrName(attr), xml->GetAttrValue(attr));
    attr = xml->GetNextAttr(attr);  
  }


  
  XMLNodePointer_t child = xml->GetChild(mainnode);
  TString cn = "";
  while (child!=0) {

    cn = xml->GetNodeName(child);

    if      (cn=="framewidth")    fWidth       = atoi(xml->GetNodeContent(child));
    else if (cn=="frameheight")   fHeight      = atoi(xml->GetNodeContent(child));
    else if (cn=="payloadformat") fFormat      = atoi(xml->GetNodeContent(child));
    //
    else if (cn=="acqmode")       fAcq_mode    = atoi(xml->GetNodeContent(child));
    else if (cn=="counterstr")    fCounters[0] = atoi(xml->GetNodeContent(child));
    else if (cn=="countersvc")    fCounters[1] = atoi(xml->GetNodeContent(child));
    else if (cn=="countersic")    fCounters[2] = atoi(xml->GetNodeContent(child));
    else if (cn=="hwtimer")       fHw_timer    = atoi(xml->GetNodeContent(child));
    else if (cn=="autoeraseint")  fAuto_erase_interval = atof(xml->GetNodeContent(child));
    else if (cn=="autoeraseintc") fAuto_erase_interval_counter = atoi(xml->GetNodeContent(child));
    else if (cn=="triggertime")   fTrigger_time = atof(xml->GetNodeContent(child));
    else if (cn=="coincmode") {
      // TODO - handle other coincidence modes.
      if (TString(xml->GetNodeContent(child))=="0x00") { fCoincidence_mode = 0x00;}
    }
    else if (cn=="coincdelay") { 
      // TODO - handle the 1-256 number to byte code conversion.
      if (xml->GetNodeContent(child)=="0x00") fCoincidence_delay = 0x00;
    }
    else if (cn=="coinclivetime") fCoinc_live_time = atof(xml->GetNodeContent(child));
    else if (cn=="pixelmanversion") fPixelman_version = xml->GetNodeContent(child);
    //
    else if (cn=="lat")    fLatitude   = atof(xml->GetNodeContent(child));
    else if (cn=="lon")    fLongitude  = atof(xml->GetNodeContent(child));
    else if (cn=="alt")    fAltitude   = atof(xml->GetNodeContent(child));
    else if (cn=="roll")   m_roll_orf  = atof(xml->GetNodeContent(child));
    else if (cn=="pitch")  m_pitch_orf = atof(xml->GetNodeContent(child));
    else if (cn=="yaw")    m_yaw_orf   = atof(xml->GetNodeContent(child));
    else if (cn=="omegax") m_omega_x   = atof(xml->GetNodeContent(child));
    else if (cn=="omegay") m_omega_y   = atof(xml->GetNodeContent(child));
    else if (cn=="omegaz") m_omega_z   = atof(xml->GetNodeContent(child));
    //
    else if (cn=="polarity")    fPolarity = atoi(xml->GetNodeContent(child));
    else if (cn=="biasvoltage") fHV       = atof(xml->GetNodeContent(child));
    //
    else if (cn=="ikrum")       fDACs[ 0] = atoi(xml->GetNodeContent(child));
    else if (cn=="disc")        fDACs[ 1] = atoi(xml->GetNodeContent(child));
    else if (cn=="preamp")      fDACs[ 2] = atoi(xml->GetNodeContent(child));
    else if (cn=="buffanaloga") fDACs[ 3] = atoi(xml->GetNodeContent(child));
    else if (cn=="buffanalogb") fDACs[ 4] = atoi(xml->GetNodeContent(child));
    else if (cn=="hist")        fDACs[ 5] = atoi(xml->GetNodeContent(child));
    else if (cn=="thl")         fDACs[ 6] = atoi(xml->GetNodeContent(child));
    else if (cn=="thlcoarse")   fDACs[ 7] = atoi(xml->GetNodeContent(child));
    else if (cn=="vcas")        fDACs[ 8] = atoi(xml->GetNodeContent(child));
    else if (cn=="fbk")         fDACs[ 9] = atoi(xml->GetNodeContent(child));
    else if (cn=="gnd")         fDACs[10] = atoi(xml->GetNodeContent(child));
    else if (cn=="ths")         fDACs[11] = atoi(xml->GetNodeContent(child));
    else if (cn=="biaslvds")    fDACs[12] = atoi(xml->GetNodeContent(child));
    else if (cn=="reflvds")     fDACs[13] = atoi(xml->GetNodeContent(child));
    //
    else if (cn=="mpxclock")   fMpx_clock     = atof(xml->GetNodeContent(child));
    else if (cn=="tpxclock")   fTimepix_clock = atof(xml->GetNodeContent(child));
    //else if (cn=="tpxclock") { 
    //  // TODO - handle the byte code conversion properly.
    //  if (xml->GetNodeContent(child)=="0x00") fTimepix_clock = 0x00;
    //}
    else if (cn=="bsactive") fBS_active = TString(xml->GetNodeContent(child))=="1";
    //
    else if (cn=="chipboardid")   fChipboardID = xml->GetNodeContent(child);
    else if (cn=="customname")    fCustom_name = xml->GetNodeContent(child);
    else if (cn=="firmware")      fFirmware    = xml->GetNodeContent(child);
    else if (cn=="interface")     fInterface   = xml->GetNodeContent(child);
    else if (cn=="mpxtype")    fMpx_type = atoi(xml->GetNodeContent(child));
    else if (cn=="appfilters") fApplied_filters = xml->GetNodeContent(child);
    //
    else if (cn=="detx")   m_x       = atof(xml->GetNodeContent(child));
    else if (cn=="dety")   m_y       = atof(xml->GetNodeContent(child));
    else if (cn=="detz")   m_z       = atof(xml->GetNodeContent(child));
    else if (cn=="eulera") m_euler_a = atof(xml->GetNodeContent(child));
    else if (cn=="eulerb") m_euler_b = atof(xml->GetNodeContent(child));
    else if (cn=="eulerc") m_euler_c = atof(xml->GetNodeContent(child));
    //
    else if (cn=="sourceid") fSourceId = xml->GetNodeContent(child);
    
    child = xml->GetNext(child);
  }
   
  // Release memory before exit
  xml->FreeDoc(xmldoc);
  delete xml;

  return true;

}//end of ParseXmlFile


//
// MoEDALMetadata::PrintContents
//
void MoEDALMetadata::PrintContents() {
  cout
    << endl
    << "* Metadata from the XML file                 "                 << endl
    << "*============================================================" << endl
    << "* Payload information"                                         << endl
    << "*------------------------------------------------------------" << endl
    << "*--> Frame width (fWidth):                   " << fWidth       << endl
    << "*--> Frame height (fHeight):                 " << fHeight      << endl
    << "*--> Dataset ID (fMPXDataSetNumber):         "
    <<       fMPXDataSetNumber << endl
    << "*"                                                             << endl
    << "* Acquisition information" << endl
    << "*------------------------------------------------------------" << endl
    << "*--> Acquisition mode:                       " << fAcq_mode    << endl
    << "*--> Counters:                               [ ";
  vector<Int_t>::iterator cit = fCounters.begin();
  for (;cit!=fCounters.end();++cit) {cout << *cit << " ";}
  cout
    << "]"                                                             << endl
    //<< "*----> [ trig. valid coinc. invalid coinc. ]"                  << endl
    << "*--> Hardware timer mode:                    " << fHw_timer    << endl
    << "*--> Auto erase interval:                    " 
      << Utils::MyWid(fAuto_erase_interval, 8, 6)                      << endl
    << "*--> Auto erase interval counter:            " 
      << fAuto_erase_interval_counter                                  << endl
    << "*--> Last trigger time:                      " 
      << Utils::MyWid(fTrigger_time, 8, 6)                             << endl
    << "*--> Coincidence mode:                       " 
      << Utils::MyHex(UChar_t(fCoincidence_mode))                      << endl
    << "*--> Coincidence delay:                      " 
      << Utils::MyHex(UChar_t(fCoincidence_delay))                     << endl
    << "*--> Coincidence live time:                  " 
      << Utils::MyWid(fCoinc_live_time, 8, 6)                          << endl
    << "*--> Pixelman version:                       "
      << fPixelman_version << endl
    << "*"                                                             << endl
    << "* Detector settings"    << endl
    << "*------------------------------------------------------------" << endl
    << "* Polarity:                                  " << fPolarity    << endl
    << "*--> Bias voltage (HV):                      " 
      << Utils::MyWid(fHV,4,1)                                         << endl
    << "*--> Device DACs:"                                             << endl
    << "*-----> IKrum:                             "<<setw(5)<<fDACs[ 0]<<endl
    << "*-----> Disc:                              "<<setw(5)<<fDACs[ 1]<<endl
    << "*-----> Preamp:                            "<<setw(5)<<fDACs[ 2]<<endl
    << "*-----> BuffAnalogA:                       "<<setw(5)<<fDACs[ 3]<<endl
    << "*-----> BuffAnalogB:                       "<<setw(5)<<fDACs[ 4]<<endl
    << "*-----> Hist:                              "<<setw(5)<<fDACs[ 5]<<endl
    << "*-----> THL:                               "<<setw(5)<<fDACs[ 6]<<endl
    << "*-----> THLCoarse:                         "<<setw(5)<<fDACs[ 7]<<endl
    << "*-----> Vcas:                              "<<setw(5)<<fDACs[ 8]<<endl
    << "*-----> FBK:                               "<<setw(5)<<fDACs[ 9]<<endl
    << "*-----> GND:                               "<<setw(5)<<fDACs[10]<<endl
    << "*-----> THS:                               "<<setw(5)<<fDACs[11]<<endl
    << "*-----> BiasLVDS:                          "<<setw(5)<<fDACs[12]<<endl
    << "*-----> RefLVDS:                           "<<setw(5)<<fDACs[13]<<endl
  //cit = fDACs.begin();
  //for (;cit!=fDACs.end();++cit) {cout << *cit << " ";}
  //cout
    << "*--> Medipix clock:                          "
      << Utils::MyWid(fMpx_clock,4,1)                                  << endl
    << "*--> Timepix clock:                          "
      << Utils::MyHex(UChar_t(fTimepix_clock))                         << endl
    << "*--> Back side preamp. enabled:              " << fBS_active   << endl
    << "*"                                                             << endl
    << "* Detector information" << endl
    << "*------------------------------------------------------------" << endl
    << "*--> Chipboard ID (fChipboardID):            " << fChipboardID << endl
    << "*--> Custom name (fCustom_name):             " << fCustom_name << endl
    << "*--> Firmware:                               " << fFirmware    << endl
    << "*--> Interface type:                         " << fInterface   << endl
    << "*--> Medipix type (1:2.1; 2:MXR; 3:TPX):     " << fMpx_type    << endl
    << "*--> Applied filters:                        ";
  if (fApplied_filters.Length()>0) cout << fApplied_filters << endl;
  else cout << "[None]" << endl;
  cout
    << "*--> Chip surface centre:  ("
      << Utils::MyWid(m_x,8,2) << ", "
      << Utils::MyWid(m_y,8,2) << ", "
      << Utils::MyWid(m_z,8,2) << ") [mm]"                             << endl
    << "*--> Detector rotation:    ("
      << Utils::MyWid(m_euler_a,8,2) << ", "
      << Utils::MyWid(m_euler_b,8,2) << ", "
      << Utils::MyWid(m_euler_c,8,2) << ") [deg.]"                     << endl
    << "*"                                                             << endl
    << "* Geospatial information" << endl
    << "*------------------------------------------------------------" << endl
    << "* Latitude:                                  " 
      << Utils::MyWid(fLatitude,9,6)  << " [deg.]"                     << endl
    << "* Longitude:                                 " 
      << Utils::MyWid(fLongitude,9,6) << " [deg.]"                     << endl
    << "* Altitude:                                " 
      << Utils::MyWid(fAltitude,9,4) << "   [km]"                      << endl
    << "*--> Lab rotational velocity:        ("
      << Utils::MyWid(m_omega_x,8,2) << ", "
      << Utils::MyWid(m_omega_y,8,2) << ", "
      << Utils::MyWid(m_omega_z,8,2) << ") [deg./s]"                   << endl
    << "*--> Lab roll, pitch, yaw:           ("
      << Utils::MyWid(m_roll_orf, 8,2) << ", "
      << Utils::MyWid(m_pitch_orf,8,2) << ", "
      << Utils::MyWid(m_yaw_orf,  8,2) << ") [deg.]"                   << endl
    << "*"                                                             << endl
    << "* Temporal information" << endl
    << "*------------------------------------------------------------" << endl
    << "* (Provided on a frame-by-frame basis - none required.)"       << endl
    << "*"                                                             << endl
    << "* Source information"                                          << endl
    << "*------------------------------------------------------------" << endl
    << "* Source ID: " << fSourceId                                    << endl
    << endl;

}// end of MoEDALMetadata::PrintContents method.
