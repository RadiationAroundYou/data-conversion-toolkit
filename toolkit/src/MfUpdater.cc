/// @file MfUpdater.cc
/// @brief Implementation of the Mafalda-format metadata updater class.

#include "MfUpdater.h"

//
// MfUpdater constructor
//
MfUpdater::MfUpdater(
    TString datasetpath,
    TString datasetmetadata,
    Bool_t dbg
)
{

  // Get the info from the XML file
  //--------------------------------

  // Instantiate the data container.
  m_pAddMetadata = new AddMetadata(datasetmetadata);
  if (dbg) {
    cout
      << "DEBUG: Created AddMetadata object with XML file '"
      << m_pAddMetadata->GetXmlPath() << endl;
    m_pAddMetadata->PrintContents();
  }//end of dbg check.

  // Create the first ROOT ntuple file.
  m_pNt = new TFile(datasetpath, "UPDATE");

  // TODO Handle non-existence of the file...

  // Get the old TTree from the ROOT file.
  m_pTr = (TTree*)m_pNt->Get("MPXTree");

  // Create the ROOT TTree for the updated version.
  m_pTrNew = new TTree("MPXTree","Medi/TimePix data");

  // Hang on, shouldn't this be retrieved from the ROOT file?
  // And then checked against the value supplied in the metadata?

  // Create the frame container.
  //m_pFrame = new FrameStruct(m_pAddMetadata->GetMPXDataSetNumber());
  m_pFrame = new FrameStruct();

  // Get the branch for the frame information and set the address.
  m_pBr = m_pTr->GetBranch("FramesData");
  m_pBr->SetAddress(&m_pFrame);

  // Create the branch for the new, updated TTree.
  m_pTrNew->Branch("FramesData", "FrameStruct", &m_pFrame, 128000, 2);

  // Get the number of frames in the file.
  Int_t nframes = m_pTr->GetEntries();

  // If the data is from the ISS, we need to retrieve historical geospatial
  // information from the timestamp. We use curlpp to retrieve the data from
  // a web-based API and extract the information from the retrieved JSON file.

  curlpp::Cleanup myCleanup; //!< For clean-up.
  curlpp::Easy myRequest;    //!< For the HTTP request.
  stringstream myUrl;        //!< String stream for the URL (inc. timestamp).
  stringstream geodata;      //!< String stream for the retrieved JSON.
  Document jsondoc;          //!< For parsing the retrived geospatial data.
  

  // Loop over the frames.
  for (Int_t i=0; i<nframes; i++) {

    // Load the TTree.
    m_pTr->LoadTree(i);

    // Load the event.
    m_pTr->GetEvent(i);

    // Payload

    // Is this Monte Carlo data?
    m_pAddMetadata->IsMCData() ? m_pFrame->SetFrameAsMCData() : m_pFrame->SetFrameAsData();

    // Payload information
    //---------------------
    // Set the dataset ID to that in the XML file.
    m_pFrame->SetDataSet(m_pAddMetadata->GetMPXDataSetNumber());
    //cout << "* New dataset ID = " << m_pFrame->GetDataSet() << endl;
    m_pFrame->SetId(i);

    // Occupancy
    m_pFrame->UpdateOccupancy();

    // Geospatial information
    //------------------------

    // If the data is from the ISS, retrieve the latitude, longitude, and
    // and altitude for the frame from the ISS API.
    if (m_pAddMetadata->GetSourceId()=="ISS") {

      // Get the start time of the frame (minus the fractional part).
      Int_t starttime = m_pFrame->GetStartTime();

      // The URL of the API request.
      myUrl << "http://jimanning.com/issapi/?unixts=" << starttime;

      // Set the HTTP request options.
      myRequest.setOpt(new curlpp::options::Url(myUrl.str()));
      myRequest.setOpt(new curlpp::options::WriteStream(&geodata));

      // Perform the request.
      myRequest.perform();

      Bool_t jsonerror = jsondoc.Parse<0>(geodata.str().c_str()).HasParseError();

      // Set the geospatial information from the retrieved JSON.
      m_pFrame->SetLatitude(jsondoc["position"]["lat"].GetDouble());
      m_pFrame->SetLongitude(jsondoc["position"]["lon"].GetDouble());
      m_pFrame->SetAltitude(1000. * jsondoc["position"]["alt"].GetDouble());

      // Write out the JSON (debugging).
      //FileStream f(stdout);
      //PrettyWriter<FileStream> writer(f);
      //jsondoc.Accept(writer);

      // Reset the string streams.
      myUrl.str("");
      geodata.str("");

    } else {
      m_pFrame->SetLatitude( m_pAddMetadata->GetLatitude() );
      m_pFrame->SetLongitude(m_pAddMetadata->GetLongitude());
      //
      m_pFrame->SetAltitude(m_pAddMetadata->GetAltitude());
    }
    //
    m_pFrame->SetRoll( m_pAddMetadata->GetRoll());
    m_pFrame->SetPitch(m_pAddMetadata->GetPitch());
    m_pFrame->SetYaw(  m_pAddMetadata->GetYaw());
    //
    m_pFrame->SetOmega_x(m_pAddMetadata->GetOmega_x());
    m_pFrame->SetOmega_y(m_pAddMetadata->GetOmega_y());
    m_pFrame->SetOmega_z(m_pAddMetadata->GetOmega_z());

    // Detector information
    //----------------------
    m_pFrame->SetCustomName(m_pAddMetadata->GetCustomName());
    m_pFrame->SetAppFilters(m_pAddMetadata->GetAppFilters());
    //
    m_pFrame->SetDet_x(m_pAddMetadata->GetDet_x()  );
    m_pFrame->SetDet_y(m_pAddMetadata->GetDet_y()  );
    m_pFrame->SetDet_z(m_pAddMetadata->GetDet_z()  );
    //
    m_pFrame->SetEulerA(m_pAddMetadata->GetEulerA());
    m_pFrame->SetEulerB(m_pAddMetadata->GetEulerB());
    m_pFrame->SetEulerC(m_pAddMetadata->GetEulerC());

    // Source information
    //--------------------
    m_pFrame->SetSourceId(m_pAddMetadata->GetSourceId());

    //break;

    // Fill the branch on the new tree with the new information.
    m_pTrNew->Fill();
  }//end of loop over the frames.

}//end of MfUpdater constructor.

//
// MfUpdater destructor
//
MfUpdater::~MfUpdater() {

  // Tidy up.

  // Delete the pointer to the metadata wrapper.
  if (m_pAddMetadata) delete m_pAddMetadata;

}//end of the MfUpdater destructor.

//
// closeNtuple
//
void MfUpdater::closeNtuple() {

  // Write the TTree contents.
  m_pTr->Delete("all");

  m_pTrNew->Write();

  // Close the ntuple file.
  m_pNt->Close();

}//end of closeNtuple method.
