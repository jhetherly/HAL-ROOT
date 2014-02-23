#include <HAL/Analysis.h>

namespace HAL
{

Analysis::Analysis (TString name, TString title, TString treeName) : 
  fChain(new TChain()), fAnalysisFlow(new Algorithm(name.Data(), title.Data())), 
  fAnalizer(new AnalysisSelector(fAnalysisFlow)) {
  fChain->SetName(treeName.Data());
  fAnalizer->fChain = fChain;
}

Analysis::~Analysis () {
  fAnalysisFlow->DeleteAlgos();
  delete fAnalysisFlow;
  delete fAnalizer;
  delete fChain;
}

void Analysis::AddRecoAlgo (Algorithm *r) {
  fAnalysisFlow->Add(r);
}

void Analysis::AddCutAlgo (Algorithm *c) {
  fAnalysisFlow->Add(c);
}

void Analysis::PrintAnalysisFlow () {
  fAnalysisFlow->ls();
}

void Analysis::SetTreeObjectName (TString name) {
  fChain->SetName(name.Data());
}

void Analysis::SetAnalysisName (TString name) {
  fAnalysisFlow->SetName(name.Data());
}

void Analysis::SetAnalysisTitle (TString title) {
  fAnalysisFlow->SetTitle(title.Data());
}

Int_t Analysis::AddFiles (TString fnames, Long64_t nentries) {
  return fChain->Add(fnames.Data(), nentries);
}

Int_t Analysis::AddFiles (TChain *fchain) {
  return fChain->Add(fchain);
}

void Analysis::PrintTree (Option_t *option) {
  fChain->Print(option);
}

const char* Analysis::GetLeafType (TString leafname) {
  return fChain->GetLeaf(leafname.Data())->GetTypeName();
}

const char* Analysis::GetLeafType (TString branchname, TString leafname) {
  return fChain->GetLeaf(branchname.Data(), leafname.Data())->GetTypeName();
}

Long64_t Analysis::Process (Option_t* option, 
    Long64_t nentries, Long64_t firstentry) {
  PrintAnalysisFlow();
  return fChain->Process(fAnalizer, option, nentries, firstentry);
}

} /* HAL */ 

