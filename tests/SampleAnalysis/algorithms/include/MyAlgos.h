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
    TLorentzVector *el_1 = NULL, *el_2 = NULL, *di_el = NULL, *temp = NULL;
    int el_1_i, el_2_i;

    // Find leading and subleading electrons in pT
    for (int i = 0; i < tr->GetInteger("jet_n"); ++i) {
      temp = HAL::makeTLVFromPtEtaPhiM(tr->GetDecimal("jet_pt", i), 
                                       tr->GetDecimal("jet_eta", i), 
                                       tr->GetDecimal("jet_phi", i), 
                                       tr->GetDecimal("jet_m", i));
      if (el_1 == NULL) {el_1_i = i; el_1 = temp; continue;}
      if (el_1->Pt() < temp->Pt()) {
        if (el_2 != NULL) delete el_2;
        el_2 = el_1;
        el_2_i = el_1_i;
        el_1 = temp;
        el_1_i = i;
        continue;
      }
      if (el_2 == NULL) {el_2 = temp; continue;}
      delete temp;
    }

    // Construct di-electron object
    if (el_1 != NULL && el_2 != NULL) {
      di_el = el_1;
      di_el->operator+=(*el_2);
      delete el_2;
    }

    // Store data
    data->SetValue("di-jet p", di_el);
    data->SetValue("di-jet leading index", el_1_i);
    data->SetValue("di-jet subleading index", el_2_i);
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

    if (data->GetTObject("di-jet p") == NULL ||
        ((TLorentzVector*)data->GetTObject("di-jet p"))->M() < 500.0) {
      Abort(); 
      return;
    }
    output->SetValue("dijet_mass", ((TLorentzVector*)data->GetTObject("di-jet p"))->M());
  }

  virtual void Clear (Option_t* option) {}
};
