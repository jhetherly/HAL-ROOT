#include "HAL.h"
#include <iostream>

class ElectronReco : public HAL::ReconstructionAlgorithm {
public:
  ElectronReco () : ReconstructionAlgorithm("Electron", "Reconstruct electrons") {}
  virtual ~ElectronReco () {} 
  virtual void Exec (Option_t* option = "") {
    HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
    std::cout << tr->GetInteger("mc_n") << "    " << tr->GetInteger("jet_n") << "    " << tr->GetInteger("ph_n") << std::endl;
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
