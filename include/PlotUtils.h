#include <TList.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <THStack.h>
#include <TFile.h>
#include <TString.h>
#include <TLegend.h>

#ifndef HAL_PLOTUTILS
#define HAL_PLOTUTILS

namespace HAL
{

// /////////////////////////////////////
// General utility functions
// /////////////////////////////////////

/**
 * Create a TH1 object
 **/
template<class T>
T* buildTH1 (TString name, TString title = "",
             Int_t nbins = 10, Double_t lowbin = 0, Double_t highbin = 10,
             TString xtitle = "", TString ytitle = "") {
  T *histo = new T(name.Data(), title.Data(), nbins, lowbin, highbin);
  histo->SetXTitle(xtitle.Data());
  histo->SetYTitle(ytitle.Data());
  return histo;
}

/**
 * Create a TH1D object
 * Caution: user's responsibility to delete returned object
 **/
TH1D* buildTH1D (TString name, TString title = "",
                 Int_t nbins = 10, Double_t lowbin = 0, Double_t highbin = 10,
                 TString xtitle = "", TString ytitle = "") {
  return buildTH1<TH1D> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

/**
 * Create a TH1F object
 * Caution: user's responsibility to delete returned object
 **/
TH1F* buildTH1F (TString name, TString title = "",
                 Int_t nbins = 10, Double_t lowbin = 0, Double_t highbin = 10,
                 TString xtitle = "", TString ytitle = "") {
  return buildTH1<TH1F> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

/**
 * Create a TH1I object
 * Caution: user's responsibility to delete returned object
 **/
TH1I* buildTH1I (TString name, TString title = "",
                 Int_t nbins = 10, Double_t lowbin = 0, Double_t highbin = 10,
                 TString xtitle = "", TString ytitle = "") {
  return buildTH1<TH1I> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

/**
 * Create a TH1C object
 * Caution: user's responsibility to delete returned object
 **/
TH1C* buildTH1C (TString name, TString title = "",
                 Int_t nbins = 10, Double_t lowbin = 0, Double_t highbin = 10,
                 TString xtitle = "", TString ytitle = "") {
  return buildTH1<TH1C> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

/**
 * Create a TH1S object
 * Caution: user's responsibility to delete returned object
 **/
TH1S* buildTH1S (TString name, TString title = "",
                 Int_t nbins = 10, Double_t lowbin = 0, Double_t highbin = 10,
                 TString xtitle = "", TString ytitle = "") {
  return buildTH1<TH1S> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

/**
 * Create a TH2 object
 **/
template<class T>
T* buildTH2 (TString name, TString title = "",
             Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
             Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
             TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  T *histo = new T(name.Data(), title.Data(), nxbins, lowxbin, highxbin,
                   nybins, lowybin, highybin);
  histo->SetXTitle(xtitle.Data());
  histo->SetYTitle(ytitle.Data());
  histo->SetZTitle(ztitle.Data());
  return histo;
}

/**
 * Create a TH2D object
 * Caution: user's responsibility to delete returned object
 **/
TH2D* buildTH2D (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH2<TH2D> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

/**
 * Create a TH2F object
 * Caution: user's responsibility to delete returned object
 **/
TH2F* buildTH2F (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH2<TH2F> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

/**
 * Create a TH2C object
 * Caution: user's responsibility to delete returned object
 **/
TH2C* buildTH2C (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH2<TH2C> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

/**
 * Create a TH2I object
 * Caution: user's responsibility to delete returned object
 **/
TH2I* buildTH2I (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH2<TH2I> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

/**
 * Create a TH2S object
 * Caution: user's responsibility to delete returned object
 **/
TH2S* buildTH2S (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH2<TH2S> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

/**
 * Create a TH3 object
 **/
template<class T>
T* buildTH3 (TString name, TString title = "",
             Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
             Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
             Int_t nzbins = 10, Double_t lowzbin = 0, Double_t highzbin = 10,
             TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  T *histo = new T(name.Data(), title.Data(), nxbins, lowxbin, highxbin,
                   nybins, lowybin, highybin, nzbins, lowzbin, highzbin);
  histo->SetXTitle(xtitle.Data());
  histo->SetYTitle(ytitle.Data());
  histo->SetZTitle(ztitle.Data());
  return histo;
}

/**
 * Create a TH3D object
 * Caution: user's responsibility to delete returned object
 **/
TH3D* buildTH3D (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 Int_t nzbins = 10, Double_t lowzbin = 0, Double_t highzbin = 10,
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH3<TH3D> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

/**
 * Create a TH3F object
 * Caution: user's responsibility to delete returned object
 **/
TH3F* buildTH3F (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 Int_t nzbins = 10, Double_t lowzbin = 0, Double_t highzbin = 10,
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH3<TH3F> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

/**
 * Create a TH3C object
 * Caution: user's responsibility to delete returned object
 **/
TH3C* buildTH3C (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 Int_t nzbins = 10, Double_t lowzbin = 0, Double_t highzbin = 10,
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH3<TH3C> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

/**
 * Create a TH3I object
 * Caution: user's responsibility to delete returned object
 **/
TH3I* buildTH3I (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 Int_t nzbins = 10, Double_t lowzbin = 0, Double_t highzbin = 10,
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH3<TH3I> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

/**
 * Create a TH3S object
 * Caution: user's responsibility to delete returned object
 **/
TH3S* buildTH3S (TString name, TString title = "",
                 Int_t nxbins = 10, Double_t lowxbin = 0, Double_t highxbin = 10,
                 Int_t nybins = 10, Double_t lowybin = 0, Double_t highybin = 10, 
                 Int_t nzbins = 10, Double_t lowzbin = 0, Double_t highzbin = 10,
                 TString xtitle = "", TString ytitle = "", TString ztitle = "") {
  return buildTH3<TH3S> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

// /////////////////////////////////////
// Formatting functions
// /////////////////////////////////////

/**
 * Format a TLegend object
 **/
void standardLegendFormat (TLegend *l, Int_t shadow = 0,
                           Color_t fill = 0, Int_t textAlign = 22);

/**
 * Format a stack of histograms (<= 9)
 **/
void standardHistogramStackFormat (THStack *s, 
                                   TString xlabel, TString ylabel,
                                   Double_t minAxis, Double_t maxAxis, 
                                   Double_t binWidth = 5.0,
                                   Double_t lineWidth = 0.5);

// /////////////////////////////////////
// Writting and drawing functions
// /////////////////////////////////////

/**
 * Load and draw a stack of histograms with a legend
 **/
//void drawTH1StackWithLegend (THStack *s, TLegend *l, TH1 **hArray,
//                             TString inputDir, const Int_t n,
//                             TString *fileNames, TString *histNames,
//                             TString legendTitle, TString *legendNames,
//                             Option_t *option = "nostack");

} /* HAL */ 


#endif /* end of include guard: HAL_PLOTUTILS */
