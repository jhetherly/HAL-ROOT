#include <TMath.h>
#include <TF1.h>
#include <HAL/Common.h>

/*
 * This is adapted from Numerical Recipes in C++
 * This adaptive step size integrator is more acurate than ROOT's
 * native TF1::Integral method. Example usage and a comparison to
 * ROOT can be found in tests/TestIntegrator.C.
 * */

#ifndef HAL_INTEGRATOR
#define HAL_INTEGRATOR

namespace HAL {
  
class Integrator {
public:
  Integrator(const Double_t &tolerance = 1.0e-12);
  template <class T>
  Double_t Integrate (T&, const Double_t &lower_bound, const Double_t &upper_bound);
  Bool_t OutOfTolerance () {return out_of_tolerance;}

private:
  Double_t TOL, toler;
  static const Double_t alpha, beta, x1, x2, x3, x[12];
  Bool_t terminate, out_of_tolerance;
  template <class T>
  Double_t int_helper (T&, const Double_t &a, const Double_t &b, const Double_t &fa,
                       const Double_t &fb, const Double_t &is);
};

template <class T>
Double_t Integrator::Integrate(T &f, const Double_t &a, const Double_t &b) {
  Double_t m, h, fa, fb, i1, i2, is, erri1, erri2, r, y[13];
  m = 0.5*(a + b);
  h = 0.5*(b - a);
  fa = y[0] = f(a);
  fb = y[12] = f(b);
  for (Int_t i = 1; i < 12; ++i) y[i] = f(m + x[i]*h);
  i2 = (h/6.0)*(y[0] + y[12] + 5.0*(y[4] + y[8]));
  i1 = (h/1470.0)*(77.0*(y[0] + y[12]) + 432.0*(y[2] + y[10]) +
       625.0*(y[4] + y[8]) + 672.0*y[6]);
  is = h*(0.0158271919734802*(y[0] + y[12]) + 0.0942738402188500*
       (y[1] + y[11]) + 0.155071987336585*(y[2] + y[10]) + 
       0.188821573960182*(y[3] + y[9]) + 0.199773405226859*
       (y[4] + y[8]) + 0.224926465333340*(y[5] + y[7]) + 
       0.242611071901408*y[6]);
  erri1 = TMath::Abs(i1 - is);
  erri2 = TMath::Abs(i2 - is);
  r = (erri2 != 0.0) ? erri1/erri2 : 1.0;
  toler = (r > 0.0 && r < 1.0) ? TOL/r : TOL;
  if (is == 0.0) is = b - a;
  is = TMath::Abs(is);
  return int_helper(f, a, b, fa, fb, is);
}

template <class T>
Double_t Integrator::int_helper(T &f, const Double_t &a, const Double_t &b, const Double_t &fa, 
                                const Double_t &fb, const Double_t &is) {
  Double_t m, h, mll, ml, mr, mrr, fmll, fml, fm, fmrr, fmr, i1, i2;
  m = 0.5*(a + b);
  h = 0.5*(b - a);
  mll = m - alpha*h;
  ml = m - beta*h;
  mr = m + beta*h;
  mrr = m + alpha*h;
  fmll = f(mll);
  fml = f(ml);
  fm = f(m);
  fmr = f(mr);
  fmrr = f(mrr);
  i2 = h/6.0*(fa + fb + 5.0*(fml + fmr));
  i1 = h/1470.0*(77.0*(fa + fb) + 432.0*(fmll + fmrr) + 625.0*(fml + fmr) + 672.0*fm);
  if (TMath::Abs(i1 - i2) <= toler*is || mll <= a || b <= mrr) {
    if ((mll <= a || b <= mrr) && terminate) {
      out_of_tolerance=kTRUE;
      terminate=kFALSE;
    }
    return i1;
  }
  else return int_helper(f, a, mll, fa, fmll, is) +
              int_helper(f, mll, ml, fmll, fml, is) +
              int_helper(f, ml, m, fml, fm, is) +
              int_helper(f, m, mr, fm, fmr, is) +
              int_helper(f, mr, mrr, fmr, fmrr, is) +
              int_helper(f, mrr, b, fmrr, fb, is);
}

#ifdef __CINT__
// Explicitly instantiate for TF1
// (as far as CINT is concerned this class is only valid for TF1's)
template Double_t Integrator::Integrate(TF1&, const Double_t &a, const Double_t &b);
template Double_t Integrator::int_helper(TF1&, const Double_t&, const Double_t&, const Double_t&,
                                         const Double_t&, const Double_t&);
#endif

} // end namespace HAL

#endif
