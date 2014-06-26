/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 */

#ifndef HAL_AnalysisSelector
#define HAL_AnalysisSelector

#include <vector>
#include <TString.h>
#include <TSelector.h>
#include <HAL/Common.h>

// forward declaration(s)
class TObject;
class TList;
class TMap;
class TTree;

namespace HAL
{
class Algorithm;
}
// end forward declaration(s)

namespace HAL {

class AnalysisSelector : public TSelector {

private:
  unsigned        fMessagePeriod;
  TString         fOutputFileName, 
                  fOutputTreeName, 
                  fOutputTreeDescription;
  Algorithm      *fAnalysisFlow;
  TTree          *fChain;                   //pointer to the analyzed TTree or TChain
  TMap           *fBranchMap;

public:
  AnalysisSelector (Algorithm *af, TTree * /*tree*/ = nullptr);
  virtual ~AnalysisSelector ();
  virtual Int_t   Version () const { return 2; }
  virtual void    Begin (TTree *tree);
  virtual void    SlaveBegin (TTree *tree);
  virtual void    Init (TTree *tree);
  virtual Bool_t  Notify ();
  virtual Bool_t  Process (Long64_t entry);
  virtual Int_t   GetEntry (Long64_t entry, Int_t getall = 0);
  virtual void    SetOption (const char *option) { fOption = option; }
  virtual void    SetObject (TObject *object) { fObject = object; }
  virtual void    SetInputList (TList *input) { fInput = input; }
  virtual TList*  GetOutputList () const { return fOutput; }
  virtual void    SlaveTerminate ();
  virtual void    Terminate ();
  void            SetTree (TTree *tree) {fChain = tree;}
  void            SetBranchMap (TMap *map) {fBranchMap = map;}
  void            SetOutputFileName (TString fname) {fOutputFileName = fname;}
  void            SetOutputTreeName (TString tname) {fOutputTreeName = tname;}
  void            SetOutputTreeDescription (TString tdescription) {fOutputTreeDescription = tdescription;}
  void            SetMessagePeriod (unsigned p = 0) {fMessagePeriod = p;}

  ClassDef(AnalysisSelector, 0);
};

} /* HAL */ 

#endif

