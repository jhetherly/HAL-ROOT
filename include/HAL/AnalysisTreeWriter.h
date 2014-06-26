/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 */

#ifndef HAL_AnalysisTreeWriter
#define HAL_AnalysisTreeWriter

#include <map>
#include <set>
#include <TString.h>
#include <HAL/Common.h>
#include <HAL/AnalysisData.h>

namespace HAL
{

//! Class for writing data to a TTree in a bin-less manner
/*!
 * This class allows for an easy way to write data to a TTree. 
 * The use can specify and default tree name along with separate 
 * trees for each branch. The user simply calls a method with 
 * the desired branch name and the value to insert. It is 
 * important to note that the type of the first value given to 
 * these methods is the value type of the branch.
 */
class AnalysisTreeWriter : public AnalysisData {

private:
  long long                                                     fCount;
  TString                                                       fOutputFileName, 
                                                                fTreeName, 
                                                                fTreeDescription;
  std::map<TString, TString, internal::string_cmp>              fBranchTreeMap;
  std::map<TString, std::set<long long>, internal::string_cmp>  fTreeIndicesMap;

public:
  AnalysisTreeWriter (const TString &ofile);
  inline void       SetTreeName (const TString &tname) {fTreeName = tname;}
  inline void       SetTreeDescription (const TString &tdescription) {fTreeDescription = tdescription;}
  inline void       SetTreeForBranch (const TString &tree, const TString &branch) {fBranchTreeMap[branch] = tree;}
  inline TString    GetTreeForBranch (const TString &branch) {return fBranchTreeMap[branch];}
  //! \cond NODOC
  inline void       IncrementCount () {++fCount;}
  void              WriteData ();
  //! \endcond

  using AnalysisData::SetValue;
  // Bool values
  virtual void      SetValue (const TString &n, const bool &v);
  virtual void      SetValue (const TString &n, const bool &v, const long long &i);
  // Decimal values
  virtual void      SetValue (const TString &n, const long double &v);
  virtual void      SetValue (const TString &n, const long double &v, const long long &i);
  // Integer values
  virtual void      SetValue (const TString &n, const long long &v);
  virtual void      SetValue (const TString &n, const long long &v, const long long &i);
  // Counting values
  virtual void      SetValue (const TString &n, const unsigned long long &v);
  virtual void      SetValue (const TString &n, const unsigned long long &v, const long long &i);

  ClassDefNV(AnalysisTreeWriter, 0);
};

} /* HAL */ 

#endif
