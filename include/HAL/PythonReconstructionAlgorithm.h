#ifndef HAL_PYTHON_RECONSRTUCTION_ALGORITHM
#define HAL_PYTHON_RECONSRTUCTION_ALGORITHM

#ifndef __CINT__
#include "Python.h"
#endif
#include <TString.h>
#include <TPython.h>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/ReconstructionAlgorithm.h>

#ifdef __CINT__
// Python
struct _object;
typedef _object PyObject;
#endif

namespace HAL {

class PythonReconstructionAlgorithm : public ReconstructionAlgorithm {
public:
  PythonReconstructionAlgorithm (TString name = "", TString title = "", TString PyFile = "", TString PyClass = "", PyObject *self = 0);
  virtual ~PythonReconstructionAlgorithm ();
  
  // User must overide these -------------------
  virtual void  Init (Option_t * options = "");
  virtual void  Exec (Option_t * options = "");
  virtual void  Clear (Option_t * options = "");
  // -------------------------------------------

private:
// private helpers for forwarding to python
  void SetupPySelf();
  PyObject* CallSelf(const char* method, PyObject* pyobject = 0);

  TString   fPyFile, fPyClass;
  PyObject* fPySelf;              //! actual python object

  //ClassDef(PythonReconstructionAlgorithm, 0);
};

} /* HAL  */ 

#endif
