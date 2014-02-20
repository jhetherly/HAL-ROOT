#include <TChain.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TString.h>
#include "Algorithm.h"
#include "AnalysisSelector.h"

#ifndef HAL_ANALYSIS
#define HAL_ANALYSIS

namespace HAL
{

class Analysis {
public:
  Analysis (TString name = "", TString title = "", TString treeName = "");
  ~Analysis ();
  void AddRecoAlgo (Algorithm *r); 
  void AddCutAlgo (Algorithm *c);
  void PrintAnalysisFlow ();
  void SetTreeObjectName (TString name);
  void SetAnalysisTitle (TString title);
  Int_t AddFiles (TString fnames, Long64_t nentries = 1234567890);
  Int_t AddFiles (TChain *fchain);
  void PrintTree (Option_t *option = "");
  const char* GetLeafType (TString leafname);
  const char* GetLeafType (TString branchname, TString leafname);
  Long64_t Process (Option_t* option = "", Long64_t nentries = 1234567890, Long64_t firstentry = 0);

private:
  TChain            fChain;
  AnalysisSelector  fAnalizer;
  Algorithm         fAnalysisFlow;
};

} /* HAL */ 

#endif
