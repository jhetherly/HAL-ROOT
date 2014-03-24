#include "HAL.h"
#include <TLorentzVector.h>
#include <iostream>

class DiJetReco : public HAL::ReconstructionAlgorithm {
public:
  DiJetReco () : ReconstructionAlgorithm("DiJetReco", "Reconstruct di-jets") {}
  virtual ~DiJetReco () {} 

  virtual void Init (Option_t* option) {}

  virtual void Exec (Option_t* option) {
    HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
    HAL::AnalysisTreeReader *tr = (HAL::AnalysisTreeReader*)GetData("RawData");
    TLorentzVector *jet_1 = NULL, *jet_2 = NULL, *di_jet = NULL, *temp = NULL;
    int jet_1_i, jet_2_i;

    // Find leading and subleading jet in pT
    for (int i = 0; i < tr->GetInteger("jet_n"); ++i) {
      temp = HAL::makeTLVFromPtEtaPhiM(tr->GetDecimal("jet_pt", i), 
                                       tr->GetDecimal("jet_eta", i), 
                                       tr->GetDecimal("jet_phi", i), 
                                       tr->GetDecimal("jet_m", i));
      if (jet_1 == NULL) {jet_1_i = i; jet_1 = temp; continue;}
      if (jet_1->Pt() < temp->Pt()) {
        if (jet_2 != NULL) delete jet_2;
        jet_2 = jet_1;
        jet_2_i = jet_1_i;
        jet_1 = temp;
        jet_1_i = i;
        continue;
      }
      if (jet_2 == NULL) {jet_2 = temp; continue;}
      delete temp;
    }

    // Construct di-jet object
    if (jet_1 != NULL && jet_2 != NULL) {
      di_jet = jet_1;
      di_jet->operator+=(*jet_2);
      delete jet_2;
    }

    // Store data
    data->SetValue("di-jet p", di_jet);
    data->SetValue("di-jet leading index", jet_1_i);
    data->SetValue("di-jet subleading index", jet_2_i);
  }

  virtual void Clear (Option_t* option) {
    HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");

    delete data->GetTObject("di-jet p");
  }
};

class DiJetCut : public HAL::CutAlgorithm {
public:
  DiJetCut () : CutAlgorithm("DiJetCut", "Make selections on di-jets") {}
  virtual ~DiJetCut () {} 

  virtual void Init (Option_t* option) {}

  virtual void Exec (Option_t* option) {
    HAL::AnalysisData *data = (HAL::AnalysisData*)GetData("UserData");
    HAL::AnalysisData *output = (HAL::AnalysisTreeWriter*)GetData("UserOutput");

    // Remove those di-jet objects whose mass is < 500.0 GeV
    if (data->GetTObject("di-jet p") == NULL ||
        ((TLorentzVector*)data->GetTObject("di-jet p"))->M() < 500.0) {
      Abort(); 
      return;
    }
    output->SetValue("dijet_mass", ((TLorentzVector*)data->GetTObject("di-jet p"))->M());
  }

  virtual void Clear (Option_t* option) {}
};
