/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 */

#ifndef HAL_AnalysisUtils
#define HAL_AnalysisUtils

#include <HAL/Common.h>

// forward declaration(s)
class TString;
class TLorentzVector;
class TH1;
class TArrayI;
// end forward declaration(s)

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

#endif /* end of include guard: HAL_AnalysisUtils */
