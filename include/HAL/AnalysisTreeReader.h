/*
 *
 * This is a general purpose TTree reading class. This class is designed
 * to handle the most popular types of data that are stored in TTree's
 * for physics analysis. In particular it can handle scalars, c-arrays,
 * std::vectors, 2D c-arrays, and std::vectors of std::vectors of the
 * following types:
 *
 *     Bool: bool, Bool_t
 *  Integer: int, Int_t, short, Short_t, long, Long_t, long long, Long64_t, signed char
 * Counting: unsigned int, UInt_t, unsigned short, UShort_t, unsigned long, ULong_t, unsigned long long, ULong64_t, unsigned char, Byte_t
 *  Decimal: float, Float_t, Float16_t, Real_t, double, Double_t, Double32_t, long double, LongDouble_t
 *   String: char, Char_t, char*, Text_t, TString, std::string
 *     Misc: TObjArray, TClonesArray, TRef, TRefArray
 *
 * NOTE: This class can only handle TObjArrays, TClonesArrays, and
 *       TRefArrays stored as scalars, c-arrays, and std::vectors.
 *
 * */
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

  unsigned int              GetDim (TString branchname, Int_t idx_1 = -1);
  bool                      GetBool (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  long long                 GetInteger (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  unsigned long long        GetCounting (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
  long double               GetDecimal (TString branchname, Int_t idx_1 = -1, Int_t idx_2 = -1);
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
  // type lists (these are for the typedefs root defines, i.e. synonyms)
  std::set<TString> fBool,
                    fInteger, fShort, fLong, fLLong, fSChar,
                    fUInteger, fUShort, fULong, fULLong, fUChar,
                    fFloat, fDouble, fLDouble,
                    fChar;

  // Container for storing the nicknames for the branches
  TMap *fBranchMap;

  // Storage for basic types
  // (used deques b/c of memory location stability and only a few elements are added to
  //  each deque, so the linear access times shouldn't be an issue)
  std::deque<bool>                     fB;  // bool type
  std::deque<long double>              fD;  // decimal number  (float, double, etc...)
  std::deque<long long>                fI;  // integer number  (Byte_t, int, long, short, etc...)
  std::deque<unsigned long long>       fC;  // counting number (unsigned int, unsigned etc...)
  //std::deque<Bool_t>                   fB;  // bool type
  //std::deque<LongDouble_t>             fD;  // decimal number  (float, double, etc...)
  //std::deque<Long64_t>                 fI;  // integer number  (Byte_t, int, long, short, etc...)
  //std::deque<ULong64_t>                fC;  // counting number (unsigned int, unsigned etc...)
  std::deque<TString>                  fS;  // string types    (char, TString, ...)
  std::deque<TObjArray>                fOA; // TObjArray
  std::deque<TClonesArray>             fCA; // TClonesArray
  std::deque<TRef>                     fR;  // TRef
  std::deque<TRefArray>                fRA; // TRefArray
  
  // storage for 1D arrays of data
  std::deque<std::vector<bool> >                fvB;  // bool type
  std::deque<std::vector<long double> >         fvD;  // decimal number
  std::deque<std::vector<long long> >           fvI;  // integer number
  std::deque<std::vector<unsigned long long> >  fvC;  // counting number
  //std::deque<std::vector<Bool_t> >        fvB;  // bool type
  //std::deque<std::vector<LongDouble_t> >  fvD;  // decimal number
  //std::deque<std::vector<Long64_t> >      fvI;  // integer number
  //std::deque<std::vector<ULong64_t> >     fvC;  // counting number
  std::deque<std::vector<TString> >             fvS;  // string types
  std::deque<std::vector<TObjArray> >           fvOA; // TObjArray
  std::deque<std::vector<TClonesArray> >        fvCA; // TClonesArray
  std::deque<std::vector<TRef> >                fvR;  // TRef
  std::deque<std::vector<TRefArray> >           fvRA; // TRefArray

  // storage for 2D arrays of data
  std::deque<std::vector<std::vector<bool> > >                fvvB;  // bool type
  std::deque<std::vector<std::vector<long double> > >         fvvD;  // decimal number
  std::deque<std::vector<std::vector<long long> > >           fvvI;  // integer number
  std::deque<std::vector<std::vector<unsigned long long> > >  fvvC;  // counting number
  //std::deque<std::vector<std::vector<Bool_t> > >        fvvB;  // bool type
  //std::deque<std::vector<std::vector<LongDouble_t> > >  fvvD;  // decimal number
  //std::deque<std::vector<std::vector<Long64_t> > >      fvvI;  // integer number
  //std::deque<std::vector<std::vector<ULong64_t> > >     fvvC;  // counting number
  std::deque<std::vector<std::vector<TString> > >             fvvS;  // string types
  std::deque<std::vector<std::vector<TRef> > >                fvvR;  // TRef


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
    StorageType            fStorageID; // this will be the proprietary type to use in the ATR
    Int_t                  fStorageIndex; // this will be the proprietary index in the deque to use in the ATR
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
