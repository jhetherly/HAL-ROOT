#include <HAL/Algorithms.h>

namespace HAL
{

/*
 * Generic functions
 * */
bool internal::determineAccessProtocol(HAL::AnalysisData *data, 
                                       TString &RawInput, 
                                       TString &RealInput) {
  TString VectorInput = TString::Format("%s:4-vec", RawInput.Data());   // either this
  TString NameInput = TString::Format("%s:ref_name", RawInput.Data());  // or this must exist

  if (data->Exists(VectorInput.Data())) {
    RealInput = RawInput;
    return true;
  }
  else if (data->Exists(NameInput.Data())) {
    RealInput = TString(data->GetString(NameInput.Data()).c_str());
    return true;
  }
  return false;
}

/*
 * Generic classes
 * */
void internal::ImportTLVAlgo::Exec (unsigned n) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());

  for (unsigned i = 0; i < n; ++i) {
    TLorentzVector *vec = MakeTLV(i);
    data->SetValue(VectorOutput.Data(), vec, i);
    data->SetValue(IndexOutput.Data(), i, i);
  }
  data->SetValue(NObjectsOutput.Data(), n);
}

void internal::ImportTLVAlgo::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  unsigned n = data->GetInteger(TString::Format("%s:nobjects", GetName().Data()).Data());
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());

  for (unsigned i = 0; i < n; ++i) {
    delete data->GetTObject(VectorOutput.Data());
  }
}

void internal::NthElementAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  unsigned n;
  TString RealInput;
  TString SortedIndexListName;
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());
  TString NameOutput = TString::Format("%s:ref_name", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());

  if (determineAccessProtocol(data, fInput, RealInput)) {
    fElementName = TString::Format("%s:4-vec", RealInput.Data());
    n = data->GetInteger(TString::Format("%s:nobjects", RealInput.Data()).Data());
    SortedIndexListName = TString::Format("%s:sorted_%s", RealInput.Data(), SortTag().Data());
  }
  else
    return;

  if (n < fN)
    return;

  if (!data->Exists(SortedIndexListName.Data())) {
    long long count = 0;
    std::vector<long long> IndexProxy;

    for (unsigned i = 0; i < n; ++i)
      IndexProxy.push_back(i);
    Sort(IndexProxy);
    for (std::vector<long long>::iterator it = IndexProxy.begin(); it != IndexProxy.end(); ++it)
      data->SetValue(SortedIndexListName.Data(), *it, count++);
  }

  long long location = data->GetInteger(SortedIndexListName.Data(), fN - 1);
  data->SetValue(NObjectsOutput.Data(), (long long)1);
  data->SetValue(NameOutput.Data(), fInput);
  data->SetValue(IndexOutput.Data(), location, 0);
}

void internal::SingleParticleTLVCut::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  TString RealInput;
  TLorentzVector *InputVec;

  if (internal::determineAccessProtocol(data, fInput, RealInput)) {
    long long InputIndex = data->GetInteger(TString::Format("%s:index", fInput.Data()).Data(), 0);
    InputVec = (TLorentzVector*)data->GetTObject(TString::Format("%s:4-vec", RealInput.Data()).Data(), InputIndex);
  }
  else
    return;

  if (CutPredicate(InputVec)) {
    Passed();
    return;
  }
  Abort();
}

void internal::SingleParticleTLVStore::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  HAL::AnalysisData *output = (HAL::AnalysisTreeWriter*)GetData("UserOutput");
  TString RealInput;
  TLorentzVector *InputVec;

  if (internal::determineAccessProtocol(data, fInput, RealInput)) {
    long long InputIndex = data->GetInteger(TString::Format("%s:index", fInput.Data()).Data(), 0);
    InputVec = (TLorentzVector*)data->GetTObject(TString::Format("%s:4-vec", RealInput.Data()).Data(), InputIndex);
  }
  else
    return;

  output->SetValue(fBranchName.Data(), StoreValue(InputVec));
}


/*
 * Actual classes
 * */

/*
 * Importing Algorithms
 * */
void IA0000::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetInteger(TString::Format("%s:nentries", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0000::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");

  long double x0 = tr->GetDecimal(TString::Format("%s:x0", GetName().Data()), i),
              x1 = tr->GetDecimal(TString::Format("%s:x1", GetName().Data()), i),
              x2 = tr->GetDecimal(TString::Format("%s:x2", GetName().Data()), i),
              x3 = tr->GetDecimal(TString::Format("%s:x3", GetName().Data()), i);
  return new TLorentzVector(x1, x2, x3, x0);
}

void IA0001::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetDim(TString::Format("%s:x1", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0001::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");

  long double x0 = tr->GetDecimal(TString::Format("%s:x0", GetName().Data()), i),
              x1 = tr->GetDecimal(TString::Format("%s:x1", GetName().Data()), i),
              x2 = tr->GetDecimal(TString::Format("%s:x2", GetName().Data()), i),
              x3 = tr->GetDecimal(TString::Format("%s:x3", GetName().Data()), i);
  return new TLorentzVector(x1, x2, x3, x0);
}

void IA0002::Exec (Option_t* /*option*/) {
  unsigned n = fNEntries;

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0002::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");

  long double x0 = tr->GetDecimal(TString::Format("%s:x0", GetName().Data()), i),
              x1 = tr->GetDecimal(TString::Format("%s:x1", GetName().Data()), i),
              x2 = tr->GetDecimal(TString::Format("%s:x2", GetName().Data()), i),
              x3 = tr->GetDecimal(TString::Format("%s:x3", GetName().Data()), i);
  return new TLorentzVector(x1, x2, x3, x0);
}

void IA0010::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetInteger(TString::Format("%s:nentries", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0010::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");

  long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiE(pT, eta, phi, e);
}

void IA0011::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetDim(TString::Format("%s:pt", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0011::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");

  long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiE(pT, eta, phi, e);
}

void IA0012::Exec (Option_t* /*option*/) {
  unsigned n = fNEntries;

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0012::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");

  long double e = tr->GetDecimal(TString::Format("%s:e", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiE(pT, eta, phi, e);
}

void IA0020::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetInteger(TString::Format("%s:nentries", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0020::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");

  long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pT", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiM(pT, eta, phi, m);
}

void IA0021::Exec (Option_t* /*option*/) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");
  unsigned n = tr->GetDim(TString::Format("%s:pt", GetName().Data()));

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0021::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");

  long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiM(pT, eta, phi, m);
}

void IA0022::Exec (Option_t* /*option*/) {
  unsigned n = fNEntries;

  ImportTLVAlgo::Exec(n);
}

TLorentzVector* IA0022::MakeTLV (unsigned i) {
  AnalysisTreeReader *tr = (AnalysisTreeReader*)GetData("RawData");

  long double m = tr->GetDecimal(TString::Format("%s:m", GetName().Data()), i),
              pT = tr->GetDecimal(TString::Format("%s:pt", GetName().Data()), i),
              eta = tr->GetDecimal(TString::Format("%s:eta", GetName().Data()), i),
              phi = tr->GetDecimal(TString::Format("%s:phi", GetName().Data()), i);
  return makeTLVFromPtEtaPhiM(pT, eta, phi, m);
}

/*
 * Filtering Algorithms
 * */
TString FA0000::SortTag () {
  return "pt";
}

bool FA0000::operator() (long long lhs, long long rhs) {
  AnalysisData *data = (AnalysisData*)GetData("UserData");
  TLorentzVector *lhs_vec = (TLorentzVector*)data->GetTObject(fElementName.Data(), lhs);
  TLorentzVector *rhs_vec = (TLorentzVector*)data->GetTObject(fElementName.Data(), rhs);

  // decending order (greatest to least)
  return (lhs_vec->Pt() > rhs_vec->Pt());
}

void FA0000::Sort (std::vector<long long> &ip) {
  std::stable_sort(ip.begin(), ip.end(), *this);
}

/*
 * Reconstruction Algorithms
 * */
void RA0000::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  TString *RealInputs = new TString[fLength];
  long long *ParentIndices = new long long[fLength];
  TLorentzVector **ParentVecs = new TLorentzVector*[fLength];
  // scalars
  TString NObjectsOutput = TString::Format("%s:nobjects", GetName().Data());
  TString NParentsOutput = TString::Format("%s:nparents", GetName().Data());
  // 1D arrays
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());
  TString IndexOutput = TString::Format("%s:index", GetName().Data());
  TString ParentNamesOutput = TString::Format("%s:parent_ref_name", GetName().Data());
  TString ParentIndicesOutput = TString::Format("%s:parent_index", GetName().Data());

  for (long long i = 0; i < fLength; ++i) {
    TString pname(fParentNames[i]);
    if (internal::determineAccessProtocol(data, pname, RealInputs[i])) {
      ParentIndices[i] = data->GetInteger(TString::Format("%s:index", fParentNames[i]).Data(), 0);
      ParentVecs[i] = (TLorentzVector*)data->GetTObject(TString::Format("%s:4-vec", RealInputs[i].Data()).Data(), ParentIndices[i]);
    }
    else
      return;
  }

  TLorentzVector *vec = new TLorentzVector(*ParentVecs[0]);
  for (long long i = 1; i < fLength; ++i)
    vec->operator+=(*ParentVecs[i]);

  data->SetValue(NObjectsOutput.Data(), (long long)1);
  data->SetValue(NParentsOutput.Data(), fLength);
  data->SetValue(VectorOutput.Data(), vec, 0);
  data->SetValue(IndexOutput.Data(), (long long)0, 0);
  for (long long i = 0; i < fLength; ++i) {
    data->SetValue(ParentNamesOutput.Data(), RealInputs[i], i);
    data->SetValue(ParentIndicesOutput.Data(), ParentIndices[i], 0);
  }
  delete[] RealInputs;
  delete[] ParentIndices;
  delete[] ParentVecs;
}

void RA0000::Clear (Option_t* /*option*/) {
  HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
  TString VectorOutput = TString::Format("%s:4-vec", GetName().Data());

  if (data->Exists(VectorOutput.Data()))
    delete data->GetTObject(VectorOutput.Data(), 0);
}

/*
 * Cutting Algorithms
 * */
bool CA0000::CutPredicate (TLorentzVector *vec) {
  return (vec->Pt() >= fCutValue);
}

bool CA0003::CutPredicate (TLorentzVector *vec) {
  return (vec->M() >= fCutValue);
}

/*
 * Exporting Algorithms
 * */
double EA0000::StoreValue (TLorentzVector *vec) {
  return vec->Pt();
}

double EA0003::StoreValue (TLorentzVector *vec) {
  return vec->M();
}

} /* HAL */ 
