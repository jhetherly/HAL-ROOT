#include <HAL/Integrator.h>

namespace HAL {
  
Integrator::Integrator(const Double_t &tol) :
  TOL(tol), terminate(kTRUE), out_of_tolerance(kFALSE) {
  const Double_t EPS = 1.0e-16;
  if (TOL < EPS) TOL = EPS;
}

const Double_t Integrator::alpha = TMath::Sqrt(2.0/3.0);
const Double_t Integrator::beta = 1.0/TMath::Sqrt(5.0);
const Double_t Integrator::x1 = 0.942882415695480;
const Double_t Integrator::x2 = 0.641853342345781;
const Double_t Integrator::x3 = 0.236383199662150;
const Double_t Integrator::x[12] = {0, -Integrator::x1, -Integrator::alpha, -Integrator::x2,
                                    -Integrator::beta, -Integrator::x3, 0.0, Integrator::x3,
                                    Integrator::beta, Integrator::x2, Integrator::alpha,
                                    Integrator::x1};

} // end namespace HAL
