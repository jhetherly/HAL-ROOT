/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 */

#ifndef HAL_Analysis
#define HAL_Analysis

#include <TString.h>
#include <HAL/Common.h>

// forward declaration(s)
class TChain;
class TTree;
class TLeaf;
class TObjString;
class TMap;

namespace HAL
{
class Algorithm;
class AnalysisSelector;
}
// end forward declaration(s)

namespace HAL
{

//! Main class that organizes and drives the analysis
/*!
 * This class is the main interface for configuring the analysis.
 * The user can specify the input tree name, the files to process,
 * the output file name, the default output tree name, any
 * required branch maps, and the processing details. This class 
 * can also print the input tree, configure the processing 
 * messages, and print the data type of a leaf. Most inportantly,
 * this class loads the algorithms that will be processes.
 */
class Analysis {

private:
  TChain            *fChain;
  Algorithm         *fAnalysisFlow;
  AnalysisSelector  *fAnalizer;
  TMap              *fBranchMap;

public:
  Analysis (TString name = "", TString title = "", TString treeName = "");
  ~Analysis ();
  void          AddAlgo (Algorithm *a); 
  void          PrintAnalysisFlow ();
  void          PrintCounterSummary ();
  void          PrintCutReport ();
  void          SetTreeObjectName (TString name);
  void          SetAnalysisName (TString name);
  void          SetAnalysisTitle (TString title);
  Int_t         AddFiles (TString fnames, Long64_t nentries = 1234567890);
  Int_t         AddFiles (TChain *fchain);
  void          SetOutputFileName (TString fname);
  void          SetOutputTreeName (TString tname);
  void          SetOutputTreeDescription (TString tdescription);
  void          SetMessagePeriod (unsigned p = 0);
  void          PrintTree (Option_t *option = "");
  TString       GetLeafType (TString leafname);
  TString       GetLeafType (TString branchname, TString leafname);
  void          MapBranch (TString branchname, TString nickname);
  Long64_t      Process (Option_t *option = "", Long64_t nentries = 1234567890, Long64_t firstentry = 0);

  ClassDefNV(Analysis, 0);
};

} /* HAL */ 

#endif

