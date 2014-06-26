/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 */

#ifndef HAL_CutAlgorithm
#define HAL_CutAlgorithm

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
