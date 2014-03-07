#include "HAL.h"
#include <iostream>

class ElectronReco : public HAL::ReconstructionAlgorithm {
public:
  ElectronReco () : ReconstructionAlgorithm("Electron", "Reconstruct electrons") {}
  virtual ~ElectronReco () {} 
  virtual void Exec (Option_t* option = "") {
    HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
    for (int i = 0; i < tr->GetDim("jet_pt"); ++i)
      std::cout << tr->GetDecimal("jet_pt", i) << "   ";
    std::cout << std::endl;
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
