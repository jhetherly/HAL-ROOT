#include <TLorentzVector.h>
#include <TH1.h>
#include <TFile.h>
#include <TString.h>
#include <Particle.h>

// /////////////////////////////////////
// General utility functions
// /////////////////////////////////////

namespace HAL {
/*
 * Construct a TLorentzVector from pT, eta, phi, and energy
 * */
TLorentzVector* makeTLVFromPtEtaPhiE (double pT, double eta,
                                      double phi, double e) {
  TLorentzVector *vec = new TLorentzVector();
  vec->SetPtEtaPhiE(pT, eta, phi, e);
  return vec;
}

/*
 * Construct a TLorentzVector from pT, eta, phi, and mass
 * */
TLorentzVector* makeTLVFromPtEtaPhiM (double pT, double eta,
                                      double phi, double m) {
  TLorentzVector *vec = new TLorentzVector();
  vec->SetPtEtaPhiM(pT, eta, phi, m);
  return vec;
}

/*
 * Fetch a TH1 object from a file
 * */
TH1* getTH1FromFileName (TString file_name, TString histo_name) {
  TFile file(file_name.Data());
  TH1 *result = (TH1*) file.Get(histo_name.Data());
  result->SetDirectory(0);
  return result;
}

}
