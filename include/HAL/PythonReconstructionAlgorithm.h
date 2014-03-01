#ifndef __CINT__
#include "Python.h"
#endif
#include <TString.h>
#include <HAL/ReconstructionAlgorithm.h>

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
  PythonReconstructionAlgorithm (TString name = "", TString title = "", PyObject *self = 0);
  virtual ~PythonReconstructionAlgorithm ();
  
  // User must overide these -------------------
  virtual void  Exec (Option_t * options = "");
  virtual void  Clear (Option_t * options = "");
  // -------------------------------------------

private:
// private helpers for forwarding to python
  void SetupPySelf();
  PyObject* CallSelf(const char* method, PyObject* pyobject = 0);

  PyObject* fPySelf;              //! actual python object

  //ClassDef(PythonReconstructionAlgorithm, 0);
};

} /* HAL  */ 

#endif
