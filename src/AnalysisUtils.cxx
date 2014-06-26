#include <HAL/AnalysisUtils.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <TH1.h>
#include <TFile.h>
#include <TArrayI.h>

namespace HAL
{

//______________________________________________________________________________
TLorentzVector* makeTLVFromPtEtaPhiE (double pT, double eta,
                                      double phi, double e) {
  TLorentzVector *vec = new TLorentzVector();
  vec->SetPtEtaPhiE(pT, eta, phi, e);
  return vec;
}

//______________________________________________________________________________
TLorentzVector* makeTLVFromPtEtaPhiM (double pT, double eta,
                                      double phi, double m) {
  TLorentzVector *vec = new TLorentzVector();
  vec->SetPtEtaPhiM(pT, eta, phi, m);
  return vec;
}

//______________________________________________________________________________
TH1* getTH1FromFileName (TString file_name, TString histo_name) {
  TFile file(file_name.Data());
  TH1 *result = (TH1*) file.Get(histo_name.Data());
  result->SetDirectory(0);
  return result;
}

//______________________________________________________________________________
TArrayI* getNextCombination (Int_t size, Int_t n,
                             TArrayI *indices) {
  if (n > size)
    return nullptr;
  if (indices == nullptr) {
    TArrayI *result = new TArrayI(n);
    for (Int_t i = 0; i < n; ++i)
      (*result)[i] = i;
    return result;
  }
  if (indices->GetSize() != n)
    return nullptr;
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
    return nullptr;
  }
  ++(*indices)[n - 1];
  return indices;
}

} /* HAL */ 
