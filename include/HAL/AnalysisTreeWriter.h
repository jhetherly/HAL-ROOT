#ifndef HAL_ANALYSISTREEWRITER
#define HAL_ANALYSISTREEWRITER

#include <TFile.h>
#include <TTree.h>
#include <string>
#include <map>
#include <vector>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/AnalysisData.h>
#include <iostream>

namespace HAL
{

class AnalysisTreeWriter : public AnalysisData {
public:
  AnalysisTreeWriter (TString ofile);
  void          SetTreeName (TString tname) {fTreeName = tname;}
  void          SetTreeDescription (TString tdescription) {fTreeDescription = tdescription;}
  void          WriteData ();
  // Decimal values
  virtual void  SetValue (std::string n, long double v) {AnalysisData::SetValue(n, v, ++fDecimalCount);}
  // Integer values
  virtual void  SetValue (std::string n, long long v) {AnalysisData::SetValue(n, v, ++fIntegerCount);}
  // Counting values
  virtual void  SetValue (std::string n, unsigned long long v) {AnalysisData::SetValue(n, v, ++fCountingCount);}

  ClassDefNV(AnalysisTreeWriter, 0);

private:
  TString     fOutputFileName, fTreeName, fTreeDescription;
  Int_t       fDecimalCount, fIntegerCount, fCountingCount;
};

} /* HAL */ 

#endif
