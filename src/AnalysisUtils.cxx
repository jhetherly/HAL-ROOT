#include <HAL/AnalysisUtils.h>

namespace HAL
{

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
