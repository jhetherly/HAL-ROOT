#include <HAL/Algorithm.h>
#include <iostream>
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
#include <aux/boost/foreach.hpp>
#endif
#include <TMath.h>
#include <TObject.h>
#include <TNamed.h>
#include <TList.h>
#include <HAL/AnalysisData.h>
#include <HAL/AnalysisTreeReader.h>
#include <HAL/AnalysisTreeWriter.h>

ClassImp(HAL::Algorithm);

namespace HAL 
{

//______________________________________________________________________________
Algorithm::Algorithm (TString name, TString title) : 
  fPrintCounter(kFALSE), fAlgorithms(), fName(name), fTitle(title), 
  fHasExecuted(kFALSE), fAbort(kFALSE), fDataList(nullptr), fAlgorithmType(""),  fCounter(0) 
{
}

//______________________________________________________________________________
Algorithm::Algorithm (const Algorithm &other) 
{
  DeleteAlgos();
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, other.fAlgorithms )
#else
  for (auto algo: other.fAlgorithms)
#endif
  {
    fAlgorithms.push_back(algo);
  }
  fName = other.fName;
  fTitle = other.fTitle;
  fDataList = other.fDataList; 
  fOption = other.fOption;
  fCounter = other.fCounter;
  fHasExecuted = kFALSE;
  fAbort = kFALSE;
}

//______________________________________________________________________________
Algorithm::~Algorithm() 
{
}

//______________________________________________________________________________
void Algorithm::Add (Algorithm *algo) 
{ 
  fAlgorithms.push_back(algo); 
}

//______________________________________________________________________________
void Algorithm::SetOutputFileName (TString filename) 
{
  // User should never call this.
  
  fOutputFileName = filename;
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->SetOutputFileName(filename);
  }
}

//______________________________________________________________________________
void Algorithm::ls () 
{
  // User should never call this.

  TString indent("");
  PrintAlgorithmHierarchy(indent);
}

//______________________________________________________________________________
void Algorithm::CounterSummary () 
{
  // User should never call this.

  TString indent("");
  std::cout << "Object Creation Summary:" << std::endl;
  CounterSummaryHelper(indent);
  std::cout << "End of Object Summary" << std::endl << std::endl;
}

//______________________________________________________________________________
void Algorithm::CutReport () 
{
  // User should never call this.

  TString indent("");
  long long base_number = -1;
  long long prev_number = -1;
  std::cout << "Efficiency Report (absolute and relative):" << std::endl;
  CutReportHelper(indent, base_number, prev_number);
  std::cout << "End of Efficiency Report" << std::endl << std::endl;
}

//______________________________________________________________________________
void Algorithm::DeleteAlgos () 
{
  // User should never call this.

#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->DeleteAlgos();
    delete algo;
    algo = nullptr;
  }
  fAlgorithms.clear(); // may double delete
}

//______________________________________________________________________________
void Algorithm::Abort () 
{
  fAbort = kTRUE;
  fHasExecuted = kFALSE;
}

//______________________________________________________________________________
void  Algorithm::CleanAlgos () 
{
  // User should never call this.

#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->CleanAlgos();
  }
  if (fHasExecuted)
    Clear();
  fHasExecuted = kFALSE;
  fAbort = kFALSE;
}

//______________________________________________________________________________
void  Algorithm::ExecuteAlgo (Option_t *option) 
{
  // User should never call this.

  fOption = option;

  Exec(option);

  fHasExecuted = kTRUE;
  ExecuteAlgos(option);

  CleanAlgos();
}

//______________________________________________________________________________
void  Algorithm::ExecuteAlgos (Option_t *option) 
{
  // User should never call this.

#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    if (algo->fHasExecuted) {
      algo->ExecuteAlgos(option);
      continue;
    }

    algo->Exec(option);
    if (algo->fAbort) break;
    algo->fHasExecuted = kTRUE;
    algo->ExecuteAlgos(option);
  }
}

//______________________________________________________________________________
void  Algorithm::InitializeAlgo (Option_t *option) 
{
  // User should never call this.

  Init(option);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->InitializeAlgo(option);
  }
}

//______________________________________________________________________________
void Algorithm::BeginAlgo (Option_t *option) 
{
  // User should never call this.

  Begin(option);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->BeginAlgo(option);
  }
}

//______________________________________________________________________________
void Algorithm::SlaveBeginAlgo (Option_t *option) 
{
  // User should never call this.

  SlaveBegin(option);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->SlaveBeginAlgo(option);
  }
}

//______________________________________________________________________________
void Algorithm::NotifyAlgo (Option_t *option) 
{
  // User should never call this.

  Notify(option);
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->NotifyAlgo(option);
  }
}

//______________________________________________________________________________
void Algorithm::SlaveTerminateAlgo (Option_t *option) 
{
  // User should never call this.

#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->SlaveTerminateAlgo(option);
  }
  SlaveTerminate(option);
}

//______________________________________________________________________________
void Algorithm::TerminateAlgo (Option_t *option) 
{
  // User should never call this.

#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->TerminateAlgo(option);
  }
  Terminate(option);
}

//______________________________________________________________________________
void Algorithm::AddData (TString name, TObject *obj) 
{
  static_cast<TNamed*>(obj)->SetName(name.Data());
  fDataList->AddLast(obj);
}

//______________________________________________________________________________
TObject* Algorithm::GetData (TString name) 
{
  return fDataList->FindObject(name.Data());
}

//______________________________________________________________________________
Bool_t Algorithm::CheckData (TString name) 
{
  return fDataList->FindObject(name.Data()) ? kTRUE : kFALSE;
}

//______________________________________________________________________________
void Algorithm::AssignDataList (TList *list) 
{
  // User should never call this.

  fDataList = list;
  // Assign data to all sub-algorithms
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->AssignDataList(list);
  }
}

//______________________________________________________________________________
void Algorithm::DeleteData (TString name) 
{
  TObject *obj = fDataList->FindObject(name.Data());
  if (obj == 0)
    throw HALException(name.Prepend("Couldn't find and delete data "));
  fDataList->Remove(obj);
  delete obj;
}

//______________________________________________________________________________
AnalysisTreeReader* Algorithm::GetRawData () 
{
  return static_cast<AnalysisTreeReader*>(fDataList->FindObject("RawData"));
}

//______________________________________________________________________________
AnalysisData* Algorithm::GetUserData () 
{
  return static_cast<AnalysisData*>(fDataList->FindObject("UserData"));
}

//______________________________________________________________________________
AnalysisTreeWriter* Algorithm::GetUserOutput () 
{
  return static_cast<AnalysisTreeWriter*>(fDataList->FindObject("UserOutput"));
}

//______________________________________________________________________________
Algorithm* Algorithm::GetAlgorithm (TString name) 
{
  Algorithm *a = nullptr;

  if (this->GetName().EqualTo(name))
    return this;
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    a = algo->GetAlgorithm(name);
    if (a != nullptr) return a;
  }
  return a;
}

//______________________________________________________________________________
void Algorithm::PrintAlgorithmHierarchy (TString indent) 
{
  std::cout << indent << fName << ": " << fTitle << std::endl;
  indent.Prepend("  ");
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->PrintAlgorithmHierarchy(indent);
  }
}

//______________________________________________________________________________
void Algorithm::CounterSummaryHelper (TString indent) 
{
  if (fPrintCounter)
    std::cout << indent << fName << ": " << fCounter << std::endl;
  indent.Prepend("  ");
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->CounterSummaryHelper(indent);
  }
}

//______________________________________________________________________________
void Algorithm::CutReportHelper (TString indent, Long64_t &bn, Long64_t &pn) 
{
  if (fAlgorithmType.EqualTo("cut", TString::kIgnoreCase)) {
    if (bn == -1 && fCounter != 0)
      bn = fCounter;
    if (pn == -1 && fCounter != 0)
      pn = fCounter;
    std::cout /*<< indent*/ << fName << ": " << TMath::Abs((double)(fCounter)/(double)(bn))*100.0 << "%     "
              << TMath::Abs((double)(fCounter)/(double)(pn))*100.0 << "%     " << fCounter << "     " 
              << pn - fCounter << std::endl;
    if (fCounter != 0)
      pn = fCounter;
  }
  indent.Prepend("  ");
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( Algorithm *algo, fAlgorithms )
#else
  for (auto algo: fAlgorithms)
#endif
  {
    algo->CutReportHelper (indent, bn, pn);
  }
}

} /* HAL */ 
