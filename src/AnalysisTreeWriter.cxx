#include <HAL/AnalysisTreeWriter.h>

ClassImp(HAL::AnalysisTreeWriter);

namespace HAL
{

AnalysisTreeWriter::AnalysisTreeWriter (TString ofile) : 
  fOutputFileName(ofile), 
  fDecimalCount(0), 
  fIntegerCount(0), 
  fCountingCount(0) {
}

void AnalysisTreeWriter::WriteData() {
  TFile f(fOutputFileName.Data(), "RECREATE");
  TTree t(fTreeName.Data(), fTreeDescription.Data());

  // Loop over maps - each key is a branch name
  //std::map<std::string, std::map<Int_t, long long> >          fIntegerIntMap;
  //std::map<std::string, std::map<Int_t, unsigned long long> > fCountingIntMap;
  //std::map<std::string, std::map<Int_t, std::string> >        fStringIntMap;
  //std::map<std::string, std::map<Int_t, TObject*> >           fTObjectIntMap;
  for (std::map<std::string, std::map<Int_t, long double> >::iterator outer = fDecimalIntMap.begin();
       outer != fDecimalIntMap.end(); ++outer) {
    std::string bname = outer->first;
    std::vector<long double> *values = new std::vector<long double>;

    for (std::map<Int_t, long double>::iterator inner = outer->second.begin();
         inner != outer->second.end(); ++inner) {
      long double value = inner->second;
      values->push_back(value);
    }
    t.Branch(bname.c_str(), "vector<long double>", &values);
  }

  t.Fill();
  t.Write();
  f.Close();
}
  
} /* HAL */ 

