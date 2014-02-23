#include <TString.h>
#include <TNamed.h>
#include <TObject.h>
#include <TList.h>
#include <list>
#include <iostream>

#ifndef HAL_ALGORITHM
#define HAL_ALGORITHM

class AnalysisData;

namespace HAL
{

class Algorithm {
public:
  // Carbon copy from the TTask class
  // (Since user doesn't inherit from TObject, user's shared 
  //  lib could be more easily imported to python using
  //  rootcint)
  
  // Constructor(s)/Destructor -----------------
  Algorithm(TString name = "", TString title = "");
  Algorithm (const Algorithm &algo);
  virtual ~Algorithm();
  // -------------------------------------------

  // User must overide these -------------------
  virtual void  Exec (Option_t * /*options*/ = "") {}
  virtual void  Clear (Option_t * /*options*/ = "") {}
  // -------------------------------------------
  
  // Setup -------------------------------------
  virtual void  Add (Algorithm *algo) { fAlgorithms.push_back(algo); }
  virtual void  ls ();
  void          SetActive (Bool_t active=kTRUE) { fActive = active; }
  void          SetBreakin (Int_t breakin=1) { fBreakin = breakin; }
  void          SetBreakout (Int_t breakout=1) { fBreakout = breakout; }
  void          DeleteAlgos ();
  void          SetName (TString name) {fName = name;}
  void          SetTitle (TString title) {fTitle = title;}
  // -------------------------------------------
  
  // Flow control ------------------------------
  virtual void  Abort ();
  virtual void  Continue();
  virtual void  CleanAlgos ();
  virtual void  ExecuteAlgo (Option_t *option = "0");
  virtual void  ExecuteAlgos (Option_t *option);
  Int_t         GetBreakin () const { return fBreakin; }
  Int_t         GetBreakout () const { return fBreakout; }
  Bool_t        IsActive () const { return fActive; }
  // -------------------------------------------
  
  // DataList related --------------------------
  void          AddData (TString name, TObject *obj);
  TObject*      GetData (TString name);
  Bool_t        CheckData (TString name);
  void          AssignDataList (TList *list); 
  // -------------------------------------------

protected:
  TList     *fDataList; // borrowed ptr

private:
  void       ls(TString indention);

  TString    fName, fTitle;

  // Carbon copy from the TTask class
  std::list<Algorithm*> fAlgorithms;
  TString               fOption;       //Option specified in ExecuteAlgo
  Int_t                 fBreakin;      //=1 if a break point set at task extry
  Int_t                 fBreakout;     //=1 if a break point set at task exit
  Bool_t                fHasExecuted;  //True if algo has executed
  Bool_t                fActive;       //true if algo is active

  static Algorithm *fgBeginAlgo;    //pointer to algo initiator
  static Algorithm *fgBreakPoint;   //pointer to current break point

};

//class Algorithm : public TTask {
//public:
//  Algorithm(const char* name, const char* title) : TTask(name, title), fData(0) {}
//  virtual ~Algorithm() {}
//  //virtual void Exec (Option_t *option) {}
//  //virtual void Clear (Option_t *option) {}
//  void AddData (TString name, TObject *obj);
//  TObject* GetData (TString name);
//  Bool_t CheckData (TString name);
//  void AssignDataMap (TMap *map);
//
//private:
//  TMap *fData;
//
//  ClassDef(Algorithm, 0);
//};

} /* HAL */ 

#endif
