#include "Algorithm.h"

#ifndef HAL_CUT_ALGORITHM
#define HAL_CUT_ALGORITHM

namespace HAL {

class CutAlgorithm : public Algorithm {
public:
  CutAlgorithm(const char* name, const char* title) : Algorithm(name, title) {}
  virtual ~CutAlgorithm() {}
  virtual void Exec (Option_t * /*option*/ = "") {}
  virtual void Clear (Option_t * /*option*/ = "") {}
};

} /* HAL  */ 

#endif
