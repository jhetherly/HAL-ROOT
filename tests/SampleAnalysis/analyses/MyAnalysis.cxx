#include "HAL.h"
#include "MyAlgos.h"

int main(int argc, char *argv[]) {
  // Make analysis object
  HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "truth");
  //HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "Delphes");
  //HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "physics");

  // Load files to the analysis object
  //a.AddFiles("/Users/jhetherly/Documents/Graduate_Work/ATLAS/ECFA/ECFA_Validation_Signal_Samples/Version_July25/AZh/ggA300mm/validation.ggA300Zh.mumu.truth*");
  a.AddFiles("/Users/jhetherly/Documents/Graduate_Work/ATLAS/ECFA/ECFA_Validation_Signal_Samples/Version_July25/AZh/ggA500mm/validation.ggA500Zh.mumu.truth*");
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
  a.AddAlgo(new HAL::Algorithms::ImportTLV("jets", "import basic jet objects"));
  //a.AddAlgo(new HAL::Algorithms::EmptyCut("number of events", "baseline event number"));
  //a.AddAlgo(new HAL::Algorithms::RankSelectionTLV("leading pt jet", "find highest pt jet", 
  //                                                "jets", // input algorithm
  //                                                1, "pt")); // rank in pt
  //a.AddAlgo(new HAL::Algorithms::RankSelectionTLV("subleading pt jet", "find 2nd highest pt jet", 
  //                                                "jets", // input algorithm
  //                                                2, "pt")); // rank in pt
  //a.AddAlgo(new HAL::Algorithms::VecAddReco("di-jet", "reconstruct a di-jet object", 
  //                                          2, "leading pt jet", "subleading pt jet"));
  //a.AddAlgo(new HAL::Algorithms::SelectTLV("di-jet50pt", "filter on di-jet pt >= 50GeV", 
  //                                         "di-jet", // input algorithm
  //                                         "pt", 50000)); // pT value
  //a.AddAlgo(new HAL::Algorithms::SelectTLV("di-jetfinal", "filter on di-jet |eta| <= 1.5", 
  //                                         "di-jet50pt", // input algorithm
  //                                         "eta", -1.5, 1.5)); // eta low and high values
  //a.AddAlgo(new HAL::Algorithms::SelectDeltaTLV("jets close", "filter on jets within deltaR of di-jet", 
  //                                              "di-jetfinal", "jets", // input, reference algorithms
  //                                              0.4)); // Delta R value
  //a.AddAlgo(new HAL::Algorithms::CutNObjects("di-jet existence cut", "make sure 1 dijet exists", "and",
  //                                           1, 1, "di-jetfinal"));
  //a.AddAlgo(new HAL::Algorithms::StoreTLV("store di-jet mass", "store the mass of the di-jet system", 
  //                                        "di-jetfinal", "m", "dijet_mass2")); // input, branch
  //a.AddAlgo(new HAL::Algorithms::StoreTLV("store di-jet pt", "store the pt of the di-jet system", 
  //                                        "di-jetfinal", "pt", "dijet_pt")); // input, branch
  //a.AddAlgo(new HAL::Algorithms::StoreTLV("store di-jet eta", "store the eta of the di-jet system", 
  //                                        "di-jetfinal", "eta", "dijet_eta")); // input, branch
  
  //a.AddAlgo(new DiJetReco());
  //a.AddAlgo(new DiJetCut());

  // Assign any branch maps
  a.MapBranch("jet_AntiKt4TruthJets_pt", "jets:pT");
  a.MapBranch("jet_AntiKt4TruthJets_eta", "jets:eta");
  a.MapBranch("jet_AntiKt4TruthJets_phi", "jets:phi");
  a.MapBranch("jet_AntiKt4TruthJets_m", "jets:m");

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

  a.SetMessagePeriod(1000);
  // Run the analysis
  //a.Process("", 1000);
  a.Process();

  a.PrintCutReport();
  a.PrintCounterSummary();
  return 0;
}
