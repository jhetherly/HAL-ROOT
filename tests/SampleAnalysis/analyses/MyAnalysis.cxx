#include "HAL.h"
#include "MyAlgos.h"

int main(int argc, char *argv[]) {
  // Make analysis object
  HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "truth");
  //HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "Delphes");
  //HAL::Analysis     a("Base Algorithm", "Test of the Analysis framework.", "physics");

  // Load files to the analysis object
  a.AddFiles("/Users/jhetherly/Documents/Graduate_Work/ATLAS/ECFA/ECFA_Validation_Signal_Samples/Version_July25/AZh/ggA500mm/validation.ggA500Zh.mumu.truth*");
  //a.AddFiles("/Users/jhetherly/Documents/Graduate_Work/ATLAS/Charged_Higgs_MC/output/h_plus_1000GeV_2HDM_mod_plus_root-s_14000GeV_25000_Events_140PU.root");
  //a.AddFiles("/data/localdata/HH/data_2012/data12_8TeV.00200804.physics_Muons.merge.NTUP_COMMON.r4644_p1517_p1575_tid01403444_00/NTUP_COMMON.01403444._000001.root.1");

  // Output settings
  a.SetOutputFileName("test.root");
  a.SetOutputTreeName("sample");
  a.SetOutputTreeDescription("This is just a test");

  //a.AddAlgo(new DiJetReco());
  
  // Create algorithm flow
  //a.AddCutAlgo(new HAL::PythonReconstructionAlgorithm("pyreco", "testing python algo", 
  //      "/Users/jhetherly/src/root_HAL/tests/SampleAnalysis/algorithms/python", 
  //      "pyalgo", 
  //      "PythonAlgo"));
  a.AddAlgo(new HAL::Algorithms::ImportParticle("jets", "import basic jet objects"));
  a.AddAlgo(new HAL::Algorithms::ImportParticle("mc", "import basic Monte Carlo objects"));
  a.AddAlgo(new HAL::Algorithms::AttachAttribute("mc charge att", "attach charge to mc particles", 
                                                 "mc", 
                                                 "mc_test_charge"));

  a.AddAlgo(new HAL::Algorithms::EmptyCut("number of events", "baseline event number"));

  a.AddAlgo(new HAL::Algorithms::SelectParticle("mc without charge", "filter on mc without charge", 
                                                "mc charge att", // input algorithm
                                                "mc_test_charge", "==", 0));
  a.AddAlgo(new HAL::Algorithms::SelectParticle("mc_5GeV", "filter on mc pt >= 5GeV", 
                                                "mc without charge", // input algorithm
                                                "pt", ">=", 5000)); // pT value

  a.AddAlgo(new HAL::Algorithms::SelectParticle("mc_neutrinos", "filter on mc id to get neutrinos", 
                                                "mc_5GeV", // input algorithm
                                                "id", 6,
                                                -16, -14, -12, 12, 14, 16)); // id values

  a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("leading pt neutrino", "find highest pt neutrino", 
                                                       "mc_neutrinos", // input algorithm
                                                       1, "pt")); // rank in pt

  a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("leading pt jet", "find highest pt jet", 
                                                       "jets", // input algorithm
                                                       1, "pt")); // rank in pt
  a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("subleading pt jet", "find 2nd highest pt jet", 
                                                       "jets", // input algorithm
                                                       2, "pt")); // rank in pt
  a.AddAlgo(new HAL::Algorithms::AttachAttribute("jet pT rank att", "attach pt rank to jets", 
                                                 "jets", 
                                                 "jets_pT_rank", "rank_pt"));

  a.AddAlgo(new HAL::Algorithms::VecAddReco("di-jet", "reconstruct a di-jet object from highest pt", 
                                            2, "leading pt jet", "subleading pt jet"));
  //a.AddAlgo(new HAL::Algorithms::VecAddReco("di-jet any", "reconstruct a di-jet object", 
  //                                          2, "jets", "jets"));
  //a.AddAlgo(new HAL::Algorithms::VecAddReco("double di-jet any", "reconstruct a double di-jet object", 
  //                                          2, "di-jet any", "di-jet any"));
  //a.AddAlgo(new HAL::Algorithms::MonitorAlgorithm("di-jet monitor", "look at the di-jet object", "double di-jet any", 100));

  a.AddAlgo(new HAL::Algorithms::SelectParticle("di-jet50pt", "filter on di-jet pt >= 50GeV", 
                                                "di-jet", // input algorithm
                                                "pt", ">=", 50000)); // pT value
  a.AddAlgo(new HAL::Algorithms::SelectParticle("di-jetfinal", "filter on di-jet |eta| <= 1.5", 
                                                "di-jet50pt", // input algorithm
                                                "eta", "inclusive", -1.5, 1.5)); // eta low and high values
  a.AddAlgo(new HAL::Algorithms::SelectRefParticle("jets close", "filter on jets within deltaR of di-jet", 
                                                   "di-jetfinal", "jets", // input, reference algorithms
                                                   0.4)); // Delta R value

  a.AddAlgo(new HAL::Algorithms::Cut("di-jet and neutrino existence cut", "make sure 1 dijet exists", 
                                     "and", 2, // logic, number of cuts
                                     "mc_neutrinos", "particle", ">=", 1,
                                     "di-jetfinal", "particle", "==", 1));

  a.AddAlgo(new HAL::Algorithms::StoreParticle("store di-jet mass", "store the mass of the di-jet system", 
                                               "di-jetfinal", "m", "dijet_mass")); // input, type, branch
  a.AddAlgo(new HAL::Algorithms::StoreParticle("store di-jet", "store the di-jet system", 
                                               "di-jetfinal", "all", "dijet")); // input, type, branch
  a.AddAlgo(new HAL::Algorithms::StoreParticle("store jets", "store jets", 
                                               "jet pT rank att", "all", "jets", "exam")); // input, type, branch, tree
  a.AddAlgo(new HAL::Algorithms::StoreParticle("store neutrinos", "store Monte Carlo neutrinos", 
                                               "mc_neutrinos", "all", "neutrinos", "test")); // input, type, branch, tree
  
  // Assign any branch maps
  a.MapBranch("jet_AntiKt4TruthJets_n", "jets:nentries");
  a.MapBranch("jet_AntiKt4TruthJets_pt", "jets:pT");
  a.MapBranch("jet_AntiKt4TruthJets_eta", "jets:eta");
  a.MapBranch("jet_AntiKt4TruthJets_phi", "jets:phi");
  a.MapBranch("jet_AntiKt4TruthJets_m", "jets:m");
  a.MapBranch("mc_n", "mc:nentries");
  a.MapBranch("mc_pt", "mc:pt");
  a.MapBranch("mc_eta", "mc:eta");
  a.MapBranch("mc_phi", "mc:phi");
  a.MapBranch("mc_m", "mc:m");
  a.MapBranch("mc_pdgId", "mc:id");
  a.MapBranch("mc_charge", "mc:charge");
  a.MapBranch("mc_charge", "mc charge att:value");
  //a.MapBranch("Jet_size", "jets:nentries");
  //a.MapBranch("Jet.PT", "jets:pT");
  //a.MapBranch("Jet.Eta", "jets:eta");
  //a.MapBranch("Jet.Phi", "jets:phi");
  //a.MapBranch("Jet.Mass", "jets:m");

  //a.MapBranch("jet_AntiKt4TruthJets_n", "jet_n");
  //a.MapBranch("jet_AntiKt4TruthJets_pt", "jet_pt");
  //a.MapBranch("jet_AntiKt4TruthJets_eta", "jet_eta");
  //a.MapBranch("jet_AntiKt4TruthJets_phi", "jet_phi");
  //a.MapBranch("jet_AntiKt4TruthJets_m", "jet_m");
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

  //a.SetMessagePeriod();
  a.SetMessagePeriod(1000);
  // Run the analysis
  //a.Process("", 1000);
  a.Process();

  a.PrintCutReport();
  a.PrintCounterSummary();
  return 0;
}
