#include <TMath.h>

/*
 * This is adapted from Numerical Recipes in C++
 * It's an adaptive step size integrator.
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

} // end namespace HAL

#endif
