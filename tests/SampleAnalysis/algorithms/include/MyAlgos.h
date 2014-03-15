#include "HAL.h"
#include <iostream>

class ElectronReco : public HAL::ReconstructionAlgorithm {
public:
  ElectronReco () : ReconstructionAlgorithm("Electron", "Reconstruct electrons") {}
  virtual ~ElectronReco () {} 
protected:
  virtual void Init (Option_t* option = "") { std::cout << "Init ElectronReco" << std::endl; }
  virtual void Exec (Option_t* option = "") {
    HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
    for (int i = 0; i < tr->GetDim("jet_pt"); ++i)
      std::cout << tr->GetDecimal("jet_pt", i) << "   ";
    std::cout << std::endl;
  }
  virtual void Clear (Option_t* option = "") {std::cout << "Clear ElectronReco" << std::endl;}
};

class ElectronTight : public HAL::CutAlgorithm {
public:
  ElectronTight () : CutAlgorithm("ElectronTight", "Making tight selections on electrons") {}
  virtual ~ElectronTight () {} 
protected:
  virtual void Init (Option_t* option = "") {std::cout << "Init ElectronTight" << std::endl;}
  virtual void Exec (Option_t* option = "") {
    std::cout << "Made Cut" << std::endl;
    Abort();
    return;
  }
  virtual void Clear (Option_t* option = "") {std::cout << "Clear ElectronTight" << std::endl;}
};
