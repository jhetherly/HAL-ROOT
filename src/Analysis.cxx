#include <HAL/Analysis.h>

ClassImp(HAL::Analysis);

namespace HAL
{

Analysis::Analysis (TString name, TString title, TString treeName) : 
  fChain(new TChain()), fAnalysisFlow(new Algorithm(name.Data(), title.Data())), 
  fAnalizer(new AnalysisSelector(fAnalysisFlow)), fBranchMap(new TMap()) {
  fChain->SetName(treeName.Data());
  fAnalizer->fChain = fChain;
}

Analysis::~Analysis () {
  fAnalysisFlow->DeleteAlgos();
  delete fAnalysisFlow;
  delete fAnalizer;
  delete fBranchMap;
}

void Analysis::AddAlgo (Algorithm *a) {
  fAnalysisFlow->Add(a);
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

void Analysis::MapBranch (TString branchname, TString nickname) {
  fBranchMap->Add(new TObjString(nickname.Data()), new TObjString(branchname.Data()));
}

Long64_t Analysis::Process (Option_t* option, 
    Long64_t nentries, Long64_t firstentry) {
  fAnalizer->fBranchMap = fBranchMap;
  PrintAnalysisFlow();
  return fChain->Process(fAnalizer, option, nentries, firstentry);
}

} /* HAL */ 

