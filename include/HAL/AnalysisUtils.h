#include <TLorentzVector.h>
#include <TH1.h>
#include <TFile.h>
#include <TString.h>
#include <HAL/Particle.h>

#ifndef HAL_ANALYSISUTILS
#define HAL_ANALYSISUTILS

namespace HAL
{

// /////////////////////////////////////
// General utility functions
// /////////////////////////////////////

/*
 * Construct a TLorentzVector from pT, eta, phi, and energy
 * Caution: user's responsibility to delete returned object
 * */
TLorentzVector* makeTLVFromPtEtaPhiE (double pT, double eta,
                                      double phi, double e);

/*
 * Construct a TLorentzVector from pT, eta, phi, and mass
 * Caution: user's responsibility to delete returned object
 * */
TLorentzVector* makeTLVFromPtEtaPhiM (double pT, double eta,
                                      double phi, double m);

/*
 * Fetch a TH1 object from a file
 * Caution: user's responsibility to delete returned object
 * */
TH1* getTH1FromFileName (TString file_name, TString histo_name);

} /* HAL */ 

#endif /* end of include guard: HAL_ANALYSISUTILS */
