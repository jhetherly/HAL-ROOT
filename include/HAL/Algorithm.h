#include <TTask.h>
#include <TObjString.h>
#include <TString.h>
#include <TObject.h>
#include <TMap.h>
#include <TList.h>
#include <iostream>

#ifndef HAL_ALGORITHM
#define HAL_ALGORITHM

class AnalysisData;

namespace HAL
{

class Algorithm : public TTask {
public:
  Algorithm(const char* name, const char* title) : TTask(name, title), fData(0) {}
  virtual ~Algorithm() {}
  virtual void Exec (Option_t *option) {std::cout << option << "  works in Algorithm" << std::endl;}
  //virtual void Clear (Option_t *option) {}
  void AddData (TString name, TObject *obj);
  TObject* GetData (TString name);
  Bool_t CheckData (TString name);
  void AssignData (TMap *map);

private:
  TMap *fData;

  ClassDef(Algorithm, 0);
};

} /* HAL */ 

#endif
