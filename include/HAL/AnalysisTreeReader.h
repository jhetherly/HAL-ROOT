#ifndef HAL_ANALYSIS_TREE_READER
#define HAL_ANALYSIS_TREE_READER

#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TLeafF.h>
#include <TRegexp.h>
#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TRef.h>
#include <TRefArray.h>
#include <TNamed.h>
#include <TMap.h>
#include <string>
#include <deque>
#include <vector>
#include <set>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>

namespace HAL {

class AnalysisTreeReader : public TNamed {
public:
  AnalysisTreeReader (TTree *tree = 0);
  virtual ~AnalysisTreeReader ();
  void SetTree (TTree *tree) {fChain = tree; fChain->SetMakeClass(1);}
  void SetEntry (Long64_t entry);
  TTree* GetTree () {return fChain;}
  void SetBranchMap (TMap *m) {fBranchMap = m;}

  Int_t                     GetDim (TString branchname, Int_t idx_1 = -1);
  Bool_t                    GetBool (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  Long64_t                  GetInteger (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  ULong64_t                 GetCounting (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  LongDouble_t              GetDecimal (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  TString                   GetString (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  TObjArray&                GetObjArray (TString branchname, Int_t idx_1 = -1);
  TClonesArray&             GetClonesArray (TString branchname, Int_t idx_1 = -1);
  TRef&                     GetRef (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  TRefArray&                GetRefArray (TString branchname, Int_t idx_1 = -1);

  ClassDef(AnalysisTreeReader, 0);

private:

  TString GetFullBranchName (TString name);

  TTree *fChain;
  Long64_t fEntry;
  enum StorageType {kB, kD, kI, kC, kS, kOA, kCA, kR, kRA,
                    kvB, kvD, kvI, kvC, kvS, kvOA, kvCA, kvR, kvRA,
                    kvvB, kvvD, kvvI, kvvC, kvvS, kvvR};
  // regex's for how to identify scalars, vec's, and arrays
  TRegexp fScalar, fVector, fVector2D, fArray, fArray2D;
  // type lists
  std::set<TString> fBool,
                    fInteger, fShort, fLong, fLLong, fSChar,
                    fUInteger, fUShort, fULong, fULLong, fUChar,
                    fFloat, fDouble, fLDouble,
                    fChar;

  // Container for storing the nicknames for the branches
  TMap *fBranchMap;

  // Storage for basic types
  std::deque<Bool_t>                   fB;  // bool type
  std::deque<LongDouble_t>             fD;  // decimal number  (float, double, etc...)
  std::deque<Long64_t>                 fI;  // integer number  (Byte_t, int, long, short, etc...)
  std::deque<ULong64_t>                fC;  // counting number (unsigned int, unsigned etc...)
  std::deque<TString>                  fS;  // string types    (char, TString, ...)
  std::deque<TObjArray>                fOA; // TObjArray
  std::deque<TClonesArray>             fCA; // TClonesArray
  std::deque<TRef>                     fR;  // TRef
  std::deque<TRefArray>                fRA; // TRefArray
  
  // storage for 1D arrays of data
  std::deque<std::vector<Bool_t> >        fvB;  // bool type
  std::deque<std::vector<LongDouble_t> >  fvD;  // decimal number
  std::deque<std::vector<Long64_t> >      fvI;  // integer number
  std::deque<std::vector<ULong64_t> >     fvC;  // counting number
  std::deque<std::vector<TString> >       fvS;  // string types
  std::deque<std::vector<TObjArray> >     fvOA; // TObjArray
  std::deque<std::vector<TClonesArray> >  fvCA; // TClonesArray
  std::deque<std::vector<TRef> >          fvR;  // TRef
  std::deque<std::vector<TRefArray> >     fvRA; // TRefArray

  // storage for 2D arrays of data
  std::deque<std::vector<std::vector<Bool_t> > >        fvvB;  // bool type
  std::deque<std::vector<std::vector<LongDouble_t> > >  fvvD;  // decimal number
  std::deque<std::vector<std::vector<Long64_t> > >      fvvI;  // integer number
  std::deque<std::vector<std::vector<ULong64_t> > >     fvvC;  // counting number
  std::deque<std::vector<std::vector<TString> > >       fvvS;  // string types
  std::deque<std::vector<std::vector<TRef> > >          fvvR;  // TRef


  // Can only handle scalars (defined in the std::set's above), c-arrays, and vectors
  class BranchManager {
  public:
    BranchManager (AnalysisTreeReader *tr = 0);
    ~BranchManager ();
    TString GetName () {return fBranchName;}
    TString GetFullType () {return fType;}
    TString GetScalarType () {return fScalarType;}
    Bool_t IsScalar () {return fScalar;}
    Bool_t IsCArray1D () {return fCArray1D;}
    Bool_t IsCArray2D () {return fCArray2D;}
    Bool_t IsVec1D () {return fVec1D;}
    Bool_t IsVec2D () {return fVec2D;}
    Bool_t Create (TString branchname);
    void SetEntry (Long64_t entry);
    StorageType GetStorageType () {return fStorageID;}
    Int_t GetStorageIndex () {return fStorageIndex;}

  private:
    StorageType            fStorageID; // this will be the proprietary index of the vector to use in the ATR
    Int_t                  fStorageIndex; // this will be the proprietary index of the vector to use in the ATR
    Bool_t                 fScalar, fCArray1D, fCArray2D, fVec1D, fVec2D;
    TString                fBranchName, fBranchTitle, fLeafTitle, fType, fScalarType;
    Int_t                  fLeafNdata;
    TBranch               *fBranch;
    AnalysisTreeReader    *fTreeReader;

    Int_t                  GetArrayLength (Int_t dim);
    void                   FindTypeInformation ();
  };

  friend class BranchManager;
  std::vector<BranchManager*>         fBranchManagers;

};

} /* HAL */ 

#endif
