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
  fInteger.insert("Int_t");
  fInteger.insert("int");
  fShort.insert("Short_t");
  fShort.insert("short");
  fShort.insert("short int");
  fLong.insert("Long_t");
  fLong.insert("long");
  fLong.insert("long int");
  fLLong.insert("Long64_t");
  fLLong.insert("long long");
  fLLong.insert("long long int");
  fSChar.insert("signed char");
  // counting type
  fUInteger.insert("UInt_t");
  fUInteger.insert("unsigned");
  fUInteger.insert("unsigned int");
  fUShort.insert("UShort_t");
  fUShort.insert("unsigned short");
  fUShort.insert("unsigned short int");
  fULong.insert("ULong_t");
  fULong.insert("unsigned long");
  fULong.insert("unsigned long int");
  fULLong.insert("ULong64_t");
  fULLong.insert("unsigned long long");
  fULLong.insert("unsigned long long int");
  fUChar.insert("unsigned char");
  // decimal type
  fFloat.insert("Float_t");
  fFloat.insert("float");
  fDouble.insert("Double_t");
  fDouble.insert("Double32_t");
  fDouble.insert("double");
  fLDouble.insert("LongDouble_t");
  fLDouble.insert("long double");
  // string type
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
  // WARNING!!!!!!!
  // THIS IS VERY PRIMATIVE
  // NEEDS TO BE MADE MORE ROBUST
  // Remove any leading or trailing whitespace
  name.Strip(TString::kBoth);

  TMapIter next(fBranchMap);
  while(TObjString *key = (TObjString*)next()){
    TString nn = key->String();
    TString bn = ((TObjString*)(fBranchMap->GetValue(key)))->String();

    if (name.BeginsWith(nn, TString::kIgnoreCase)) {
      name.Replace(0, nn.Length(), bn);
      break;
    }
  }
  return name;
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

  if (branchmanager->IsScalar() && branchmanager->GetStorageType() != kOA && branchmanager->GetStorageType() != kCA && branchmanager->GetStorageType() != kRA)
    throw HALException(bname.Prepend("Tried getting dimension of scalar in branch: ").Data());
  if (branchmanager->IsCArray1D() || branchmanager->IsVec1D())
    return fvI[branchmanager->GetStorageIndex()].size();
  if (branchmanager->IsCArray2D() || branchmanager->IsVec2D())
    return fvvI[branchmanager->GetStorageIndex()][idx_1].size();
  // WARNING !!!!!
  // need to check length of ObjArray, etc... and get length from BM, not have logic here for all the different possiblities

  throw HALException(bname.Prepend("Couldn't find integer data in branch: ").Data());
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

  throw HALException(bname.Prepend("Couldn't find integer data in branch: ").Data());
}


// ///////////////////////////////////////
// BranchManager private class
// ///////////////////////////////////////

AnalysisTreeReader::BranchManager::BranchManager (AnalysisTreeReader *tr) : 
  fScalar(kFALSE), fCArray1D(kFALSE), fCArray2D(kFALSE), 
  fVec1D(kFALSE), fVec2D(kFALSE), fTreeReader(tr) {
}

AnalysisTreeReader::BranchManager::~BranchManager () {
}

Bool_t AnalysisTreeReader::BranchManager::Create (TString branchname) {
  Int_t branchstatus = 0;

  fBranchName = branchname;
  fType = fTreeReader->fChain->GetLeaf(fBranchName.Data())->GetTypeName();
  fLeafTitle = fTreeReader->fChain->GetLeaf(fBranchName.Data())->GetTitle();

  // Determine scalar, c-array, and  vector
  if (fLeafTitle.Contains(fTreeReader->fArray))
    fCArray1D = kTRUE;
  else if(fLeafTitle.Contains(fTreeReader->fArray2D))
    fCArray2D = kTRUE;
  else if (fType.Contains(fTreeReader->fVector))
    fVec1D = kTRUE;
  else if (fType.Contains(fTreeReader->fVector2D))
    fVec2D = kTRUE;
  else
    fScalar = kTRUE;

  // ////////////////////////////////////////////////////
  // Initialize for all different types
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
    else if (fType.CompareTo("TObject") == 0) {
      TObject temp;
      fStorageID = kO;
      fTreeReader->fO.push_back(temp);
      fStorageIndex = fTreeReader->fO.size() - 1;
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
  // ////////////////////////////////////////////////////
  // ////////////////////////////////////////////////////

  // Determine the scalar type
  fScalarType = fType;
  if (!fScalarType.Contains(fTreeReader->fScalar)) { // not pure scalar
    if (fScalarType.Contains(fTreeReader->fVector) || fScalarType.Contains(fTreeReader->fVector2D)) {
      fScalarType.Remove(fScalarType.First('>'), fScalarType.Length());
      fScalarType.Remove(0, fScalarType.Last('<') + 1);
    }
  }

  branchstatus = SetEntry(fTreeReader->fEntry);
  
  if (branchstatus == TTree::kVoidPtr || branchstatus == TTree::kNoCheck || branchstatus == TTree::kMatch)
    return kTRUE;
  return kFALSE;
}

Int_t AnalysisTreeReader::BranchManager::SetEntry (Long64_t entry) {
  Int_t branchstatus = 0;

  if (fScalar) {
    if (fTreeReader->fBool.count(fType) != 0) {
      Bool_t temp = kFALSE;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fB[fStorageIndex] = temp;
    }
    else if (fTreeReader->fInteger.count(fType) != 0) {
      Int_t temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fShort.count(fType) != 0) {
      Short_t temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fLong.count(fType) != 0) {
      Long_t temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fLLong.count(fType) != 0) {
      Long64_t temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fSChar.count(fType) != 0) {
      signed char temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fI[fStorageIndex] = temp;
    }
    else if (fTreeReader->fUInteger.count(fType) != 0) {
      UInt_t temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fUShort.count(fType) != 0) {
      UShort_t temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fULong.count(fType) != 0) {
      ULong_t temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fULLong.count(fType) != 0) {
      ULong64_t temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fUChar.count(fType) != 0) {
      unsigned char temp = 0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fC[fStorageIndex] = temp;
    }
    else if (fTreeReader->fFloat.count(fType) != 0) {
      Float_t temp = 0.0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fD[fStorageIndex] = temp;
    }
    else if (fTreeReader->fDouble.count(fType) != 0) {
      Double_t temp = 0.0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fD[fStorageIndex] = temp;
    }
    else if (fTreeReader->fLDouble.count(fType) != 0) {
      LongDouble_t temp = 0.0;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fD[fStorageIndex] = temp;
    }
    else if (fTreeReader->fChar.count(fType) != 0) {
      Char_t temp;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fS[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("string") == 0) {
      std::string temp;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fS[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TString") == 0) {
      TString temp;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fS[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TObjString") == 0) {
      TObjString temp;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fS[fStorageIndex] = temp.String();
    }
    else if (fType.CompareTo("TObject") == 0) {
      TObject temp;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fO[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TObjArray") == 0) {
      TObjArray temp;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fOA[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TClonesArray") == 0) {
      TClonesArray temp;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fCA[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TRef") == 0) {
      TRef temp;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fR[fStorageIndex] = temp;
    }
    else if (fType.CompareTo("TRefArray") == 0) {
      TRefArray temp;
      branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
          &temp, &fBranch);
      fBranch->GetEntry(entry);
      fTreeReader->fRA[fStorageIndex] = temp;
    }
  } // end if scalar

  return branchstatus;
}

// ///////////////////////////////////////
// ///////////////////////////////////////

} /* HAL */ 
