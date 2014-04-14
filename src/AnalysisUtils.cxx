#include <HAL/AnalysisUtils.h>

namespace HAL
{

TString gaMakeNObjectsLabel (const TString &input) {
  return TString::Format("%s:nobjects", input.Data());
}

TString gaMakeIndexLabel (const TString &input) {
  return TString::Format("%s:index", input.Data());
}

TString gaMakeRefNameLabel (const TString &input) {
  return TString::Format("%s:ref_name", input.Data());
}

TString gaMakeVecLabel (const TString &input) {
  return TString::Format("%s:4-vec", input.Data());
}

TString gaMakeSortedListLabel (const TString &input, const TString &tag) {
  return TString::Format("%s:sorted_%s", input.Data(), tag.Data());
}

TString gaMakeNParentsLabel (const TString &input) {
  return TString::Format("%s:nparents", input.Data());
}

TString gaMakeParentIndexLabel (const TString &input) {
  return TString::Format("%s:nparent_index", input.Data());
}

TString gaMakeParentRefNameLabel (const TString &input) {
  return TString::Format("%s:nparent_ref_name", input.Data());
}

bool gaCheckRefName (AnalysisData *data, const TString &input, TString &ref_name) {
  return internal::determineAccessProtocol(data, input, ref_name);
}

bool internal::determineAccessProtocol(HAL::AnalysisData *data, 
                                       const TString &RawInput, 
                                       TString &RealInput) {
  TString VectorInput = gaMakeVecLabel(RawInput);
  TString NameInput = gaMakeRefNameLabel(RawInput);

  if (data->Exists(VectorInput.Data())) {
    RealInput = RawInput;
    return true;
  }
  else if (data->Exists(NameInput.Data())) {
    RealInput = data->GetString(NameInput);
    return true;
  }
  return false;
}

TLorentzVector* makeTLVFromPtEtaPhiE (double pT, double eta,
                                      double phi, double e) {
  TLorentzVector *vec = new TLorentzVector();
  vec->SetPtEtaPhiE(pT, eta, phi, e);
  return vec;
}

TLorentzVector* makeTLVFromPtEtaPhiM (double pT, double eta,
                                      double phi, double m) {
  TLorentzVector *vec = new TLorentzVector();
  vec->SetPtEtaPhiM(pT, eta, phi, m);
  return vec;
}

TH1* getTH1FromFileName (TString file_name, TString histo_name) {
  TFile file(file_name.Data());
  TH1 *result = (TH1*) file.Get(histo_name.Data());
  result->SetDirectory(0);
  return result;
}

TArrayI* getNextCombination (Int_t size, Int_t n,
                             TArrayI *indices) {
  if (n > size)
    return NULL;
  if (indices == NULL) {
    TArrayI *result = new TArrayI(n);
    for (Int_t i = 0; i < n; ++i)
      (*result)[i] = i;
    return result;
  }
  if (indices->GetSize() != n)
    return NULL;
  for (Int_t i = 0; i < n - 1; ++i) { // loop through all members except last
    if ((*indices)[i] + 1 == (*indices)[i + 1]) // reset index
      (*indices)[i] = i;
    else {
      ++(*indices)[i];
      return indices;
    }
  }
  if ((*indices)[n - 1] + 1 == size) {
    delete indices;
    return NULL;
  }
  ++(*indices)[n - 1];
  return indices;
}

} /* HAL */ 
