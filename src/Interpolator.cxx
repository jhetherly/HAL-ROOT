#include <Interpolator.h>

namespace HAL {

#ifndef __CINT__

Int_t InterpBase::locate (const Double_t &x) {
  Int_t ju, jm, jl;
  Int_t result;
  Bool_t ascnd = (xx[n - 1] >= xx[0]);
  jl = 0;
  ju = n - 1;
  while (ju - jl > 1) {
    jm = (int)(((unsigned)(ju + jl)) >> 1);
    if ((x >= xx[jm]) == ascnd) jl = jm;
    else ju = jm; 
  }
  cor = TMath::Abs(jl - jsav) > dj ? 0 : 1;
  jsav = jl;
  result = TMath::Min(n - mm, jl - ((mm - 2) >> 1));
  return TMath::Max(0, result);
}

Int_t InterpBase::hunt (const Double_t &x) {
  Int_t jl = jsav, jm, ju, inc = 1;
  Int_t result;
  Bool_t ascnd = (xx[n - 1] >= xx[0]);
  if (jl < 0 || jl > n - 1) {jl = 0; ju = n - 1;} 
  else {
    if ((x >= xx[jl]) == ascnd) {
      while (1) {
        ju = jl + inc;
        if (ju >= n - 1) {ju = n - 1; break;}
        else if ((x < xx[ju]) == ascnd) break;
        else {jl = ju; inc += inc;}
      }
    } 
    else {
      ju = jl;
      while (1) {
        jl -= inc;
        if (jl <= 0) {jl = 0; break;}
        else if ((x >= xx[jl]) == ascnd) break;
        else {ju = jl; inc += inc;}
      }
    }
  }
  while (ju - jl > 1) {
    jm = (int)(((unsigned)(ju + jl)) >> 1);
    if ((x >= xx[jm]) == ascnd) jl = jm;
    else ju = jm;
  }
  cor = TMath::Abs(jl - jsav) > dj ? 0 : 1;
  jsav = jl;
  result = TMath::Min(n - mm, jl - ((mm - 2)>>1));
  return TMath::Max(0, result);
}

Double_t PolyInterp::rawinterp(const Int_t &jl, const Double_t &x) {
  Int_t i, m, ns = 0;
  Double_t y, den, dif, dift, ho, hp, w;
  Double_t *xa = &xx[jl], *ya = &yy[jl];
  Double_t c[mm], d[mm];
  
  dif = TMath::Abs(x - xa[0]);
  for (i = 0; i < mm; i++) {
    if ((dift = TMath::Abs(x - xa[i])) < dif) {ns = i; dif = dift;}
    c[i] = ya[i];
    d[i] = ya[i];
  }
  y = ya[ns--];
  for (m = 1; m < mm; m++) {
    for (i = 0; i < mm - m; i++) {
      ho = xa[i] - x;
      hp = xa[i + m] - x;
      w = c[i + 1] - d[i];
      den = ho - hp;
      if (den == 0.0) {throw std::exception();}
      den = w/den;
      d[i] = hp*den;
      c[i] = ho*den;
    }
    y += (dy = (2*(ns + 1) < (mm - m) ? c[ns + 1] : d[ns--]));
  }
  return y;
}

Double_t Poly2DInterp::interp (const Double_t &x1p, const Double_t &x2p) {
  Int_t k;
  Int_t x1jlo = x1terp.cor ? x1terp.hunt(x1p) : x1terp.locate(x1p);
  Int_t x2jlo = x2terp.cor ? x2terp.hunt(x2p) : x2terp.locate(x2p);
  std::vector<Double_t> midValues(x2terp.n);
  
  for (k = 0; k < x2terp.mm; ++k) {
    x1terp.yy = y[x2jlo + k];
    midValues[x2jlo + k] = x1terp.rawinterp(x1jlo, x1p);
  }
  x2terp.yy = &midValues[0];
  return(x2terp.rawinterp(x2jlo, x2p));
}

#endif // if not __CINT__

}
