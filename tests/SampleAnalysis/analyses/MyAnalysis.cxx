#include "HAL.h"
#include "MyAlgos.h"

int main(int argc, char *argv[]) {
  // Make analysis object
  HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "truth");
  //HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "Delphes");
  //HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "physics");

  // Load files to the analysis object
  a.AddFiles("/Users/jhetherly/Documents/Graduate_Work/ATLAS/ECFA/ECFA_Validation_Signal_Samples/Version_July25/AZh/ggA300mm/validation.ggA300Zh.mumu.truth*");
  //a.AddFiles("/Users/jhetherly/Documents/Graduate_Work/ATLAS/Charged_Higgs_MC/output/h_plus_1000GeV_2HDM_mod_plus_root-s_14000GeV_25000_Events_140PU.root");
  //a.AddFiles("/data/localdata/HH/data_2012/data12_8TeV.00200804.physics_Muons.merge.NTUP_COMMON.r4644_p1517_p1575_tid01403444_00/NTUP_COMMON.01403444._000001.root.1");

  // Output settings
  a.SetOutputFileName("test.root");
  a.SetOutputTreeName("physics");
  a.SetOutputTreeDescription("This is just a test");

  // Create algorithm flow
  //a.AddCutAlgo(new HAL::PythonReconstructionAlgorithm("pyreco", "testing python algo", 
  //      "/Users/jhetherly/src/root_HAL/tests/SampleAnalysis/algorithms/python", 
  //      "pyalgo", 
  //      "PythonAlgo"));
  a.AddRecoAlgo(new HAL::IA0001("electrons", "base electron import"));
  a.AddRecoAlgo(new DiJetReco());
  a.AddCutAlgo(new DiJetCut());

  // Assign any branch maps
  a.MapBranch("el_n", "electrons:nentries");
  a.MapBranch("el_pt", "electrons:x1");
  a.MapBranch("el_eta", "electrons:x2");
  a.MapBranch("el_phi", "electrons:x3");
  a.MapBranch("el_m", "electrons:x0");
  a.MapBranch("jet_AntiKt4TruthJets_eta", "jet_eta");
  a.MapBranch("jet_AntiKt4TruthJets_phi", "jet_phi");
  a.MapBranch("jet_AntiKt4TruthJets_m", "jet_m");
  a.MapBranch("jet_AntiKt4TruthJets_n", "jet_n");
  a.MapBranch("jet_AntiKt4TruthJets_pt", "jet_pt");
  a.MapBranch("jet_AntiKt4TruthJets_eta", "jet_eta");
  a.MapBranch("jet_AntiKt4TruthJets_phi", "jet_phi");
  a.MapBranch("jet_AntiKt4TruthJets_m", "jet_m");
  //a.MapBranch("Jet_size", "jet_n");
  //a.MapBranch("Jet.PT", "jet_pt");
  //a.MapBranch("Jet.Eta", "jet_eta");
  //a.MapBranch("Jet.Phi", "jet_phi");
  //a.MapBranch("Jet.Mass", "jet_m");
  //a.MapBranch("jet_AntiKt4LCTopo_n", "jet_n");
  //a.MapBranch("jet_AntiKt4LCTopo_pt", "jet_pt");
  //a.MapBranch("jet_AntiKt4LCTopo_eta", "jet_eta");
  //a.MapBranch("jet_AntiKt4LCTopo_phi", "jet_phi");
  //a.MapBranch("jet_AntiKt4LCTopo_m", "jet_m");

  // Actually run the analysis
  //a.Process("", 50);
  a.Process();

  return 0;
}
