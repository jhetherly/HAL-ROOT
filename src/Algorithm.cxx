#include <HAL/Algorithm.h>

namespace HAL {

void Algorithm::AddData (TString name, TObject *obj) {
  fData->Add(new TObjString(name.Data()), obj);
}

TObject* Algorithm::GetData (TString name) {
  return fData->GetValue(name.Data());
}

Bool_t Algorithm::CheckData (TString name) {
  return fData->FindObject(name.Data()) ? kTRUE : kFALSE;
}

void Algorithm::AssignData (TMap *map) {
  fData = map;
  // Assign data to all sub-algorithms
  TIter next(fTasks);
  Algorithm *algo;
  while ((algo = (Algorithm*)next())) {
    algo->AssignData(map);
  }
}

} /* HAL */ 
