/// @file MfFilter.cc
/// @brief Implementation of the Mafalda-format pixel filter class.

#include "MfFilter.h"

//
// MfFilter constructor
//
MfFilter::MfFilter(
    TString datasetpath,
    TString filterpath,
    TString filtername,
    Bool_t dbg
)
{

  // Get the pixel filter info from the filter file
  //------------------------------------------------

  // Level 1 "trigger"?
  map<int,int> filtermap;

  //cout << "DEBUG: reading the filter map." << endl;
  ifstream filterfile(filterpath);

  Int_t x, y, C;

  while (filterfile >> x >> y >> C) {
    //cout << "DEBUG: x y C = " << x << ", " << y << ", " << C << endl;
    filtermap[256 * y + x] = C;
  }

  map<int,int>::iterator fit = filtermap.begin();

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

  // Loop over the frames.
  for (Int_t i=0; i<nframes; i++) {
    //cout << "FRAME " << i << endl;
    m_pTr->LoadTree(i);
    m_pTr->GetEvent(i);

    m_pFrame->SetAppFilters(filtername);

    // Clear the pixel mask.
    m_pFrame->ClearLVL1();

    // Loop over the pixels in the mask.
    for (fit=filtermap.begin(); fit!=filtermap.end(); ++fit) {
      Int_t x = fit->first % 256;
      Int_t y = fit->first / 256;
      //cout << "DEBUG: (x, y) = (" << x << ", " << y << ") => X = " << X << endl;
      m_pFrame->SetLVL1(x, y, m_pFrame->GetFrameWidth(), fit->second);
    }

    //break;

    // Fill the branch on the new tree with the new information.
    m_pTrNew->Fill();
  }//end of loop over the frames.

}//end of MfFilter constructor.

//
// MfFilter destructor
//
MfFilter::~MfFilter() {

  // Tidy up.

}//end of the MfFilter destructor.

//
// closeNtuple
//
void MfFilter::closeNtuple() {

  // Write the TTree contents.
  m_pTr->Delete("all");

  m_pTrNew->Write();

  // Close the ntuple file.
  m_pNt->Close();

}//end of closeNtuple method.
