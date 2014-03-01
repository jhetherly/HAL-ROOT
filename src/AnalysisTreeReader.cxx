#include <HAL/AnalysisTreeReader.h>

ClassImp(HAL::AnalysisTreeReader);

namespace HAL {

AnalysisTreeReader::AnalysisTreeReader (TTree *t) : fChain(t), 
  fScalar("^[a-zA-Z][a-zA-Z0-9_]+$"),
  fVector("^vector<[a-zA-Z][a-zA-Z0-9_]+>$"), 
  fVector2D("^vector<vector<[a-zA-Z][a-zA-Z0-9_]+> >$"), 
  fArray("^[a-zA-Z_][a-zA-Z0-9_]+[[a-zA-Z0-9_]+]$"), 
  fArray2D("^[a-zA-Z_][a-zA-Z0-9_]+[[a-zA-Z0-9_]+][[a-zA-Z0-9_]+]$") {
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

Int_t AnalysisTreeReader::GetInt (TString branchname, Int_t idx_1, Int_t idx_2) {
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


Bool_t AnalysisTreeReader::BranchManager::Create (TString branchname) {
  Int_t branchstatus = 0;

  fBranchName = branchname;
  fType = fTreeReader->fChain->GetLeaf(fBranchName.Data())->GetTypeName();

  // determine scalar or not first
  if (fType == "Int_t" || fType == "int") {
    Int_t temp = 1;
    fTreeReader->fI.push_back(temp);
    fStorageIndex = fTreeReader->fI.size() - 1;
    fStorageID = kI;
    branchstatus = fTreeReader->fChain->SetBranchAddress(fBranchName.Data(), 
        &(fTreeReader->fI[fStorageIndex]), &fBranch);
  }
  else if (fType == "Float16_t" || fType == "Float_t" || fType == "float") {
  }
  else { // void*
  }

  SetEntry(fTreeReader->fEntry);

  // Determine the scalar type
  fScalarType = fType;
  if (!fScalarType.Contains(fTreeReader->fScalar)) { // not pure scalar
    //if (fScalarType.Contains(fTreeReader->fArray) || fScalarType.Contains(fTreeReader->fArray2D))
    //  fScalarType.Remove(fScalarType.First("["));
    if (fScalarType.Contains(fTreeReader->fVector) || fScalarType.Contains(fTreeReader->fVector2D)) {
      fScalarType.Remove(fScalarType.First('>'), fScalarType.Length());
      fScalarType.Remove(0, fScalarType.Last('<') + 1);
    }
  }
  
  if (branchstatus == TTree::kVoidPtr || branchstatus == TTree::kNoCheck || branchstatus == TTree::kMatch)
    return kTRUE;
  return kFALSE;
}

} /* HAL */ 
