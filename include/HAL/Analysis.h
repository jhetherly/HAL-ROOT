#ifndef HAL_ANALYSIS
#define HAL_ANALYSIS

#include <TChain.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TString.h>
#include <TObjString.h>
#include <TMap.h>
#include <HAL/Common.h>
#include <HAL/Algorithm.h>
#include <HAL/AnalysisSelector.h>

namespace HAL
{

class Analysis {
public:
  Analysis (TString name = "", TString title = "", TString treeName = "");
  ~Analysis ();
  void          AddAlgo (Algorithm *a); 
  void          PrintAnalysisFlow ();
  void          PrintCounterSummary () {fAnalysisFlow->CounterSummary();}
  void          PrintCutReport () {fAnalysisFlow->CutReport();}
  void          SetTreeObjectName (TString name);
  void          SetAnalysisName (TString name);
  void          SetAnalysisTitle (TString title);
  Int_t         AddFiles (TString fnames, Long64_t nentries = 1234567890);
  Int_t         AddFiles (TChain *fchain);
  void          SetOutputFileName (TString fname) {fAnalizer->SetOutputFileName(fname);}
  void          SetOutputTreeName (TString tname) {fAnalizer->SetOutputTreeName(tname);}
  void          SetOutputTreeDescription (TString tdescription) {fAnalizer->SetOutputTreeDescription(tdescription);}
  void          SetMessagePeriod (unsigned p = 0) {fAnalizer->SetMessagePeriod(p);}
  void          PrintTree (Option_t *option = "");
  const char*   GetLeafType (TString leafname);
  const char*   GetLeafType (TString branchname, TString leafname);
  void          MapBranch (TString branchname, TString nickname);
  Long64_t      Process (Option_t *option = "", Long64_t nentries = 1234567890, Long64_t firstentry = 0);

private:
  TChain            *fChain;
  Algorithm         *fAnalysisFlow;
  AnalysisSelector  *fAnalizer;
  TMap              *fBranchMap;

  ClassDefNV(Analysis, 0);
};

} /* HAL */ 

#endif

