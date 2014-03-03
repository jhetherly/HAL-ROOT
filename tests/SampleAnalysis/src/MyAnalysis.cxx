#include "HAL.h"
#include "MyAlgos.h"
#include <iostream>

int main(int argc, char *argv[]) {
  HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "truth");

  a.AddFiles("/Users/jhetherly/Documents/Graduate_Work/ATLAS/ECFA/ECFA_Validation_Signal_Samples/Version_July25/AZh/ggA300mm/validation.ggA300Zh.mumu.truth*");

  a.AddRecoAlgo(new ElectronReco());
  a.AddCutAlgo(new ElectronTight());

  a.MapBranch("jet_AntiKt4TruthJets_", "jet_");

  //a.PrintTree();
  a.Process();

  return 0;
}
