#ifndef HAL_PYTHON_RECONSRTUCTION_ALGORITHM
#define HAL_PYTHON_RECONSRTUCTION_ALGORITHM

#include <TString.h>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/Algorithm.h>

// Python
struct _object;
typedef _object PyObject;

namespace HAL {

class PythonAlgorithm : public Algorithm {
public:
  PythonAlgorithm (TString name = "", TString title = "", TString PyPath = "", TString PyFile = "", TString PyClass = "", PyObject *self = 0);
  virtual ~PythonAlgorithm ();
  
  // User can overide these --------------------
  virtual void  Init (Option_t * options = "");
  virtual void  Exec (Option_t * options = "");
  virtual void  Clear (Option_t * options = "");
  // -------------------------------------------
private:
  // private helpers for forwarding to python
  void SetupPySelf();
  // Steals pyargs and pykw arguements
  PyObject* CallSelf(TString method, PyObject *pyargs = NULL);

  TString   fPyPath, fPyFile, fPyClass;
  PyObject* fPySelf;              //! actual python object

  //ClassDef(PythonAlgorithm, 0);
};

} /* HAL  */ 

#endif