/// @file AddMetadata.cc
/// @brief Implementation of the additional meta data container class.

#include "AddMetadata.h"

//
// AddMetadata constructor
//
AddMetadata::AddMetadata(
  TString xmlPath
  )
 :
  m_xmlPath(xmlPath),
  fMPXDataSetNumber(""),
  fCustom_name(""), 
  fLatitude(0.0), fLongitude(0.0), fAltitude(0.0),
  m_omega_x(0.0),  m_omega_y(0.0),   m_omega_z(0.0),
  m_roll_orf(0.0), m_pitch_orf(0.0), m_yaw_orf(0.0),
  //
  m_x(0.0), m_y(0.0), m_z(0.0),
  m_euler_a(0.0), m_euler_b(0.0), m_euler_c(0.0),
  //
  fSourceId("")
{

  fApplied_filters = "";

  //
  // Leave the primary vertex (PV) vectors empty - this
  // calibration data is "keepin' it real".
  m_primaryVertex_x.clear();
  m_primaryVertex_y.clear();
  m_primaryVertex_z.clear();

  if (!ParseXmlFile()) { cout<<"ERROR: Invalid XML file."<<endl; exit(1);} 

} //end of constructor.

//
// AddMetadata destructor
//
AddMetadata::~AddMetadata() {}

//
// AddMetadata::ParseXmlFile
//
Bool_t AddMetadata::ParseXmlFile() {

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
    if      (cn=="ismc") m_isMCData = TString(xml->GetNodeContent(child))=="1";
    //
    else if (cn=="lat") fLatitude  = atof(xml->GetNodeContent(child));
    else if (cn=="lon") fLongitude = atof(xml->GetNodeContent(child));
    else if (cn=="alt") fAltitude  = atof(xml->GetNodeContent(child));
    //
    else if (cn=="customname")    fCustom_name = xml->GetNodeContent(child);
    //
    else if (cn=="appfilters") fApplied_filters = xml->GetNodeContent(child);
    //
    else if (cn=="detx") m_x = atof(xml->GetNodeContent(child));
    else if (cn=="dety") m_y = atof(xml->GetNodeContent(child));
    else if (cn=="detz") m_z = atof(xml->GetNodeContent(child));
    else if (cn=="eulera") m_euler_a = atof(xml->GetNodeContent(child));
    else if (cn=="eulerb") m_euler_b = atof(xml->GetNodeContent(child));
    else if (cn=="eulerc") m_euler_c = atof(xml->GetNodeContent(child));
    //
    else if (cn=="omegax") m_omega_x   = atof(xml->GetNodeContent(child));
    else if (cn=="omegay") m_omega_y   = atof(xml->GetNodeContent(child));
    else if (cn=="omegaz") m_omega_z   = atof(xml->GetNodeContent(child));
    else if (cn=="roll")   m_roll_orf  = atof(xml->GetNodeContent(child));
    else if (cn=="pitch")  m_pitch_orf = atof(xml->GetNodeContent(child));
    else if (cn=="yaw")    m_yaw_orf   = atof(xml->GetNodeContent(child));
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
// AddMetadata::PrintContents
//
void AddMetadata::PrintContents() {
  cout
    << endl
    << "* Metadata from the XML file                 "                 << endl
    << "*============================================================" << endl
    << "* Payload"                                                     << endl
    << "*------------------------------------------------------------" << endl
    << "*--> Data type:                              ";
    if (m_isMCData) {cout<<"Monte Carlo";}else{cout<<"Real data";} cout<< endl;
  cout
    << "*"                                                             << endl
    << "* Payload information"                                         << endl
    << "*------------------------------------------------------------" << endl
    << "*--> Dataset ID (fMPXDataSetNumber):         "
    <<       fMPXDataSetNumber << endl
    << "*"                                                             << endl;
  cout
    << "*"                                                             << endl
    << "* Detector information" << endl
    << "*------------------------------------------------------------" << endl
    << "*--> Custom name (fCustom_name):             " << fCustom_name << endl
    << "*--> Applied filters:                        ";
  if (fApplied_filters.Length()>0) cout << fApplied_filters << endl;
  else cout << "[None]" << endl;
  cout
    << "*-->"                                                          << endl
    << "*--> Chip surface centre:  ("
      << Utils::MyWid(m_x,8,2) << ", "
      << Utils::MyWid(m_y,8,2) << ", "
      << Utils::MyWid(m_z,8,2) << ") [mm]"                             << endl
    << "*--> Chip rotation:        ("
      << Utils::MyWid(m_euler_a,8,2) << ", "
      << Utils::MyWid(m_euler_b,8,2) << ", "
      << Utils::MyWid(m_euler_c,8,2) << ") [deg.]"                     << endl
    << "*"                                                             << endl
    << "* Geospatial information" << endl
    << "*------------------------------------------------------------" << endl
    << "* Latitude:                                  " 
      << Utils::MyWid(fLatitude,9,6)                                   << endl
    << "* Longitude:                                 " 
      << Utils::MyWid(fLongitude,9,6)                                  << endl
    << "* Altitude:                                " 
      << Utils::MyWid(fAltitude,9,4) << "   [km]"                      << endl
    << "*--> Lab rotational velocity: ("
      << Utils::MyWid(m_omega_x,8,2) << ", "
      << Utils::MyWid(m_omega_y,8,2) << ", "
      << Utils::MyWid(m_omega_z,8,2) << ") [deg./s]"                   << endl
    << "*--> Lab roll, pitch, yaw:    ("
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
}// end of AddMetadata::PrintContents method.
