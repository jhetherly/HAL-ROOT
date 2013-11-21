#include <TString.h>
#include <TMath.h>
#include <TMatrixD.h>
#include <TArrayD.h>
#include <TH1.h>
#include <TF2.h>

/*
 * This class is designed to perform an n-sided optimization given the signal and
 * background histograms (TH1) as well as a fitness function (TF2) to evaluate 
 * them against. The sided-ness of an optimization refers to the number of
 * internal partitions that are used in determining the best intervals based on
 * the given fitness function.
 * e.g.: n = 2 -> 0, 1, 2, m
 *       0 and m represent the first and last bins in the histograms.
 *       The algorithm will adjust the positions of the bins at 1 and 2 so
 *       as to maximize the fitness function. A possible result could be:
 *       0, 10, 48, m   "odd"
 *       The inner bins have been maximally adjusted and the algorithm picks out
 *       the odd numbered intervals (the first and third or 0-10 and 48-m).
 * The algorithm will automatically check all lower-order n-sides first. (e.g.
 * if n = 3 it will check n = 1 and n = 2 first). The algorithm returns a TMatrixD
 * whose rows are the intervals given in units of the x-axis, not bin number.
 * In the above example the matrix might be (if bin 0->0, 10->50, 48->240, and 
 * m=100->500):
 *      |   0     50  |
 *      | 240    500  |
 * 
 * Usage:
 *    CutOptimizer co;
 *    TMatrixD *result = co.Optimize(sig, bkg, 2);
 * The first and sencond arguments are the signal and background TH1's for a
 * 2-sided cut specified by the third argument. A fourth argument can specify if
 * the intervals should be "even" or "odd" (default is both). A user can also give
 * a rebin value as the fifth argument as well as an xlow and xhigh value to
 * restrict the range of the bins (entered in units of the x-axis) as sixth and
 * seventh arguments.
 * A user specified fitness function can be supplied as an argument to the 
 * constructor or in the SetFitnessFunction. This function can be retrieved with
 * the GetFitnessFunction function. 
 *
 * Requirements:
 *  - The fitness function must accept the signal and background values as the
 *    first and second arguments respectively. Look at default constructor for an
 *    example
 *  - The fitness function must not have the name "co-default"
 *  - The signal and background histograms must have the same number of bins.
 *
 * */

#ifndef HAL_CUTOPTIMIZER
#define HAL_CUTOPTIMIZER

// /////////////////////////////////////
// Optimization class
// /////////////////////////////////////

namespace HAL {

class CutOptimizer {
public:
  CutOptimizer (TF2 *st = NULL) {
    if (st == NULL)
      // default is the ATLAS test statistic (x=signal, y=background)
      f = new TF2("co-default", "sqrt(2.0*((x + y)*log(1.0 + x/y) - x))", 0, 10, 0, 10);
    else
      f = st;
  }
  ~CutOptimizer () {TString fName("co-default"); if (f->GetName() == fName) {delete f;}}
  void SetFitnessFunction (TF2 *st = NULL) {if (st != NULL) f = st;}
  TF2* GetFitnessFunction () {return f;}
  TMatrixD* Optimize (TH1 *sig, TH1 *bkg, Int_t n, TString side = "both", 
                      Int_t rebin = 1, Double_t x_min = TMath::QuietNaN(),
                      Double_t x_max = TMath::QuietNaN());
private:
  TF2 *f; // fitness function
  Double_t ComputeMaxStatistic (TH1*, TH1*, Int_t, TString,
                                Double_t, Double_t, TMatrixD**);
  Bool_t MoveXArray (TArrayD&);
}; // end class CutOptimizer

} // end namespace HAL

#endif

