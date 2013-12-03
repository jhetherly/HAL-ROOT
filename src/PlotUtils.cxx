#include <PlotUtils.h>

namespace HAL
{

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


