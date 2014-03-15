#ifndef HAL_RECONSRTUCTION_ALGORITHM
#define HAL_RECONSRTUCTION_ALGORITHM

#include <TString.h>
#include <HAL/Common.h>
#include <HAL/Algorithm.h>

namespace HAL {

class ReconstructionAlgorithm : public Algorithm {
public:
  ReconstructionAlgorithm(TString name = "", TString title = "") : Algorithm(name.Prepend("Reco - "), title) {}
  virtual ~ReconstructionAlgorithm() {}
};

} /* HAL  */ 

#endif
