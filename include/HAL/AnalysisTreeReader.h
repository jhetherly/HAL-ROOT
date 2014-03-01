#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TRegexp.h>
#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TRef.h>
#include <TRefArray.h>
#include <TNamed.h>
#include <TMap.h>
#include <deque>
#include <vector>
#include <iostream>
#include <HAL/Exceptions.h>

#ifndef HAL_ANALYSIS_TREE_READER
#define HAL_ANALYSIS_TREE_READER

namespace HAL {

class AnalysisTreeReader : public TNamed {
public:
  AnalysisTreeReader (TTree *tree = 0);
  virtual ~AnalysisTreeReader ();
  void SetTree (TTree *tree) {fChain = tree;}
  void SetEntry (Long64_t entry);
  TTree* GetTree () {return fChain;}
  void SetBranchMap (TMap *m) {fBranchMap = m;}

  //Int_t                   getDim (TString branchname, Int_t idx_1 = -1);
  //Bool_t                  getBool (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //Byte_t                  getByte (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  Int_t                   GetInt (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //UInt_t                  getUInt (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //Short_t                 getShort (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //UShort_t                getUShort (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //Long64_t                getLong (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //ULong64_t               getULong (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //Float16_t               getFloat (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //Double32_t              getDouble (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //LongDouble_t            getLongDouble (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //Char_t                  getChar (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //TString                 getTString (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //TObject*                getTObjectPtr (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  //void*                   getVoidPtr (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);

  ClassDef(AnalysisTreeReader, 0);

private:

  TString GetFullBranchName (TString name);

  TTree *fChain;
  Long64_t fEntry;
  enum StorageType {kB, kBy, kI};
  // regex's for how to identify scalars, vec's, and arrays
  TRegexp fScalar, fVector, fVector2D, fArray, fArray2D;

  // Container for storing the nicknames for the branches
  TMap *fBranchMap;

  // Storage for basic types
  std::vector<Bool_t>                   fB;
  std::vector<Byte_t>                   fBy; // unsigned char
  std::deque<Int_t>                    fI;
  std::vector<UInt_t>                   fUI;
  std::vector<Short_t>                  fS;
  std::vector<UShort_t>                 fUS;
  std::vector<Long64_t>                 fL64;
  std::vector<ULong64_t>                fUL64;
  std::vector<Float16_t>                fF16;
  std::vector<Double32_t>               fD32;
  std::vector<LongDouble_t>             fLD;
  std::vector<Char_t>                   fC;
  std::vector<TString>                  fTS;
  std::vector<TObject*>                 fTOptr;
  std::vector<TObjArray*>               fTOAptr;
  std::vector<TClonesArray*>            fTCAptr;
  std::vector<TRef*>                    fTRptr;
  std::vector<TRefArray*>               fTRAptr;
  std::vector<void*>                    fvptr;

  // storage for 1D arrays of data
  std::vector<std::vector<Bool_t> >                   fvB;
  std::vector<std::vector<Byte_t> >                   fvBy;
  std::vector<std::vector<Int_t> >                    fvI;
  std::vector<std::vector<UInt_t> >                   fvUI;
  std::vector<std::vector<Short_t> >                  fvS;
  std::vector<std::vector<UShort_t> >                 fvUS;
  std::vector<std::vector<Long64_t> >                 fvL64;
  std::vector<std::vector<ULong64_t> >                fvUL64;
  std::vector<std::vector<Float16_t> >                fvF16;
  std::vector<std::vector<Double32_t> >               fvD32;
  std::vector<std::vector<LongDouble_t> >             fvLD;
  std::vector<std::vector<Char_t> >                   fvC;
  std::vector<std::vector<TString> >                  fvTS;
  std::vector<std::vector<TObject*> >                 fvTOptr;
  std::vector<std::vector<TObjArray*> >               fvTOAptr;
  std::vector<std::vector<TClonesArray*> >            fvTCAptr;
  std::vector<std::vector<TRef*> >                    fvTRptr;
  std::vector<std::vector<TRefArray*> >               fvTRAptr;
  std::vector<std::vector<void*> >                    fvvptr;

  // storage for 2D arrays of data
  std::vector<std::vector<std::vector<Bool_t> > >                   fvvB;
  std::vector<std::vector<std::vector<Byte_t> > >                   fvvBy;
  std::vector<std::vector<std::vector<Int_t> > >                    fvvI;
  std::vector<std::vector<std::vector<UInt_t> > >                   fvvUI;
  std::vector<std::vector<std::vector<Short_t> > >                  fvvS;
  std::vector<std::vector<std::vector<UShort_t> > >                 fvvUS;
  std::vector<std::vector<std::vector<Long64_t> > >                 fvvL64;
  std::vector<std::vector<std::vector<ULong64_t> > >                fvvUL64;
  std::vector<std::vector<std::vector<Float16_t> > >                fvvF16;
  std::vector<std::vector<std::vector<Double32_t> > >               fvvD32;
  std::vector<std::vector<std::vector<LongDouble_t> > >             fvvLD;
  std::vector<std::vector<std::vector<Char_t> > >                   fvvC;
  std::vector<std::vector<std::vector<TString> > >                  fvvTS;
  std::vector<std::vector<std::vector<TObject*> > >                 fvvTOptr;
  std::vector<std::vector<std::vector<TRef*> > >                    fvvTRptr;
  std::vector<std::vector<std::vector<void*> > >                    fvvvptr;



  class BranchManager {
  public:
    BranchManager (AnalysisTreeReader *tr = 0) : fTreeReader(tr) {}
    ~BranchManager () {}
    TString GetName () {return fBranchName;}
    TString GetFullType () {return fType;}
    TString GetScalarType () {return fScalarType;}
    Bool_t Create (TString branchname);
    void SetEntry (Long64_t entry) {fBranch->GetEntry(entry);}
    StorageType GetStorageType () {return fStorageID;}
    Int_t GetStorageIndex () {return fStorageIndex;}

  private:
    StorageType            fStorageID; // this will be the proprietary index of the vector to use in the ATR
    Int_t                  fStorageIndex; // this will be the proprietary index of the vector to use in the ATR
    TString                fBranchName, fType, fScalarType;
    TBranch               *fBranch;
    AnalysisTreeReader    *fTreeReader;
  };

  friend class BranchManager;
  std::vector<BranchManager*>         fBranchManagers;

};

} /* HAL */ 

#endif
