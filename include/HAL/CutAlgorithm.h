#ifndef HAL_CUT_ALGORITHM
#define HAL_CUT_ALGORITHM

#include <TString.h>
#include <HAL/Common.h>
#include <HAL/Algorithm.h>

namespace HAL {

class CutAlgorithm : public Algorithm {
public:
  CutAlgorithm(TString name = "", TString title = "") : Algorithm(name, title) {
    fAlgorithmType = "cut";
  }
  virtual ~CutAlgorithm() {}

protected:
  void        Passed ();
};

} /* HAL  */ 

#endif
