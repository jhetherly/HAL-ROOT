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

} /* HAL */ 
