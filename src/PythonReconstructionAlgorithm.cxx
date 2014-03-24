#ifndef __CINT__
#include "Python.h"
#endif

#include <iostream>
#include <HAL/PythonReconstructionAlgorithm.h>

//ClassImp(HAL::PythonReconstructionAlgorithm);

namespace HAL {

PythonReconstructionAlgorithm::PythonReconstructionAlgorithm (TString name, TString title, 
                                                              TString pypath, TString pyfile, 
                                                              TString pyclass, PyObject *self) : 
  ReconstructionAlgorithm(name, title), fPyPath(pypath), fPyFile(pyfile), fPyClass(pyclass) {
  if ( self ) {
    // steal reference as this is us, as seen from python
    fPySelf = self;
  } else {
    fPySelf = NULL;
  }
}

PythonReconstructionAlgorithm::~PythonReconstructionAlgorithm () {
  // Destructor. Only deref if still holding on to Py_None (circular otherwise).
  //if ( fPySelf == Py_None ) {
  //  Py_DECREF( fPySelf );
  //}
  if (fPySelf)
    Py_DECREF(fPySelf);
}

void  PythonReconstructionAlgorithm::Init (Option_t *options) {
  // First function called, and used to setup the python self; forward call.
  SetupPySelf();

  // Setup DataList on python side
  // Can't use PyCapsule_New on versions of python < 2.7
  //PyObject *result = CallSelf("AssignDataList", Py_BuildValue("(N)", PyCapsule_New(fDataList, NULL, NULL)));
  
  PyObject *result = CallSelf("Init", Py_BuildValue("(s)", options));

  Py_XDECREF( result );
}

void  PythonReconstructionAlgorithm::Exec (Option_t *options) {
  PyObject* result = CallSelf("Exec", Py_BuildValue("(s)", options));

  Py_XDECREF( result );
}

void  PythonReconstructionAlgorithm::Clear (Option_t *options) {
  PyObject* result = CallSelf("Clear", Py_BuildValue("(s)", options));

  Py_XDECREF( result );
}

void PythonReconstructionAlgorithm::SetupPySelf() {
  int class_check;
  PyObject *po_HALmod = NULL, 
           *po_USRmod = NULL, 
           *po_HALclass = NULL, 
           *po_USRclass = NULL, 
           *po_USRpath = NULL,
           *po_USRargs = NULL,
           *po_SYSmod = NULL,
           *po_SYSpath = NULL;

  // Initialize python if need be
  if (!Py_IsInitialized())
    Py_Initialize();

  if (fPySelf) 
    Py_DECREF(fPySelf);

  // Load sys module and add user's path as first element to sys.path
  po_SYSmod = PyImport_ImportModule("sys");
  po_SYSpath = PyObject_GetAttrString(po_SYSmod, "path");
  po_USRpath = Py_BuildValue("s", fPyPath.Data());
  if (PyList_Insert(po_SYSpath, 0, po_USRpath))
    throw HALException("Couldn't modify system path.");


  // Load in HAL module
  po_HALmod = PyImport_ImportModule("HAL");
  if (po_HALmod == NULL)
    throw HALException("Couldn't locate HAL.py to load as a module for python.");
  // Find HAL class to compare user class to
  po_HALclass = PyObject_GetAttrString(po_HALmod, "ReconstructionAlgorithm");
  if (po_HALclass == NULL)
    throw HALException("Couldn't locate HAL class.");


  // Load in user's module
  po_USRmod = PyImport_ImportModule(fPyFile.Data());
  if (po_USRmod == NULL)
    throw HALException(fPyFile.Prepend("Couldn't locate user's file to load as a module for python: ").Data());
  // Find user's class
  po_USRclass = PyObject_GetAttrString(po_USRmod, fPyClass.Data());
  if (po_USRclass == NULL)
    throw HALException(fPyClass.Prepend("Couldn't locate user's python class: ").Data());

  
  // Make sure that the user's class properly inherits from the HAL class
  class_check = PyObject_IsSubclass(po_USRclass, po_HALclass);
  if (class_check == -1)
    throw HALException(fPyClass.Prepend("Error in determining class relationship for: ").Data());
  if (class_check == 0)
    throw HALException(fPyClass.Prepend("Make sure your python class inherits from the proper HAL class: ").Data());


  // Call python class to make the object
  if (!PyCallable_Check(po_USRclass))
    throw HALException(fPyClass.Prepend("Python class couldn't be called: ").Data());
  po_USRargs = Py_BuildValue("()");
  fPySelf = PyObject_CallObject(po_USRclass, po_USRargs);
  if (fPySelf == NULL)
    throw HALException("Couldn't instantiate the python class.");
  
  Py_DECREF(po_SYSmod);
  Py_DECREF(po_SYSpath);
  Py_DECREF(po_USRpath);
  Py_DECREF(po_HALmod);
  Py_DECREF(po_USRmod);
  Py_DECREF(po_HALclass);
  Py_DECREF(po_USRclass);
  Py_DECREF(po_USRargs);
}

// Steals args and kw arguements
PyObject* PythonReconstructionAlgorithm::CallSelf (TString method, 
                                                   PyObject *po_args) {
  // Forward <method> to python.
  PyObject *result = NULL;
  if (po_args == NULL) po_args = Py_BuildValue("()");

  PyObject *po_method = PyObject_GetAttrString( fPySelf, method.Data());
  //if (po_method == NULL) // method wasn't overwritten on python side, ignore
  //  result = Py_None;
  //else 
  //  result = PyObject_Call(po_method, po_args, po_kw);
  if (po_method != NULL)
    result = PyObject_CallObject(po_method, po_args);

  Py_XDECREF( po_method );
  Py_DECREF(po_args);

  return result;
}

} /* HAL */ 
