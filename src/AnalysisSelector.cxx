#include <HAL/AnalysisSelector.h>
#include <iostream>
#include <TObjString.h>
#include <TTree.h>
#include <TList.h>
#include <TMap.h>
#include <TFile.h>
#include <HAL/Algorithm.h>
#include <HAL/AnalysisData.h>
#include <HAL/AnalysisTreeReader.h>
#include <HAL/AnalysisTreeWriter.h>

ClassImp(HAL::AnalysisSelector);

namespace HAL {

//______________________________________________________________________________
AnalysisSelector::AnalysisSelector (Algorithm *af, TTree*) : 
  fMessagePeriod(0), fAnalysisFlow(af), fChain(nullptr)  
{
  fInput = new TList();
}

//______________________________________________________________________________
AnalysisSelector::~AnalysisSelector () 
{
  fInput->Delete("slow"); 
  delete fInput; 
}

//______________________________________________________________________________
Int_t AnalysisSelector::GetEntry (Long64_t entry, Int_t getall) 
{ 
  return (fChain != nullptr) ? fChain->GetTree()->GetEntry(entry, getall) : 0; 
}

//______________________________________________________________________________
void AnalysisSelector::Init (TTree *tree) 
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).

  if (!tree) return;

  static_cast<AnalysisTreeReader*>(fInput->FindObject("RawData"))->SetTree(tree);
  static_cast<AnalysisTreeReader*>(fInput->FindObject("RawData"))->Init();

  fAnalysisFlow->InitializeAlgo(GetOption());
}

//______________________________________________________________________________
Bool_t AnalysisSelector::Notify () 
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. 
  // The return value is currently not used.

  if (fChain->GetCurrentFile()) {
    std::cout << "\n\nProcessing file: " << fChain->GetCurrentFile()->GetName() 
              << std::endl;
    fAnalysisFlow->NotifyAlgo(GetOption());
    static_cast<AnalysisTreeReader*>(fInput->FindObject("RawData"))->Notify();
  }
  if (fMessagePeriod != 0)
    std::cout << std::endl;
  return kTRUE;
}

//______________________________________________________________________________
void AnalysisSelector::Begin (TTree * /*tree*/) 
{
  // The Begin() function is called at the start of the query.
  // When running with PROOF Begin() is only called on the client.
  // The tree argument is deprecated (on PROOF 0 is passed).

  fAnalysisFlow->BeginAlgo(GetOption());
}

//______________________________________________________________________________
void AnalysisSelector::SlaveBegin (TTree * /*tree*/) 
{
  // The SlaveBegin() function is called after the Begin() function.
  // When running with PROOF SlaveBegin() is called on each slave server.
  // The tree argument is deprecated (on PROOF 0 is passed).

  TString option = GetOption();

  fAnalysisFlow->AssignDataList(fInput);
  fAnalysisFlow->SetOutputFileName(fOutputFileName);

  AnalysisTreeReader *atr = new AnalysisTreeReader();
  atr->SetBranchMap(fBranchMap);

  AnalysisData *ad = new AnalysisData();

  AnalysisTreeWriter *atw = new AnalysisTreeWriter(fOutputFileName);
  atw->SetTreeName(fOutputTreeName);
  atw->SetTreeDescription(fOutputTreeDescription);

  fAnalysisFlow->AddData("RawData", atr);
  fAnalysisFlow->AddData("UserData", ad);
  fAnalysisFlow->AddData("UserOutput", atw);

  fAnalysisFlow->SlaveBeginAlgo(GetOption());
}

//______________________________________________________________________________
Bool_t AnalysisSelector::Process (Long64_t entry) 
{
  // The Process() function is called for each entry in the tree (or possibly
  // keyed object in the case of PROOF) to be processed. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  // It can be passed to either AnalysisSelector::GetEntry() or TBranch::GetEntry()
  // to read either all or the required parts of the data. When processing
  // keyed objects with PROOF, the object is already loaded and is available
  // via the fObject pointer.
  //
  // This function should contain the "body" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.
  //
  // The processing can be stopped by calling Abort().
  //
  // Use fStatus to set the return value of TTree::Process().
  //
  // The return value is currently not used.

  if (fMessagePeriod != 0 && (entry + 1) % fMessagePeriod == 0) {
    std::cout.flush();
    std::cout << "\r" << "Processing event: " << entry + 1;
  }

  static_cast<AnalysisTreeReader*>(fInput->FindObject("RawData"))->SetEntry(entry);
  static_cast<AnalysisTreeWriter*>(fInput->FindObject("UserOutput"))->IncrementCount();

  // Execute (and then implicitly clean) all algorithms
  fAnalysisFlow->ExecuteAlgo(GetOption());

  return kTRUE;
}

//______________________________________________________________________________
void AnalysisSelector::SlaveTerminate () 
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

  // Delete user data
  fAnalysisFlow->DeleteData("UserData");
  // Delete raw data
  fAnalysisFlow->DeleteData("RawData");

  fAnalysisFlow->SlaveTerminateAlgo(GetOption());
  static_cast<AnalysisTreeWriter*>(fAnalysisFlow->GetData("UserOutput"))->WriteData();
}

//______________________________________________________________________________
void AnalysisSelector::Terminate () 
{
  // The Terminate() function is the last function to be called during
  // a query. It always runs on the client, it can be used to present
  // the results graphically or save the results to file.

  fAnalysisFlow->TerminateAlgo(GetOption());
  if (fMessagePeriod != 0)
    std::cout << std::endl << std::endl;
}

} /* HAL */ 
