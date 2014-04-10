#include <HAL/Algorithm.h>

ClassImp(HAL::Algorithm);

namespace HAL {

Algorithm::Algorithm (TString name, TString title) : 
  fDataList(0), fName(name), fTitle(title), fAlgorithms(), 
  fHasExecuted(kFALSE), fAbort(kFALSE) {
}

Algorithm::Algorithm (const Algorithm &other) {
  // Copy constructor. 
  DeleteAlgos();
  for (std::list<Algorithm*>::const_iterator algo = other.fAlgorithms.begin();
       algo != other.fAlgorithms.end(); ++algo)
    fAlgorithms.push_back(*algo);
  fName = other.fName;
  fTitle = other.fTitle;
  fDataList = other.fDataList; 
  fOption = other.fOption;
  fHasExecuted = kFALSE;
  fAbort = kFALSE;
}

Algorithm::~Algorithm() {
}

void Algorithm::SetOutputFileName (TString filename) {
  fOutputFileName = filename;
  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo)
    (*algo)->SetOutputFileName(filename);
}

void Algorithm::ls () {
  TString indent("");
  ls(indent);
}

void Algorithm::DeleteAlgos () {
  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo) {
    (*algo)->DeleteAlgos();
    delete *algo;
  }
  fAlgorithms.clear(); // may double delete
}

// Be sure the calling method cleans its own memory
void Algorithm::Abort () {
  fAbort = kTRUE;
  fHasExecuted = kFALSE;
}

void  Algorithm::CleanAlgos () {
  for (std::list<Algorithm*>::reverse_iterator algo = fAlgorithms.rbegin();
       algo != fAlgorithms.rend(); ++algo)
    (*algo)->CleanAlgos();
  if (fHasExecuted)
    Clear();
  fHasExecuted = kFALSE;
  fAbort = kFALSE;
}

void  Algorithm::ExecuteAlgo (Option_t *option) {

  fOption = option;

  Exec(option);

  fHasExecuted = kTRUE;
  ExecuteAlgos(option);

  CleanAlgos();
}

void  Algorithm::ExecuteAlgos (Option_t *option) {

  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo) {

    if ((*algo)->fHasExecuted) {
      (*algo)->ExecuteAlgos(option);
      continue;
    }

    (*algo)->Exec(option);
    if ((*algo)->fAbort) break;
    (*algo)->fHasExecuted = kTRUE;
    (*algo)->ExecuteAlgos(option);
  }
}

void  Algorithm::InitializeAlgo (Option_t *option) {
  Init(option);
  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo)
    (*algo)->InitializeAlgo(option);
}

void Algorithm::BeginAlgo (Option_t *option) {
  Begin(option);
  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo)
    (*algo)->BeginAlgo(option);
}

void Algorithm::SlaveBeginAlgo (Option_t *option) {
  SlaveBegin(option);
  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo)
    (*algo)->SlaveBeginAlgo(option);
}

void Algorithm::NotifyAlgo (Option_t *option) {
  Notify(option);
  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo)
    (*algo)->NotifyAlgo(option);
}

void Algorithm::SlaveTerminateAlgo (Option_t *option) {
  for (std::list<Algorithm*>::reverse_iterator algo = fAlgorithms.rbegin();
       algo != fAlgorithms.rend(); ++algo)
    (*algo)->SlaveTerminateAlgo(option);
  SlaveTerminate(option);
}

void Algorithm::TerminateAlgo (Option_t *option) {
  for (std::list<Algorithm*>::reverse_iterator algo = fAlgorithms.rbegin();
       algo != fAlgorithms.rend(); ++algo)
    (*algo)->TerminateAlgo(option);
  Terminate(option);
}

void Algorithm::AddData (TString name, TObject *obj) {
  ((TNamed*)obj)->SetName(name.Data());
  fDataList->AddLast(obj);
}

TObject* Algorithm::GetData (TString name) {
  return fDataList->FindObject(name.Data());
}

Bool_t Algorithm::CheckData (TString name) {
  return fDataList->FindObject(name.Data()) ? kTRUE : kFALSE;
}

void Algorithm::AssignDataList (TList *list) {
  fDataList = list;
  // Assign data to all sub-algorithms
  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo)
    (*algo)->AssignDataList(list);
}

void Algorithm::DeleteData (TString name) {
  TObject *obj = fDataList->FindObject(name.Data());
  if (obj == 0)
    throw HALException(name.Prepend("Couldn't find and delete data "));
  fDataList->Remove(obj);
  delete obj;
}

void Algorithm::ls (TString indent) {
  std::cout << indent << fName << ": " << fTitle << std::endl;
  indent.Prepend("  ");
  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo)
    (*algo)->ls(indent);
}

} /* HAL */ 
