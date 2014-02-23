#include "Algorithm.h"

#ifndef HAL_RECONSRTUCTION_ALGORITHM
#define HAL_RECONSRTUCTION_ALGORITHM

namespace HAL {

class ReconstructionAlgorithm : public Algorithm {
public:
  ReconstructionAlgorithm(const char* name, const char* title) : Algorithm(name, title) {}
  virtual ~ReconstructionAlgorithm() {}
  virtual void Exec (Option_t * /*option*/ = "") {}
  virtual void Clear (Option_t * /*option*/ = "") {}
};

} /* HAL  */ 

#endif
