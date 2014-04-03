#ifndef HAL_CUT_ALGORITHM
#define HAL_CUT_ALGORITHM

#include <TString.h>
#include <HAL/Common.h>
#include <HAL/Algorithm.h>

namespace HAL {

class CutAlgorithm : public Algorithm {
public:
  CutAlgorithm(TString name = "", TString title = "") : Algorithm(name, title), fNPassed(0) {}
  virtual ~CutAlgorithm() {}

  long long   GetNumberPassed () {return fNPassed;}

protected:
  void        Passed ();

private:
  long long   fNPassed;
};

} /* HAL  */ 

#endif
