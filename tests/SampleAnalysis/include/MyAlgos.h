#include "HAL.h"
#include <iostream>

class ElectronReco : public HAL::ReconstructionAlgorithm {
public:
  ElectronReco () : ReconstructionAlgorithm("Electron", "Reconstruct electrons") {}
  virtual ~ElectronReco () {} 
  virtual void Exec (Option_t* option = "") {
    HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
    std::cout << tr->GetInt("mc_n") << "    " << tr->GetInt("jet_n") << "    " << tr->GetInt("ph_n") << std::endl;
    //tr->GetInt("mc_n");
    //tr->GetInt("jet_n");
    //tr->GetInt("ph_n");
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
