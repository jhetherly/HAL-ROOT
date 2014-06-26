/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 */

#ifndef HAL_AnalysisTreeReader
#define HAL_AnalysisTreeReader

#include <RVersion.h>

// For ROOT 6 HAL uses TTreeReader
//#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)

#include <string>
#include <deque>
#include <vector>
#include <set>
#include <map>
#include <TNamed.h>
#include <TString.h>
#include <TRegexp.h>
#include <TTree.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TRef.h>
#include <TRefArray.h>
#include <HAL/Common.h>

// forward declaration(s)
class TBranch;
class TFile;
class TLeaf;
class TMap;

namespace HAL 
{
namespace internal 
{
class BranchManager;
}
}
// end forward declaration(s)

namespace HAL 
{

//! Class for the easy extraction of data from a TTree
/*!
 * This class allows for the easy retrieval of data stored in a TTree.
 * Through normal function calls the user can access data stored as
 * scalars, C-arrays, STL vectors, 2D C-arrays, and STL vectors of
 * vectors. A listing of the allowable types is given below. If a
 * class that is stored in the TTree can be decomposed in 
 * 'MakeClassMode,' this class can read it as well. The user should
 * not need to worry about allocating memory or setting branch 
 * addresses. There are hard limits to the lengths of a C-array and
 * 2D C-array that can be read. They are 10000 and 10000x100 
 * respectively. If HAL is compiled with ROOT version 6 or later, 
 * this restriction is lifted.\n
 * _Data Types That Can be Read:_
 * | Boolean | Integer | Counting | Decimal | String | Misc |
 * | :-----: | :-----: | :------: | :-----: | :----: | :--: |
 * | bool/Bool_t | int/Int_t | unsigned int/UInt_t | float/Float_t/Float16_t/Real_t | char/Char_t/Text_t | TObjArray |
 * |  | short/Short_t | unsigned short/UShort_t | double/Double_t/Double32_t | char* | TClonesArray |
 * |  | long/Long_t | unsigned long/ULong_t | long double/LongDouble_t | TString | TRef |
 * |  | long long/Long64_t | unsigned long long/ULong64_t |  | TObjString | TRefArray |
 * |  | signed char | unsigned char |  | std::string | TRefArray |
 */
class AnalysisTreeReader : public TNamed {

private:

  TString GetFullBranchName (TString name);

  TTree *fChain;
  Long64_t fEntry;
  enum StorageType {kB, kD, kI, kC, kS, kOA, kCA, kR, kRA,
                    kvB, kvD, kvI, kvC, kvS, kvOA, kvCA, kvR, kvRA,
                    kvvB, kvvD, kvvI, kvvC, kvvS, kvvR};
  // regex's for how to identify scalars, vec's, and arrays
  TRegexp fScalar, fVector, fVector2D, fArray, fArray2D;
  // type lists (these are for the typedefs ROOT defines, i.e. synonyms)
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
  std::deque<std::vector<std::vector<TString> > >             fvvS;  // string types
  std::deque<std::vector<std::vector<TRef> > >                fvvR;  // TRef


  friend class internal::BranchManager;
  std::vector<internal::BranchManager*>                               fBranchManagers;
  std::map<TString, internal::BranchManager*, internal::string_cmp>   fNickNameBranchMap;
  internal::BranchManager*        GetBranchManager (const TString&);

public:

  AnalysisTreeReader (TTree *tree = 0);
  virtual ~AnalysisTreeReader ();
  void      SetTree (TTree *tree) {fChain = tree; fChain->SetMakeClass(1);}
  void      SetEntry (Long64_t entry);
  Long64_t  GetEntryNumber () {return fEntry;}
  TTree*    GetTree () {return fChain;}
  TString   GetBranchName (const TString &name);
  void      Init ();
  Bool_t    Notify ();
  void      SetBranchMap (TMap *m) {fBranchMap = m;}
  bool      CheckBranchMapNickname (const TString &name);

  unsigned int              GetRank (const TString &branchname);
  unsigned int              GetDim (const TString &branchname, const long long &idx_1 = -1);
  bool                      GetBool (const TString &branchname, const long long &idx_1 = -1, const long long &idx_2 = -1);
  long long                 GetInteger (const TString &branchname, const long long &idx_1 = -1, const long long &idx_2 = -1);
  unsigned long long        GetCounting (const TString &branchname, const long long &idx_1 = -1, const long long &idx_2 = -1);
  long double               GetDecimal (const TString &branchname, const long long &idx_1 = -1, const long long &idx_2 = -1);
  TString                   GetString (const TString &branchname, const long long &idx_1 = -1, const long long &idx_2 = -1);
  TObjArray&                GetObjArray (const TString &branchname, const long long &idx_1 = -1);
  TClonesArray&             GetClonesArray (const TString &branchname, const long long &idx_1 = -1);
  TRef&                     GetRef (const TString &branchname, const long long &idx_1 = -1, const long long &idx_2 = -1);
  TRefArray&                GetRefArray (const TString &branchname, const long long &idx_1 = -1);

  ClassDef(AnalysisTreeReader, 0);

};

namespace internal {

// Can only handle scalars (defined in the std::set's above), c-arrays, and vectors
class BranchManager {
public:
  BranchManager (AnalysisTreeReader *tr = 0);
  ~BranchManager ();
  TString     GetName () {return fBranchName;}
  TString     GetFullType () {return fType;}
  TString     GetScalarType () {return fScalarType;}
  Bool_t      IsScalar () {return fScalar;}
  Bool_t      IsCArray1D () {return fCArray1D;}
  Bool_t      IsCArray2D () {return fCArray2D;}
  Bool_t      IsVec1D () {return fVec1D;}
  Bool_t      IsVec2D () {return fVec2D;}
  Bool_t      Create (TString branchname);
  void        Init ();
  void        SetEntry (Long64_t entry);
  AnalysisTreeReader::StorageType GetStorageType () {return fStorageID;}
  Int_t       GetStorageIndex () {return fStorageIndex;}

private:
  AnalysisTreeReader::StorageType            fStorageID; // this will be the proprietary type to use in the ATR
  Int_t                  fStorageIndex; // this will be the proprietary index in the deque to use in the ATR
  Bool_t                 fScalar, fCArray1D, fCArray2D, fVec1D, fVec2D;
  TString                fBranchName, fBranchTitle, fLeafTitle, fType, fScalarType, fFileName;
  Int_t                  fLeafNdata;
  TBranch               *fBranch;
  AnalysisTreeReader    *fTreeReader;

  static const int       fMaxBufferLength = 10000;
  static const int       fMaxBufferLength2 = 100;
  Int_t                  GetArrayLength (Int_t rank);
  Int_t                  GetMaxArrayLength (Int_t rank);
  void                   FindTypeInformation ();

  bool                                  fB;
  signed char                           fSC;
  int                                   fI;
  short                                 fSI;
  long                                  fL;
  long long                             fLL;
  unsigned char                         fUC;
  unsigned int                          fUI;
  unsigned short                        fUSI;
  unsigned long                         fUL;
  unsigned long long                    fULL;
  float                                 fF;
  double                                fD;
  long double                           fLD;
  char                                  fC;
  TString                               fTS;
  TObjString                            fTOS;
  std::string                           fstdS;
  TObjArray                             fTOA;
  TClonesArray                          fTCA;
  TRef                                  fTR;
  TRefArray                             fTRA;
  bool                                 *fcB;
  signed char                          *fcSC;
  int                                  *fcI;
  short                                *fcSI;
  long                                 *fcL;
  long long                            *fcLL;
  unsigned char                        *fcUC;
  unsigned int                         *fcUI;
  unsigned short                       *fcUSI;
  unsigned long                        *fcUL;
  unsigned long long                   *fcULL;
  float                                *fcF;
  double                               *fcD;
  long double                          *fcLD;
  char                                 *fcC;
  TString                              *fcTS;
  TObjString                           *fcTOS;
  std::string                          *fcstdS;
  TObjArray                            *fcTOA;
  TClonesArray                         *fcTCA;
  TRef                                 *fcTR;
  TRefArray                            *fcTRA;
  bool                                **fccB;
  signed char                         **fccSC;
  int                                 **fccI;
  short                               **fccSI;
  long                                **fccL;
  long long                           **fccLL;
  unsigned char                       **fccUC;
  unsigned int                        **fccUI;
  unsigned short                      **fccUSI;
  unsigned long                       **fccUL;
  unsigned long long                  **fccULL;
  float                               **fccF;
  double                              **fccD;
  long double                         **fccLD;
  char                                **fccC;
  TString                             **fccTS;
  TObjString                          **fccTOS;
  std::string                         **fccstdS;
  TRef                                **fccTR;
  std::vector<bool>                    *fvB;
  std::vector<signed char>             *fvSC;
  std::vector<int>                     *fvI;
  std::vector<short>                   *fvSI;
  std::vector<long>                    *fvL;
  std::vector<long long>               *fvLL;
  std::vector<unsigned char>           *fvUC;
  std::vector<unsigned int>            *fvUI;
  std::vector<unsigned short>          *fvUSI;
  std::vector<unsigned long>           *fvUL;
  std::vector<unsigned long long>      *fvULL;
  std::vector<float>                   *fvF;
  std::vector<double>                  *fvD;
  std::vector<long double>             *fvLD;
  std::vector<char>                    *fvC;
  std::vector<TString>                 *fvTS;
  std::vector<TObjString>              *fvTOS;
  std::vector<std::string>             *fvstdS;
  std::vector<TObjArray>               *fvTOA;
  std::vector<TClonesArray>            *fvTCA;
  std::vector<TRef>                    *fvTR;
  std::vector<TRefArray>               *fvTRA;
  std::vector<std::vector<bool> >                 *fvvB;
  std::vector<std::vector<signed char> >          *fvvSC;
  std::vector<std::vector<int> >                  *fvvI;
  std::vector<std::vector<short> >                *fvvSI;
  std::vector<std::vector<long> >                 *fvvL;
  std::vector<std::vector<long long> >            *fvvLL;
  std::vector<std::vector<unsigned char> >        *fvvUC;
  std::vector<std::vector<unsigned int> >         *fvvUI;
  std::vector<std::vector<unsigned short> >       *fvvUSI;
  std::vector<std::vector<unsigned long> >        *fvvUL;
  std::vector<std::vector<unsigned long long> >   *fvvULL;
  std::vector<std::vector<float> >                *fvvF;
  std::vector<std::vector<double> >               *fvvD;
  std::vector<std::vector<long double> >          *fvvLD;
  std::vector<std::vector<char> >                 *fvvC;
  std::vector<std::vector<TString> >              *fvvTS;
  std::vector<std::vector<TObjString> >           *fvvTOS;
  std::vector<std::vector<std::string> >          *fvvstdS;
  std::vector<std::vector<TRef> >                 *fvvTR;

  bool        fIsB, fIsSC, fIsI, fIsSI, fIsL, fIsLL, fIsUC, fIsUI;
  bool        fIsUSI, fIsUL, fIsULL, fIsF, fIsD, fIsLD, fIsC, fIsTS;
  bool        fIsTOS, fIsstdS, fIsTOA, fIsTCA, fIsTR, fIsTRA;
  //int         fRows, fColumns;
};

} /* internal */ 

} /* HAL */ 

//#else // ROOT 6 and above

//#endif // Version

#endif // Include guards
