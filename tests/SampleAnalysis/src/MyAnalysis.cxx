#include "HAL.h"
#include "MyAlgos.h"
#include <iostream>

int main(int argc, char *argv[]) {
  HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "truth");
  //HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "Delphes");

  a.AddFiles("/Users/jhetherly/Documents/Graduate_Work/ATLAS/ECFA/ECFA_Validation_Signal_Samples/Version_July25/AZh/ggA300mm/validation.ggA300Zh.mumu.truth*");
  //a.AddFiles("/Users/jhetherly/Documents/Graduate_Work/ATLAS/Charged_Higgs_MC/output/h_plus_1000GeV_2HDM_mod_plus_root-s_14000GeV_25000_Events_140PU.root");

  a.AddRecoAlgo(new ElectronReco());
  a.AddCutAlgo(new ElectronTight());

  a.MapBranch("jet_AntiKt4TruthJets_", "jet_");
  //a.MapBranch("Jet.PT", "jet_pt");

  //a.PrintTree();
  a.Process();
  //a.Process("", 50);

  return 0;
}
