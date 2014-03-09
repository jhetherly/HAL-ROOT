#include <HAL/Algorithm.h>

ClassImp(HAL::Algorithm);

namespace HAL {

Algorithm* Algorithm::fgBeginAlgo  = 0;
Algorithm* Algorithm::fgBreakPoint = 0;

Algorithm::Algorithm (TString name, TString title) : 
  fDataList(0), fName(name), fTitle(title), fAlgorithms(), 
  fBreakin(0), fBreakout(0), fHasExecuted(kFALSE), fActive(kTRUE) {
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
  fBreakin = other.fBreakin;
  fBreakout = other.fBreakout;
  fHasExecuted = kFALSE;
  fActive = other.fActive;
}

Algorithm::~Algorithm() {
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

void Algorithm::Abort () {
  if (!fgBeginAlgo) {
    std::cout << " Nothing to abort: No algorithm currently running." << std::endl;
    return;
  }
  CleanAlgos();
  fgBeginAlgo  = 0;
  fgBreakPoint = 0;
}

void Algorithm::Continue () {
  if (!fgBeginAlgo) {
    std::cout << " No algorithm to continue." << std::endl;
    return;
  }
  fgBreakPoint = 0;

  fgBeginAlgo->ExecuteAlgos(fOption.Data());

  if (!fgBreakPoint) {
    fgBeginAlgo->CleanAlgos();
    fgBeginAlgo = 0;
  }
}

void  Algorithm::CleanAlgos () {
  if (fBreakin)  fBreakin  = 1;
  if (fBreakout) fBreakout = 1;
  fHasExecuted = kFALSE;
  Clear();
  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo)
    (*algo)->CleanAlgos();
}

void  Algorithm::ExecuteAlgo (Option_t *option) {

  if (fgBeginAlgo) {
    std::cerr << "Cannot execute algorithm:" << fName 
              << ", already running algorithm: " << fgBeginAlgo->fName << std::endl;
    return;
  }
  if (!IsActive()) return;

  fOption = option;
  fgBeginAlgo = this;
  fgBreakPoint = 0;

  if (fBreakin) return;
  Exec(option);

  fHasExecuted = kTRUE;
  ExecuteAlgos(option);

  if (fBreakout) return;

  if (!fgBreakPoint) {
    fgBeginAlgo->CleanAlgos();
    fgBeginAlgo = 0;
  }
}

void  Algorithm::ExecuteAlgos (Option_t *option) {

  for (std::list<Algorithm*>::iterator algo = fAlgorithms.begin();
       algo != fAlgorithms.end(); ++algo) {

    if (fgBreakPoint) return;
    if (!(*algo)->IsActive()) continue;
    if ((*algo)->fHasExecuted) {
      (*algo)->ExecuteAlgos(option);
      continue;
    }
    if ((*algo)->fBreakin == 1) {
      std::cout << "Break at entry of algorithm: " << (*algo)->fName << std::endl;
      fgBreakPoint = this;
      (*algo)->fBreakin++;
      return;
    }

    (*algo)->Exec(option);
    (*algo)->fHasExecuted = kTRUE;
    (*algo)->ExecuteAlgos(option);
    if ((*algo)->fBreakout == 1) {
      std::cout << "Break at exit of algorithm: " << (*algo)->fName << std::endl;
      fgBreakPoint = this;
      (*algo)->fBreakout++;
      return;
    }
  }
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
