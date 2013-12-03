#include "PlotUtils.h"

namespace HAL
{

TH1D* buildTH1D (TString name, TString title,
                 Int_t nbins, Double_t lowbin, Double_t highbin,
                 TString xtitle, TString ytitle) {
  return buildTH1<TH1D> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

TH1F* buildTH1F (TString name, TString title,
                 Int_t nbins, Double_t lowbin, Double_t highbin,
                 TString xtitle, TString ytitle) {
  return buildTH1<TH1F> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

TH1I* buildTH1I (TString name, TString title,
                 Int_t nbins, Double_t lowbin, Double_t highbin,
                 TString xtitle, TString ytitle) {
  return buildTH1<TH1I> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

TH1C* buildTH1C (TString name, TString title,
                 Int_t nbins, Double_t lowbin, Double_t highbin,
                 TString xtitle, TString ytitle) {
  return buildTH1<TH1C> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

TH1S* buildTH1S (TString name, TString title,
                 Int_t nbins, Double_t lowbin, Double_t highbin,
                 TString xtitle, TString ytitle) {
  return buildTH1<TH1S> (name, title, nbins, lowbin, highbin, xtitle, ytitle);
}

TH2D* buildTH2D (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH2<TH2D> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

TH2F* buildTH2F (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH2<TH2F> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

TH2C* buildTH2C (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH2<TH2C> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

TH2I* buildTH2I (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH2<TH2I> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

TH2S* buildTH2S (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH2<TH2S> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin, 
                         xtitle, ytitle, ztitle);
}

TH3D* buildTH3D (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 Int_t nzbins, Double_t lowzbin, Double_t highzbin,
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH3<TH3D> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

TH3F* buildTH3F (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 Int_t nzbins, Double_t lowzbin, Double_t highzbin,
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH3<TH3F> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

TH3C* buildTH3C (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 Int_t nzbins, Double_t lowzbin, Double_t highzbin,
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH3<TH3C> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

TH3I* buildTH3I (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 Int_t nzbins, Double_t lowzbin, Double_t highzbin,
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH3<TH3I> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

TH3S* buildTH3S (TString name, TString title,
                 Int_t nxbins, Double_t lowxbin, Double_t highxbin,
                 Int_t nybins, Double_t lowybin, Double_t highybin, 
                 Int_t nzbins, Double_t lowzbin, Double_t highzbin,
                 TString xtitle, TString ytitle, TString ztitle) {
  return buildTH3<TH3S> (name, title, nxbins, lowxbin, highxbin, nybins, lowybin, highybin,
                         nzbins, lowzbin, highzbin, xtitle, ytitle, ztitle);
}

void standardLegendFormat (TLegend *l, Int_t shadow,
                           Color_t fill, Int_t textAlign) {
    l->SetShadowColor(shadow);
    l->SetFillColor(fill);
    l->SetTextAlign(textAlign);
}

void standardHistogramStackFormat (THStack *s, 
                                   TString xlabel, TString ylabel,
                                   Double_t minAxis, Double_t maxAxis, 
                                   Double_t binWidth,
                                   Double_t lineWidth) {
    /**
     * assumes a constant bin width histogram
     * and the number of histograms is between 1 and 9 (inclusive)
     **/
    Color_t color_array[9] = {1, 4, 2, 8, 9, 6, 7, 3, 5};
    TList *hlist = s->GetHists();
    Int_t hlist_length = hlist->LastIndex() + 1;

    for (Int_t i = 0; i < hlist_length; ++i) {
        TH1 *h = (TH1*)hlist->At(i);
        Double_t current_bin_width = h->GetBinWidth(0);
        if (current_bin_width < binWidth)
            h->Rebin(binWidth/current_bin_width);
        h->SetLineColor(color_array[i]);
        h->SetLineWidth(lineWidth);
    }
    s->GetXaxis()->SetRangeUser(minAxis, maxAxis);
    s->GetXaxis()->SetTitle(xlabel.Data());
    s->GetYaxis()->SetTitle(ylabel.Data());

}

//void drawTH1StackWithLegend (THStack *s, TLegend *l, TH1 **hArray,
//                             TString inputDir, const Int_t n,
//                             TString *fileNames, TString *histNames,
//                             TString legendTitle, TString *legendNames,
//                             Option_t *option) {
//    l->SetHeader(legendTitle);
//    for (Int_t i = 0; i < n; ++i) {
//        hArray[i] = (TH1*) getTH1FromFileName((inputDir+"/")+fileNames[i], 
//                                              histNames[i]);
//        s->Add(hArray[i]);
//        l->AddEntry(hArray[i], legendNames[i], "l");
//    }
//    s->Draw(option);
//    l->Draw();
//}

} /* HAL */ 

