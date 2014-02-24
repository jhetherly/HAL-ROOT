#ifndef __CINT__
#include "Python.h"
#endif
#include <TString.h>
#include "ReconstructionAlgorithm.h"

#ifndef HAL_PYTHON_RECONSRTUCTION_ALGORITHM
#define HAL_PYTHON_RECONSRTUCTION_ALGORITHM

#ifdef __CINT__
// Python
struct _object;
typedef _object PyObject;
#endif

namespace HAL {

class PythonReconstructionAlgorithm : public ReconstructionAlgorithm {
public:
  PythonReconstructionAlgorithm (TString name, TString title) : ReconstructionAlgorithm(name, title) {}
  virtual ~PythonReconstructionAlgorithm () {}
};

} /* HAL  */ 

#endif
