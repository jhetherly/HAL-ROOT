#include <HAL/AnalysisTreeReader.h>

ClassImp(HAL::AnalysisTreeReader);

namespace HAL {

AnalysisTreeReader::AnalysisTreeReader (TTree *t) : fChain(t), 
  fScalar("^[a-zA-Z][a-zA-Z0-9_]+$"),
  fVector("^vector<[a-zA-Z][a-zA-Z0-9_]+>$"), 
  fVector2D("^vector<vector<[a-zA-Z][a-zA-Z0-9_]+> >$"), 
  fArray("^[a-zA-Z_][a-zA-Z0-9_]+[[a-zA-Z0-9_]+]$"), 
  fArray2D("^[a-zA-Z_][a-zA-Z0-9_]+[[a-zA-Z0-9_]+][[a-zA-Z0-9_]+]$") {
  // bool type
  fBool.insert("Bool_t");
  fBool.insert("bool");
  // integer type
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
  // counting type
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
  // decimal type
  fFloat.insert("Real_t");
  fFloat.insert("Float_t");
  fFloat.insert("Float16_t");
  fFloat.insert("float");
  fDouble.insert("Double_t");
  fDouble.insert("Double32_t");
  fDouble.insert("double");
  fLDouble.insert("LongDouble_t");
  fLDouble.insert("long double");
  // string type
  fChar.insert("Text_t");
  fChar.insert("Char_t");
  fChar.insert("char");
}

AnalysisTreeReader::~AnalysisTreeReader () {
}

void AnalysisTreeReader::SetEntry (Long64_t entry) {
  fEntry = entry;

  // Update all branches
  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    (*bm)->SetEntry(entry);
  }
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

    if (name.BeginsWith(nn, TString::kIgnoreCase)) {
      name.Replace(0, nn.Length(), bn);
      break;
    }
  }
  // Recheck if branch or leaf has name
  if (fChain->FindBranch(name.Data()))
    return name;
  if (fChain->FindLeaf(name.Data()))
    return TString(fChain->GetLeaf(name.Data())->GetBranch()->GetName());

  throw HALException(name.Prepend("Couldn't find branch: ").Data());
}

Int_t AnalysisTreeReader::GetDim (TString branchname, Int_t idx_1) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }

  if (branchmanager->GetStorageType() == kOA)
    return fOA[branchmanager->GetStorageIndex()].GetEntries();
  if (branchmanager->GetStorageType() == kCA)
    return fCA[branchmanager->GetStorageIndex()].GetEntries();
  if (branchmanager->GetStorageType() == kRA)
    return fRA[branchmanager->GetStorageIndex()].GetEntries();
  if (branchmanager->IsScalar())
    throw HALException(bname.Prepend("Tried getting dimension of scalar in branch: ").Data());
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
    return fvOA[branchmanager->GetStorageIndex()][idx_1].GetEntries();
  }
  if (branchmanager->GetStorageType() == kvCA) {
    if (idx_1 == -1)
      return fvCA[branchmanager->GetStorageIndex()].size();
    return fvCA[branchmanager->GetStorageIndex()][idx_1].GetEntries();
  }
  if (branchmanager->GetStorageType() == kvRA) {
    if (idx_1 == -1)
      return fvRA[branchmanager->GetStorageIndex()].size();
    return fvRA[branchmanager->GetStorageIndex()][idx_1].GetEntries();
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

  throw HALException(bname.Prepend("Error in finding dimensions in branch: ").Data());
}

Bool_t AnalysisTreeReader::GetBool (TString branchname, Int_t idx_1, Int_t idx_2) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }

  if (branchmanager->GetStorageType() == kB)
    return fB[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvB)
    return fvB[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvB)
    return fvvB[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(bname.Prepend("Couldn't find bool data in branch: ").Data());
}

Long64_t AnalysisTreeReader::GetInteger (TString branchname, Int_t idx_1, Int_t idx_2) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }

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

  throw HALException(bname.Prepend("Couldn't find integer number data in branch: ").Data());
}

ULong64_t AnalysisTreeReader::GetCounting (TString branchname, Int_t idx_1, Int_t idx_2) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Couldn't find branch: ").Data());
  }

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

  throw HALException(bname.Prepend("Couldn't find counting number data in branch: ").Data());
}

LongDouble_t AnalysisTreeReader::GetDecimal (TString branchname, Int_t idx_1, Int_t idx_2) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Error in setting up branch: ").Data());
  }

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

  throw HALException(bname.Prepend("Couldn't find decimal number data in branch: ").Data());
}

TString AnalysisTreeReader::GetString (TString branchname, Int_t idx_1, Int_t idx_2) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Error in setting up branch: ").Data());
  }

  if (branchmanager->GetStorageType() == kS)
    return fS[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvS)
    return fvS[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvS)
    return fvvS[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(bname.Prepend("Couldn't find string data in branch: ").Data());
}

TObjArray& AnalysisTreeReader::GetObjArray (TString branchname, Int_t idx_1) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Error in setting up branch: ").Data());
  }

  if (branchmanager->GetStorageType() == kOA)
    return fOA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvOA)
    return fvOA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(bname.Prepend("Couldn't find TObjArray data in branch: ").Data());
}

TClonesArray& AnalysisTreeReader::GetClonesArray (TString branchname, Int_t idx_1) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Error in setting up branch: ").Data());
  }

  if (branchmanager->GetStorageType() == kCA)
    return fCA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvCA)
    return fvCA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(bname.Prepend("Couldn't find TClonesArray data in branch: ").Data());
}

TRef& AnalysisTreeReader::GetRef (TString branchname, Int_t idx_1, Int_t idx_2) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Error in setting up branch: ").Data());
  }

  if (branchmanager->GetStorageType() == kR)
    return fR[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvR)
    return fvR[branchmanager->GetStorageIndex()][idx_1];
  if (branchmanager->GetStorageType() == kvvR)
    return fvvR[branchmanager->GetStorageIndex()][idx_1][idx_2];

  throw HALException(bname.Prepend("Couldn't find TRef data in branch: ").Data());
}

TRefArray& AnalysisTreeReader::GetRefArray (TString branchname, Int_t idx_1) {
  TString bname = GetFullBranchName( branchname );
  BranchManager *branchmanager = NULL;

  for (std::vector<BranchManager*>::iterator bm = fBranchManagers.begin(); 
       bm != fBranchManagers.end(); ++bm) {
    if ((*bm)->GetName() == bname)
      branchmanager = *bm;
  }
  if (branchmanager == NULL) {
    branchmanager = new BranchManager(this);
    if (branchmanager->Create(bname))
      fBranchManagers.push_back(branchmanager);
    else
      throw HALException(bname.Prepend("Error in setting up branch: ").Data());
  }

  if (branchmanager->GetStorageType() == kRA)
    return fRA[branchmanager->GetStorageIndex()];
  if (branchmanager->GetStorageType() == kvRA)
    return fvRA[branchmanager->GetStorageIndex()][idx_1];

  throw HALException(bname.Prepend("Couldn't find TRefArray data in branch: ").Data());
}


// ///////////////////////////////////////
// BranchManager private class
// ///////////////////////////////////////

AnalysisTreeReader::BranchManager::BranchManager (AnalysisTreeReader *tr) : 
  fScalar(kFALSE), fCArray1D(kFALSE), fCArray2D(kFALSE), 
  fVec1D(kFALSE), fVec2D(kFALSE), fBranch(NULL), fTreeReader(tr) {
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
             fType.CompareTo("string") == 0 ||
             fType.CompareTo("TString") == 0 ||
             fType.CompareTo("TObjString") == 0) {
      fStorageID = kS;
      fTreeReader->fS.push_back("");
      fStorageIndex = fTreeReader->fS.size() - 1;
    }
    else if (fType.CompareTo("TObjArray") == 0) {
      TObjArray temp;
      fStorageID = kOA;
      fTreeReader->fOA.push_back(temp);
      fStorageIndex = fTreeReader->fOA.size() - 1;
    }
    else if (fType.CompareTo("TClonesArray") == 0) {
      TClonesArray temp;
      fStorageID = kCA;
      fTreeReader->fCA.push_back(temp);
      fStorageIndex = fTreeReader->fCA.size() - 1;
    }
    else if (fType.CompareTo("TRef") == 0) {
      TRef temp;
      fStorageID = kR;
      fTreeReader->fR.push_back(temp);
      fStorageIndex = fTreeReader->fR.size() - 1;
    }
    else if (fType.CompareTo("TRefArray") == 0) {
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
      std::vector<Bool_t> temp;
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
      std::vector<Long64_t> temp;
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
      std::vector<ULong64_t> temp;
      fStorageID = kvC;
      fTreeReader->fvC.push_back(temp);
      fStorageIndex = fTreeReader->fvC.size() - 1;
    }
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fScalarType) != 0 || 
             fTreeReader->fDouble.count(fScalarType) != 0 ||
             fTreeReader->fLDouble.count(fScalarType) != 0) {
      std::vector<LongDouble_t> temp;
      fStorageID = kvD;
      fTreeReader->fvD.push_back(temp);
      fStorageIndex = fTreeReader->fvD.size() - 1;
    }
    // Test for string types
    else if (fTreeReader->fChar.count(fScalarType) != 0 || 
             fScalarType.CompareTo("string") == 0 ||
             fScalarType.CompareTo("TString") == 0 ||
             fScalarType.CompareTo("TObjString") == 0) {
      std::vector<TString> temp;
      fStorageID = kvS;
      fTreeReader->fvS.push_back(temp);
      fStorageIndex = fTreeReader->fvS.size() - 1;
    }
    else if (fScalarType.CompareTo("TObjArray") == 0) {
      std::vector<TObjArray> temp;
      fStorageID = kvOA;
      fTreeReader->fvOA.push_back(temp);
      fStorageIndex = fTreeReader->fvOA.size() - 1;
    }
    else if (fScalarType.CompareTo("TClonesArray") == 0) {
      std::vector<TClonesArray> temp;
      fStorageID = kvCA;
      fTreeReader->fvCA.push_back(temp);
      fStorageIndex = fTreeReader->fvCA.size() - 1;
    }
    else if (fScalarType.CompareTo("TRef") == 0) {
      std::vector<TRef> temp;
      fStorageID = kvR;
      fTreeReader->fvR.push_back(temp);
      fStorageIndex = fTreeReader->fvR.size() - 1;
    }
    else if (fScalarType.CompareTo("TRefArray") == 0) {
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
      std::vector<std::vector<Bool_t> > temp;
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
      std::vector<std::vector<Long64_t> > temp;
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
      std::vector<std::vector<ULong64_t> > temp;
      fStorageID = kvvC;
      fTreeReader->fvvC.push_back(temp);
      fStorageIndex = fTreeReader->fvvC.size() - 1;
    }
    // Test for decimal types
    else if (fTreeReader->fFloat.count(fScalarType) != 0 || 
             fTreeReader->fDouble.count(fScalarType) != 0 ||
             fTreeReader->fLDouble.count(fScalarType) != 0) {
      std::vector<std::vector<LongDouble_t> > temp;
      fStorageID = kvvD;
      fTreeReader->fvvD.push_back(temp);
      fStorageIndex = fTreeReader->fvvD.size() - 1;
    }
    // Test for string types
    else if (fTreeReader->fChar.count(fScalarType) != 0 || 
             fScalarType.CompareTo("string") == 0 ||
             fScalarType.CompareTo("TString") == 0 ||
             fScalarType.CompareTo("TObjString") == 0) {
      std::vector<std::vector<TString> > temp;
      fStorageID = kvvS;
      fTreeReader->fvvS.push_back(temp);
      fStorageIndex = fTreeReader->fvvS.size() - 1;
    }
    else if (fScalarType.CompareTo("TRef") == 0) {
      std::vector<std::vector<TRef> > temp;
      fStorageID = kvvR;
      fTreeReader->fvvR.push_back(temp);
      fStorageIndex = fTreeReader->fvvR.size() - 1;
    }
  } // end if 2D vector/c-array
  // ////////////////////////////////////////////////////
  // ////////////////////////////////////////////////////

  SetEntry(fTreeReader->fEntry);

  return kTRUE;
}


// ////////////////////////////////////////////////////
// More tediousness
// ////////////////////////////////////////////////////

void AnalysisTreeReader::BranchManager::SetEntry (Long64_t entry) {

  if (fBranch != NULL)
    fBranch->ResetAddress();

  if (fScalar) {
    if (fTreeReader->fBool.count(fType) != 0) {
      bool temp = kFALSE;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fB[fStorageIndex] = temp;
    }
    else if (fTreeReader->fInteger.count(fType) != 0) {
      int temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fShort.count(fType) != 0) {
      Short_t temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fLong.count(fType) != 0) {
      Long_t temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fLLong.count(fType) != 0) {
      Long64_t temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fSChar.count(fType) != 0) {
      signed char temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fUInteger.count(fType) != 0) {
      unsigned int temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fUShort.count(fType) != 0) {
      UShort_t temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fULong.count(fType) != 0) {
      ULong_t temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fULLong.count(fType) != 0) {
      ULong64_t temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fUChar.count(fType) != 0) {
      unsigned char temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fFloat.count(fType) != 0) {
      Float_t temp = 0.0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fD[fStorageIndex] = temp;
    }
    else if (fTreeReader->fDouble.count(fType) != 0) {
      Double_t temp = 0.0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fD[fStorageIndex] = temp;
    }
    else if (fTreeReader->fLDouble.count(fType) != 0) {
      LongDouble_t temp = 0.0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fD[fStorageIndex] = temp;
    }
    else if (fTreeReader->fChar.count(fType) != 0) {
      Char_t temp;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fS[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("string") == 0) {
      std::string temp;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fS[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TString") == 0) {
      TString temp;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fS[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TObjString") == 0) {
      TObjString temp;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fS[fStorageIndex] = temp.String();
    }
    else if (fType.CompareTo("TObjArray") == 0) {
      TObjArray temp;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fOA[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TClonesArray") == 0) {
      TClonesArray temp;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fCA[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TRef") == 0) {
      TRef temp;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fR[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TRefArray") == 0) {
      TRefArray temp;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fRA[fStorageIndex] = temp;
    }
  } // end if scalar
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fCArray1D) {
    if (fTreeReader->fBool.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      bool temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvB[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvB[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fInteger.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      int temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fShort.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Short_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fLong.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Long_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fLLong.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Long64_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fSChar.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      signed char temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvI[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fUInteger.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      unsigned int temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fUShort.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      UShort_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fULong.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      ULong_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fULLong.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      ULong64_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fUChar.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      unsigned char temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvC[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fFloat.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Float_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fDouble.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Double_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fLDouble.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      LongDouble_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvD[fStorageIndex].push_back(temp[i]);
    }
    else if (fTreeReader->fChar.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Char_t temp[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(temp[i]);
    }
    else if (fScalarType.CompareTo("string") == 0) {
      Int_t n = GetArrayLength(1);
      std::string *temp = new std::string[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(temp[i].c_str());
    }
    else if (fScalarType.CompareTo("TString") == 0) {
      Int_t n = GetArrayLength(1);
      TString *temp = new TString[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(temp[i]);
      delete temp;
    }
    else if (fScalarType.CompareTo("TObjString") == 0) {
      Int_t n = GetArrayLength(1);
      TObjString *temp = new TObjString[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvS[fStorageIndex].push_back(temp[i].GetString());
      delete temp;
    }
    else if (fScalarType.CompareTo("TObjArray") == 0) {
      Int_t n = GetArrayLength(1);
      TObjArray *temp = new TObjArray[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvOA[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvOA[fStorageIndex].push_back(temp[i]);
      delete temp;
    }
    else if (fScalarType.CompareTo("TClonesArray") == 0) {
      Int_t n = GetArrayLength(1);
      TClonesArray *temp = new TClonesArray[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvCA[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvCA[fStorageIndex].push_back(temp[i]);
      delete temp;
    }
    else if (fScalarType.CompareTo("TRef") == 0) {
      Int_t n = GetArrayLength(1);
      TRef *temp = new TRef[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvR[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvR[fStorageIndex].push_back(temp[i]);
      delete temp;
    }
    else if (fScalarType.CompareTo("TRefArray") == 0) {
      Int_t n = GetArrayLength(1);
      TRefArray *temp = new TRefArray[n];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvRA[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i)
        fTreeReader->fvRA[fStorageIndex].push_back(temp[i]);
      delete temp;
    }
  } // end if 1D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fCArray2D) {
    if (fTreeReader->fBool.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      bool temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvB[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<Bool_t> row;
        fTreeReader->fvvB[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvB[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fInteger.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      int temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fShort.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      Short_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fLong.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      Long_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fLLong.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      Long64_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fSChar.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      signed char temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvI[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fUInteger.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      unsigned int temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fUShort.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      UShort_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fULong.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      ULong_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fULLong.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      ULong64_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fUChar.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      unsigned char temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvC[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fFloat.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      Float_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvD[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<LongDouble_t> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvD[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fDouble.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      Double_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvD[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<LongDouble_t> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvD[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fLDouble.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      LongDouble_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvD[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<LongDouble_t> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvD[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fTreeReader->fChar.count(fScalarType) != 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      Char_t temp[n][m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvS[fStorageIndex].back().push_back(temp[i][j]);
      }
    }
    else if (fScalarType.CompareTo("string") == 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      std::string **temp = new std::string*[n];
      for (Int_t i = 0; i < n; ++i)
        temp[n] = new std::string[m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvS[fStorageIndex].back().push_back(temp[i][j].c_str());
      }
      for (Int_t i = 0; i < n; ++i)
        delete[] temp[i];
      delete[] temp;
    }
    else if (fScalarType.CompareTo("TString") == 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      TString **temp = new TString*[n];
      for (Int_t i = 0; i < n; ++i)
        temp[n] = new TString[m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvS[fStorageIndex].back().push_back(temp[i][j]);
      }
      for (Int_t i = 0; i < n; ++i)
        delete[] temp[i];
      delete[] temp;
    }
    else if (fScalarType.CompareTo("TObjString") == 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      TObjString **temp = new TObjString*[n];
      for (Int_t i = 0; i < n; ++i)
        temp[n] = new TObjString[m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvS[fStorageIndex].back().push_back(temp[i][j].GetString());
      }
      for (Int_t i = 0; i < n; ++i)
        delete[] temp[i];
      delete[] temp;
    }
    else if (fScalarType.CompareTo("TRef") == 0) {
      Int_t n = GetArrayLength(1);
      Int_t m = GetArrayLength(2);
      TRef **temp = new TRef*[n];
      for (Int_t i = 0; i < n; ++i)
        temp[n] = new TRef[m];
      fBranch->SetAddress(temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvR[fStorageIndex].clear();
      for (Int_t i = 0; i < n; ++i) {
        std::vector<TRef> row;
        fTreeReader->fvvR[fStorageIndex].push_back(row);
        for (Int_t j = 0; j < m; ++j)
          fTreeReader->fvvR[fStorageIndex].back().push_back(temp[i][j]);
      }
      for (Int_t i = 0; i < n; ++i)
        delete[] temp[i];
      delete[] temp;
    }
  }  //end if 2D c-array
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec1D) {
    if (fTreeReader->fBool.count(fScalarType) != 0) {
      std::vector<bool> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvB[fStorageIndex].clear();
      for (std::vector<bool>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvB[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fInteger.count(fScalarType) != 0) {
      std::vector<int> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (std::vector<int>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fShort.count(fScalarType) != 0) {
      std::vector<Short_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (std::vector<Short_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fLong.count(fScalarType) != 0) {
      std::vector<Long_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (std::vector<Long_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fLLong.count(fScalarType) != 0) {
      std::vector<Long64_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (std::vector<Long64_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fSChar.count(fScalarType) != 0) {
      std::vector<signed char> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvI[fStorageIndex].clear();
      for (std::vector<signed char>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvI[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fUInteger.count(fScalarType) != 0) {
      std::vector<unsigned int> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (std::vector<unsigned int>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fUShort.count(fScalarType) != 0) {
      std::vector<UShort_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (std::vector<UShort_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fULong.count(fScalarType) != 0) {
      std::vector<ULong_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (std::vector<ULong_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fULLong.count(fScalarType) != 0) {
      std::vector<ULong64_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (std::vector<ULong64_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fUChar.count(fScalarType) != 0) {
      std::vector<unsigned char> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvC[fStorageIndex].clear();
      for (std::vector<unsigned char>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvC[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fFloat.count(fScalarType) != 0) {
      std::vector<Float_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      for (std::vector<Float_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvD[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fDouble.count(fScalarType) != 0) {
      std::vector<Double_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      for (std::vector<Double_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvD[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fLDouble.count(fScalarType) != 0) {
      std::vector<LongDouble_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvD[fStorageIndex].clear();
      for (std::vector<LongDouble_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvD[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fTreeReader->fChar.count(fScalarType) != 0) {
      std::vector<Char_t> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      for (std::vector<Char_t>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvS[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fScalarType.CompareTo("string") == 0) {
      std::vector<std::string> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      for (std::vector<std::string>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvS[fStorageIndex].push_back(it->c_str());
      delete temp;
    }
    else if (fScalarType.CompareTo("TString") == 0) {
      std::vector<TString> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      for (std::vector<TString>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvS[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fScalarType.CompareTo("TObjString") == 0) {
      std::vector<TObjString> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvS[fStorageIndex].clear();
      for (std::vector<TObjString>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvS[fStorageIndex].push_back(it->GetString());
      delete temp;
    }
    else if (fScalarType.CompareTo("TObjArray") == 0) {
      std::vector<TObjArray> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvOA[fStorageIndex].clear();
      for (std::vector<TObjArray>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvOA[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fScalarType.CompareTo("TClonesArray") == 0) {
      std::vector<TClonesArray> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvCA[fStorageIndex].clear();
      for (std::vector<TClonesArray>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvCA[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fScalarType.CompareTo("TRef") == 0) {
      std::vector<TRef> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvR[fStorageIndex].clear();
      for (std::vector<TRef>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvR[fStorageIndex].push_back(*it);
      delete temp;
    }
    else if (fScalarType.CompareTo("TRefArray") == 0) {
      std::vector<TRefArray> *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvRA[fStorageIndex].clear();
      for (std::vector<TRefArray>::iterator it = temp->begin(); it != temp->end(); ++it)
        fTreeReader->fvRA[fStorageIndex].push_back(*it);
      delete temp;
    }
  } // end if 1D vector
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////
  else if (fVec2D) {
    if (fTreeReader->fBool.count(fScalarType) != 0) {
      std::vector<std::vector<bool> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvB[fStorageIndex].clear();
      for (std::vector<std::vector<bool> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<Bool_t> row;
        fTreeReader->fvvB[fStorageIndex].push_back(row);
        for (std::vector<bool>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvB[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fInteger.count(fScalarType) != 0) {
      std::vector<std::vector<int> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (std::vector<std::vector<int> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (std::vector<int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fShort.count(fScalarType) != 0) {
      std::vector<std::vector<Short_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (std::vector<std::vector<Short_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (std::vector<Short_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fLong.count(fScalarType) != 0) {
      std::vector<std::vector<Long_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (std::vector<std::vector<Long_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (std::vector<Long_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fLLong.count(fScalarType) != 0) {
      std::vector<std::vector<Long64_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (std::vector<std::vector<Long64_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (std::vector<Long64_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fSChar.count(fScalarType) != 0) {
      std::vector<std::vector<signed char> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvI[fStorageIndex].clear();
      for (std::vector<std::vector<signed char> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<Long64_t> row;
        fTreeReader->fvvI[fStorageIndex].push_back(row);
        for (std::vector<signed char>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvI[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fUInteger.count(fScalarType) != 0) {
      std::vector<std::vector<unsigned int> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (std::vector<std::vector<unsigned int> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (std::vector<unsigned int>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fUShort.count(fScalarType) != 0) {
      std::vector<std::vector<UShort_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (std::vector<std::vector<UShort_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (std::vector<UShort_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fULong.count(fScalarType) != 0) {
      std::vector<std::vector<ULong_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (std::vector<std::vector<ULong_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (std::vector<ULong_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fULLong.count(fScalarType) != 0) {
      std::vector<std::vector<ULong64_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (std::vector<std::vector<ULong64_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (std::vector<ULong64_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fUChar.count(fScalarType) != 0) {
      std::vector<std::vector<unsigned char> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvC[fStorageIndex].clear();
      for (std::vector<std::vector<unsigned char> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<ULong64_t> row;
        fTreeReader->fvvC[fStorageIndex].push_back(row);
        for (std::vector<unsigned char>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvC[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fFloat.count(fScalarType) != 0) {
      std::vector<std::vector<Float_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvD[fStorageIndex].clear();
      for (std::vector<std::vector<Float_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<LongDouble_t> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        for (std::vector<Float_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvD[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fDouble.count(fScalarType) != 0) {
      std::vector<std::vector<Double_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvD[fStorageIndex].clear();
      for (std::vector<std::vector<Double_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<LongDouble_t> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        for (std::vector<Double_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvD[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fLDouble.count(fScalarType) != 0) {
      std::vector<std::vector<LongDouble_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvD[fStorageIndex].clear();
      for (std::vector<std::vector<LongDouble_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<LongDouble_t> row;
        fTreeReader->fvvD[fStorageIndex].push_back(row);
        for (std::vector<LongDouble_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvD[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fTreeReader->fChar.count(fScalarType) != 0) {
      std::vector<std::vector<Char_t> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      for (std::vector<std::vector<Char_t> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        for (std::vector<Char_t>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvS[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fScalarType.CompareTo("string") == 0) {
      std::vector<std::vector<std::string> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      for (std::vector<std::vector<std::string> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        for (std::vector<std::string>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvS[fStorageIndex].back().push_back(it->c_str());
      }
      delete temp;
    }
    else if (fScalarType.CompareTo("TString") == 0) {
      std::vector<std::vector<TString> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      for (std::vector<std::vector<TString> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        for (std::vector<TString>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvS[fStorageIndex].back().push_back(*it);
      }
      delete temp;
    }
    else if (fScalarType.CompareTo("TObjString") == 0) {
      std::vector<std::vector<TObjString> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvS[fStorageIndex].clear();
      for (std::vector<std::vector<TObjString> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<TString> row;
        fTreeReader->fvvS[fStorageIndex].push_back(row);
        for (std::vector<TObjString>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvS[fStorageIndex].back().push_back(it->GetString());
      }
      delete temp;
    }
    else if (fScalarType.CompareTo("TRef") == 0) {
      std::vector<std::vector<TRef> > *temp = 0;
      fBranch->SetAddress(&temp);
      fBranch->GetEntry(entry);
      fTreeReader->fvvR[fStorageIndex].clear();
      for (std::vector<std::vector<TRef> >::iterator iit = temp->begin(); iit != temp->end(); ++iit) {
        std::vector<TRef> row;
        fTreeReader->fvvR[fStorageIndex].push_back(row);
        for (std::vector<TRef>::iterator it = iit->begin(); it != iit->end(); ++it)
          fTreeReader->fvvR[fStorageIndex].back().push_back(*it);
      }
      delete temp;
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
