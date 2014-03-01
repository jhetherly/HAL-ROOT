#include <HAL/PythonReconstructionAlgorithm.h>

//ClassImp(HAL::PythonReconstructionAlgorithm);

namespace HAL {

PythonReconstructionAlgorithm::PythonReconstructionAlgorithm (TString name, TString title, PyObject *self) : 
  ReconstructionAlgorithm(name, title) {
}

PythonReconstructionAlgorithm::~PythonReconstructionAlgorithm () {
}

void  PythonReconstructionAlgorithm::Exec (Option_t *options) {
}

void  PythonReconstructionAlgorithm::Clear (Option_t *options) {
}

void PythonReconstructionAlgorithm::SetupPySelf() {
}

PyObject* PythonReconstructionAlgorithm::CallSelf(const char *method, PyObject *pyobject) {
  return pyobject;
}

} /* HAL */ 
