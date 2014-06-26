#include <HAL/Analysis.h>
#include <TChain.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TObjString.h>
#include <TMap.h>
#include <HAL/Algorithm.h>
#include <HAL/AnalysisSelector.h>

ClassImp(HAL::Analysis);

namespace HAL
{

//______________________________________________________________________________
Analysis::Analysis (TString name, TString title, TString treeName) : 
  fChain(new TChain()), fAnalysisFlow(new Algorithm(name.Data(), title.Data())), 
  fAnalizer(new AnalysisSelector(fAnalysisFlow)), fBranchMap(new TMap()) 
{
  fChain->SetName(treeName.Data());
  fAnalizer->SetTree(fChain);
}

//______________________________________________________________________________
Analysis::~Analysis () 
{
  fAnalysisFlow->DeleteAlgos();
  delete fAnalysisFlow;
  delete fAnalizer;
  delete fBranchMap;
}

//______________________________________________________________________________
void Analysis::AddAlgo (Algorithm *a) 
{
  fAnalysisFlow->Add(a);
}

//______________________________________________________________________________
void Analysis::PrintAnalysisFlow () 
{
  fAnalysisFlow->ls();
}

//______________________________________________________________________________
void Analysis::PrintCounterSummary () 
{
  fAnalysisFlow->CounterSummary();
}

//______________________________________________________________________________
void Analysis::PrintCutReport () 
{
  fAnalysisFlow->CutReport();
}

//______________________________________________________________________________
void Analysis::SetTreeObjectName (TString name) 
{
  fChain->SetName(name.Data());
}

//______________________________________________________________________________
void Analysis::SetAnalysisName (TString name) 
{
  fAnalysisFlow->SetName(name.Data());
}

//______________________________________________________________________________
void Analysis::SetAnalysisTitle (TString title) 
{
  fAnalysisFlow->SetTitle(title.Data());
}

//______________________________________________________________________________
Int_t Analysis::AddFiles (TString fnames, Long64_t nentries) 
{
  return fChain->Add(fnames.Data(), nentries);
}

//______________________________________________________________________________
Int_t Analysis::AddFiles (TChain *fchain) 
{
  return fChain->Add(fchain);
}

//______________________________________________________________________________
void Analysis::SetOutputFileName (TString fname) 
{
  fAnalizer->SetOutputFileName(fname);
}

//______________________________________________________________________________
void Analysis::SetOutputTreeName (TString tname) 
{
  fAnalizer->SetOutputTreeName(tname);
}

//______________________________________________________________________________
void Analysis::SetOutputTreeDescription (TString tdescription) 
{
  fAnalizer->SetOutputTreeDescription(tdescription);
}

//______________________________________________________________________________
void Analysis::SetMessagePeriod (unsigned p) 
{
  fAnalizer->SetMessagePeriod(p);
}

//______________________________________________________________________________
void Analysis::PrintTree (Option_t *option) 
{
  fChain->Print(option);
}

//______________________________________________________________________________
TString Analysis::GetLeafType (TString leafname) 
{
  return TString(fChain->GetLeaf(leafname.Data())->GetTypeName());
}

//______________________________________________________________________________
TString Analysis::GetLeafType (TString branchname, TString leafname) 
{
  return TString(fChain->GetLeaf(branchname.Data(), leafname.Data())->GetTypeName());
}

//______________________________________________________________________________
void Analysis::MapBranch (TString branchname, TString nickname) 
{
  fBranchMap->Add(new TObjString(nickname.Data()), new TObjString(branchname.Data()));
}

//______________________________________________________________________________
Long64_t Analysis::Process (Option_t* option, 
                            Long64_t nentries, Long64_t firstentry) 
{
  fAnalizer->SetBranchMap(fBranchMap);
  PrintAnalysisFlow();
  return fChain->Process(fAnalizer, option, nentries, firstentry);
}

} /* HAL */ 

