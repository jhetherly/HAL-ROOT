#ifndef HAL_ANALYSISUTILS
#define HAL_ANALYSISUTILS

#include <TLorentzVector.h>
#include <TH1.h>
#include <TFile.h>
#include <TString.h>
#include <TArrayI.h>
#include <HAL/Common.h>
#include <HAL/AnalysisData.h>

namespace HAL
{

// /////////////////////////////////////
// Generic Algorithm (GA) data interface functions
// /////////////////////////////////////

/*
 * Make a data label compatable with the HAL generic algorithms interface
 * */
TString gaMakeNObjectsLabel (const TString &input);
TString gaMakeIndexLabel (const TString &input);
TString gaMakeRefNameLabel (const TString &input);
TString gaMakeVecLabel (const TString &input);
TString gaMakeSortedListLabel (const TString &input, const TString &tag);
TString gaMakeNParentsLabel (const TString &input);
TString gaMakeParentIndexLabel (const TString &input);
TString gaMakeParentRefNameLabel (const TString &input);

bool    gaCheckRefName (AnalysisData *data, const TString &input, TString &ref_name);

namespace internal
{

/*
 * General function for determining how to access information
 * stored in 'UserData' (either reference or direct access)
 * */
bool determineAccessProtocol(HAL::AnalysisData *data, const TString &RawInput, TString &RealInput);

} // end namespace internal


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


/*
 * This function is designed to help iterate over all unique
 * n-tupled combinations of a collection. What this function 
 * actually returns is an array of indices or NULL if the 
 * end is reached. All memory is managed by the function
 * itself. Upon reaching the last n-tuple the function 
 * returns NULL, indicating the end of the algorithm. For 
 * usage look in the tests folder for 'TestAlgorithms.C'. 
 * */
TArrayI* getNextCombination (Int_t size, Int_t n, TArrayI *indices = NULL);

} /* HAL */ 

#endif /* end of include guard: HAL_ANALYSISUTILS */
