#include <HAL/AnalysisTreeReader.h>
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
#include <aux/boost/foreach.hpp>
#endif
#include <TBranch.h>
#include <TFile.h>
#include <TLeaf.h>
#include <TMap.h>
#include <HAL/Exceptions.h>

ClassImp(HAL::AnalysisTreeReader);

namespace HAL {

// For ROOT 6 HAL uses TTreeReader
//#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)

//______________________________________________________________________________
AnalysisTreeReader::AnalysisTreeReader (TTree *t) : fChain(t), 
  fScalar("^[a-zA-Z][a-zA-Z0-9_]+$"),
  fVector("^vector[ ]*<[ ]*[a-zA-Z][a-zA-Z0-9_]+[ ]*>$"), // vector<scalar>
  fVector2D("^vector[ ]*<[ ]*vector[ ]*<[ ]*[a-zA-Z][a-zA-Z0-9_]+[ ]*>[ ]*>$"), // vector<vector<scalar> >
  fArray("^[a-zA-Z_][a-zA-Z0-9_]*[[a-zA-Z0-9_]+]$"), // name[i]
  fArray2D("^[a-zA-Z_][a-zA-Z0-9_]*[[a-zA-Z0-9_]+][[a-zA-Z0-9_]+]$") /* name[i][j] */ 
{ 
  // bool types
  fBool.insert("Bool_t");
  fBool.insert("bool");
  // integer types
  fInteger.insert("int");
  if (sizeof(int) == sizeof(Int_t))
    fInteger.insert("Int_t");
  fShort.insert("Short_t");
  fShort.insert("short");
  fShort.insert("short int");
  if (sizeof(long int) == sizeof(Int_t))
    fLong.insert("Int_t");
  fLong.insert("Long_t");
  fLong.insert("long");
  fLong.insert("long int");
  fLLong.insert("Long64_t");
  fLLong.insert("long long");
  fLLong.insert("long long int");
  fSChar.insert("signed char");
  // counting types
  if (sizeof(unsigned int) == sizeof(UInt_t))
    fUInteger.insert("UInt_t");
  fUInteger.insert("unsigned");
  fUInteger.insert("unsigned int");
  fUShort.insert("UShort_t");
  fUShort.insert("unsigned short");
  fUShort.insert("unsigned short int");
  if (sizeof(unsigned long int) == sizeof(UInt_t))
    fULong.insert("UInt_t");
  fULong.insert("ULong_t");
  fULong.insert("unsigned long");
  fULong.insert("unsigned long int");
  fULLong.insert("ULong64_t");
  fULLong.insert("unsigned long long");
  fULLong.insert("unsigned long long int");
  fUChar.insert("unsigned char");
  fUChar.insert("Byte_t");
  // decimal types
  fFloat.insert("Real_t");
  fFloat.insert("Float_t");
  fFloat.insert("Float16_t");
  fFloat.insert("float");
  fDouble.insert("Double_t");
  fDouble.insert("Double32_t");
  fDouble.insert("double");
  if (sizeof(double) == sizeof(LongDouble_t))
    fDouble.insert("LongDouble_t");
  else
    fLDouble.insert("LongDouble_t");
  fLDouble.insert("long double");
  // string types
  fChar.insert("Text_t");
  fChar.insert("Char_t");
  fChar.insert("char");
}

//______________________________________________________________________________
AnalysisTreeReader::~AnalysisTreeReader () 
{
}

//______________________________________________________________________________
void AnalysisTreeReader::SetEntry (Long64_t entry) 
{
  std::set<internal::BranchManager*> unique_bms;

  fEntry = entry;

  // Update all branches
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<TString, internal::BranchManager*> bm;
  BOOST_FOREACH( bm, fNickNameBranchMap )
#else
  for (auto bm: fNickNameBranchMap)
#endif
  {
    if (unique_bms.insert(bm.second).second)
      bm.second->SetEntry(entry);
  }
}

//______________________________________________________________________________
TString AnalysisTreeReader::GetBranchName (const TString &name) 
{
  TPair *kv_pair = static_cast<TPair*>(fBranchMap->FindObject(name.Data()));
  if (kv_pair == nullptr)
    return name;
  return static_cast<TObjString*>(kv_pair->Value())->GetString();
}

//______________________________________________________________________________
void AnalysisTreeReader::Init () 
{
  std::set<internal::BranchManager*> unique_bms;

  // Init all branches
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<TString, internal::BranchManager*> bm;
  BOOST_FOREACH( bm, fNickNameBranchMap )
#else
  for (auto bm: fNickNameBranchMap)
#endif
  {
    if (unique_bms.insert(bm.second).second)
      bm.second->Init();
  }
}

//______________________________________________________________________________
Bool_t AnalysisTreeReader::Notify () 
{
  std::set<internal::BranchManager*> unique_bms;

  // Init all branches
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<TString, internal::BranchManager*> bm;
  BOOST_FOREACH( bm, fNickNameBranchMap )
#else
  for (auto bm: fNickNameBranchMap)
#endif
  {
    if (unique_bms.insert(bm.second).second)
      bm.second->Init();
  }
  return kTRUE;
}

//______________________________________________________________________________
bool AnalysisTreeReader::CheckBranchMapNickname (const TString &name) 
{
  TMapIter next(fBranchMap);
  while(TObjString *key = static_cast<TObjString*>(next())){
    TString nn = key->String();

    if (name.EqualTo(nn, TString::kIgnoreCase))
      return true;
  }
  return false;
}

//______________________________________________________________________________
TString AnalysisTreeReader::GetFullBranchName (TString name) 
{
  // Remove any leading or trailing whitespace
  name.Strip(TString::kBoth);

  // Check if branch or leaf has name
  if (fChain->FindBranch(name.Data()))
    return name;
  if (fChain->FindLeaf(name.Data()))
    return TString(fChain->GetLeaf(name.Data())->GetBranch()->GetName());

  // Substitute branch mapping
  TMapIter next(fBranchMap);
  while(TObjString *key = static_cast<TObjString*>(next())){
    TString nn = key->String();
    TString bn = static_cast<TObjString*>(fBranchMap->GetValue(key))->String();
    TString oldname = name;

    if (name.BeginsWith(nn, TString::kIgnoreCase)) {
      name.Replace(0, nn.Length(), bn);
    }
    
    // Check if branch or leaf has name
    if (fChain->FindBranch(name.Data()))
      return name;
    if (fChain->FindLeaf(name.Data()))
      return TString(fChain->GetLeaf(name.Data())->GetBranch()->GetName());
    name = oldname;
  }

  throw HALException(name.Prepend("Couldn't find branch: ").Data());
}

//______________________________________________________________________________
unsigned int AnalysisTreeReader::GetRank (const TString &branchname) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->IsScalar())
    return 0;
  if (branchmanager->IsCArray1D() || branchmanager->IsVec1D())
    return 1;
  if (branchmanager->IsCArray2D() || branchmanager->IsVec2D())
    return 2;

  throw HALException(branchmanager->GetName().Prepend("Couldn't find rank for branch: ").Data());
}

//______________________________________________________________________________
unsigned int AnalysisTreeReader::GetDim (const TString &branchname, const long long &idx_1) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kOA)
    return (unsigned int)fOA[branchmanager->GetStorageIndex()].GetEntries();
  if (branchmanager->GetStorageType() == kCA)
    return (unsigned int)fCA[branchmanager->GetStorageIndex()].GetEntries();
  if (branchmanager->GetStorageType() == kRA)
    return (unsigned int)fRA[branchmanager->GetStorageIndex()].GetEntries();
  if (branchmanager->IsScalar())
    throw HALException(GetFullBranchName( branchname ).Prepend("Tried getting dimension of scalar in branch: ").Data());
  if (branchmanager->GetStorageType() == kvB && idx_1 == -1)
    return fvB[branchmanager->GetStorageIndex()].size();
  if (branchmanager->GetStorageType() == kvD && idx_1 == -1)
    return fvD[branchmanager->GetStorageIndex()].size();
  if (branchmanager->GetStorageType() == kvI && idx_1 == -1)
    return fvI[branchmanager->GetStorageIndex()].size();
  if (branchmanager->GetStorageType() == kvC && idx_1 == -1)
    return fvC[branchmanager->GetStorageIndex()].size();
  if (branchmanager->GetStorageType() == kvS && idx_1 == -1)
    return fvS[branchmanager->GetStorageIndex()].size();
  if (branchmanager->GetStorageType() == kvR && idx_1 == -1)
    return fvR[branchmanager->GetStorageIndex()].size();
  if (branchmanager->GetStorageType() == kvOA) {
    if (idx_1 == -1)
      return fvOA[branchmanager->GetStorageIndex()].size();
    return (unsigned int)fvOA[branchmanager->GetStorageIndex()][idx_1].GetEntries();
  }
  if (branchmanager->GetStorageType() == kvCA) {
    if (idx_1 == -1)
      return fvCA[branchmanager->GetStorageIndex()].size();
    return (unsigned int)fvCA[branchmanager->GetStorageIndex()][idx_1].GetEntries();
  }
  if (branchmanager->GetStorageType() == kvRA) {
    if (idx_1 == -1)
      return fvRA[branchmanager->GetStorageIndex()].size();
    return (unsigned int)fvRA[branchmanager->GetStorageIndex()][idx_1].GetEntries();
  }
  if (branchmanager->GetStorageType() == kvvB) {
    if (idx_1 == -1)
      return fvvB[branchmanager->GetStorageIndex()].size();
    return fvvB[branchmanager->GetStorageIndex()][idx_1].size();
  }
  if (branchmanager->GetStorageType() == kvvD) {
    if (idx_1 == -1)
      return fvvD[branchmanager->GetStorageIndex()].size();
    return fvvD[branchmanager->GetStorageIndex()][idx_1].size();
  }
  if (branchmanager->GetStorageType() == kvvI) {
    if (idx_1 == -1)
      return fvvI[branchmanager->GetStorageIndex()].size();
    return fvvI[branchmanager->GetStorageIndex()][idx_1].size();
  }
  if (branchmanager->GetStorageType() == kvvC) {
    if (idx_1 == -1)
      return fvvC[branchmanager->GetStorageIndex()].size();
    return fvvC[branchmanager->GetStorageIndex()][idx_1].size();
  }
  if (branchmanager->GetStorageType() == kvvS) {
    if (idx_1 == -1)
      return fvvS[branchmanager->GetStorageIndex()].size();
    return fvvS[branchmanager->GetStorageIndex()][idx_1].size();
  }
  if (branchmanager->GetStorageType() == kvvR) {
    if (idx_1 == -1)
      return fvvR[branchmanager->GetStorageIndex()].size();
    return fvvR[branchmanager->GetStorageIndex()][idx_1].size();
  }

  throw HALException(GetFullBranchName( branchname ).Prepend("Error in finding dimensions in branch: ").Data());
}

//______________________________________________________________________________
bool AnalysisTreeReader::GetBool (const TString &branchname, 
                                  const long long &idx_1, const long long &idx_2) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kB)
    return fB[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvB)
    return fvB[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvB)
    return fvvB[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kI)
    return (bool)fI[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvI)
    return (bool)fvI[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvI)
    return (bool)fvvI[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kC)
    return (bool)fC[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvC)
    return (bool)fvC[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvC)
    return (bool)fvvC[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kD)
    return (bool)fD[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvD)
    return (bool)fvD[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvD)
    return (bool)fvvD[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find bool data in branch: ").Data());
}

//______________________________________________________________________________
long long AnalysisTreeReader::GetInteger (const TString &branchname, 
                                          const long long &idx_1, 
                                          const long long &idx_2) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kI)
    return fI[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvI)
    return fvI[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvI)
    return fvvI[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kC)
    return fC[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvC)
    return fvC[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvC)
    return fvvC[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kD)
    return fD[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvD)
    return fvD[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvD)
    return fvvD[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kB)
    return (long long)fB[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvB)
    return (long long)fvB[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvB)
    return (long long)fvvB[branchmanager->GetStorageIndex()][idx_1][idx_2];
  // special case of char as 8-bit data holder
  if (branchmanager->GetStorageType() == kvS && fChar.count(branchmanager->GetScalarType()) != 0)
    return (signed char)fvS[branchmanager->GetStorageIndex()][idx_1].Data()[0];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find integer number data in branch: ").Data());
}

//______________________________________________________________________________
unsigned long long AnalysisTreeReader::GetCounting (const TString &branchname, 
                                                    const long long &idx_1, 
                                                    const long long &idx_2) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kC)
    return fC[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvC)
    return fvC[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvC)
    return fvvC[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kI)
    return fI[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvI)
    return fvI[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvI)
    return fvvI[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kD)
    return fD[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvD)
    return fvD[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvD)
    return fvvD[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kB)
    return (unsigned long long)fB[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvB)
    return (unsigned long long)fvB[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvB)
    return (unsigned long long)fvvB[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find counting number data in branch: ").Data());
}

//______________________________________________________________________________
long double AnalysisTreeReader::GetDecimal (const TString &branchname, 
                                            const long long &idx_1, 
                                            const long long &idx_2) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kD)
    return fD[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvD) 
    return fvD[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvD)
    return fvvD[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kI)
    return fI[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvI)
    return fvI[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvI)
    return fvvI[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kC)
    return fC[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvC)
    return fvC[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvC)
    return fvvC[branchmanager->GetStorageIndex()][idx_1][idx_2];
  if (branchmanager->GetStorageType() == kB)
    return (long double)fB[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvB)
    return (long double)fvB[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvB)
    return (long double)fvvB[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find decimal number data in branch: ").Data());
}

//______________________________________________________________________________
TString AnalysisTreeReader::GetString (const TString &branchname, 
                                       const long long &idx_1, 
                                       const long long &idx_2) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kS)
    return fS[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvS && 
      fvS[branchmanager->GetStorageIndex()].back().Length() == 1 && 
      fvS[branchmanager->GetStorageIndex()].back()[0] == '\0') {
    TString s("");
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
    BOOST_FOREACH( TString c, fvS[branchmanager->GetStorageIndex()] )
#else
    for (auto c: fvS[branchmanager->GetStorageIndex()])
#endif
      s.Prepend(c);
    return s;
  }
  if (branchmanager->GetStorageType() == kvS)
    return fvS[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvS && 
      fvvS[branchmanager->GetStorageIndex()][idx_1].back().Length() == 1 && 
      fvvS[branchmanager->GetStorageIndex()][idx_1].back()[0] == '\0') {
    TString s("");
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
    BOOST_FOREACH( TString c, fvvS[branchmanager->GetStorageIndex()][idx_1] )
#else
    for (auto c: fvvS[branchmanager->GetStorageIndex()][idx_1])
#endif
      s.Prepend(c);
    return s;
  }
  if (branchmanager->GetStorageType() == kvvS)
    return fvvS[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find string data in branch: ").Data());
}

TObjArray& AnalysisTreeReader::GetObjArray (const TString &branchname, const long long &idx_1) {
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kOA)
    return fOA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvOA)
    return fvOA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TObjArray data in branch: ").Data());
}

//______________________________________________________________________________
TClonesArray& AnalysisTreeReader::GetClonesArray (const TString &branchname, 
                                                  const long long &idx_1) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kCA)
    return fCA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvCA)
    return fvCA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TClonesArray data in branch: ").Data());
}

//______________________________________________________________________________
TRef& AnalysisTreeReader::GetRef (const TString &branchname, 
                                  const long long &idx_1, const long long &idx_2) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kR)
    return fR[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvR)
    return fvR[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvR)
    return fvvR[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TRef data in branch: ").Data());
}

//______________________________________________________________________________
TRefArray& AnalysisTreeReader::GetRefArray (const TString &branchname, 
                                            const long long &idx_1) 
{
  internal::BranchManager *branchmanager = nullptr;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == nullptr)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kRA)
    return fRA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvRA)
    return fvRA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TRefArray data in branch: ").Data());
}

//______________________________________________________________________________
internal::BranchManager* AnalysisTreeReader::GetBranchManager (const TString &branchname) {
  internal::BranchManager *branchmanager = nullptr;

  if (fNickNameBranchMap.count(branchname) == 0) {
    bool already_stored = false;
    TString bname = GetFullBranchName( branchname );
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
    std::pair<TString, internal::BranchManager*> bm;
    BOOST_FOREACH( bm, fNickNameBranchMap )
#else
    for (auto bm: fNickNameBranchMap)
#endif
    {
      if (bm.second->GetName().EqualTo(bname)) {
        already_stored = true;
        fNickNameBranchMap[branchname] = bm.second;
        branchmanager = bm.second;
        break;
      }
    }
    if (!already_stored) {
      branchmanager = new internal::BranchManager(this);
      if (branchmanager->Create(bname))
        fNickNameBranchMap[branchname] = branchmanager;
    }
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

  return branchmanager;
}




// ///////////////////////////////////////
// BranchManager private class
// ///////////////////////////////////////

internal::BranchManager::BranchManager (AnalysisTreeReader *tr) : 
  fScalar(kFALSE), fCArray1D(kFALSE), fCArray2D(kFALSE), 
  fVec1D(kFALSE), fVec2D(kFALSE), fBranch(nullptr), fTreeReader(tr),
  fcB(nullptr), fcSC(nullptr), fcI(nullptr), fcSI(nullptr), fcL(nullptr), fcLL(nullptr),
  fcUC(nullptr), fcUI(nullptr), fcUSI(nullptr), fcUL(nullptr), fcULL(nullptr), fcF(nullptr),
  fcD(nullptr), fcLD(nullptr), fcC(nullptr), fcTS(nullptr), fcTOS(nullptr), fcstdS(nullptr),
  fcTOA(nullptr), fcTCA(nullptr), fcTR(nullptr), fcTRA(nullptr),
  fccB(nullptr), fccSC(nullptr), fccI(nullptr), fccSI(nullptr), fccL(nullptr), fccLL(nullptr),
  fccUC(nullptr), fccUI(nullptr), fccUSI(nullptr), fccUL(nullptr), fccULL(nullptr), fccF(nullptr),
  fccD(nullptr), fccLD(nullptr), fccC(nullptr), fccTS(nullptr), fccTOS(nullptr), fccstdS(nullptr),
  fccTR(nullptr),
  fvB(nullptr), fvSC(nullptr), fvI(nullptr), fvSI(nullptr), fvL(nullptr), fvLL(nullptr),
  fvUC(nullptr), fvUI(nullptr), fvUSI(nullptr), fvUL(nullptr), fvULL(nullptr), fvF(nullptr),
  fvD(nullptr), fvLD(nullptr), fvC(nullptr), fvTS(nullptr), fvTOS(nullptr), fvstdS(nullptr),
  fvTOA(nullptr), fvTCA(nullptr), fvTR(nullptr), fvTRA(nullptr),
  fvvB(nullptr), fvvSC(nullptr), fvvI(nullptr), fvvSI(nullptr), fvvL(nullptr), fvvLL(nullptr),
  fvvUC(nullptr), fvvUI(nullptr), fvvUSI(nullptr), fvvUL(nullptr), fvvULL(nullptr), fvvF(nullptr),
  fvvD(nullptr), fvvLD(nullptr), fvvC(nullptr), fvvTS(nullptr), fvvTOS(nullptr), fvvstdS(nullptr),
  fvvTR(nullptr), 
  fIsB (false), fIsSC(false), fIsI(false), fIsSI(false), 
  fIsL(false), fIsLL(false), fIsUC(false), fIsUI(false), fIsUSI(false), 
  fIsUL(false), fIsULL(false), fIsF(false), fIsD(false), fIsLD(false), 
  fIsC(false), fIsTS(false), fIsTOS(false), fIsstdS(false), fIsTOA(false), 
  fIsTCA(false), fIsTR(false), fIsTRA(false)/*,
  fRows(0), fColumns(0)*/ {
}

internal::BranchManager::~BranchManager () {
}

Bool_t internal::BranchManager::Create (TString branchname) {

  fBranchName = branchname;

  fBranch = fTreeReader->fChain->GetBranch(fBranchName.Data());
  if (fBranch == nullptr)
    return kFALSE;

  fBranchTitle = fBranch->GetTitle();

  FindTypeInformation();

  // ////////////////////////////////////////////////////
  // Initialize data containers for all different types
  // ////////////////////////////////////////////////////
  if (fScalar) {
    // Test for bool types
    if (fTreeReader->fBool.count(fType) != 0) {
      fStorageID = AnalysisTreeReader::kB;
      fTreeReader->fB.push_back(kFALSE);
      fStorageIndex = fTreeReader->fB.size() - 1;
    }
    // Test for integer types
    else if (fTreeReader->fInteger.count(fType) != 0 || 
             fTreeReader->fShort.count(fType) != 0 ||
             fTreeReader->fLong.count(fType) != 0 ||
             fTreeReader->fLLong.count(fType) != 0 ||
             fTreeReader->fSChar.count(fType) != 0) {
      fStorageID = AnalysisTreeReader::kI;
      fTreeReader->fI.push_back(0);
      fStorageIndex = fTreeReader->fI.size() - 1;
    }
    // Test for counting types
    else if (fTreeReader->fUInteger.count(fType) != 0 || 
             fTreeReader->fUShort.count(fType) != 0 ||
             fTreeReader->fULong.count(fType) != 0 ||
             fTreeReader->fULLong.count(fType) != 0 ||
             fTreeReader->fUChar.count(fType) != 0) {
      fStorageID = AnalysisTreeReader::kC;
      fTreeReader->fC.push_back(0);
      fStorageIndex = fTreeReader->fC.size() - 1;
    }
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fType) != 0 || 
             fTreeReader->fDouble.count(fType) != 0 ||
             fTreeReader->fLDouble.count(fType) != 0) {
      fStorageID = AnalysisTreeReader::kD;
      fTreeReader->fD.push_back(0.0);
      fStorageIndex = fTreeReader->fD.size() - 1;
    }
    // Test for string types
    else if (fTreeReader->fChar.count(fType) != 0 || 
             fType.EqualTo("string") ||
             fType.EqualTo("TString") ||
             fType.EqualTo("TObjString")) {
      fStorageID = AnalysisTreeReader::kS;
      fTreeReader->fS.push_back("");
      fStorageIndex = fTreeReader->fS.size() - 1;
    }
    else if (fType.EqualTo("TObjArray")) {
      TObjArray temp;
      fStorageID = AnalysisTreeReader::kOA;
      fTreeReader->fOA.push_back(temp);
      fStorageIndex = fTreeReader->fOA.size() - 1;
    }
    else if (fType.EqualTo("TClonesArray")) {
      TClonesArray temp;
      fStorageID = AnalysisTreeReader::kCA;
      fTreeReader->fCA.push_back(temp);
      fStorageIndex = fTreeReader->fCA.size() - 1;
    }
    else if (fType.EqualTo("TRef")) {
      TRef temp;
      fStorageID = AnalysisTreeReader::kR;
      fTreeReader->fR.push_back(temp);
      fStorageIndex = fTreeReader->fR.size() - 1;
    }
    else if (fType.EqualTo("TRefArray")) {
      TRefArray temp;
      fStorageID = AnalysisTreeReader::kRA;
      fTreeReader->fRA.push_back(temp);
      fStorageIndex = fTreeReader->fRA.size() - 1;
    }
  } // end if scalar
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec1D || fCArray1D) {
    // Test for bool types
    if (fTreeReader->fBool.count(fScalarType) != 0) {
      std::vector<bool> temp;
      fStorageID = AnalysisTreeReader::kvB;
      fTreeReader->fvB.push_back(temp);
      fStorageIndex = fTreeReader->fvB.size() - 1;
    }
    // Test for integer types
    else if (fTreeReader->fInteger.count(fScalarType) != 0 || 
             fTreeReader->fShort.count(fScalarType) != 0 ||
             fTreeReader->fLong.count(fScalarType) != 0 ||
             fTreeReader->fLLong.count(fScalarType) != 0 ||
             fTreeReader->fSChar.count(fScalarType) != 0) {
      std::vector<long long> temp;
      fStorageID = AnalysisTreeReader::kvI;
      fTreeReader->fvI.push_back(temp);
      fStorageIndex = fTreeReader->fvI.size() - 1;
    }
    // Test for counting types
    else if (fTreeReader->fUInteger.count(fScalarType) != 0 || 
             fTreeReader->fUShort.count(fScalarType) != 0 ||
             fTreeReader->fULong.count(fScalarType) != 0 ||
             fTreeReader->fULLong.count(fScalarType) != 0 ||
             fTreeReader->fUChar.count(fScalarType) != 0) {
      std::vector<unsigned long long> temp;
      fStorageID = AnalysisTreeReader::kvC;
      fTreeReader->fvC.push_back(temp);
      fStorageIndex = fTreeReader->fvC.size() - 1;
    }
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fScalarType) != 0 || 
             fTreeReader->fDouble.count(fScalarType) != 0 ||
             fTreeReader->fLDouble.count(fScalarType) != 0) {
      std::vector<long double> temp;
      fStorageID = AnalysisTreeReader::kvD;
      fTreeReader->fvD.push_back(temp);
      fStorageIndex = fTreeReader->fvD.size() - 1;
    }
    // Test for string types
    else if (fTreeReader->fChar.count(fScalarType) != 0 || 
             fScalarType.EqualTo("string") ||
             fScalarType.EqualTo("TString") ||
             fScalarType.EqualTo("TObjString")) {
      std::vector<TString> temp;
      fStorageID = AnalysisTreeReader::kvS;
      fTreeReader->fvS.push_back(temp);
      fStorageIndex = fTreeReader->fvS.size() - 1;
    }
    else if (fScalarType.EqualTo("TObjArray")) {
      std::vector<TObjArray> temp;
      fStorageID = AnalysisTreeReader::kvOA;
      fTreeReader->fvOA.push_back(temp);
      fStorageIndex = fTreeReader->fvOA.size() - 1;
    }
    else if (fScalarType.EqualTo("TClonesArray")) {
      std::vector<TClonesArray> temp;
      fStorageID = AnalysisTreeReader::kvCA;
      fTreeReader->fvCA.push_back(temp);
      fStorageIndex = fTreeReader->fvCA.size() - 1;
    }
    else if (fScalarType.EqualTo("TRef")) {
      std::vector<TRef> temp;
      fStorageID = AnalysisTreeReader::kvR;
      fTreeReader->fvR.push_back(temp);
      fStorageIndex = fTreeReader->fvR.size() - 1;
    }
    else if (fScalarType.EqualTo("TRefArray")) {
      std::vector<TRefArray> temp;
      fStorageID = AnalysisTreeReader::kvRA;
      fTreeReader->fvRA.push_back(temp);
      fStorageIndex = fTreeReader->fvRA.size() - 1;
    }
  } // end if 1D vector/c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec2D || fCArray2D) {
    // Test for bool types
    if (fTreeReader->fBool.count(fScalarType) != 0) {
      std::vector<std::vector<bool> > temp;
      fStorageID = AnalysisTreeReader::kvvB;
      fTreeReader->fvvB.push_back(temp);
      fStorageIndex = fTreeReader->fvvB.size() - 1;
    }
    // Test for integer types
    else if (fTreeReader->fInteger.count(fScalarType) != 0 || 
             fTreeReader->fShort.count(fScalarType) != 0 ||
             fTreeReader->fLong.count(fScalarType) != 0 ||
             fTreeReader->fLLong.count(fScalarType) != 0 ||
             fTreeReader->fSChar.count(fScalarType) != 0) {
      std::vector<std::vector<long long> > temp;
      fStorageID = AnalysisTreeReader::kvvI;
      fTreeReader->fvvI.push_back(temp);
      fStorageIndex = fTreeReader->fvvI.size() - 1;
    }
    // Test for counting types
    else if (fTreeReader->fUInteger.count(fScalarType) != 0 || 
             fTreeReader->fUShort.count(fScalarType) != 0 ||
             fTreeReader->fULong.count(fScalarType) != 0 ||
             fTreeReader->fULLong.count(fScalarType) != 0 ||
             fTreeReader->fUChar.count(fScalarType) != 0) {
      std::vector<std::vector<unsigned long long> > temp;
      fStorageID = AnalysisTreeReader::kvvC;
      fTreeReader->fvvC.push_back(temp);
      fStorageIndex = fTreeReader->fvvC.size() - 1;
    }
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fScalarType) != 0 || 
             fTreeReader->fDouble.count(fScalarType) != 0 ||
             fTreeReader->fLDouble.count(fScalarType) != 0) {
      std::vector<std::vector<long double> > temp;
      fStorageID = AnalysisTreeReader::kvvD;
      fTreeReader->fvvD.push_back(temp);
      fStorageIndex = fTreeReader->fvvD.size() - 1;
    }
    // Test for string types
    else if (fTreeReader->fChar.count(fScalarType) != 0 || 
             fScalarType.EqualTo("string") ||
             fScalarType.EqualTo("TString") ||
             fScalarType.EqualTo("TObjString")) {
      std::vector<std::vector<TString> > temp;
      fStorageID = AnalysisTreeReader::kvvS;
      fTreeReader->fvvS.push_back(temp);
      fStorageIndex = fTreeReader->fvvS.size() - 1;
    }
    else if (fScalarType.EqualTo("TRef")) {
      std::vector<std::vector<TRef> > temp;
      fStorageID = AnalysisTreeReader::kvvR;
      fTreeReader->fvvR.push_back(temp);
      fStorageIndex = fTreeReader->fvvR.size() - 1;
    }
  } // end if 2D vector/c-array
  // //////////////////////////////////////////////////
  // Determine the scalar type for fast retrieval later
  // //////////////////////////////////////////////////
  if (fScalar) {
    // Test for bool types
    if (fTreeReader->fBool.count(fType) != 0) 
      fIsB = true;
    // Test for integer types
    else if (fTreeReader->fInteger.count(fType) != 0) 
      fIsI = true;
    else if (fTreeReader->fShort.count(fType) != 0) 
      fIsSI = true;
    else if (fTreeReader->fLong.count(fType) != 0) 
      fIsL = true;
    else if (fTreeReader->fLLong.count(fType) != 0) 
      fIsLL = true;
    else if (fTreeReader->fSChar.count(fType) != 0) 
      fIsSC = true;
    // Test for counting types
    else if (fTreeReader->fUInteger.count(fType) != 0) 
      fIsUI = true;
    else if (fTreeReader->fUShort.count(fType) != 0) 
      fIsUSI = true;
    else if (fTreeReader->fULong.count(fType) != 0) 
      fIsUL = true;
    else if (fTreeReader->fULLong.count(fType) != 0) 
      fIsULL = true;
    else if (fTreeReader->fUChar.count(fType) != 0) 
      fIsUC = true;
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fType) != 0)
      fIsF = true;
    else if (fTreeReader->fDouble.count(fType) != 0) 
      fIsD = true;
    else if (fTreeReader->fLDouble.count(fType) != 0) 
      fIsLD = true;
    // Test for string types
    else if (fTreeReader->fChar.count(fType) != 0) 
      fIsC = true;
    else if (fType.EqualTo("string")) 
      fIsstdS = true;
    else if (fType.EqualTo("TString")) 
      fIsTS = true;
    else if (fType.EqualTo("TObjString")) 
      fIsTOS = true;
    else if (fType.EqualTo("TClonesArray"))
      fIsTCA = true;
    else if (fType.EqualTo("TRef")) 
      fIsTR = true;
    else if (fType.EqualTo("TRefArray"))
      fIsTRA = true;
  } // end if scalar
  else if (fVec1D || fCArray1D || fVec2D || fCArray2D) {
    // Test for bool types
    if (fTreeReader->fBool.count(fScalarType) != 0) 
      fIsB = true;
    // Test for integer types
    else if (fTreeReader->fInteger.count(fScalarType) != 0) 
      fIsI = true;
    else if (fTreeReader->fShort.count(fScalarType) != 0) 
      fIsSI = true;
    else if (fTreeReader->fLong.count(fScalarType) != 0) 
      fIsL = true;
    else if (fTreeReader->fLLong.count(fScalarType) != 0) 
      fIsLL = true;
    else if (fTreeReader->fSChar.count(fScalarType) != 0) 
      fIsSC = true;
    // Test for counting types
    else if (fTreeReader->fUInteger.count(fScalarType) != 0) 
      fIsUI = true;
    else if (fTreeReader->fUShort.count(fScalarType) != 0) 
      fIsUSI = true;
    else if (fTreeReader->fULong.count(fScalarType) != 0) 
      fIsUL = true;
    else if (fTreeReader->fULLong.count(fScalarType) != 0) 
      fIsULL = true;
    else if (fTreeReader->fUChar.count(fScalarType) != 0) 
      fIsUC = true;
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fScalarType) != 0) 
      fIsF = true;
    else if (fTreeReader->fDouble.count(fScalarType) != 0) 
      fIsD = true;
    else if (fTreeReader->fLDouble.count(fScalarType) != 0) 
      fIsLD = true;
    // Test for string types
    else if (fTreeReader->fChar.count(fScalarType) != 0) 
      fIsC = true;
    else if (fScalarType.EqualTo("string")) 
      fIsstdS = true;
    else if (fScalarType.EqualTo("TString")) 
      fIsTS = true;
    else if (fScalarType.EqualTo("TObjString")) 
      fIsTOS = true;
    else if (fScalarType.EqualTo("TObjArray")) 
      fIsTOA = true;
    else if (fScalarType.EqualTo("TClonesArray")) 
      fIsTCA = true;
    else if (fScalarType.EqualTo("TRef")) 
      fIsTR = true;
    else if (fScalarType.EqualTo("TRefArray"))
      fIsTRA = true;
  } // end if 1D vector/c-array
  else
    return kFALSE;
  // ////////////////////////////////////////////////////
  // ////////////////////////////////////////////////////

  Init();
  SetEntry(fTreeReader->fEntry);

  return kTRUE;
}


// ////////////////////////////////////////////////////
// More tediousness
// ////////////////////////////////////////////////////

void internal::BranchManager::Init () {

  fBranch = fTreeReader->fChain->GetBranch(fBranchName.Data());
  if (fBranch == nullptr)
    throw HALException(fBranchName.Prepend("Couldn't find branch: "));
  fFileName = fTreeReader->fChain->GetCurrentFile()->GetName();

  if (fScalar) {
    if (fIsB)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fB, &fBranch);
    else if (fIsI)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fI, &fBranch);
    else if (fIsSI)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fSI, &fBranch);
    else if (fIsL)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fL, &fBranch);
    else if (fIsLL)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fLL, &fBranch);
    else if (fIsSC)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fSC, &fBranch);
    else if (fIsUI)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fUI, &fBranch);
    else if (fIsUSI)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fUSI, &fBranch);
    else if (fIsUL)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fUL, &fBranch);
    else if (fIsULL)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fULL, &fBranch);
    else if (fIsUC)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fUC, &fBranch);
    else if (fIsF)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fF, &fBranch);
    else if (fIsD)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fD, &fBranch);
    else if (fIsLD)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fLD, &fBranch);
    else if (fIsC)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fC, &fBranch);
    else if (fIsstdS)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fstdS, &fBranch);
    else if (fIsTS)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fTS, &fBranch);
    else if (fIsTOS)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fTOS, &fBranch);
    else if (fIsTOA)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fTOA, &fBranch);
    else if (fIsTCA)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fTCA, &fBranch);
    else if (fIsTR)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fTR, &fBranch);
    else if (fIsTRA)
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fTRA, &fBranch);
  } // end if scalar
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fCArray1D) {
    if (fIsB) {
      fcB = new bool[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcB, &fBranch);
    }
    else if (fIsI) {
      //fcI = nullptr;
      fcI = new int[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcI, &fBranch);
    }
    else if (fIsSI) {
      fcSI = new short int[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcSI, &fBranch);
    }
    else if (fIsL) {
      fcL = new long[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcL, &fBranch);
    }
    else if (fIsLL) {
      fcLL = new long long[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcLL, &fBranch);
    }
    else if (fIsSC) {
      fcSC = new signed char[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcSC, &fBranch);
    }
    else if (fIsUI) {
      fcUI = new unsigned int[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcUI, &fBranch);
    }
    else if (fIsUSI) {
      fcUSI = new unsigned short int[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcUSI, &fBranch);
    }
    else if (fIsUL) {
      fcUL = new unsigned long[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcUL, &fBranch);
    }
    else if (fIsULL) {
      fcULL = new unsigned long long[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcULL, &fBranch);
    }
    else if (fIsUC) {
      fcUC = new unsigned char[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcUC, &fBranch);
    }
    else if (fIsF) {
      //fcF = nullptr;
      fcF = new float[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcF, &fBranch);
    }
    else if (fIsD) {
      fcD = new double[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcD, &fBranch);
    }
    else if (fIsLD) {
      fcLD = new long double[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcLD, &fBranch);
    }
    else if (fIsC) {
      fcC = new char[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcC, &fBranch);
    }
    else if (fIsstdS) {
      fcstdS = new std::string[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcstdS, &fBranch);
    }
    else if (fIsTS) {
      fcTS = new TString[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTS, &fBranch);
    }
    else if (fIsTOS) {
      fcTOS = new TObjString[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTOS, &fBranch);
    }
    else if (fIsTOA) {
      fcTOA = new TObjArray[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTOA, &fBranch);
    }
    else if (fIsTCA) {
      fcTCA = new TClonesArray[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTCA, &fBranch);
    }
    else if (fIsTR) {
      fcTR = new TRef[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTR, &fBranch);
    }
    else if (fIsTRA) {
      fcTRA = new TRefArray[fMaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTRA, &fBranch);
    }
  } // end if 1D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fCArray2D) {
    if (fIsB) {
      fccB = new bool*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccB[i] = new bool[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccB, &fBranch);
    }
    else if (fIsI) {
      fccI = new int*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccI[i] = new int[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccI, &fBranch);
    }
    else if (fIsSI) {
      fccSI = new short int*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccSI[i] = new short int[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccSI, &fBranch);
    }
    else if (fIsL) {
      fccL = new long*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccL[i] = new long[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccL, &fBranch);
    }
    else if (fIsLL) {
      fccLL = new long long*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccLL[i] = new long long[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccLL, &fBranch);
    }
    else if (fIsSC) {
      fccSC = new signed char*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccSC[i] = new signed char[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccSC, &fBranch);
    }
    else if (fIsUI) {
      fccUI = new unsigned int*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccUI[i] = new unsigned int[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccUI, &fBranch);
    }
    else if (fIsUSI) {
      fccUSI = new unsigned short int*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccUSI[i] = new unsigned short int[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccUSI, &fBranch);
    }
    else if (fIsUL) {
      fccUL = new unsigned long*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccUL[i] = new unsigned long[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccUL, &fBranch);
    }
    else if (fIsULL) {
      fccULL = new unsigned long long*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccULL[i] = new unsigned long long[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccULL, &fBranch);
    }
    else if (fIsUC) {
      fccUC = new unsigned char*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccUC[i] = new unsigned char[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccUC, &fBranch);
    }
    else if (fIsF) {
      fccF = new float*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccF[i] = new float[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccF, &fBranch);
    }
    else if (fIsD) {
      fccD = new double*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccD[i] = new double[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccD, &fBranch);
    }
    else if (fIsLD) {
      fccLD = new long double*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccLD[i] = new long double[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccLD, &fBranch);
    }
    else if (fIsC) {
      fccC = new char*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccC[i] = new char[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccC, &fBranch);
    }
    else if (fIsstdS) {
      fccstdS = new std::string*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccstdS[i] = new std::string[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccstdS, &fBranch);
    }
    else if (fIsTS) {
      fccTS = new TString*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccTS[i] = new TString[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccTS, &fBranch);
    }
    else if (fIsTOS) {
      fccTOS = new TObjString*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccTOS[i] = new TObjString[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccTOS, &fBranch);
    }
    else if (fIsTR) {
      fccTR = new TRef*[fMaxBufferLength];
      for (int i = 0; i < fMaxBufferLength; ++i) fccTR[i] = new TRef[fMaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccTR, &fBranch);
    }
  } // end if 2D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec1D) {
    if (fIsB) {
      fvB = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvB, &fBranch);
    }
    else if (fIsI) {
      fvI = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvI, &fBranch);
    }
    else if (fIsSI) {
      fvSI = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvSI, &fBranch);
    }
    else if (fIsL) {
      fvL = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvL, &fBranch);
    }
    else if (fIsLL) {
      fvLL = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvLL, &fBranch);
    }
    else if (fIsSC) {
      fvSC = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvSC, &fBranch);
    }
    else if (fIsUI) {
      fvUI = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvUI, &fBranch);
    }
    else if (fIsUSI) {
      fvUSI = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvUSI, &fBranch);
    }
    else if (fIsUL) {
      fvUL = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvUL, &fBranch);
    }
    else if (fIsULL) {
      fvULL = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvULL, &fBranch);
    }
    else if (fIsUC) {
      fvUC = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvUC, &fBranch);
    }
    else if (fIsF) {
      fvF = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvF, &fBranch);
    }
    else if (fIsD) {
      fvD = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvD, &fBranch);
    }
    else if (fIsLD) {
      fvLD = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvLD, &fBranch);
    }
    else if (fIsC) {
      fvC = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvC, &fBranch);
    }
    else if (fIsstdS) {
      fvstdS = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvstdS, &fBranch);
    }
    else if (fIsTS) {
      fvTS = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTS, &fBranch);
    }
    else if (fIsTOS) {
      fvTOS = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTOS, &fBranch);
    }
    else if (fIsTOA) {
      fvTOA = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTOA, &fBranch);
    }
    else if (fIsTCA) {
      fvTCA = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTCA, &fBranch);
    }
    else if (fIsTR) {
      fvTR = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTR, &fBranch);
    }
    else if (fIsTRA) {
      fvTRA = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTRA, &fBranch);
    }
  } // end if 1D vector
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec2D) {
    if (fIsB) {
      fvvB = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvB, &fBranch);
    }
    else if (fIsI) {
      fvvI = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvI, &fBranch);
    }
    else if (fIsSI) {
      fvvSI = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvSI, &fBranch);
    }
    else if (fIsL) {
      fvvL = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvL, &fBranch);
    }
    else if (fIsLL) {
      fvvLL = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvLL, &fBranch);
    }
    else if (fIsSC) {
      fvvSC = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvSC, &fBranch);
    }
    else if (fIsUI) {
      fvvUI = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvUI, &fBranch);
    }
    else if (fIsUSI) {
      fvvUSI = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvUSI, &fBranch);
    }
    else if (fIsUL) {
      fvvUL = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvUL, &fBranch);
    }
    else if (fIsULL) {
      fvvULL = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvULL, &fBranch);
    }
    else if (fIsUC) {
      fvvUC = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvUC, &fBranch);
    }
    else if (fIsF) {
      fvvF = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvF, &fBranch);
    }
    else if (fIsD) {
      fvvD = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvD, &fBranch);
    }
    else if (fIsLD) {
      fvvLD = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvLD, &fBranch);
    }
    else if (fIsC) {
      fvvC = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvC, &fBranch);
    }
    else if (fIsstdS) {
      fvvstdS = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvstdS, &fBranch);
    }
    else if (fIsTS) {
      fvvTS = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvTS, &fBranch);
    }
    else if (fIsTOS) {
      fvvTOS = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvTOS, &fBranch);
    }
    else if (fIsTR) {
      fvvTR = nullptr;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvTR, &fBranch);
    }
  } // end if 2D vector
  // ////////////////////////////////////////////////////
  // ////////////////////////////////////////////////////
}


void internal::BranchManager::SetEntry (Long64_t entry) {

  if (fScalar) {
    fBranch->GetEntry(entry);
    if (fIsB)
      fTreeReader->fB[fStorageIndex] = fB;
    else if (fIsI)
      fTreeReader->fI[fStorageIndex] = fI;
    else if (fIsSI)
      fTreeReader->fI[fStorageIndex] = fSI;
    else if (fIsL)
      fTreeReader->fI[fStorageIndex] = fL;
    else if (fIsLL)
      fTreeReader->fI[fStorageIndex] = fLL;
    else if (fIsSC)
      fTreeReader->fI[fStorageIndex] = fSC;
    else if (fIsUI)
      fTreeReader->fC[fStorageIndex] = fUI;
    else if (fIsUSI)
      fTreeReader->fC[fStorageIndex] = fUSI;
    else if (fIsUL)
      fTreeReader->fC[fStorageIndex] = fUL;
    else if (fIsULL)
      fTreeReader->fC[fStorageIndex] = fULL;
    else if (fIsUC)
      fTreeReader->fC[fStorageIndex] = fUC;
    else if (fIsF)
      fTreeReader->fD[fStorageIndex] = fF;
    else if (fIsD)
      fTreeReader->fD[fStorageIndex] = fD;
    else if (fIsLD)
      fTreeReader->fD[fStorageIndex] = fLD;
    else if (fIsC)
      fTreeReader->fS[fStorageIndex] = fC;
    else if (fIsstdS)
      fTreeReader->fS[fStorageIndex] = fstdS;
    else if (fIsTS)
      fTreeReader->fS[fStorageIndex] = fTS;
    else if (fIsTOS)
      fTreeReader->fS[fStorageIndex] = fTOS.String();
    else if (fIsTOA)
      fTreeReader->fOA[fStorageIndex] = fTOA;
    else if (fIsTCA)
      fTreeReader->fCA[fStorageIndex] = fTCA;
    else if (fIsTR)
      fTreeReader->fR[fStorageIndex] = fTR;
    else if (fIsTRA)
      fTreeReader->fRA[fStorageIndex] = fTRA;
  } // end if scalar
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fCArray1D) {
    Int_t n = GetArrayLength(1);
    if (fIsB) {
      fBranch->GetEntry(entry);
      fTreeReader->fvB[fStorageIndex].clear();
      fTreeReader->fvB[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvB[fStorageIndex].push_back(fcB[i]);
    }
    else if (fIsI) {
      //if (fcI == nullptr) {
      //  if (n != 0) n *= 50;
      //  else n = fMaxBufferLength;
      //  fcI = new int[n];
      //  fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcI, &fBranch);
      //  std::cout << "Allocated int array of size " << n << " for " << fBranchName << std::endl;
      //}
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcI[i]);
    }
    else if (fIsSI) {
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcSI[i]);
    }
    else if (fIsL) {
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcL[i]);
    }
    else if (fIsLL) {
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcLL[i]);
    }
    else if (fIsSC) {
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcSC[i]);
    }
    else if (fIsUI) {
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUI[i]);
    }
    else if (fIsUSI) {
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUSI[i]);
    }
    else if (fIsUL) {
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUL[i]);
    }
    else if (fIsULL) {
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcULL[i]);
    }
    else if (fIsUC) {
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUC[i]);
    }
    else if (fIsF) {
      //if (fcF == nullptr) {
      //  if (n != 0) n *= 50;
      //  else n = fMaxBufferLength;
      //  fcF = new float[n];
      //  fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcF, &fBranch);
      //  std::cout << "Allocated int array of size " << n << " for " << fBranchName << std::endl;
      //}
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(fcF[i]);
    }
    else if (fIsD) {
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(fcD[i]);
    }
    else if (fIsLD) {
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(fcLD[i]);
    }
    else if (fIsC) {
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcC[i]);
    }
    else if (fIsstdS) {
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcstdS[i].c_str());
    }
    else if (fIsTS) {
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcTS[i]);
    }
    else if (fIsTOS) {
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcTOS[i].String());
    }
    else if (fIsTOA) {
      fBranch->GetEntry(entry);
      fTreeReader->fvOA[fStorageIndex].clear();
      fTreeReader->fvOA[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvOA[fStorageIndex].push_back(fcTOA[i]);
    }
    else if (fIsTCA) {
      fBranch->GetEntry(entry);
      fTreeReader->fvCA[fStorageIndex].clear();
      fTreeReader->fvCA[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvCA[fStorageIndex].push_back(fcTCA[i]);
    }
    else if (fIsTR) {
      fBranch->GetEntry(entry);
      fTreeReader->fvR[fStorageIndex].clear();
      fTreeReader->fvR[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvR[fStorageIndex].push_back(fcTR[i]);
    }
    else if (fIsTRA) {
      fBranch->GetEntry(entry);
      fTreeReader->fvRA[fStorageIndex].clear();
      fTreeReader->fvRA[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvRA[fStorageIndex].push_back(fcTRA[i]);
    }
  } // end if 1D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fCArray2D) {
    Int_t n = GetArrayLength(1);
    Int_t m = GetArrayLength(2);
    if (fIsB) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvB[fStorageIndex].clear();
      fTreeReader->fvvB[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<bool> row;
        fTreeReader->fvvB[fStorageIndex].push_back(row);
        fTreeReader->fvvB[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvB[fStorageIndex].back().push_back(fccB[i][j]);
      }
    }
    else if (fIsI) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(fccI[i][j]);
      }
    }
    else if (fIsSI) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(fccSI[i][j]);
      }
    }
    else if (fIsL) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(fccL[i][j]);
      }
    }
    else if (fIsLL) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(fccLL[i][j]);
      }
    }
    else if (fIsSC) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(fccSC[i][j]);
      }
    }
    else if (fIsUI) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(fccUI[i][j]);
      }
    }
    else if (fIsUSI) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(fccUSI[i][j]);
      }
    }
    else if (fIsUL) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(fccUL[i][j]);
      }
    }
    else if (fIsULL) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(fccULL[i][j]);
      }
    }
    else if (fIsUC) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(fccUC[i][j]);
      }
    }
    else if (fIsF) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvD[fStorageIndex].clear();
      fTreeReader->fvvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<long double> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        fTreeReader->fvvD[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvD[fStorageIndex].back().push_back(fccF[i][j]);
      }
    }
    else if (fIsD) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvD[fStorageIndex].clear();
      fTreeReader->fvvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<long double> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        fTreeReader->fvvD[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvD[fStorageIndex].back().push_back(fccD[i][j]);
      }
    }
    else if (fIsLD) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvD[fStorageIndex].clear();
      fTreeReader->fvvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<long double> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        fTreeReader->fvvD[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvD[fStorageIndex].back().push_back(fccLD[i][j]);
      }
    }
    else if (fIsC) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvS[fStorageIndex].back().push_back(fccC[i][j]);
      }
    }
    else if (fIsstdS) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvS[fStorageIndex].back().push_back(fccstdS[i][j].c_str());
      }
    }
    else if (fIsTS) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvS[fStorageIndex].back().push_back(fccTS[i][j]);
      }
    }
    else if (fIsTOS) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvS[fStorageIndex].back().push_back(fccTOS[i][j].String());
      }
    }
    else if (fIsTR) {
      fBranch->GetEntry(entry);
      fTreeReader->fvvR[fStorageIndex].clear();
      fTreeReader->fvvR[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TRef> row;
        fTreeReader->fvvR[fStorageIndex].push_back(row);
        fTreeReader->fvvR[fStorageIndex].back().reserve(m);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvR[fStorageIndex].back().push_back(fccTR[i][j]);
      }
    }
  }  //end if 2D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec1D) {
    fBranch->GetEntry(entry);
    if (fIsB) {
      unsigned n = fvB->size();
      fTreeReader->fvB[fStorageIndex].clear();
      fTreeReader->fvB[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( bool it, *fvB )
#else
      for (auto it: *fvB)
#endif
        fTreeReader->fvB[fStorageIndex].push_back(it);
    }
    else if (fIsI) {
      unsigned n = fvI->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( int it, *fvI )
#else
      for (auto it: *fvI)
#endif
        fTreeReader->fvI[fStorageIndex].push_back(it);
    }
    else if (fIsSI) {
      unsigned n = fvSI->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( short int it, *fvSI )
#else
      for (auto it: *fvSI)
#endif
        fTreeReader->fvI[fStorageIndex].push_back(it);
    }
    else if (fIsL) {
      unsigned n = fvL->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( long int it, *fvL )
#else
      for (auto it: *fvL)
#endif
        fTreeReader->fvI[fStorageIndex].push_back(it);
    }
    else if (fIsLL) {
      unsigned n = fvLL->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( long long int it, *fvLL )
#else
      for (auto it: *fvLL)
#endif
        fTreeReader->fvI[fStorageIndex].push_back(it);
    }
    else if (fIsSC) {
      unsigned n = fvSC->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( signed char it, *fvSC )
#else
      for (auto it: *fvSC)
#endif
        fTreeReader->fvI[fStorageIndex].push_back(it);
    }
    else if (fIsUI) {
      unsigned n = fvUI->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( unsigned int it, *fvUI )
#else
      for (auto it: *fvUI)
#endif
        fTreeReader->fvC[fStorageIndex].push_back(it);
    }
    else if (fIsUSI) {
      unsigned n = fvUSI->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( unsigned short int it, *fvUSI )
#else
      for (auto it: *fvUSI)
#endif
        fTreeReader->fvC[fStorageIndex].push_back(it);
    }
    else if (fIsUL) {
      unsigned n = fvUL->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( unsigned long int it, *fvUL )
#else
      for (auto it: *fvUL)
#endif
        fTreeReader->fvC[fStorageIndex].push_back(it);
    }
    else if (fIsULL) {
      unsigned n = fvULL->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( unsigned long long int it, *fvULL )
#else
      for (auto it: *fvULL)
#endif
        fTreeReader->fvC[fStorageIndex].push_back(it);
    }
    else if (fIsUC) {
      unsigned n = fvUC->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( unsigned char it, *fvUC )
#else
      for (auto it: *fvUC)
#endif
        fTreeReader->fvC[fStorageIndex].push_back(it);
    }
    else if (fIsF) {
      unsigned n = fvF->size();
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( float it, *fvF )
#else
      for (auto it: *fvF)
#endif
        fTreeReader->fvD[fStorageIndex].push_back(it);
    }
    else if (fIsD) {
      unsigned n = fvD->size();
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( double it, *fvD )
#else
      for (auto it: *fvD)
#endif
        fTreeReader->fvD[fStorageIndex].push_back(it);
    }
    else if (fIsLD) {
      unsigned n = fvLD->size();
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( long double it, *fvLD )
#else
      for (auto it: *fvLD)
#endif
        fTreeReader->fvD[fStorageIndex].push_back(it);
    }
    else if (fIsC) {
      unsigned n = fvC->size();
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( char it, *fvC )
#else
      for (auto it: *fvC)
#endif
        fTreeReader->fvS[fStorageIndex].push_back(it);
    }
    else if (fIsstdS) {
      unsigned n = fvstdS->size();
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::string it, *fvstdS )
#else
      for (auto it: *fvstdS)
#endif
        fTreeReader->fvS[fStorageIndex].push_back(it.c_str());
    }
    else if (fIsTS) {
      unsigned n = fvTS->size();
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( TString it, *fvTS )
#else
      for (auto it: *fvTS)
#endif
        fTreeReader->fvS[fStorageIndex].push_back(it);
    }
    else if (fIsTOS) {
      unsigned n = fvTOS->size();
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( TObjString it, *fvTOS )
#else
      for (auto it: *fvTOS)
#endif
        fTreeReader->fvS[fStorageIndex].push_back(it.String());
    }
    else if (fIsTOA) {
      unsigned n = fvTOA->size();
      fTreeReader->fvOA[fStorageIndex].clear();
      fTreeReader->fvOA[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( TObjArray it, *fvTOA )
#else
      for (auto it: *fvTOA)
#endif
        fTreeReader->fvOA[fStorageIndex].push_back(it);
    }
    else if (fIsTCA) {
      unsigned n = fvTCA->size();
      fTreeReader->fvCA[fStorageIndex].clear();
      fTreeReader->fvCA[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( TClonesArray it, *fvTCA )
#else
      for (auto it: *fvTCA)
#endif
        fTreeReader->fvCA[fStorageIndex].push_back(it);
    }
    else if (fIsTR) {
      unsigned n = fvTR->size();
      fTreeReader->fvR[fStorageIndex].clear();
      fTreeReader->fvR[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( TRef it, *fvTR )
#else
      for (auto it: *fvTR)
#endif
        fTreeReader->fvR[fStorageIndex].push_back(it);
    }
    else if (fIsTRA) {
      unsigned n = fvTRA->size();
      fTreeReader->fvRA[fStorageIndex].clear();
      fTreeReader->fvRA[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( TRefArray it, *fvTRA )
#else
      for (auto it: *fvTRA)
#endif
        fTreeReader->fvRA[fStorageIndex].push_back(it);
    }
  } // end if 1D vector
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec2D) {
    fBranch->GetEntry(entry);
    if (fIsB) {
      unsigned n = fvvB->size();
      fTreeReader->fvvB[fStorageIndex].clear();
      fTreeReader->fvvB[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<bool> iit, *fvvB )
#else
      for (auto iit: *fvvB)
#endif
      {
        unsigned m = iit.size();
        std::vector<bool> row;
        fTreeReader->fvvB[fStorageIndex].push_back(row);
        fTreeReader->fvvB[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( bool it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvB[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsI) {
      unsigned n = fvvI->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<int> iit, *fvvI )
#else
      for (auto iit: *fvvI)
#endif
      {
        unsigned m = iit.size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( int it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvI[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsSI) {
      unsigned n = fvvSI->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<short int> iit, *fvvSI )
#else
      for (auto iit: *fvvSI)
#endif
      {
        unsigned m = iit.size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( short int it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvI[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsL) {
      unsigned n = fvvL->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<long int> iit, *fvvL )
#else
      for (auto iit: *fvvL)
#endif
      {
        unsigned m = iit.size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( long int it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvI[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsLL) {
      unsigned n = fvvLL->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<long long int> iit, *fvvLL )
#else
      for (auto iit: *fvvLL)
#endif
      {
        unsigned m = iit.size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( long long int it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvI[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsSC) {
      unsigned n = fvvSC->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<signed char> iit, *fvvSC )
#else
      for (auto iit: *fvvSC)
#endif
      {
        unsigned m = iit.size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( signed char it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvI[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsUI) {
      unsigned n = fvvUI->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<unsigned int> iit, *fvvUI )
#else
      for (auto iit: *fvvUI)
#endif
      {
        unsigned m = iit.size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( unsigned int it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvC[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsUSI) {
      unsigned n = fvvUSI->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<unsigned short int> iit, *fvvUSI )
#else
      for (auto iit: *fvvUSI)
#endif
      {
        unsigned m = iit.size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( unsigned short int it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvC[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsUL) {
      unsigned n = fvvUL->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<unsigned long int> iit, *fvvUL )
#else
      for (auto iit: *fvvUL)
#endif
      {
        unsigned m = iit.size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( unsigned long int it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvC[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsULL) {
      unsigned n = fvvULL->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<unsigned long long int> iit, *fvvULL )
#else
      for (auto iit: *fvvULL)
#endif
      {
        unsigned m = iit.size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( unsigned long long int it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvC[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsUC) {
      unsigned n = fvvUC->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<unsigned char> iit, *fvvUC )
#else
      for (auto iit: *fvvUC)
#endif
      {
        unsigned m = iit.size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( unsigned char it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvC[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsF) {
      unsigned n = fvvF->size();
      fTreeReader->fvvD[fStorageIndex].clear();
      fTreeReader->fvvD[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<float> iit, *fvvF )
#else
      for (auto iit: *fvvF)
#endif
      {
        unsigned m = iit.size();
        std::vector<long double> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        fTreeReader->fvvD[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( float it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvD[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsD) {
      unsigned n = fvvD->size();
      fTreeReader->fvvD[fStorageIndex].clear();
      fTreeReader->fvvD[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<double> iit, *fvvD )
#else
      for (auto iit: *fvvD)
#endif
      {
        unsigned m = iit.size();
        std::vector<long double> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        fTreeReader->fvvD[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( double it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvD[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsLD) {
      unsigned n = fvvLD->size();
      fTreeReader->fvvD[fStorageIndex].clear();
      fTreeReader->fvvD[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<long double> iit, *fvvLD )
#else
      for (auto iit: *fvvLD)
#endif
      {
        unsigned m = iit.size();
        std::vector<long double> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        fTreeReader->fvvD[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( long double it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvD[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsC) {
      unsigned n = fvvC->size();
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<char> iit, *fvvC )
#else
      for (auto iit: *fvvC)
#endif
      {
        unsigned m = iit.size();
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( char it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvS[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsstdS) {
      unsigned n = fvvstdS->size();
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<std::string> iit, *fvvstdS )
#else
      for (auto iit: *fvvstdS)
#endif
      {
        unsigned m = iit.size();
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( std::string it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvS[fStorageIndex].back().push_back(it.c_str());
      }
    }
    else if (fIsTS) {
      unsigned n = fvvTS->size();
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<TString> iit, *fvvTS )
#else
      for (auto iit: *fvvTS)
#endif
      {
        unsigned m = iit.size();
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( TString it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvS[fStorageIndex].back().push_back(it);
      }
    }
    else if (fIsTOS) {
      unsigned n = fvvTOS->size();
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<TObjString> iit, *fvvTOS )
#else
      for (auto iit: *fvvTOS)
#endif
      {
        unsigned m = iit.size();
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( TObjString it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvS[fStorageIndex].back().push_back(it.String());
      }
    }
    else if (fIsTR) {
      unsigned n = fvvTR->size();
      fTreeReader->fvvR[fStorageIndex].clear();
      fTreeReader->fvvR[fStorageIndex].reserve(n);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
      BOOST_FOREACH( std::vector<TRef> iit, *fvvTR )
#else
      for (auto iit: *fvvTR)
#endif
      {
        unsigned m = iit.size();
        std::vector<TRef> row;
        fTreeReader->fvvR[fStorageIndex].push_back(row);
        fTreeReader->fvvR[fStorageIndex].back().reserve(m);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
        BOOST_FOREACH( TRef it, iit )
#else
        for (auto it: iit)
#endif
          fTreeReader->fvvR[fStorageIndex].back().push_back(it);
      }
    }
  } // end if 2D vector
  // ////////////////////////////////////////////////////
  // ////////////////////////////////////////////////////
}

// ///////////////////////////////////////
// ///////////////////////////////////////

Int_t internal::BranchManager::GetArrayLength (Int_t rank) {
  TString size;
  if (fLeafTitle.Contains(fTreeReader->fArray2D) || fLeafTitle.Contains(fTreeReader->fArray))
    size = fLeafTitle;
  else if (fBranchTitle.Contains(fTreeReader->fArray2D) || fBranchTitle.Contains(fTreeReader->fArray))
    size = fBranchTitle;
  else
    return fLeafNdata;

  for (Int_t i = 0; i < rank; ++i)
    size.Remove(0, size.First('[') + 1);
  size.Remove(size.First(']'), size.Length());

  // size is just a static number, so return it
  if (size.Contains(TRegexp("[^a-zA-Z_]")))
    return size.Atoi();

  return fTreeReader->GetInteger(size);
}

Int_t internal::BranchManager::GetMaxArrayLength (Int_t rank) {
  Long64_t nentries = fTreeReader->fChain->GetEntries();
  Long64_t current_entry = fTreeReader->fEntry;
  Int_t max_length = 0;

  if (rank == 1) {
    for (Long64_t i = current_entry; i < nentries; ++i) {
      fTreeReader->SetEntry(i);
      Int_t n = GetArrayLength(1);

      if (n > max_length)
        max_length = n;
    }
    fTreeReader->SetEntry(current_entry);
    return max_length;
  }
  else if (rank == 2)
    return GetArrayLength(2); // 2D arrays can only be static
  throw HALException("rank may only be 1 or 2");
}

void internal::BranchManager::FindTypeInformation() {
  TLeaf *l = nullptr;

  // branch name = leaf name
  if (fBranch->FindLeaf(fBranchName.Data()))
    l = fBranch->GetLeaf(fBranchName.Data());
  else { // take branch's (only) leaf and get type info from it
    TObjArray *ll = fBranch->GetListOfLeaves();
    if (ll->GetEntries() != 1)
      throw HALException(fBranchName.Append(": This branch has too many leaves. Can't find data."));
    l = static_cast<TLeaf*>(ll->At(0));
  }
  
  fType = l->GetTypeName();
  fLeafTitle = l->GetTitle();
  fLeafNdata = l->GetNdata();

  // Determine scalar, c-array, and  vector
  if (fLeafTitle.Contains(fTreeReader->fArray) || fBranchTitle.Contains(fTreeReader->fArray) || fLeafNdata > 1)
    fCArray1D = kTRUE;
  else if(fLeafTitle.Contains(fTreeReader->fArray2D) || fBranchTitle.Contains(fTreeReader->fArray2D))
    fCArray2D = kTRUE;
  else if (fType.Contains(fTreeReader->fVector))
    fVec1D = kTRUE;
  else if (fType.Contains(fTreeReader->fVector2D))
    fVec2D = kTRUE;
  else if (fType.Contains(fTreeReader->fScalar))
    fScalar = kTRUE;

  if (!fScalar && !fCArray1D && !fCArray2D && !fVec1D && !fVec2D)
    throw HALException(fBranchName.Prepend("Coundn't determine meta-type of branch: "));

  // Determine the scalar type of vector
  fScalarType = fType;
  if (!fScalar) { // not pure scalar
    if (fScalarType.Contains(fTreeReader->fVector) || fScalarType.Contains(fTreeReader->fVector2D)) {
      fScalarType.Remove(fScalarType.First('>'), fScalarType.Length());
      fScalarType.Remove(0, fScalarType.Last('<') + 1);
    }
  }

  return;
}

//#else

//#endif // Version

} /* HAL */ 
