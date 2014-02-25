#include <TString.h>
#include <HAL/Algorithm.h>

#ifndef HAL_RECONSRTUCTION_ALGORITHM
#define HAL_RECONSRTUCTION_ALGORITHM

namespace HAL {

class ReconstructionAlgorithm : public Algorithm {
public:
  ReconstructionAlgorithm(TString name, TString title) : Algorithm(name.Prepend("Reco - "), title) {}
  virtual ~ReconstructionAlgorithm() {}
};

} /* HAL  */ 

#endif
