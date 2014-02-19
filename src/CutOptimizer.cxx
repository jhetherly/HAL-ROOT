#include <HAL/CutOptimizer.h>

// /////////////////////////////////////
// Definitions
// /////////////////////////////////////

namespace HAL {

#ifndef __CINT__

TMatrixD* CutOptimizer::Optimize (TH1 *sig, TH1 *bkg, Int_t n, TString side, 
                                  Int_t rebin, Double_t x_min, Double_t x_max) {
  TH1 *s, *b;
  TMatrixD *mat = NULL;
  TMatrixD *temp_mat = NULL;
  Double_t max_stat = 0.0;
  side.ToLower();

  if (rebin < 1 || n < 1)
    return mat;
  // Make new histograms after rebinning
  s = sig->Rebin(rebin, "s-john117");
  b = bkg->Rebin(rebin, "b-john117");
  if (TMath::IsNaN(x_min))
    x_min = s->GetXaxis()->GetFirst();
  else
    x_min = s->GetXaxis()->FindBin(x_min);
  if (TMath::IsNaN(x_max))
    x_max = s->GetXaxis()->GetLast();
  else
    x_max = s->GetXaxis()->FindBin(x_max);
  if (x_max <= x_min)
    return mat;
  for (Int_t i = 0; i < n; ++i) {
    Double_t stat = ComputeMaxStatistic(s, b, i+1, side, x_min, x_max, &temp_mat);
    if (stat >= max_stat) {
      max_stat = stat;
      if (mat != NULL)
        delete mat;
      mat = temp_mat;
    }
  }
  // Format ranges
  for (Int_t i = 0; i < mat->GetNrows(); ++i) {
    if (i == 0 && x_min == (*mat)(i, 0))
      (*mat)(i, 0) = s->GetXaxis()->GetBinLowEdge((*mat)(i, 0));
    else
      (*mat)(i, 0) = s->GetXaxis()->GetBinUpEdge((*mat)(i, 0));
    (*mat)(i, 1) = s->GetXaxis()->GetBinUpEdge((*mat)(i, 1));
  }

  delete s;
  delete b;

  return mat;
}

Double_t CutOptimizer::ComputeMaxStatistic(TH1 *s, TH1 *b, Int_t n, TString side,
                                           Double_t x_min, Double_t x_max, TMatrixD **mat) {
  Double_t max_statistic = 0.0;
  TString max_choice = "";
  TArrayD x_array(n+2), max_x_array(n+2);
  Bool_t continue_to_move = kTRUE;

  x_array[0] = x_min;
  for (Int_t i = 0; i < n; ++i) {
    x_array[i+1] = x_min + i;
  }
  x_array[n+1] = x_max;
  while (continue_to_move) {
    Double_t odd_stat = 0.0, even_stat = 0.0, sig_even_sum = 0.0,
             sig_odd_sum = 0.0, bkg_even_sum = 0.0, bkg_odd_sum = 0.0;
    sig_odd_sum += s->Integral(x_min, x_array[1]);
    bkg_odd_sum += b->Integral(x_min, x_array[1]);
    for (Int_t i = 0; i < n; ++i) {
      if (i % 2 == 0) {
        sig_even_sum += s->Integral(x_array[i+1]+1, x_array[i+2]);
        bkg_even_sum += b->Integral(x_array[i+1]+1, x_array[i+2]);
      }
      else {
        sig_odd_sum += s->Integral(x_array[i+1]+1, x_array[i+2]);
        bkg_odd_sum += b->Integral(x_array[i+1]+1, x_array[i+2]);
      }
    }
    try {
      odd_stat = (*f)(sig_odd_sum, bkg_odd_sum);
    }
    catch (...) {odd_stat = 0.0;}
    try {
      even_stat = (*f)(sig_even_sum, bkg_even_sum);
    }
    catch (...) {even_stat = 0.0;}
    if (side != "even") {
      if (odd_stat >= max_statistic) {
        max_statistic = odd_stat;
        max_x_array = x_array;
        max_choice = "odd";
      }
    }
    if (side != "odd") {
      if (even_stat >= max_statistic) {
        max_statistic = even_stat;
        max_x_array = x_array;
        max_choice = "even";
      }
    }
    continue_to_move = MoveXArray(x_array);
  } // end while moving x
  // fill mat with ranges
  if (n % 2 != 0) // odd n
    (*mat) = new TMatrixD((n + 1)/2, 2);
  else if (max_choice == "odd") // even n
    (*mat) = new TMatrixD(n/2 + 1, 2);
  else // even n
    (*mat) = new TMatrixD(n/2, 2);
  if (max_choice == "odd") {
    for (Int_t i = 0; i < (**mat).GetNrows(); ++i) {
      (**mat)(i, 0) = max_x_array[2*i];
      (**mat)(i, 1) = max_x_array[2*i + 1];
    }
  }
  else {
    for (Int_t i = 0; i < (**mat).GetNrows(); ++i) {
      (**mat)(i, 0) = max_x_array[2*i + 1];
      (**mat)(i, 1) = max_x_array[2*i + 2];
    }
  }

  return max_statistic;
}

Bool_t CutOptimizer::MoveXArray (TArrayD &x_array) {
  Int_t array_size = x_array.GetSize();
  Int_t n = array_size - 2;

  for (Int_t i = 1; i < n; ++i) {
    if (x_array[i] + 1 == x_array[i + 1])
      x_array[i] = i;
    else {
      ++x_array[i];
      return kTRUE;
    }
  }
  if (x_array[n] + 1 == x_array[n + 1])
    return kFALSE;

  ++x_array[n];
  return kTRUE;
}

#endif // if not __CINT__

} // end namespace HAL
