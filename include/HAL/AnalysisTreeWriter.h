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

namespace HAL
{

class AnalysisTreeWriter : public AnalysisData {
public:
  AnalysisTreeWriter (TString ofile);
  void          SetTreeName (TString tname) {fTreeName = tname;}
  void          SetTreeDescription (TString tdescription) {fTreeDescription = tdescription;}
  void          WriteData ();
  using AnalysisData::SetValue;
  // Bool values
  virtual void  SetValue (std::string n, bool v);
  virtual void  SetValue (std::string n, bool v, long long i);
  // Decimal values
  virtual void  SetValue (std::string n, long double v);
  virtual void  SetValue (std::string n, long double v, long long i);
  // Integer values
  virtual void  SetValue (std::string n, long long v);
  virtual void  SetValue (std::string n, long long v, long long i);
  // Counting values
  virtual void  SetValue (std::string n, unsigned long long v);
  virtual void  SetValue (std::string n, unsigned long long v, long long i);

  ClassDefNV(AnalysisTreeWriter, 0);

private:
  TString                                                 fOutputFileName, fTreeName, fTreeDescription;
  std::map<std::string, long long>                        fBoolCount, fDecimalCount, fIntegerCount, fCountingCount;
  std::map<std::string, std::map<long long, long long> >  fBoolIntCount, fDecimalIntCount, fIntegerIntCount, fCountingIntCount;
};

} /* HAL */ 

#endif
