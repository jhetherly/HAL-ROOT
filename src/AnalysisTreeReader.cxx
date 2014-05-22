#include <HAL/AnalysisTreeReader.h>
//#include <iostream>

ClassImp(HAL::AnalysisTreeReader);

namespace HAL {

AnalysisTreeReader::AnalysisTreeReader (TTree *t) : fChain(t), 
  fScalar("^[a-zA-Z][a-zA-Z0-9_]+$"),
  fVector("^vector[ ]*<[ ]*[a-zA-Z][a-zA-Z0-9_]+[ ]*>$"), // vector<scalar>
  fVector2D("^vector[ ]*<[ ]*vector[ ]*<[ ]*[a-zA-Z][a-zA-Z0-9_]+[ ]*>[ ]*>$"), // vector<vector<scalar> >
  fArray("^[a-zA-Z_][a-zA-Z0-9_]*[[a-zA-Z0-9_]+]$"), // name[i]
  fArray2D("^[a-zA-Z_][a-zA-Z0-9_]*[[a-zA-Z0-9_]+][[a-zA-Z0-9_]+]$")//, /* name[i][j] */ 
  /*fDirector(t, -1)*/ { 

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

AnalysisTreeReader::~AnalysisTreeReader () {
}

void AnalysisTreeReader::SetEntry (Long64_t entry) {
  std::set<BranchManager*> unique_bms;

  fEntry = entry;

  // Update all branch proxies first
  //fDirector.SetReadEntry(entry);

  // Update all branches
  for (std::map<TString, BranchManager*>::iterator bm = fNickNameBranchMap.begin(); 
       bm != fNickNameBranchMap.end(); ++bm) {
    if (unique_bms.insert(bm->second).second)
      bm->second->SetEntry(entry);
  }
}

void AnalysisTreeReader::Init () {
  std::set<BranchManager*> unique_bms;

  //fDirector.SetTree(fChain);
  // Init all branches and proxies
  for (std::map<TString, BranchManager*>::iterator bm = fNickNameBranchMap.begin(); 
       bm != fNickNameBranchMap.end(); ++bm) {
    if (unique_bms.insert(bm->second).second)
      bm->second->Init();
  }
}

Bool_t AnalysisTreeReader::Notify () {
  //fDirector.SetTree(fChain);
  return kTRUE;
}

bool AnalysisTreeReader::CheckBranchMapNickname (const TString &name) {
  TMapIter next(fBranchMap);
  while(TObjString *key = (TObjString*)next()){
    TString nn = key->String();

    if (name.EqualTo(nn, TString::kIgnoreCase))
      return true;
  }
  return false;
}

TString AnalysisTreeReader::GetFullBranchName (TString name) {
  // Remove any leading or trailing whitespace
  name.Strip(TString::kBoth);

  // Check if branch or leaf has name
  if (fChain->FindBranch(name.Data()))
    return name;
  if (fChain->FindLeaf(name.Data()))
    return TString(fChain->GetLeaf(name.Data())->GetBranch()->GetName());

  // Substitute branch mapping
  TMapIter next(fBranchMap);
  while(TObjString *key = (TObjString*)next()){
    TString nn = key->String();
    TString bn = ((TObjString*)(fBranchMap->GetValue(key)))->String();
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

unsigned int AnalysisTreeReader::GetRank (const TString &branchname) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->IsScalar())
    return 0;
  if (branchmanager->IsCArray1D() || branchmanager->IsVec1D())
    return 1;
  if (branchmanager->IsCArray2D() || branchmanager->IsVec2D())
    return 2;

  throw HALException(branchmanager->GetName().Prepend("Couldn't find rank for branch: ").Data());
}

unsigned int AnalysisTreeReader::GetDim (const TString &branchname, const long long &idx_1) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
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

bool AnalysisTreeReader::GetBool (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
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

long long AnalysisTreeReader::GetInteger (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
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

unsigned long long AnalysisTreeReader::GetCounting (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
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

long double AnalysisTreeReader::GetDecimal (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
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

TString AnalysisTreeReader::GetString (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kS)
    return fS[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvS && 
      fvS[branchmanager->GetStorageIndex()].back().Length() == 1 && 
      fvS[branchmanager->GetStorageIndex()].back()[0] == '\0') {
    TString s("");
    for (std::vector<TString>::iterator it = fvS[branchmanager->GetStorageIndex()].begin();
         it != fvS[branchmanager->GetStorageIndex()].end(); ++it)
      s.Prepend(*it);
    return s;
  }
  if (branchmanager->GetStorageType() == kvS)
    return fvS[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvS && 
      fvvS[branchmanager->GetStorageIndex()][idx_1].back().Length() == 1 && 
      fvvS[branchmanager->GetStorageIndex()][idx_1].back()[0] == '\0') {
    TString s("");
    for (std::vector<TString>::iterator it = fvvS[branchmanager->GetStorageIndex()][idx_1].begin();
         it != fvvS[branchmanager->GetStorageIndex()][idx_1].end(); ++it)
      s.Prepend(*it);
    return s;
  }
  if (branchmanager->GetStorageType() == kvvS)
    return fvvS[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find string data in branch: ").Data());
}

TObjArray& AnalysisTreeReader::GetObjArray (const TString &branchname, const long long &idx_1) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kOA)
    return fOA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvOA)
    return fvOA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TObjArray data in branch: ").Data());
}

TClonesArray& AnalysisTreeReader::GetClonesArray (const TString &branchname, const long long &idx_1) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kCA)
    return fCA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvCA)
    return fvCA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TClonesArray data in branch: ").Data());
}

TRef& AnalysisTreeReader::GetRef (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kR)
    return fR[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvR)
    return fvR[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvR)
    return fvvR[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TRef data in branch: ").Data());
}

TRefArray& AnalysisTreeReader::GetRefArray (const TString &branchname, const long long &idx_1) {
  BranchManager *branchmanager = NULL;

  branchmanager = GetBranchManager(branchname);
  if (branchmanager == NULL)
    throw HALException(branchname.Copy().Prepend("Couldn't configure branch: ").Data());

  if (branchmanager->GetStorageType() == kRA)
    return fRA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvRA)
    return fvRA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TRefArray data in branch: ").Data());
}

AnalysisTreeReader::BranchManager* AnalysisTreeReader::GetBranchManager (const TString &branchname) {
  BranchManager *branchmanager = NULL;

  if (fNickNameBranchMap.count(branchname) == 0) {
    bool already_stored = false;
    TString bname = GetFullBranchName( branchname );
    for (std::map<TString, BranchManager*>::iterator bm = fNickNameBranchMap.begin(); 
         bm != fNickNameBranchMap.end(); ++bm) {
      if (bm->second->GetName().EqualTo(bname)) {
        already_stored = true;
        fNickNameBranchMap[branchname] = bm->second;
        branchmanager = bm->second;
        break;
      }
    }
    if (!already_stored) {
      branchmanager = new BranchManager(this);
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

AnalysisTreeReader::BranchManager::BranchManager (AnalysisTreeReader *tr) : 
  fScalar(kFALSE), fCArray1D(kFALSE), fCArray2D(kFALSE), 
  fVec1D(kFALSE), fVec2D(kFALSE), fBranch(NULL), fTreeReader(tr),
  fcB(NULL), fcSC(NULL), fcI(NULL), fcSI(NULL), fcL(NULL), fcLL(NULL),
  fcUC(NULL), fcUI(NULL), fcUSI(NULL), fcUL(NULL), fcULL(NULL), fcF(NULL),
  fcD(NULL), fcLD(NULL), fcC(NULL), fcTS(NULL), fcTOS(NULL), fcstdS(NULL),
  fcTOA(NULL), fcTCA(NULL), fcTR(NULL), fcTRA(NULL),
  fccB(NULL), fccSC(NULL), fccI(NULL), fccSI(NULL), fccL(NULL), fccLL(NULL),
  fccUC(NULL), fccUI(NULL), fccUSI(NULL), fccUL(NULL), fccULL(NULL), fccF(NULL),
  fccD(NULL), fccLD(NULL), fccC(NULL), fccTS(NULL), fccTOS(NULL), fccstdS(NULL),
  fccTR(NULL),
  fvB(NULL), fvSC(NULL), fvI(NULL), fvSI(NULL), fvL(NULL), fvLL(NULL),
  fvUC(NULL), fvUI(NULL), fvUSI(NULL), fvUL(NULL), fvULL(NULL), fvF(NULL),
  fvD(NULL), fvLD(NULL), fvC(NULL), fvTS(NULL), fvTOS(NULL), fvstdS(NULL),
  fvTOA(NULL), fvTCA(NULL), fvTR(NULL), fvTRA(NULL),
  fvvB(NULL), fvvSC(NULL), fvvI(NULL), fvvSI(NULL), fvvL(NULL), fvvLL(NULL),
  fvvUC(NULL), fvvUI(NULL), fvvUSI(NULL), fvvUL(NULL), fvvULL(NULL), fvvF(NULL),
  fvvD(NULL), fvvLD(NULL), fvvC(NULL), fvvTS(NULL), fvvTOS(NULL), fvvstdS(NULL),
  fvvTR(NULL), 
  //fbpF(NULL),
  fIsB (false), fIsSC(false), fIsI(false), fIsSI(false), 
  fIsL(false), fIsLL(false), fIsUC(false), fIsUI(false), fIsUSI(false), 
  fIsUL(false), fIsULL(false), fIsF(false), fIsD(false), fIsLD(false), 
  fIsC(false), fIsTS(false), fIsTOS(false), fIsstdS(false), fIsTOA(false), 
  fIsTCA(false), fIsTR(false), fIsTRA(false) {
}

AnalysisTreeReader::BranchManager::~BranchManager () {
}

Bool_t AnalysisTreeReader::BranchManager::Create (TString branchname) {

  fBranchName = branchname;

  fBranch = fTreeReader->fChain->GetBranch(fBranchName.Data());
  if (fBranch == NULL)
    return kFALSE;

  fBranchTitle = fBranch->GetTitle();

  FindTypeInformation();

  // ////////////////////////////////////////////////////
  // Initialize data containers for all different types
  // ////////////////////////////////////////////////////
  if (fScalar) {
    // Test for bool types
    if (fTreeReader->fBool.count(fType) != 0) {
      fStorageID = kB;
      fTreeReader->fB.push_back(kFALSE);
      fStorageIndex = fTreeReader->fB.size() - 1;
    }
    // Test for integer types
    else if (fTreeReader->fInteger.count(fType) != 0 || 
             fTreeReader->fShort.count(fType) != 0 ||
             fTreeReader->fLong.count(fType) != 0 ||
             fTreeReader->fLLong.count(fType) != 0 ||
             fTreeReader->fSChar.count(fType) != 0) {
      fStorageID = kI;
      fTreeReader->fI.push_back(0);
      fStorageIndex = fTreeReader->fI.size() - 1;
    }
    // Test for counting types
    else if (fTreeReader->fUInteger.count(fType) != 0 || 
             fTreeReader->fUShort.count(fType) != 0 ||
             fTreeReader->fULong.count(fType) != 0 ||
             fTreeReader->fULLong.count(fType) != 0 ||
             fTreeReader->fUChar.count(fType) != 0) {
      fStorageID = kC;
      fTreeReader->fC.push_back(0);
      fStorageIndex = fTreeReader->fC.size() - 1;
    }
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fType) != 0 || 
             fTreeReader->fDouble.count(fType) != 0 ||
             fTreeReader->fLDouble.count(fType) != 0) {
      fStorageID = kD;
      fTreeReader->fD.push_back(0.0);
      fStorageIndex = fTreeReader->fD.size() - 1;
    }
    // Test for string types
    else if (fTreeReader->fChar.count(fType) != 0 || 
             fType.EqualTo("string") ||
             fType.EqualTo("TString") ||
             fType.EqualTo("TObjString")) {
      fStorageID = kS;
      fTreeReader->fS.push_back("");
      fStorageIndex = fTreeReader->fS.size() - 1;
    }
    else if (fType.EqualTo("TObjArray")) {
      TObjArray temp;
      fStorageID = kOA;
      fTreeReader->fOA.push_back(temp);
      fStorageIndex = fTreeReader->fOA.size() - 1;
    }
    else if (fType.EqualTo("TClonesArray")) {
      TClonesArray temp;
      fStorageID = kCA;
      fTreeReader->fCA.push_back(temp);
      fStorageIndex = fTreeReader->fCA.size() - 1;
    }
    else if (fType.EqualTo("TRef")) {
      TRef temp;
      fStorageID = kR;
      fTreeReader->fR.push_back(temp);
      fStorageIndex = fTreeReader->fR.size() - 1;
    }
    else if (fType.EqualTo("TRefArray")) {
      TRefArray temp;
      fStorageID = kRA;
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
      fStorageID = kvB;
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
      fStorageID = kvI;
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
      fStorageID = kvC;
      fTreeReader->fvC.push_back(temp);
      fStorageIndex = fTreeReader->fvC.size() - 1;
    }
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fScalarType) != 0 || 
             fTreeReader->fDouble.count(fScalarType) != 0 ||
             fTreeReader->fLDouble.count(fScalarType) != 0) {
      std::vector<long double> temp;
      fStorageID = kvD;
      fTreeReader->fvD.push_back(temp);
      fStorageIndex = fTreeReader->fvD.size() - 1;
    }
    // Test for string types
    else if (fTreeReader->fChar.count(fScalarType) != 0 || 
             fScalarType.EqualTo("string") ||
             fScalarType.EqualTo("TString") ||
             fScalarType.EqualTo("TObjString")) {
      std::vector<TString> temp;
      fStorageID = kvS;
      fTreeReader->fvS.push_back(temp);
      fStorageIndex = fTreeReader->fvS.size() - 1;
    }
    else if (fScalarType.EqualTo("TObjArray")) {
      std::vector<TObjArray> temp;
      fStorageID = kvOA;
      fTreeReader->fvOA.push_back(temp);
      fStorageIndex = fTreeReader->fvOA.size() - 1;
    }
    else if (fScalarType.EqualTo("TClonesArray")) {
      std::vector<TClonesArray> temp;
      fStorageID = kvCA;
      fTreeReader->fvCA.push_back(temp);
      fStorageIndex = fTreeReader->fvCA.size() - 1;
    }
    else if (fScalarType.EqualTo("TRef")) {
      std::vector<TRef> temp;
      fStorageID = kvR;
      fTreeReader->fvR.push_back(temp);
      fStorageIndex = fTreeReader->fvR.size() - 1;
    }
    else if (fScalarType.EqualTo("TRefArray")) {
      std::vector<TRefArray> temp;
      fStorageID = kvRA;
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
      fStorageID = kvvB;
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
      fStorageID = kvvI;
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
      fStorageID = kvvC;
      fTreeReader->fvvC.push_back(temp);
      fStorageIndex = fTreeReader->fvvC.size() - 1;
    }
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fScalarType) != 0 || 
             fTreeReader->fDouble.count(fScalarType) != 0 ||
             fTreeReader->fLDouble.count(fScalarType) != 0) {
      std::vector<std::vector<long double> > temp;
      fStorageID = kvvD;
      fTreeReader->fvvD.push_back(temp);
      fStorageIndex = fTreeReader->fvvD.size() - 1;
    }
    // Test for string types
    else if (fTreeReader->fChar.count(fScalarType) != 0 || 
             fScalarType.EqualTo("string") ||
             fScalarType.EqualTo("TString") ||
             fScalarType.EqualTo("TObjString")) {
      std::vector<std::vector<TString> > temp;
      fStorageID = kvvS;
      fTreeReader->fvvS.push_back(temp);
      fStorageIndex = fTreeReader->fvvS.size() - 1;
    }
    else if (fScalarType.EqualTo("TRef")) {
      std::vector<std::vector<TRef> > temp;
      fStorageID = kvvR;
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

void AnalysisTreeReader::BranchManager::Init () {

  fBranch = fTreeReader->fChain->GetBranch(fBranchName.Data());
  if (fBranch == NULL)
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
      fcB = new bool[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcB, &fBranch);
    }
    else if (fIsI) {
      fcI = new int[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcI, &fBranch);
    }
    else if (fIsSI) {
      fcSI = new short int[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcSI, &fBranch);
    }
    else if (fIsL) {
      fcL = new long int[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcL, &fBranch);
    }
    else if (fIsLL) {
      fcLL = new long long[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcLL, &fBranch);
    }
    else if (fIsSC) {
      fcSC = new signed char[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcSC, &fBranch);
    }
    else if (fIsUI) {
      fcUI = new unsigned int[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcUI, &fBranch);
    }
    else if (fIsUSI) {
      fcUSI = new unsigned short int[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcUSI, &fBranch);
    }
    else if (fIsUL) {
      fcUL = new unsigned long[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcUL, &fBranch);
    }
    else if (fIsULL) {
      fcULL = new unsigned long long[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcULL, &fBranch);
    }
    else if (fIsUC) {
      fcUC = new unsigned char[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcUC, &fBranch);
    }
    else if (fIsF) {
      //if (fbpF == NULL) {
      //  //fbpF = new ROOT::TClaFloatProxy(&(fTreeReader->fDirector), fBranchName.Data());
      //  fbpF = new ROOT::TArrayFloatProxy(&(fTreeReader->fDirector), fBranchName.Data());
      //  fTreeReader->fDirector.SetTree(fTreeReader->GetTree());
      //  fTreeReader->fDirector.SetReadEntry(fTreeReader->GetEntryNumber());
      //  //fbpF->Setup();
      //  //std::cout << fbpF->IsInitialized() << std::endl;
      //  //std::cout << fbpF->Read() << std::endl;
      //}
      //fTreeReader->fDirector.SetTree(fTreeReader->GetTree());
      //fTreeReader->fDirector.SetReadEntry(fTreeReader->GetEntryNumber());

      fcF = new float[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcF, &fBranch);
    }
    else if (fIsD) {
      fcD = new double[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcD, &fBranch);
    }
    else if (fIsLD) {
      fcLD = new long double[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcLD, &fBranch);
    }
    else if (fIsC) {
      fcC = new char[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcC, &fBranch);
    }
    else if (fIsstdS) {
      fcstdS = new std::string[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcstdS, &fBranch);
    }
    else if (fIsTS) {
      fcTS = new TString[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTS, &fBranch);
    }
    else if (fIsTOS) {
      fcTOS = new TObjString[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTOS, &fBranch);
    }
    else if (fIsTOA) {
      fcTOA = new TObjArray[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTOA, &fBranch);
    }
    else if (fIsTCA) {
      fcTCA = new TClonesArray[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTCA, &fBranch);
    }
    else if (fIsTR) {
      fcTR = new TRef[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTR, &fBranch);
    }
    else if (fIsTRA) {
      fcTRA = new TRefArray[MaxBufferLength];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fcTRA, &fBranch);
    }
  } // end if 1D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fCArray2D) {
    if (fIsB) {
      fccB = new bool*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccB[i] = new bool[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccB, &fBranch);
    }
    else if (fIsI) {
      fccI = new int*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccI[i] = new int[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccI, &fBranch);
    }
    else if (fIsSI) {
      fccSI = new short int*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccSI[i] = new short int[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccSI, &fBranch);
    }
    else if (fIsL) {
      fccL = new long int*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccL[i] = new long int[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccL, &fBranch);
    }
    else if (fIsLL) {
      fccLL = new long long*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccLL[i] = new long long int[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccLL, &fBranch);
    }
    else if (fIsSC) {
      fccSC = new signed char*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccSC[i] = new signed char[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccSC, &fBranch);
    }
    else if (fIsUI) {
      fccUI = new unsigned int*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccUI[i] = new unsigned int[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccUI, &fBranch);
    }
    else if (fIsUSI) {
      fccUSI = new unsigned short int*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccUSI[i] = new unsigned short int[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccUSI, &fBranch);
    }
    else if (fIsUL) {
      fccUL = new unsigned long*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccUL[i] = new unsigned long[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccUL, &fBranch);
    }
    else if (fIsULL) {
      fccULL = new unsigned long long*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccULL[i] = new unsigned long long[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccULL, &fBranch);
    }
    else if (fIsUC) {
      fccUC = new unsigned char*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccUC[i] = new unsigned char[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccUC, &fBranch);
    }
    else if (fIsF) {
      fccF = new float*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccF[i] = new float[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccF, &fBranch);
    }
    else if (fIsD) {
      fccD = new double*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccD[i] = new double[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccD, &fBranch);
    }
    else if (fIsLD) {
      fccLD = new long double*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccLD[i] = new long double[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccLD, &fBranch);
    }
    else if (fIsC) {
      fccC = new char*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccC[i] = new char[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccC, &fBranch);
    }
    else if (fIsstdS) {
      fccstdS = new std::string*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccstdS[i] = new std::string[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccstdS, &fBranch);
    }
    else if (fIsTS) {
      fccTS = new TString*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccTS[i] = new TString[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccTS, &fBranch);
    }
    else if (fIsTOS) {
      fccTOS = new TObjString*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccTOS[i] = new TObjString[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccTOS, &fBranch);
    }
    else if (fIsTR) {
      fccTR = new TRef*[MaxBufferLength];
      for (int i = 0; i < MaxBufferLength; ++i) fccTR[i] = new TRef[MaxBufferLength2];
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), fccTR, &fBranch);
    }
  } // end if 2D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec1D) {
    if (fIsB) {
      fvB = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvB, &fBranch);
    }
    else if (fIsI) {
      fvI = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvI, &fBranch);
    }
    else if (fIsSI) {
      fvSI = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvSI, &fBranch);
    }
    else if (fIsL) {
      fvL = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvL, &fBranch);
    }
    else if (fIsLL) {
      fvLL = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvLL, &fBranch);
    }
    else if (fIsSC) {
      fvSC = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvSC, &fBranch);
    }
    else if (fIsUI) {
      fvUI = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvUI, &fBranch);
    }
    else if (fIsUSI) {
      fvUSI = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvUSI, &fBranch);
    }
    else if (fIsUL) {
      fvUL = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvUL, &fBranch);
    }
    else if (fIsULL) {
      fvULL = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvULL, &fBranch);
    }
    else if (fIsUC) {
      fvUC = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvUC, &fBranch);
    }
    else if (fIsF) {
      fvF = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvF, &fBranch);
    }
    else if (fIsD) {
      fvD = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvD, &fBranch);
    }
    else if (fIsLD) {
      fvLD = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvLD, &fBranch);
    }
    else if (fIsC) {
      fvC = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvC, &fBranch);
    }
    else if (fIsstdS) {
      fvstdS = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvstdS, &fBranch);
    }
    else if (fIsTS) {
      fvTS = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTS, &fBranch);
    }
    else if (fIsTOS) {
      fvTOS = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTOS, &fBranch);
    }
    else if (fIsTOA) {
      fvTOA = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTOA, &fBranch);
    }
    else if (fIsTCA) {
      fvTCA = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTCA, &fBranch);
    }
    else if (fIsTR) {
      fvTR = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTR, &fBranch);
    }
    else if (fIsTRA) {
      fvTRA = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvTRA, &fBranch);
    }
  } // end if 1D vector
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec2D) {
    if (fIsB) {
      fvvB = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvB, &fBranch);
    }
    else if (fIsI) {
      fvvI = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvI, &fBranch);
    }
    else if (fIsSI) {
      fvvSI = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvSI, &fBranch);
    }
    else if (fIsL) {
      fvvL = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvL, &fBranch);
    }
    else if (fIsLL) {
      fvvLL = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvLL, &fBranch);
    }
    else if (fIsSC) {
      fvvSC = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvSC, &fBranch);
    }
    else if (fIsUI) {
      fvvUI = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvUI, &fBranch);
    }
    else if (fIsUSI) {
      fvvUSI = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvUSI, &fBranch);
    }
    else if (fIsUL) {
      fvvUL = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvUL, &fBranch);
    }
    else if (fIsULL) {
      fvvULL = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvULL, &fBranch);
    }
    else if (fIsUC) {
      fvvUC = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvUC, &fBranch);
    }
    else if (fIsF) {
      fvvF = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvF, &fBranch);
    }
    else if (fIsD) {
      fvvD = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvD, &fBranch);
    }
    else if (fIsLD) {
      fvvLD = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvLD, &fBranch);
    }
    else if (fIsC) {
      fvvC = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvC, &fBranch);
    }
    else if (fIsstdS) {
      fvvstdS = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvstdS, &fBranch);
    }
    else if (fIsTS) {
      fvvTS = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvTS, &fBranch);
    }
    else if (fIsTOS) {
      fvvTOS = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvTOS, &fBranch);
    }
    else if (fIsTR) {
      fvvTR = NULL;
      fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), &fvvTR, &fBranch);
    }
  } // end if 2D vector
  // ////////////////////////////////////////////////////
  // ////////////////////////////////////////////////////
}


void AnalysisTreeReader::BranchManager::SetEntry (Long64_t entry) {

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
    //if (!fIsF) fBranch->GetEntry(entry);
    fBranch->GetEntry(entry);
    if (fIsB) {
      fTreeReader->fvB[fStorageIndex].clear();
      fTreeReader->fvB[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvB[fStorageIndex].push_back(fcB[i]);
    }
    else if (fIsI) {
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcI[i]);
    }
    else if (fIsSI) {
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcSI[i]);
    }
    else if (fIsL) {
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcL[i]);
    }
    else if (fIsLL) {
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcLL[i]);
    }
    else if (fIsSC) {
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcSC[i]);
    }
    else if (fIsUI) {
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUI[i]);
    }
    else if (fIsUSI) {
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUSI[i]);
    }
    else if (fIsUL) {
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUL[i]);
    }
    else if (fIsULL) {
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcULL[i]);
    }
    else if (fIsUC) {
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUC[i]);
    }
    else if (fIsF) {
      // NEW !!!!!!!
      //leaf->SetAddress(new float[n]);
      //std::cout << "starting float: " << fBranchName << "    " << fBranch->GetListOfLeaves()->At(0)->GetName() << std::endl;
      //std::cout << fBranch->GetNleaves() << "    " << ((TLeaf*)fBranch->GetListOfLeaves()->At(0))->GetLen() << std::endl;
      //std::cout << fBranch->GetNleaves() << "    " << fBranch->GetListOfBaskets()->GetEntries() << std::endl;
      ////fBranch->SetAddress(new float[n]);
      //fBranch->GetEntry(entry);
      //TLeaf *leaf = (TLeaf*) fBranch->GetListOfLeaves()->At(0);
      //leaf->SetRange();
      //std::cout << "max: " << leaf->GetMaximum() << std::endl;
      //TBufferFile array_buffer(TBuffer::kRead);
      //fBranch->ReadBasket(array_buffer);
      ////leaf->FillBasket(array_buffer);
      ////float *temp = new float[n];
      //float *temp = NULL;
      //std::cout << "From Buffer: " << array_buffer.ReadArray(temp) << std::endl;
      ////leaf->ResetAddress(0);

      //fTreeReader->fvD[fStorageIndex].clear();
      //fTreeReader->fvD[fStorageIndex].reserve(n);
      //for (Int_t i = 0; i < n; ++i) {
      //  //fTreeReader->fvD[fStorageIndex].push_back(temp[i]);
      //  //fTreeReader->fvD[fStorageIndex].push_back(leaf->GetValue(i));
      //  //fTreeReader->fvD[fStorageIndex].push_back(fbpF->At(i));
      //  //std::cout << fTreeReader->fvD[fStorageIndex].back() << " ";
      //}
      //std::cout << " got float values" << std::endl;
      // NEW !!!!!!!
      
      //fBranch->Reset();
      //fcF = new float[n];
      //fBranch->SetAddress(fcF);
      //fBranch->GetEntry(entry);
      //fTreeReader->fvD[fStorageIndex].clear();
      //fTreeReader->fvD[fStorageIndex].reserve(n);
      //for (Int_t i = 0; i < n; ++i) {
      //  fTreeReader->fvD[fStorageIndex].push_back(fcF[i]);
      //}

      //TBufferFile buf(TBuffer::kRead);
      //TBuffer *buf = fTreeReader->GetTree()->GetTransientBuffer(1024);
      //float *temp = new float[n];
      //buf.SetBuffer(temp, n);
      //fBranch->GetEntry(entry);
      //TLeaf *leaf = (TLeaf*) fBranch->GetListOfLeaves()->At(0);
      //leaf->FillBasket(buf);
      //leaf->SetRange();
      //leaf->SetAddress(temp);
      //std::cout << n << "    " << leaf->GetLen() << "    " << leaf->GetNdata() << "    " << leaf->IsRange() << std::endl;
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i) {
        //std::cout << leaf->GetValue(i) << "  ";
        //fTreeReader->fvD[fStorageIndex].push_back(temp[i]);
        fTreeReader->fvD[fStorageIndex].push_back(fcF[i]);
        //fTreeReader->fvD[fStorageIndex].push_back(fbpF->At(i));
      }
      //std::cout << std::endl;
      //delete[] temp;
    }
    else if (fIsD) {
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(fcD[i]);
    }
    else if (fIsLD) {
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(fcLD[i]);
    }
    else if (fIsC) {
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcC[i]);
    }
    else if (fIsstdS) {
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcstdS[i].c_str());
    }
    else if (fIsTS) {
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcTS[i]);
    }
    else if (fIsTOS) {
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcTOS[i].String());
    }
    else if (fIsTOA) {
      fTreeReader->fvOA[fStorageIndex].clear();
      fTreeReader->fvOA[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvOA[fStorageIndex].push_back(fcTOA[i]);
    }
    else if (fIsTCA) {
      fTreeReader->fvCA[fStorageIndex].clear();
      fTreeReader->fvCA[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvCA[fStorageIndex].push_back(fcTCA[i]);
    }
    else if (fIsTR) {
      fTreeReader->fvR[fStorageIndex].clear();
      fTreeReader->fvR[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvR[fStorageIndex].push_back(fcTR[i]);
    }
    else if (fIsTRA) {
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
    fBranch->GetEntry(entry);
    if (fIsB) {
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
  //else if (fCArray1D) {
  //  Int_t n = GetArrayLength(1);
  //  fBranch->GetEntry(entry);
  //  if (fIsB) {
  //    fTreeReader->fvB[fStorageIndex].clear();
  //    fTreeReader->fvB[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvB[fStorageIndex].push_back(fcB[i]);
  //  }
  //  else if (fIsI) {
  //    fTreeReader->fvI[fStorageIndex].clear();
  //    fTreeReader->fvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvI[fStorageIndex].push_back(fcI[i]);
  //  }
  //  else if (fIsSI) {
  //    fTreeReader->fvI[fStorageIndex].clear();
  //    fTreeReader->fvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvI[fStorageIndex].push_back(fcSI[i]);
  //  }
  //  else if (fIsL) {
  //    fTreeReader->fvI[fStorageIndex].clear();
  //    fTreeReader->fvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvI[fStorageIndex].push_back(fcL[i]);
  //  }
  //  else if (fIsLL) {
  //    fTreeReader->fvI[fStorageIndex].clear();
  //    fTreeReader->fvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvI[fStorageIndex].push_back(fcLL[i]);
  //  }
  //  else if (fIsSC) {
  //    fTreeReader->fvI[fStorageIndex].clear();
  //    fTreeReader->fvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvI[fStorageIndex].push_back(fcSC[i]);
  //  }
  //  else if (fIsUI) {
  //    fTreeReader->fvC[fStorageIndex].clear();
  //    fTreeReader->fvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvC[fStorageIndex].push_back(fcUI[i]);
  //  }
  //  else if (fIsUSI) {
  //    fTreeReader->fvC[fStorageIndex].clear();
  //    fTreeReader->fvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvC[fStorageIndex].push_back(fcUSI[i]);
  //  }
  //  else if (fIsUL) {
  //    fTreeReader->fvC[fStorageIndex].clear();
  //    fTreeReader->fvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvC[fStorageIndex].push_back(fcUL[i]);
  //  }
  //  else if (fIsULL) {
  //    fTreeReader->fvC[fStorageIndex].clear();
  //    fTreeReader->fvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvC[fStorageIndex].push_back(fcULL[i]);
  //  }
  //  else if (fIsUC) {
  //    fTreeReader->fvC[fStorageIndex].clear();
  //    fTreeReader->fvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvC[fStorageIndex].push_back(fcUC[i]);
  //  }
  //  else if (fIsF) {
  //    fTreeReader->fvD[fStorageIndex].clear();
  //    fTreeReader->fvD[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) 
  //      fTreeReader->fvD[fStorageIndex].push_back(fcF[i]);
  //  }
  //  else if (fIsD) {
  //    fTreeReader->fvD[fStorageIndex].clear();
  //    fTreeReader->fvD[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvD[fStorageIndex].push_back(fcD[i]);
  //  }
  //  else if (fIsLD) {
  //    fTreeReader->fvD[fStorageIndex].clear();
  //    fTreeReader->fvD[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvD[fStorageIndex].push_back(fcLD[i]);
  //  }
  //  else if (fIsC) {
  //    fTreeReader->fvS[fStorageIndex].clear();
  //    fTreeReader->fvS[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvS[fStorageIndex].push_back(fcC[i]);
  //  }
  //  else if (fIsstdS) {
  //    fTreeReader->fvS[fStorageIndex].clear();
  //    fTreeReader->fvS[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvS[fStorageIndex].push_back(fcstdS[i].c_str());
  //  }
  //  else if (fIsTS) {
  //    fTreeReader->fvS[fStorageIndex].clear();
  //    fTreeReader->fvS[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvS[fStorageIndex].push_back(fcTS[i]);
  //  }
  //  else if (fIsTOS) {
  //    fTreeReader->fvS[fStorageIndex].clear();
  //    fTreeReader->fvS[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvS[fStorageIndex].push_back(fcTOS[i].String());
  //  }
  //  else if (fIsTOA) {
  //    fTreeReader->fvOA[fStorageIndex].clear();
  //    fTreeReader->fvOA[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvOA[fStorageIndex].push_back(fcTOA[i]);
  //  }
  //  else if (fIsTCA) {
  //    fTreeReader->fvCA[fStorageIndex].clear();
  //    fTreeReader->fvCA[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvCA[fStorageIndex].push_back(fcTCA[i]);
  //  }
  //  else if (fIsTR) {
  //    fTreeReader->fvR[fStorageIndex].clear();
  //    fTreeReader->fvR[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvR[fStorageIndex].push_back(fcTR[i]);
  //  }
  //  else if (fIsTRA) {
  //    fTreeReader->fvRA[fStorageIndex].clear();
  //    fTreeReader->fvRA[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i)
  //      fTreeReader->fvRA[fStorageIndex].push_back(fcTRA[i]);
  //  }
  //} // end if 1D c-array
  //// //////////////////////////////////////////////////////////////
  //// //////////////////////////////////////////////////////////////
  //// //////////////////////////////////////////////////////////////
  //else if (fCArray2D) {
  //  Int_t n = GetArrayLength(1);
  //  Int_t m = GetArrayLength(2);
  //  fBranch->GetEntry(entry);
  //  if (fIsB) {
  //    fTreeReader->fvvB[fStorageIndex].clear();
  //    fTreeReader->fvvB[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<bool> row;
  //      fTreeReader->fvvB[fStorageIndex].push_back(row);
  //      fTreeReader->fvvB[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvB[fStorageIndex].back().push_back(fccB[i][j]);
  //    }
  //  }
  //  else if (fIsI) {
  //    fTreeReader->fvvI[fStorageIndex].clear();
  //    fTreeReader->fvvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<long long> row;
  //      fTreeReader->fvvI[fStorageIndex].push_back(row);
  //      fTreeReader->fvvI[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvI[fStorageIndex].back().push_back(fccI[i][j]);
  //    }
  //  }
  //  else if (fIsSI) {
  //    fTreeReader->fvvI[fStorageIndex].clear();
  //    fTreeReader->fvvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<long long> row;
  //      fTreeReader->fvvI[fStorageIndex].push_back(row);
  //      fTreeReader->fvvI[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvI[fStorageIndex].back().push_back(fccSI[i][j]);
  //    }
  //  }
  //  else if (fIsL) {
  //    fTreeReader->fvvI[fStorageIndex].clear();
  //    fTreeReader->fvvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<long long> row;
  //      fTreeReader->fvvI[fStorageIndex].push_back(row);
  //      fTreeReader->fvvI[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvI[fStorageIndex].back().push_back(fccL[i][j]);
  //    }
  //  }
  //  else if (fIsLL) {
  //    fTreeReader->fvvI[fStorageIndex].clear();
  //    fTreeReader->fvvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<long long> row;
  //      fTreeReader->fvvI[fStorageIndex].push_back(row);
  //      fTreeReader->fvvI[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvI[fStorageIndex].back().push_back(fccLL[i][j]);
  //    }
  //  }
  //  else if (fIsSC) {
  //    fTreeReader->fvvI[fStorageIndex].clear();
  //    fTreeReader->fvvI[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<long long> row;
  //      fTreeReader->fvvI[fStorageIndex].push_back(row);
  //      fTreeReader->fvvI[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvI[fStorageIndex].back().push_back(fccSC[i][j]);
  //    }
  //  }
  //  else if (fIsUI) {
  //    fTreeReader->fvvC[fStorageIndex].clear();
  //    fTreeReader->fvvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<unsigned long long> row;
  //      fTreeReader->fvvC[fStorageIndex].push_back(row);
  //      fTreeReader->fvvC[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvC[fStorageIndex].back().push_back(fccUI[i][j]);
  //    }
  //  }
  //  else if (fIsUSI) {
  //    fTreeReader->fvvC[fStorageIndex].clear();
  //    fTreeReader->fvvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<unsigned long long> row;
  //      fTreeReader->fvvC[fStorageIndex].push_back(row);
  //      fTreeReader->fvvC[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvC[fStorageIndex].back().push_back(fccUSI[i][j]);
  //    }
  //  }
  //  else if (fIsUL) {
  //    fTreeReader->fvvC[fStorageIndex].clear();
  //    fTreeReader->fvvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<unsigned long long> row;
  //      fTreeReader->fvvC[fStorageIndex].push_back(row);
  //      fTreeReader->fvvC[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvC[fStorageIndex].back().push_back(fccUL[i][j]);
  //    }
  //  }
  //  else if (fIsULL) {
  //    fTreeReader->fvvC[fStorageIndex].clear();
  //    fTreeReader->fvvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<unsigned long long> row;
  //      fTreeReader->fvvC[fStorageIndex].push_back(row);
  //      fTreeReader->fvvC[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvC[fStorageIndex].back().push_back(fccULL[i][j]);
  //    }
  //  }
  //  else if (fIsUC) {
  //    fTreeReader->fvvC[fStorageIndex].clear();
  //    fTreeReader->fvvC[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<unsigned long long> row;
  //      fTreeReader->fvvC[fStorageIndex].push_back(row);
  //      fTreeReader->fvvC[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvC[fStorageIndex].back().push_back(fccUC[i][j]);
  //    }
  //  }
  //  else if (fIsF) {
  //    fTreeReader->fvvD[fStorageIndex].clear();
  //    fTreeReader->fvvD[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<long double> row;
  //      fTreeReader->fvvD[fStorageIndex].push_back(row);
  //      fTreeReader->fvvD[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvD[fStorageIndex].back().push_back(fccF[i][j]);
  //    }
  //  }
  //  else if (fIsD) {
  //    fTreeReader->fvvD[fStorageIndex].clear();
  //    fTreeReader->fvvD[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<long double> row;
  //      fTreeReader->fvvD[fStorageIndex].push_back(row);
  //      fTreeReader->fvvD[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvD[fStorageIndex].back().push_back(fccD[i][j]);
  //    }
  //  }
  //  else if (fIsLD) {
  //    fTreeReader->fvvD[fStorageIndex].clear();
  //    fTreeReader->fvvD[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<long double> row;
  //      fTreeReader->fvvD[fStorageIndex].push_back(row);
  //      fTreeReader->fvvD[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvD[fStorageIndex].back().push_back(fccLD[i][j]);
  //    }
  //  }
  //  else if (fIsC) {
  //    fTreeReader->fvvS[fStorageIndex].clear();
  //    fTreeReader->fvvS[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<TString> row;
  //      fTreeReader->fvvS[fStorageIndex].push_back(row);
  //      fTreeReader->fvvS[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvS[fStorageIndex].back().push_back(fccC[i][j]);
  //    }
  //  }
  //  else if (fIsstdS) {
  //    fTreeReader->fvvS[fStorageIndex].clear();
  //    fTreeReader->fvvS[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<TString> row;
  //      fTreeReader->fvvS[fStorageIndex].push_back(row);
  //      fTreeReader->fvvS[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvS[fStorageIndex].back().push_back(fccstdS[i][j].c_str());
  //    }
  //  }
  //  else if (fIsTS) {
  //    fTreeReader->fvvS[fStorageIndex].clear();
  //    fTreeReader->fvvS[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<TString> row;
  //      fTreeReader->fvvS[fStorageIndex].push_back(row);
  //      fTreeReader->fvvS[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvS[fStorageIndex].back().push_back(fccTS[i][j]);
  //    }
  //  }
  //  else if (fIsTOS) {
  //    fTreeReader->fvvS[fStorageIndex].clear();
  //    fTreeReader->fvvS[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<TString> row;
  //      fTreeReader->fvvS[fStorageIndex].push_back(row);
  //      fTreeReader->fvvS[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvS[fStorageIndex].back().push_back(fccTOS[i][j].String());
  //    }
  //  }
  //  else if (fIsTR) {
  //    fTreeReader->fvvR[fStorageIndex].clear();
  //    fTreeReader->fvvR[fStorageIndex].reserve(n);
  //    for (Int_t i = 0; i < n; ++i) {
  //      std::vector<TRef> row;
  //      fTreeReader->fvvR[fStorageIndex].push_back(row);
  //      fTreeReader->fvvR[fStorageIndex].back().reserve(m);
  //      for (Int_t j = 0; j < m; ++j)
  //        fTreeReader->fvvR[fStorageIndex].back().push_back(fccTR[i][j]);
  //    }
  //  }
  //}  //end if 2D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec1D) {
    fBranch->GetEntry(entry);
    if (fIsB) {
      unsigned n = fvB->size();
      fTreeReader->fvB[fStorageIndex].clear();
      fTreeReader->fvB[fStorageIndex].reserve(n);
      for (std::vector<bool>::iterator it = fvB->begin(); it != fvB->end(); ++it)
        fTreeReader->fvB[fStorageIndex].push_back(*it);
    }
    else if (fIsI) {
      unsigned n = fvI->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (std::vector<int>::iterator it = fvI->begin(); it != fvI->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
    }
    else if (fIsSI) {
      unsigned n = fvSI->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (std::vector<short int>::iterator it = fvSI->begin(); it != fvSI->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
    }
    else if (fIsL) {
      unsigned n = fvL->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (std::vector<long int>::iterator it = fvL->begin(); it != fvL->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
    }
    else if (fIsLL) {
      unsigned n = fvLL->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (std::vector<long long int>::iterator it = fvLL->begin(); it != fvLL->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
    }
    else if (fIsSC) {
      unsigned n = fvSC->size();
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (std::vector<signed char>::iterator it = fvSC->begin(); it != fvSC->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
    }
    else if (fIsUI) {
      unsigned n = fvUI->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (std::vector<unsigned int>::iterator it = fvUI->begin(); it != fvUI->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
    }
    else if (fIsUSI) {
      unsigned n = fvUSI->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (std::vector<unsigned short int>::iterator it = fvUSI->begin(); it != fvUSI->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
    }
    else if (fIsUL) {
      unsigned n = fvUL->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (std::vector<unsigned long int>::iterator it = fvUL->begin(); it != fvUL->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
    }
    else if (fIsULL) {
      unsigned n = fvULL->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (std::vector<unsigned long long int>::iterator it = fvULL->begin(); it != fvULL->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
    }
    else if (fIsUC) {
      unsigned n = fvUC->size();
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (std::vector<unsigned char>::iterator it = fvUC->begin(); it != fvUC->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
    }
    else if (fIsF) {
      unsigned n = fvF->size();
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (std::vector<float>::iterator it = fvF->begin(); it != fvF->end(); ++it)
        fTreeReader->fvD[fStorageIndex].push_back(*it);
    }
    else if (fIsD) {
      unsigned n = fvD->size();
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (std::vector<double>::iterator it = fvD->begin(); it != fvD->end(); ++it)
        fTreeReader->fvD[fStorageIndex].push_back(*it);
    }
    else if (fIsLD) {
      unsigned n = fvLD->size();
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (std::vector<long double>::iterator it = fvLD->begin(); it != fvLD->end(); ++it)
        fTreeReader->fvD[fStorageIndex].push_back(*it);
    }
    else if (fIsC) {
      unsigned n = fvC->size();
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (std::vector<char>::iterator it = fvC->begin(); it != fvC->end(); ++it)
        fTreeReader->fvS[fStorageIndex].push_back(*it);
    }
    else if (fIsstdS) {
      unsigned n = fvstdS->size();
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (std::vector<std::string>::iterator it = fvstdS->begin(); it != fvstdS->end(); ++it)
        fTreeReader->fvS[fStorageIndex].push_back((*it).c_str());
    }
    else if (fIsTS) {
      unsigned n = fvTS->size();
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (std::vector<TString>::iterator it = fvTS->begin(); it != fvTS->end(); ++it)
        fTreeReader->fvS[fStorageIndex].push_back(*it);
    }
    else if (fIsTOS) {
      unsigned n = fvTOS->size();
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (std::vector<TObjString>::iterator it = fvTOS->begin(); it != fvTOS->end(); ++it)
        fTreeReader->fvS[fStorageIndex].push_back((*it).String());
    }
    else if (fIsTOA) {
      unsigned n = fvTOA->size();
      fTreeReader->fvOA[fStorageIndex].clear();
      fTreeReader->fvOA[fStorageIndex].reserve(n);
      for (std::vector<TObjArray>::iterator it = fvTOA->begin(); it != fvTOA->end(); ++it)
        fTreeReader->fvOA[fStorageIndex].push_back(*it);
    }
    else if (fIsTCA) {
      unsigned n = fvTCA->size();
      fTreeReader->fvCA[fStorageIndex].clear();
      fTreeReader->fvCA[fStorageIndex].reserve(n);
      for (std::vector<TClonesArray>::iterator it = fvTCA->begin(); it != fvTCA->end(); ++it)
        fTreeReader->fvCA[fStorageIndex].push_back(*it);
    }
    else if (fIsTR) {
      unsigned n = fvTR->size();
      fTreeReader->fvR[fStorageIndex].clear();
      fTreeReader->fvR[fStorageIndex].reserve(n);
      for (std::vector<TRef>::iterator it = fvTR->begin(); it != fvTR->end(); ++it)
        fTreeReader->fvR[fStorageIndex].push_back(*it);
    }
    else if (fIsTRA) {
      unsigned n = fvTRA->size();
      fTreeReader->fvRA[fStorageIndex].clear();
      fTreeReader->fvRA[fStorageIndex].reserve(n);
      for (std::vector<TRefArray>::iterator it = fvTRA->begin(); it != fvTRA->end(); ++it)
        fTreeReader->fvRA[fStorageIndex].push_back(*it);
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
      for (std::vector<std::vector<bool> >::iterator iit = fvvB->begin(); iit != fvvB->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<bool> row;
        fTreeReader->fvvB[fStorageIndex].push_back(row);
        fTreeReader->fvvB[fStorageIndex].back().reserve(m);
        for (std::vector<bool>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvB[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsI) {
      unsigned n = fvvI->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (std::vector<std::vector<int> >::iterator iit = fvvI->begin(); iit != fvvI->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (std::vector<int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsSI) {
      unsigned n = fvvSI->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (std::vector<std::vector<short int> >::iterator iit = fvvSI->begin(); iit != fvvSI->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (std::vector<short int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsL) {
      unsigned n = fvvL->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (std::vector<std::vector<long int> >::iterator iit = fvvL->begin(); iit != fvvL->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (std::vector<long int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsLL) {
      unsigned n = fvvLL->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (std::vector<std::vector<long long int> >::iterator iit = fvvLL->begin(); iit != fvvLL->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (std::vector<long long int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsSC) {
      unsigned n = fvvSC->size();
      fTreeReader->fvvI[fStorageIndex].clear();
      fTreeReader->fvvI[fStorageIndex].reserve(n);
      for (std::vector<std::vector<signed char> >::iterator iit = fvvSC->begin(); iit != fvvSC->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<long long> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        fTreeReader->fvvI[fStorageIndex].back().reserve(m);
        for (std::vector<signed char>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsUI) {
      unsigned n = fvvUI->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (std::vector<std::vector<unsigned int> >::iterator iit = fvvUI->begin(); iit != fvvUI->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (std::vector<unsigned int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsUSI) {
      unsigned n = fvvUSI->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (std::vector<std::vector<unsigned short int> >::iterator iit = fvvUSI->begin(); iit != fvvUSI->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (std::vector<unsigned short int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsUL) {
      unsigned n = fvvUL->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (std::vector<std::vector<unsigned long int> >::iterator iit = fvvUL->begin(); iit != fvvUL->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (std::vector<unsigned long int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsULL) {
      unsigned n = fvvULL->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (std::vector<std::vector<unsigned long long int> >::iterator iit = fvvULL->begin(); iit != fvvULL->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (std::vector<unsigned long long int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsUC) {
      unsigned n = fvvUC->size();
      fTreeReader->fvvC[fStorageIndex].clear();
      fTreeReader->fvvC[fStorageIndex].reserve(n);
      for (std::vector<std::vector<unsigned char> >::iterator iit = fvvUC->begin(); iit != fvvUC->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<unsigned long long> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        fTreeReader->fvvC[fStorageIndex].back().reserve(m);
        for (std::vector<unsigned char>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsF) {
      unsigned n = fvvF->size();
      fTreeReader->fvvD[fStorageIndex].clear();
      fTreeReader->fvvD[fStorageIndex].reserve(n);
      for (std::vector<std::vector<float> >::iterator iit = fvvF->begin(); iit != fvvF->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<long double> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        fTreeReader->fvvD[fStorageIndex].back().reserve(m);
        for (std::vector<float>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvD[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsD) {
      unsigned n = fvvD->size();
      fTreeReader->fvvD[fStorageIndex].clear();
      fTreeReader->fvvD[fStorageIndex].reserve(n);
      for (std::vector<std::vector<double> >::iterator iit = fvvD->begin(); iit != fvvD->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<long double> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        fTreeReader->fvvD[fStorageIndex].back().reserve(m);
        for (std::vector<double>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvD[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsLD) {
      unsigned n = fvvLD->size();
      fTreeReader->fvvD[fStorageIndex].clear();
      fTreeReader->fvvD[fStorageIndex].reserve(n);
      for (std::vector<std::vector<long double> >::iterator iit = fvvLD->begin(); iit != fvvLD->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<long double> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        fTreeReader->fvvD[fStorageIndex].back().reserve(m);
        for (std::vector<long double>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvD[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsC) {
      unsigned n = fvvC->size();
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
      for (std::vector<std::vector<char> >::iterator iit = fvvC->begin(); iit != fvvC->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
        for (std::vector<char>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvS[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsstdS) {
      unsigned n = fvvstdS->size();
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
      for (std::vector<std::vector<std::string> >::iterator iit = fvvstdS->begin(); iit != fvvstdS->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
        for (std::vector<std::string>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvS[fStorageIndex].back().push_back(it->c_str());
      }
    }
    else if (fIsTS) {
      unsigned n = fvvTS->size();
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
      for (std::vector<std::vector<TString> >::iterator iit = fvvTS->begin(); iit != fvvTS->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
        for (std::vector<TString>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvS[fStorageIndex].back().push_back(*it);
      }
    }
    else if (fIsTOS) {
      unsigned n = fvvTOS->size();
      fTreeReader->fvvS[fStorageIndex].clear();
      fTreeReader->fvvS[fStorageIndex].reserve(n);
      for (std::vector<std::vector<TObjString> >::iterator iit = fvvTOS->begin(); iit != fvvTOS->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        fTreeReader->fvvS[fStorageIndex].back().reserve(m);
        for (std::vector<TObjString>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvS[fStorageIndex].back().push_back(it->String());
      }
    }
    else if (fIsTR) {
      unsigned n = fvvTR->size();
      fTreeReader->fvvR[fStorageIndex].clear();
      fTreeReader->fvvR[fStorageIndex].reserve(n);
      for (std::vector<std::vector<TRef> >::iterator iit = fvvTR->begin(); iit != fvvTR->end(); ++iit) {
        unsigned m = iit->size();
        std::vector<TRef> row;
        fTreeReader->fvvR[fStorageIndex].push_back(row);
        fTreeReader->fvvR[fStorageIndex].back().reserve(m);
        for (std::vector<TRef>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvR[fStorageIndex].back().push_back(*it);
      }
    }
  } // end if 2D vector
  // ////////////////////////////////////////////////////
  // ////////////////////////////////////////////////////
}

// ///////////////////////////////////////
// ///////////////////////////////////////

Int_t AnalysisTreeReader::BranchManager::GetArrayLength (Int_t rank) {
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

Int_t AnalysisTreeReader::BranchManager::GetMaxArrayLength (Int_t rank) {
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

void AnalysisTreeReader::BranchManager::FindTypeInformation() {
  TLeaf *l = NULL;

  // branch name = leaf name
  if (fBranch->FindLeaf(fBranchName.Data()))
    l = fBranch->GetLeaf(fBranchName.Data());
  else { // take branch's (only) leaf and get type info from it
    TObjArray *ll = fBranch->GetListOfLeaves();
    if (ll->GetEntries() != 1)
      throw HALException(fBranchName.Append(": This branch has too many leaves. Can't find data."));
    l = (TLeaf*)ll->At(0);
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

} /* HAL */ 
