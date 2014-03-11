#ifndef HAL_INTERPOLATE
#define HAL_INTERPOLATE

#include <TMath.h>
#include <TArrayD.h>
#include <vector>
#include <exception>
#include <HAL/Common.h>

/*
 * This is adapted from Numerical Recipes in C++
 * These are n-point Lagrangian 1D and 2D interpolators.
 * */

namespace HAL {

class InterpBase {
public:
  InterpBase (Double_t *x, Double_t *y, Int_t xs, Int_t m) 
    : n(xs), mm(m + 1), cor(0), jsav(0), xx(x), yy(y) {
    dj = TMath::Max(1, (int) TMath::Power((Double_t) n, 0.25));
  }
  
  virtual ~InterpBase () {}

  Int_t locate (const Double_t &x);
  Int_t hunt (const Double_t &x);
  
  Double_t Interp (const Double_t &x) {
    Int_t jlo = cor ? hunt(x) : locate(x);
    return rawinterp(jlo, x);
  }

  Double_t virtual rawinterp (const Int_t &jlo, const Double_t &x) = 0;

  Int_t     n, mm, cor, jsav, dj;
  Double_t *xx, *yy;
};

class PolyInterp : public InterpBase {
public:
  PolyInterp(Double_t *x_values, Double_t *y_values, Int_t size, Int_t order)
    : InterpBase(x_values, y_values, size, order), dy(0.0) {}
  
  Double_t GetError () {return dy;}
private:
  Double_t rawinterp(const Int_t &jl, const Double_t &x);
  Double_t dy;

  friend class Poly2DInterp;
};

class Poly2DInterp {
public:
  Double_t **y;
  
  Poly2DInterp (Double_t *x1_values, Double_t *x2_values, Int_t x1_size,
                Int_t x2_size, Double_t **y_matrix, Int_t x1_order, Int_t x2_order)
    : y(y_matrix), m(x1_size), n(x2_size), mm(x1_order), nn(x2_order), 
      x1terp(x1_values, NULL, x1_size, mm), x2terp(x2_values, NULL, x2_size, nn) {}

  Double_t Interp (const Double_t &x1_value, const Double_t &x2_value);
  Double_t GetX1Error () {return x1terp.dy;}
  Double_t GetX2Error () {return x2terp.dy;}

private:
  Int_t m, n, mm, nn;
  PolyInterp x1terp, x2terp;
};

} // end namespace HAL

#endif
