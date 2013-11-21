#include <TMath.h>
#include <vector>
#include <exception>

/*
 * This is adapted from Numerical Recipes in C++
 * This is an n-poInt_t Lagrangian interpolator.
 * */

#ifndef HAL_INTERPOLATE
#define HAL_INTERPOLATE

namespace HAL {

class InterpBase {
private:
  Int_t     n, mm, cor, jsav, dj;
  Double_t *xx, *yy;

  InterpBase (Double_t *x, Double_t *y, Int_t xs, Int_t m) 
    : n(xs), mm(m + 1), cor(0), jsav(0), xx(x), yy(y) {
    dj = TMath::Max(1, (int) TMath::Power((Double_t) n, 0.25));
  }
  
  virtual ~InterpBase () {}
  
  Double_t interp (const Double_t &x) {
    Int_t jlo = cor ? hunt(x) : locate(x);
    return rawinterp(jlo, x);
  }

  Int_t locate (const Double_t &x);
  Int_t hunt (const Double_t &x);

  Double_t virtual rawinterp (const Int_t &jlo, const Double_t &x) = 0;

  friend class PolyInterp;
  friend class Poly2DInterp;
};

class PolyInterp : public InterpBase {
public:
  Double_t dy;
      
  PolyInterp(Double_t *xv, Double_t *yv, Int_t xvs, Int_t m)
    : InterpBase(xv, yv, xvs, m), dy(0.0) {}
  
  Double_t rawinterp(const Int_t &jl, const Double_t &x);
};

class Poly2DInterp {
public:
  Double_t **y;
  
  Poly2DInterp (Double_t *x1v, Double_t *x2v, Int_t x1vs, Int_t x2vs, Double_t **ym, Int_t mp, Int_t np)
    : y(ym), m(x1vs), n(x2vs), mm(mp), nn(np), x1terp(x1v, NULL, x1vs, mm), x2terp(x2v, NULL, x2vs, nn) {}

  Double_t interp (const Double_t&, const Double_t&);

private:
  Int_t m, n, mm, nn;
  PolyInterp x1terp, x2terp;
};

} // end namespace HAL

#endif
