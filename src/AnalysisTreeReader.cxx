#include <HAL/AnalysisTreeReader.h>

ClassImp(HAL::AnalysisTreeReader);

namespace HAL {

AnalysisTreeReader::AnalysisTreeReader (TTree *t) : fChain(t), 
  fScalar("^[a-zA-Z][a-zA-Z0-9_]+$"),
  fVector("^vector[ ]*<[ ]*[a-zA-Z][a-zA-Z0-9_]+[ ]*>$"), // vector<scalar>
  fVector2D("^vector[ ]*<[ ]*vector[ ]*<[ ]*[a-zA-Z][a-zA-Z0-9_]+[ ]*>[ ]*>$"), // vector<vector<scalar> >
  fArray("^[a-zA-Z_][a-zA-Z0-9_]*[[a-zA-Z0-9_]+]$"), // name[i]
  fArray2D("^[a-zA-Z_][a-zA-Z0-9_]*[[a-zA-Z0-9_]+][[a-zA-Z0-9_]+]$") /* name[i][j] */ { 

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
  fEntry = entry;

  // Update all branches
  for (std::map<TString, BranchManager*, internal::string_cmp>::iterator bm = fNickNameBranchMap.begin(); 
       bm != fNickNameBranchMap.end(); ++bm)
    bm->second->SetEntry(entry);
}

void AnalysisTreeReader::Init () {
  // Init all branches
  for (std::map<TString, BranchManager*, internal::string_cmp>::iterator bm = fNickNameBranchMap.begin(); 
       bm != fNickNameBranchMap.end(); ++bm)
    bm->second->Init();
}

bool AnalysisTreeReader::CheckBranchMapNickname (const TString &name) {
  TMapIter next(fBranchMap);
  while(TObjString *key = (TObjString*)next()){
    TString nn = key->String();

    if (name.EqualTo(nn))
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

unsigned int AnalysisTreeReader::GetDim (const TString &branchname, const long long &idx_1) {
  BranchManager *branchmanager = NULL;

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

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

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

  if (branchmanager->GetStorageType() == kB)
    return fB[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvB)
    return fvB[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvB)
    return fvvB[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find bool data in branch: ").Data());
}

long long AnalysisTreeReader::GetInteger (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

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
  // special case of char as 8-bit data holder
  if (branchmanager->GetStorageType() == kvS && fChar.count(branchmanager->GetScalarType()) != 0)
    return (signed char)fvS[branchmanager->GetStorageIndex()][idx_1].Data()[0];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find integer number data in branch: ").Data());
}

unsigned long long AnalysisTreeReader::GetCounting (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

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

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find counting number data in branch: ").Data());
}

long double AnalysisTreeReader::GetDecimal (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

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

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find decimal number data in branch: ").Data());
}

TString AnalysisTreeReader::GetString (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

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

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

  if (branchmanager->GetStorageType() == kOA)
    return fOA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvOA)
    return fvOA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TObjArray data in branch: ").Data());
}

TClonesArray& AnalysisTreeReader::GetClonesArray (const TString &branchname, const long long &idx_1) {
  BranchManager *branchmanager = NULL;

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

  if (branchmanager->GetStorageType() == kCA)
    return fCA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvCA)
    return fvCA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TClonesArray data in branch: ").Data());
}

TRef& AnalysisTreeReader::GetRef (const TString &branchname, const long long &idx_1, const long long &idx_2) {
  BranchManager *branchmanager = NULL;

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

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

  if (fNickNameBranchMap.count(branchname) == 0) {
    branchmanager = new BranchManager(this);
    TString bname = GetFullBranchName( branchname );
    if (branchmanager->Create(bname))
      fNickNameBranchMap[branchname] = branchmanager;
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }
  else
    branchmanager = fNickNameBranchMap[branchname];

  if (branchmanager->GetStorageType() == kRA)
    return fRA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvRA)
    return fvRA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(GetFullBranchName( branchname ).Prepend("Couldn't find TRefArray data in branch: ").Data());
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
  fRows(0), fColumns(0),
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
    throw HALException(fBranchName.Prepend("Couldn't find branch: "));

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

  if (fBranch != NULL)
    fBranch->ResetAddress();

  if (fScalar) {
    if (fIsB)
      fBranch->SetAddress(&fB);
    else if (fIsI)
      fBranch->SetAddress(&fI);
    else if (fIsSI)
      fBranch->SetAddress(&fSI);
    else if (fIsL)
      fBranch->SetAddress(&fL);
    else if (fIsLL)
      fBranch->SetAddress(&fLL);
    else if (fIsSC)
      fBranch->SetAddress(&fSC);
    else if (fIsUI)
      fBranch->SetAddress(&fUI);
    else if (fIsUSI)
      fBranch->SetAddress(&fUSI);
    else if (fIsUL)
      fBranch->SetAddress(&fUL);
    else if (fIsULL)
      fBranch->SetAddress(&fULL);
    else if (fIsUC)
      fBranch->SetAddress(&fUC);
    else if (fIsF)
      fBranch->SetAddress(&fF);
    else if (fIsD)
      fBranch->SetAddress(&fD);
    else if (fIsLD)
      fBranch->SetAddress(&fLD);
    else if (fIsC)
      fBranch->SetAddress(&fC);
    else if (fIsstdS)
      fBranch->SetAddress(&fstdS);
    else if (fIsTS)
      fBranch->SetAddress(&fTS);
    else if (fIsTOS)
      fBranch->SetAddress(&fTOS);
    else if (fIsTOA)
      fBranch->SetAddress(&fTOA);
    else if (fIsTCA)
      fBranch->SetAddress(&fTCA);
    else if (fIsTR)
      fBranch->SetAddress(&fTR);
    else if (fIsTRA)
      fBranch->SetAddress(&fTRA);
  } // end if scalar
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fCArray1D) {
    if (fIsB)
      fBranch->SetAddress(fcB);
    else if (fIsI)
      fBranch->SetAddress(fcI);
    else if (fIsSI)
      fBranch->SetAddress(fcSI);
    else if (fIsL)
      fBranch->SetAddress(fcL);
    else if (fIsLL)
      fBranch->SetAddress(fcLL);
    else if (fIsSC)
      fBranch->SetAddress(fcSC);
    else if (fIsUI)
      fBranch->SetAddress(fcUI);
    else if (fIsUSI)
      fBranch->SetAddress(fcUSI);
    else if (fIsUL)
      fBranch->SetAddress(fcUL);
    else if (fIsULL)
      fBranch->SetAddress(fcULL);
    else if (fIsUC)
      fBranch->SetAddress(fcUC);
    else if (fIsF)
      fBranch->SetAddress(fcF);
    else if (fIsD)
      fBranch->SetAddress(fcD);
    else if (fIsLD)
      fBranch->SetAddress(fcLD);
    else if (fIsC)
      fBranch->SetAddress(fcC);
    else if (fIsstdS)
      fBranch->SetAddress(fcstdS);
    else if (fIsTS)
      fBranch->SetAddress(fcTS);
    else if (fIsTOS)
      fBranch->SetAddress(fcTOS);
    else if (fIsTOA)
      fBranch->SetAddress(fcTOA);
    else if (fIsTCA)
      fBranch->SetAddress(fcTCA);
    else if (fIsTR)
      fBranch->SetAddress(fcTR);
    else if (fIsTRA)
      fBranch->SetAddress(fcTRA);
  } // end if 1D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fCArray2D) {
    if (fIsB)
      fBranch->SetAddress(fccB);
    else if (fIsI)
      fBranch->SetAddress(fccI);
    else if (fIsSI)
      fBranch->SetAddress(fccSI);
    else if (fIsL)
      fBranch->SetAddress(fccL);
    else if (fIsLL)
      fBranch->SetAddress(fccLL);
    else if (fIsSC)
      fBranch->SetAddress(fccSC);
    else if (fIsUI)
      fBranch->SetAddress(fccUI);
    else if (fIsUSI)
      fBranch->SetAddress(fccUSI);
    else if (fIsUL)
      fBranch->SetAddress(fccUL);
    else if (fIsULL)
      fBranch->SetAddress(fccULL);
    else if (fIsUC)
      fBranch->SetAddress(fccUC);
    else if (fIsF)
      fBranch->SetAddress(fccF);
    else if (fIsD)
      fBranch->SetAddress(fccD);
    else if (fIsLD)
      fBranch->SetAddress(fccLD);
    else if (fIsC)
      fBranch->SetAddress(fccC);
    else if (fIsstdS)
      fBranch->SetAddress(fccstdS);
    else if (fIsTS)
      fBranch->SetAddress(fccTS);
    else if (fIsTOS)
      fBranch->SetAddress(fccTOS);
    else if (fIsTR)
      fBranch->SetAddress(fccTR);
  }  //end if 2D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec1D) {
    if (fIsB)
      fBranch->SetAddress(&fvB);
    else if (fIsI)
      fBranch->SetAddress(&fvI);
    else if (fIsSI)
      fBranch->SetAddress(&fvSI);
    else if (fIsL)
      fBranch->SetAddress(&fvL);
    else if (fIsLL)
      fBranch->SetAddress(&fvLL);
    else if (fIsSC)
      fBranch->SetAddress(&fvSC);
    else if (fIsUI)
      fBranch->SetAddress(&fvUI);
    else if (fIsUSI)
      fBranch->SetAddress(&fvUSI);
    else if (fIsUL)
      fBranch->SetAddress(&fvUL);
    else if (fIsULL)
      fBranch->SetAddress(&fvULL);
    else if (fIsUC)
      fBranch->SetAddress(&fvUC);
    else if (fIsF)
      fBranch->SetAddress(&fvF);
    else if (fIsD)
      fBranch->SetAddress(&fvD);
    else if (fIsLD)
      fBranch->SetAddress(&fvLD);
    else if (fIsC)
      fBranch->SetAddress(&fvC);
    else if (fIsstdS)
      fBranch->SetAddress(&fvstdS);
    else if (fIsTS)
      fBranch->SetAddress(&fvTS);
    else if (fIsTOS)
      fBranch->SetAddress(&fvTOS);
    else if (fIsTOA)
      fBranch->SetAddress(&fvTOA);
    else if (fIsTCA)
      fBranch->SetAddress(&fvTCA);
    else if (fIsTR)
      fBranch->SetAddress(&fvTR);
    else if (fIsTRA)
      fBranch->SetAddress(&fvTRA);
  } // end if 1D vector
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec2D) {
    if (fIsB)
      fBranch->SetAddress(&fvvB);
    else if (fIsI)
      fBranch->SetAddress(&fvvI);
    else if (fIsSI)
      fBranch->SetAddress(&fvvSI);
    else if (fIsL)
      fBranch->SetAddress(&fvvL);
    else if (fIsLL)
      fBranch->SetAddress(&fvvLL);
    else if (fIsSC)
      fBranch->SetAddress(&fvvSC);
    else if (fIsUI)
      fBranch->SetAddress(&fvvUI);
    else if (fIsUSI)
      fBranch->SetAddress(&fvvUSI);
    else if (fIsUL)
      fBranch->SetAddress(&fvvUL);
    else if (fIsULL)
      fBranch->SetAddress(&fvvULL);
    else if (fIsUC)
      fBranch->SetAddress(&fvvUC);
    else if (fIsF)
      fBranch->SetAddress(&fvvF);
    else if (fIsD)
      fBranch->SetAddress(&fvvD);
    else if (fIsLD)
      fBranch->SetAddress(&fvvLD);
    else if (fIsC)
      fBranch->SetAddress(&fvvC);
    else if (fIsstdS)
      fBranch->SetAddress(&fvvstdS);
    else if (fIsTS)
      fBranch->SetAddress(&fvvTS);
    else if (fIsTOS)
      fBranch->SetAddress(&fvvTOS);
    else if (fIsTR)
      fBranch->SetAddress(&fvvTR);
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
    if (fIsB) {
      if (fRows < n){
        if (fcB) delete[] fcB;
        fcB = new bool[n];
        fBranch->SetAddress(fcB);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvB[fStorageIndex].clear();
      fTreeReader->fvB[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvB[fStorageIndex].push_back(fcB[i]);
    }
    else if (fIsI) {
      if (fRows < n){
        if (fcI) delete[] fcI;
        fcI = new int[n];
        fBranch->SetAddress(fcI);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcI[i]);
    }
    else if (fIsSI) {
      if (fRows < n){
        if (fcSI) delete[] fcSI;
        fcSI = new short int[n];
        fBranch->SetAddress(fcSI);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcSI[i]);
    }
    else if (fIsL) {
      if (fRows < n){
        if (fcL) delete[] fcL;
        fcL = new long int[n];
        fBranch->SetAddress(fcL);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcL[i]);
    }
    else if (fIsLL) {
      if (fRows < n){
        if (fcLL) delete[] fcLL;
        fcLL = new long long int[n];
        fBranch->SetAddress(fcLL);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcLL[i]);
    }
    else if (fIsSC) {
      if (fRows < n){
        if (fcSC) delete[] fcSC;
        fcSC = new signed char[n];
        fBranch->SetAddress(fcSC);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      fTreeReader->fvI[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(fcSC[i]);
    }
    else if (fIsUI) {
      if (fRows < n){
        if (fcUI) delete[] fcUI;
        fcUI = new unsigned int[n];
        fBranch->SetAddress(fcUI);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUI[i]);
    }
    else if (fIsUSI) {
      if (fRows < n){
        if (fcUSI) delete[] fcUSI;
        fcUSI = new unsigned short int[n];
        fBranch->SetAddress(fcUSI);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUSI[i]);
    }
    else if (fIsUL) {
      if (fRows < n){
        if (fcUL) delete[] fcUL;
        fcUL = new unsigned long int[n];
        fBranch->SetAddress(fcUL);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUL[i]);
    }
    else if (fIsULL) {
      if (fRows < n){
        if (fcULL) delete[] fcULL;
        fcULL = new unsigned long long int[n];
        fBranch->SetAddress(fcULL);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcULL[i]);
    }
    else if (fIsUC) {
      if (fRows < n){
        if (fcUC) delete[] fcUC;
        fcUC = new unsigned char[n];
        fBranch->SetAddress(fcUC);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      fTreeReader->fvC[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(fcUC[i]);
    }
    else if (fIsF) {
      if (fRows < n){
        if (fcF) delete[] fcF;
        fcF = new float[n];
        fBranch->SetAddress(fcF);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(fcF[i]);
    }
    else if (fIsD) {
      if (fRows < n){
        if (fcD) delete[] fcD;
        fcD = new double[n];
        fBranch->SetAddress(fcD);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(fcD[i]);
    }
    else if (fIsLD) {
      if (fRows < n){
        if (fcLD) delete[] fcLD;
        fcLD = new long double[n];
        fBranch->SetAddress(fcLD);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      fTreeReader->fvD[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(fcLD[i]);
    }
    else if (fIsC) {
      if (fRows < n){
        if (fcC) delete[] fcC;
        fcC = new char[n];
        fBranch->SetAddress(fcC);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcC[i]);
    }
    else if (fIsstdS) {
      if (fRows < n){
        if (fcstdS) delete[] fcstdS;
        fcstdS = new std::string[n];
        fBranch->SetAddress(fcstdS);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcstdS[i].c_str());
    }
    else if (fIsTS) {
      if (fRows < n){
        if (fcTS) delete[] fcTS;
        fcTS = new TString[n];
        fBranch->SetAddress(fcTS);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcTS[i]);
    }
    else if (fIsTOS) {
      if (fRows < n){
        if (fcTOS) delete[] fcTOS;
        fcTOS = new TObjString[n];
        fBranch->SetAddress(fcTOS);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      fTreeReader->fvS[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(fcTOS[i].String());
    }
    else if (fIsTOA) {
      if (fRows < n){
        if (fcTOA) delete[] fcTOA;
        fcTOA = new TObjArray[n];
        fBranch->SetAddress(fcTOA);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvOA[fStorageIndex].clear();
      fTreeReader->fvOA[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvOA[fStorageIndex].push_back(fcTOA[i]);
    }
    else if (fIsTCA) {
      if (fRows < n){
        if (fcTCA) delete[] fcTCA;
        fcTCA = new TClonesArray[n];
        fBranch->SetAddress(fcTCA);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvCA[fStorageIndex].clear();
      fTreeReader->fvCA[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvCA[fStorageIndex].push_back(fcTCA[i]);
    }
    else if (fIsTR) {
      if (fRows < n){
        if (fcTR) delete[] fcTR;
        fcTR = new TRef[n];
        fBranch->SetAddress(fcTR);
        fRows = n;
      }
      fBranch->GetEntry(entry);
      fTreeReader->fvR[fStorageIndex].clear();
      fTreeReader->fvR[fStorageIndex].reserve(n);
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvR[fStorageIndex].push_back(fcTR[i]);
    }
    else if (fIsTRA) {
      if (fRows < n){
        if (fcTRA) delete[] fcTRA;
        fcTRA = new TRefArray[n];
        fBranch->SetAddress(fcTRA);
        fRows = n;
      }
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
      if (fRows < n || fColumns < m){
        if (fccB) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccB[i];
          delete[] fccB;
        }
        fccB = new bool*[n];
        for (Int_t i = 0; i < n; ++i) fccB[i] = new bool[m];
        fBranch->SetAddress(fccB);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccI) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccI[i];
          delete[] fccI;
        }
        fccI = new int*[n];
        for (Int_t i = 0; i < n; ++i) fccI[i] = new int[m];
        fBranch->SetAddress(fccI);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccSI) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccSI[i];
          delete[] fccSI;
        }
        fccSI = new short int*[n];
        for (Int_t i = 0; i < n; ++i) fccSI[i] = new short int[m];
        fBranch->SetAddress(fccSI);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccL) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccL[i];
          delete[] fccL;
        }
        fccL = new long int*[n];
        for (Int_t i = 0; i < n; ++i) fccL[i] = new long int[m];
        fBranch->SetAddress(fccL);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccLL) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccLL[i];
          delete[] fccLL;
        }
        fccLL = new long long int*[n];
        for (Int_t i = 0; i < n; ++i) fccLL[i] = new long long int[m];
        fBranch->SetAddress(fccLL);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccSC) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccSC[i];
          delete[] fccSC;
        }
        fccSC = new signed char*[n];
        for (Int_t i = 0; i < n; ++i) fccSC[i] = new signed char[m];
        fBranch->SetAddress(fccSC);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccUI) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccUI[i];
          delete[] fccUI;
        }
        fccUI = new unsigned int*[n];
        for (Int_t i = 0; i < n; ++i) fccUI[i] = new unsigned int[m];
        fBranch->SetAddress(fccUI);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccUSI) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccUSI[i];
          delete[] fccUSI;
        }
        fccUSI = new unsigned short int*[n];
        for (Int_t i = 0; i < n; ++i) fccUSI[i] = new unsigned short int[m];
        fBranch->SetAddress(fccUSI);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccUL) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccUL[i];
          delete[] fccUL;
        }
        fccUL = new unsigned long int*[n];
        for (Int_t i = 0; i < n; ++i) fccUL[i] = new unsigned long int[m];
        fBranch->SetAddress(fccUL);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccULL) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccULL[i];
          delete[] fccULL;
        }
        fccULL = new unsigned long long int*[n];
        for (Int_t i = 0; i < n; ++i) fccULL[i] = new unsigned long long int[m];
        fBranch->SetAddress(fccULL);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccUC) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccUC[i];
          delete[] fccUC;
        }
        fccUC = new unsigned char*[n];
        for (Int_t i = 0; i < n; ++i) fccUC[i] = new unsigned char[m];
        fBranch->SetAddress(fccUC);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccF) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccF[i];
          delete[] fccF;
        }
        fccF = new float*[n];
        for (Int_t i = 0; i < n; ++i) fccF[i] = new float[m];
        fBranch->SetAddress(fccF);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccD) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccD[i];
          delete[] fccD;
        }
        fccD = new double*[n];
        for (Int_t i = 0; i < n; ++i) fccD[i] = new double[m];
        fBranch->SetAddress(fccD);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccLD) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccLD[i];
          delete[] fccLD;
        }
        fccLD = new long double*[n];
        for (Int_t i = 0; i < n; ++i) fccLD[i] = new long double[m];
        fBranch->SetAddress(fccLD);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccC) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccC[i];
          delete[] fccC;
        }
        fccC = new char*[n];
        for (Int_t i = 0; i < n; ++i) fccC[i] = new char[m];
        fBranch->SetAddress(fccC);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccstdS) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccstdS[i];
          delete[] fccstdS;
        }
        fccstdS = new std::string*[n];
        for (Int_t i = 0; i < n; ++i) fccstdS[i] = new std::string[m];
        fBranch->SetAddress(fccstdS);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccTS) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccTS[i];
          delete[] fccTS;
        }
        fccTS = new TString*[n];
        for (Int_t i = 0; i < n; ++i) fccTS[i] = new TString[m];
        fBranch->SetAddress(fccTS);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccTOS) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccTOS[i];
          delete[] fccTOS;
        }
        fccTOS = new TObjString*[n];
        for (Int_t i = 0; i < n; ++i) fccTOS[i] = new TObjString[m];
        fBranch->SetAddress(fccTOS);
        fRows = n;
        fColumns = m;
      }
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
      if (fRows < n || fColumns < m){
        if (fccTR) {
          for (Int_t i = 0; i < fRows; ++i) delete[] fccTR[i];
          delete[] fccTR;
        }
        fccTR = new TRef*[n];
        for (Int_t i = 0; i < n; ++i) fccTR[i] = new TRef[m];
        fBranch->SetAddress(fccTR);
        fRows = n;
        fColumns = m;
      }
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

Int_t AnalysisTreeReader::BranchManager::GetArrayLength (Int_t dim) {
  TString size;
  if (fLeafTitle.Contains(fTreeReader->fArray2D) || fLeafTitle.Contains(fTreeReader->fArray))
    size = fLeafTitle;
  else if (fBranchTitle.Contains(fTreeReader->fArray2D) || fBranchTitle.Contains(fTreeReader->fArray))
    size = fBranchTitle;
  else
    return fLeafNdata;

  for (Int_t i = 0; i < dim; ++i)
    size.Remove(0, size.First('[') + 1);
  size.Remove(size.First(']'), size.Length());

  // size is just a static number, so return it
  if (size.Contains(TRegexp("[^a-zA-Z_]")))
    return size.Atoi();

  return fTreeReader->GetInteger(size);
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
