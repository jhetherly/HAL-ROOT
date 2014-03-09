#include <TROOT.h>
#include <TSelector.h>
#include <TTree.h>
#include <TMap.h>
#include <TObjString.h>
#include <vector>
#include <HAL/Common.h>
#include <HAL/Algorithm.h>
#include <HAL/AnalysisTreeReader.h>

#ifndef HAL_ANALYSIS_SELECTOR
#define HAL_ANALYSIS_SELECTOR

namespace HAL {

class AnalysisSelector : public TSelector {
public :
  AnalysisSelector (Algorithm *af, TTree * /*tree*/ =0) : fAnalysisFlow(af), fChain(0)  { fInput = new TList(); }
  virtual ~AnalysisSelector () { /*fInput->Delete("slow"); delete fInput;*/ }
  virtual Int_t   Version () const { return 2; }
  virtual void    Begin (TTree *tree);
  virtual void    SlaveBegin (TTree *tree);
  virtual void    Init (TTree *tree);
  virtual Bool_t  Notify ();
  virtual Bool_t  Process (Long64_t entry);
  virtual Int_t   GetEntry (Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
  virtual void    SetOption (const char *option) { fOption = option; }
  virtual void    SetObject (TObject *obj) { fObject = obj; }
  virtual void    SetInputList (TList *input) { fInput = input; }
  virtual TList  *GetOutputList () const { return fOutput; }
  virtual void    SlaveTerminate ();
  virtual void    Terminate ();

  Algorithm      *fAnalysisFlow;
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  TMap           *fBranchMap;

  ClassDef(AnalysisSelector, 0);
};

} /* HAL */ 

#endif

