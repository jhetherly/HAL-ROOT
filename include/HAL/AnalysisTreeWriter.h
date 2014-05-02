#ifndef HAL_ANALYSISTREEWRITER
#define HAL_ANALYSISTREEWRITER

#include <TFile.h>
#include <TTree.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/AnalysisData.h>

namespace HAL
{

class AnalysisTreeWriter : public AnalysisData {
public:
  AnalysisTreeWriter (const TString &ofile);
  inline void       SetTreeName (const TString &tname) {fTreeName = tname;}
  inline void       SetTreeDescription (const TString &tdescription) {fTreeDescription = tdescription;}
  inline void       SetTreeForBranch (const TString &tree, const TString &branch) {fBranchTreeMap[branch] = tree;}
  inline TString    GetTreeForBranch (const TString &branch) {return fBranchTreeMap[branch];}
  inline void       IncrementCount () {++fCount;}
  void              WriteData ();
  using AnalysisData::SetValue;
  // Bool values
  virtual void      SetValue (const TString &n, const bool &v);
  virtual void      SetValue (const TString &n, const bool &v, const long long &i);
  //void              Set1DValue (const std::string &n, bool v);
  // Decimal values
  virtual void      SetValue (const TString &n, const long double &v);
  virtual void      SetValue (const TString &n, const long double &v, const long long &i);
  //void              Set1DValue (const std::string &n, long double v);
  // Integer values
  virtual void      SetValue (const TString &n, const long long &v);
  virtual void      SetValue (const TString &n, const long long &v, const long long &i);
  //void              Set1DValue (const std::string &n, long long v);
  // Counting values
  virtual void      SetValue (const TString &n, const unsigned long long &v);
  virtual void      SetValue (const TString &n, const unsigned long long &v, const long long &i);
  //void              Set1DValue (const std::string &n, unsigned long long v);

  ClassDefNV(AnalysisTreeWriter, 0);

private:
  long long                                                     fCount;
  TString                                                       fOutputFileName, fTreeName, fTreeDescription;
  //std::map<std::string, long long>                              fBoolCount, fDecimalCount, fIntegerCount, fCountingCount;
  //std::map<std::string, std::map<long long, long long> >        fBoolIntCount, fDecimalIntCount, fIntegerIntCount, fCountingIntCount;
  std::map<TString, TString, internal::string_cmp>              fBranchTreeMap;
  std::map<TString, std::set<long long>, internal::string_cmp>  fTreeIndicesMap;
};

} /* HAL */ 

#endif
