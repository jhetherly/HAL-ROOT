#include "HAL.h"

class ElectronReco : public HAL::ReconstructionAlgorithm {
public:
  ElectronReco () : ReconstructionAlgorithm("Electron", "Reconstruct electrons") {}
  virtual ~ElectronReco () {} 
  virtual void Exec (Option_t* option = "") {
  }
  virtual void Clear (Option_t* option = "") {}
};

class ElectronTight : public HAL::CutAlgorithm {
public:
  ElectronTight () : CutAlgorithm("ElectronTight", "Making tight selections on electrons") {}
  virtual ~ElectronTight () {} 
  virtual void Exec (Option_t* option = "") {
  }
  virtual void Clear (Option_t* option = "") {}
};
