#include <HAL/Algorithms/AttachAttribute.h>

namespace HAL
{

/*
 * Generic classes
 * */
internal::AugmentValueAlgo::AugmentValueAlgo (TString name, TString title, 
    TString input, TString attribute_name) : 
  HAL::Algorithm(name, title), fInput(input), fAttributeLabel(attribute_name) {
}

void  internal::AugmentValueAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = GetRawData();
  HAL::AnalysisData *data = GetUserData();
  HAL::GenericData *gen_data = new GenericData(GetName(), true);
  HAL::GenericData *input_data = NULL;

  data->SetValue(GetName(), gen_data);

  if (data->Exists(fInput)) 
    input_data = (GenericData*)data->GetTObject(fInput);
  else
    return;

  for (ParticlePtrsIt particle = input_data->GetParticleBegin(); 
       particle != input_data->GetParticleEnd(); ++ particle) {
    ParticlePtr p = new Particle(**particle);
    StoreValue(tr, p, p->GetOriginIndex()); // must be origin index as this is branch index
    gen_data->AddParticle(p);
    p->SetOwner(GetName());
    p->SetOwnerIndex(gen_data->GetNParticles() - 1);
  }
  IncreaseCounter(gen_data->GetNParticles());
}

void internal::AugmentValueAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}







/*
 * Actual classes
 * */

/*
 * Importing Algorithms
 * */

void Algorithms::AttachAttribute::StoreValue (AnalysisTreeReader *tr, 
                                               ParticlePtr particle, long long i) {
  if (fUserValue)
    particle->SetAttribute(fAttributeLabel, fValue);
  if (fBranchValue)
    particle->SetAttribute(fAttributeLabel, tr->GetDecimal(fBranchLabel, i));
  if (fPropertyValue) {
    if (fPtRank || fMRank || fERank || fEtRank || fP3Rank) {
      HAL::AnalysisData *data = GetUserData();
      HAL::GenericData *owner_data = NULL;
      HAL::GenericData *input_data = NULL;
      TString   sort_tag;
      long long norigin;

      if (fPtRank) sort_tag = "4v_pt";
      if (fMRank) sort_tag = "4v_m";
      if (fERank) sort_tag = "4v_e";
      if (fEtRank) sort_tag = "4v_et";
      if (fP3Rank) sort_tag = "4v_p3";

      // Need to look for actual owner for sorting purposes
      owner_data = (GenericData*)data->GetTObject(particle->GetOwner());

      norigin = owner_data->GetNParticles();

      // Create sorted list for owner data
      if (!owner_data->HasParticles(sort_tag)) {
        HAL::ParticlePtrs sorted_particles;

        for (unsigned j = 0; j < norigin; ++j) 
          sorted_particles.push_back(owner_data->GetParticle(j));
        Sort(sorted_particles);
        owner_data->SetParticles(sort_tag, sorted_particles);
      }

      // loop over the sorted list
      bool found = false;
      ParticlePtrs sorted_list = owner_data->GetParticles(sort_tag);
      long long origin_count = 0, input_count = 0;
      long long owner_index = particle->GetOwnerIndex();
      if (fRefCompare) input_data = (GenericData*)data->GetTObject(fRefParticles);
      else input_data = (GenericData*)data->GetTObject(fInput);
      for (ParticlePtrsIt sorted_particle = sorted_list.begin(); 
           sorted_particle != sorted_list.end(); ++ sorted_particle) {
        ++origin_count;
        for (ParticlePtrsIt my_particle = input_data->GetParticleBegin(); 
             my_particle != input_data->GetParticleEnd(); ++ my_particle) {
          if (*sorted_particle == *my_particle) {
            ++input_count;
            if (*sorted_particle == owner_data->GetParticle(owner_index)) {
              particle->SetAttribute(fAttributeLabel, input_count);
              found = true;
            }
            break;
          }
        }
        if (found) break;
      }
    }
  }
}

bool Algorithms::AttachAttribute::operator() (ParticlePtr lhs, ParticlePtr rhs) {
  TLorentzVector *lhs_vec = lhs->GetP();
  TLorentzVector *rhs_vec = rhs->GetP();

  if (fPtRank)
    return (lhs_vec->Pt() > rhs_vec->Pt());
  if (fMRank)
    return (lhs_vec->M() > rhs_vec->M());
  if (fERank)
    return (lhs_vec->E() > rhs_vec->E());
  if (fEtRank)
    return (lhs_vec->Et() > rhs_vec->Et());
  if (fP3Rank)
    return (lhs_vec->P() > rhs_vec->P());
  throw HALException(GetName().Prepend("Couldn't determine sorting information: "));
}

void Algorithms::AttachAttribute::Sort (ParticlePtrs &sl) {
  std::stable_sort(sl.begin(), sl.end(), *this);
}

} /* HAL */ 
