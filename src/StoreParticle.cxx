#include <HAL/Algorithms/StoreParticle.h>

namespace HAL
{

/*
 * Generic class
 * */
internal::ParticlesTLVStore::ParticlesTLVStore (TString name, TString title, TString input, TString bname) :
  Algorithm(name, title), fSearchedForAttributes(false), fBranchName(bname), fInput(input) {

  fNParticles = TString::Format("%s_n", fBranchName.Data());
}

void internal::ParticlesTLVStore::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeWriter *output = GetUserOutput();
  HAL::GenericData *input_data = NULL;
  long long i = 0;

  if (data->Exists(fInput))
    input_data = (GenericData*)data->GetTObject(fInput);
  else
    return;

  if (!fSearchedForAttributes) {
    i = 0;
    fSearchedForAttributes = true;
    for (ParticlePtrsIt particle = input_data->GetParticleBegin(); 
         particle != input_data->GetParticleEnd(); ++ particle) {
      for (std::map<TString, long double>::iterator attribute = (*particle)->GetAttributes().begin();
           attribute != (*particle)->GetAttributes().end(); ++attribute) {
        bool found = false;
        for (std::map<TString, bool>::iterator stored_attribute = fAttributeFlags.begin();
             stored_attribute != fAttributeFlags.end(); ++stored_attribute) {
          if (attribute->first.EqualTo(stored_attribute->first)) found = true;
        }
        if (i == 0 || (found && fAttributeFlags[attribute->first])) fAttributeFlags[attribute->first] = true;
        else fAttributeFlags[attribute->first] = false;
      }
      ++i;
    }
    for (std::map<TString, bool>::iterator stored_attribute = fAttributeFlags.begin();
         stored_attribute != fAttributeFlags.end(); ++stored_attribute) {
      fAttributeLabels[stored_attribute->first] = 
        TString::Format("%s_%s", fBranchName.Data(), stored_attribute->first.Data());
    }
  }

  i = 0;
  output->SetValue(fNParticles, input_data->GetNParticles());
  for (ParticlePtrsIt particle = input_data->GetParticleBegin(); 
      particle != input_data->GetParticleEnd(); ++ particle) 
    StoreValue(output, i++, (*particle));
}








/*
 * Actual classes
 * */

/*
 * Exporting Algorithms
 * */

Algorithms::StoreParticle::StoreParticle (TString name, TString title, TString input, 
    TString property, TString bname, TString tree) :
  ParticlesTLVStore(name, title, input, bname), fAll(false), fAttributes(false), fPt(false), fM(false), 
  fE(false), fEt(false), fP3(false), fEta(false), fPhi(false), fID(false), fCharge(false), 
  fTreeName(tree) {

  if (property.EqualTo("pt", TString::kIgnoreCase))
    fPt = true;
  else if (property.EqualTo("m", TString::kIgnoreCase))
    fM = true;
  else if (property.EqualTo("e", TString::kIgnoreCase))
    fE = true;
  else if (property.EqualTo("et", TString::kIgnoreCase))
    fEt = true;
  else if (property.EqualTo("p3", TString::kIgnoreCase))
    fP3 = true;
  else if (property.EqualTo("eta", TString::kIgnoreCase))
    fEta = true;
  else if (property.EqualTo("phi", TString::kIgnoreCase))
    fPhi = true;
  else if (property.EqualTo("id", TString::kIgnoreCase))
    fID = true;
  else if (property.EqualTo("charge", TString::kIgnoreCase))
    fCharge = true;
  else if (property.EqualTo("all", TString::kIgnoreCase))
    fAll = true;
  else if (property.EqualTo("attributes", TString::kIgnoreCase))
    fAttributes = true;

  fPtLabel = TString::Format("%s_pt", fBranchName.Data());
  fEtLabel = TString::Format("%s_et", fBranchName.Data());
  fP3Label = TString::Format("%s_p", fBranchName.Data());
  fEtaLabel = TString::Format("%s_eta", fBranchName.Data());
  fPhiLabel = TString::Format("%s_phi", fBranchName.Data());
  fMLabel = TString::Format("%s_m", fBranchName.Data());
  fELabel = TString::Format("%s_e", fBranchName.Data());
  fIDLabel = TString::Format("%s_id", fBranchName.Data());
  fChargeLabel = TString::Format("%s_charge", fBranchName.Data());
}

void  Algorithms::StoreParticle::Init (Option_t* /*option*/) {
  HAL::AnalysisTreeWriter *output = GetUserOutput();

  output->SetTreeForBranch(fTreeName, fNParticles);

  if (fPt) {
    output->SetTreeForBranch(fTreeName, fBranchName);
    return;
  }
  if (fM) {
    output->SetTreeForBranch(fTreeName, fBranchName);
    return;
  }
  if (fE) {
    output->SetTreeForBranch(fTreeName, fBranchName);
    return;
  }
  if (fEt) {
    output->SetTreeForBranch(fTreeName, fBranchName);
    return;
  }
  if (fP3) {
    output->SetTreeForBranch(fTreeName, fBranchName);
    return;
  }
  if (fEta) {
    output->SetTreeForBranch(fTreeName, fBranchName);
    return;
  }
  if (fPhi) {
    output->SetTreeForBranch(fTreeName, fBranchName);
    return;
  }
  if (fID) {
    output->SetTreeForBranch(fTreeName, fBranchName);
    return;
  }
  if (fCharge) {
    output->SetTreeForBranch(fTreeName, fBranchName);
    return;
  }
  if (fAll) {
    output->SetTreeForBranch(fTreeName, fPtLabel);
    output->SetTreeForBranch(fTreeName, fEtaLabel);
    output->SetTreeForBranch(fTreeName, fPhiLabel);
    output->SetTreeForBranch(fTreeName, fMLabel);
    output->SetTreeForBranch(fTreeName, fELabel);
    output->SetTreeForBranch(fTreeName, fIDLabel);
    output->SetTreeForBranch(fTreeName, fChargeLabel);
    return;
  }
  if (fAttributes) { // Can't assign tree hear b/c attributes don't exist yet
    return;
  }
}

void Algorithms::StoreParticle::StoreValue (HAL::AnalysisTreeWriter *output, long long i, HAL::ParticlePtr particle) {
  TLorentzVector *vec = particle->GetP();

  if (fPt) {
    output->SetValue(fBranchName, vec->Pt(), i);
    return;
  }
  if (fM) {
    output->SetValue(fBranchName, vec->M(), i);
    return;
  }
  if (fE) {
    output->SetValue(fBranchName, vec->E(), i);
    return;
  }
  if (fEt) {
    output->SetValue(fBranchName, vec->Et(), i);
    return;
  }
  if (fP3) {
    output->SetValue(fBranchName, vec->P(), i);
    return;
  }
  if (fEta) {
    output->SetValue(fBranchName, vec->Eta(), i);
    return;
  }
  if (fPhi) {
    output->SetValue(fBranchName, vec->Phi(), i);
    return;
  }
  if (fID) {
    output->SetValue(fBranchName, particle->GetID(), i);
    return;
  }
  if (fCharge) {
    output->SetValue(fBranchName, particle->GetCharge(), i);
    return;
  }
  if (fAll) {
    for (std::map<TString, TString>::iterator it = fAttributeLabels.begin();
         it != fAttributeLabels.end(); ++it) {
      output->SetTreeForBranch(fTreeName, it->second);
      output->SetValue(it->second, particle->GetAttribute(it->first), i);
    }
    output->SetValue(fPtLabel, vec->Pt(), i);
    output->SetValue(fEtaLabel, vec->Eta(), i);
    output->SetValue(fPhiLabel, vec->Phi(), i);
    output->SetValue(fMLabel, vec->M(), i);
    output->SetValue(fELabel, vec->E(), i);
    output->SetValue(fIDLabel, particle->GetID(), i);
    output->SetValue(fChargeLabel, particle->GetCharge(), i);
    return;
  }
  if (fAttributes) {
    for (std::map<TString, TString>::iterator it = fAttributeLabels.begin();
         it != fAttributeLabels.end(); ++it) {
      output->SetTreeForBranch(fTreeName, it->second);
      output->SetValue(it->second, particle->GetAttribute(it->first), i);
    }
    return;
  }
  throw HAL::HALException(GetName().Prepend("Couldn't determine what to store: "));
}

} /* HAL */ 
