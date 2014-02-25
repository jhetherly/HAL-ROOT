#include <TString.h>
#include <HAL/Algorithm.h>

#ifndef HAL_CUT_ALGORITHM
#define HAL_CUT_ALGORITHM

namespace HAL {

class CutAlgorithm : public Algorithm {
public:
  CutAlgorithm(TString name, TString title) : Algorithm(name.Prepend("Cut  - "), title) {}
  virtual ~CutAlgorithm() {}
};

} /* HAL  */ 

#endif
