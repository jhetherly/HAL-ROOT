#ifndef HAL_ALGORITHM
#define HAL_ALGORITHM

#include <TString.h>
#include <TNamed.h>
#include <TObject.h>
#include <TList.h>
#include <list>
#include <iostream>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>

class AnalysisData;

namespace HAL
{

class Algorithm {
public:
  // (Almost) carbon copy from the TTask class
  // (Since user doesn't inherit from TObject, user's shared 
  //  lib could be more easily imported to python using
  //  rootcint)
  
  // Constructor(s)/Destructor -----------------
  Algorithm(TString name = "", TString title = "");
  Algorithm (const Algorithm &algo);
  virtual ~Algorithm();
  // -------------------------------------------

  // Setup -------------------------------------
  void          Add (Algorithm *algo) { fAlgorithms.push_back(algo); }
  void          ls ();
  void          DeleteAlgos ();
  void          SetName (TString name) {fName = name;}
  void          SetTitle (TString title) {fTitle = title;}
  // -------------------------------------------
  
  // Flow control ------------------------------
  void          Abort ();
  void          CleanAlgos ();
  void          ExecuteAlgo (Option_t *option = "0");
  void          ExecuteAlgos (Option_t *option);
  void          InitializeAlgo (Option_t *option);
  // -------------------------------------------
  
  // DataList related --------------------------
  void          AddData (TString name, TObject *obj);
  TObject*      GetData (TString name);
  Bool_t        CheckData (TString name);
  void          DeleteData (TString name);
  void          AssignDataList (TList *list); 
  // -------------------------------------------
  
  // Data related ------------------------------
  TString       GetName () {return fName;}
  TString       GetTitle () {return fTitle;}
  // -------------------------------------------

protected:
  // User must overide these -------------------
  virtual void  Init (Option_t * /*options*/ = "") {}
  virtual void  Exec (Option_t * /*options*/ = "") {}
  virtual void  Clear (Option_t * /*options*/ = "") {}
  // -------------------------------------------

  TList     *fDataList; // borrowed ptr

private:
  void       ls(TString indention);

  TString    fName, fTitle;

  // Carbon copy from the TTask class
  std::list<Algorithm*> fAlgorithms;
  TString               fOption;       //Option specified in ExecuteAlgo
  Bool_t                fHasExecuted;  //True if algo has executed
  Bool_t                fAbort;        //True if algo has signaled an abort

  static Algorithm *fgBeginAlgo;    //pointer to algo initiator
  static Algorithm *fgBreakPoint;   //pointer to current break point

  ClassDef(Algorithm, 0);
};

} /* HAL */ 

#endif
