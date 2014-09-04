/// @file WriteToNtuple.cc
/// @brief Implementation of the ntuple writing class.

// Local include statements.
#include "WriteToNtuple.h"

//
// WriteToNtuple constructor
//
WriteToNtuple::WriteToNtuple(TString dataSet, TString tempScratchDir){

  m_MPXDataSetNumber = dataSet;
  m_ntupleFileName = "";

  // change dir if requested
  if(tempScratchDir.Length() > 0){
    m_ntupleFileName += tempScratchDir;
    m_ntupleFileName += "/";
  }
  
  //m_ntupleFileName += "MPXNtuple_"+m_MPXDataSetNumber+".root";
  m_ntupleFileName += m_MPXDataSetNumber+"_0000000001.root";
  nt = new TFile(m_ntupleFileName, "RECREATE");
  t2 = new TTree("MPXTree","Medi/TimePix data");

  m_frame = new FrameStruct(m_MPXDataSetNumber);
  t2->Branch("FramesData", "FrameStruct", &m_frame, 128000, 2);
  
}

//
// Destructor
//
WriteToNtuple::~WriteToNtuple() {

  // Delete the TTree.
  if (t2) t2->Delete();

  // Delete the ntuple file.
  if (nt) nt->Delete();

  // Delete the frame container.
  if (m_frame) delete m_frame;

}//end of destructor.

//
// WriteToNtuple::fillVars
//
void WriteToNtuple::fillVars(FramesHandler * frameHandlerObj, bool rewind_metadata) {

  // Get the frame data into the frame container pointer.
  m_frame = frameHandlerObj->getFrameStructObject();

  // Get the current ntuple file.
  nt->cd();

  // Fill the branches of the TTree.
  t2->Fill();

  // Clean up the frame container.
  frameHandlerObj->RewindAll(rewind_metadata);

}//end of fillVars method.

//
// WriteToNtuple::closeNtuple
//
void WriteToNtuple::closeNtuple()
{

  t2->Write();
  nt->Close();

}
