#include <HAL/Algorithms/VecAddReco.h>

namespace HAL
{

/*
 * Reconstruction Algorithms
 * */

Algorithms::VecAddReco::VecAddReco (TString name, TString title, long long length, ...) :
    Algorithm(name, title), fLength(length) {
  fParentNames = new const char*[fLength];
  va_list arguments;  // store the variable list of arguments

  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < fLength; ++i)
    fParentNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

Algorithms::VecAddReco::~VecAddReco() {
  delete[] fParentNames;
}

void Algorithms::VecAddReco::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::GenericData *gen_data = new GenericData(GetName(), true);
  HAL::GenericData *input_data = NULL;
  std::set<std::set<ParticlePtr> > UniqueTuples;

  data->SetValue(GetName(), gen_data);

  // Find unique sets of tuples 
  // (relies on unique particles having unique addresses)
  for (long long i = 0; i < fLength; ++i) {
    if (data->Exists(fParentNames[i]))
      input_data = (GenericData*)data->GetTObject(fParentNames[i]);
    else
      return;

    std::set<std::set<ParticlePtr> > NewTuples;
    if (UniqueTuples.size() == 0) {
      // Loop over current set of particles and add either the particle 
      // or its parents to the ntuple
      for (ParticlePtrsIt particle = input_data->GetParticleBegin(); 
           particle != input_data->GetParticleEnd(); ++ particle) {

        bool                  unique = true;
        std::set<ParticlePtr> new_tuple;

        if ((*particle)->GetNParticles("parents") > 0) {
          ParticlePtrs parents = (*particle)->GetParticles("parents");
          for (ParticlePtrsIt parent = parents.begin(); 
              parent != parents.end(); ++ parent) {
            if (new_tuple.insert(*parent).second == false) {
              unique = false;
              break;
            }
          }
        }
        else {
          if (new_tuple.insert(*particle).second == false)
            continue;
        }

        if (unique)
          NewTuples.insert(new_tuple);
      }
    }
    else {
      for (std::set<std::set<ParticlePtr> >::iterator current_tuple = UniqueTuples.begin();
           current_tuple != UniqueTuples.end(); ++current_tuple) {

        // Loop over current set of particles and add either the particle 
        // or its parents to the ntuple
        for (ParticlePtrsIt particle = input_data->GetParticleBegin(); 
             particle != input_data->GetParticleEnd(); ++ particle) {

          bool                  unique = true;
          std::set<ParticlePtr> new_tuple = *current_tuple;

          if ((*particle)->GetNParticles("parents") > 0) {
            ParticlePtrs parents = (*particle)->GetParticles("parents");
            for (ParticlePtrsIt parent = parents.begin(); 
                 parent != parents.end(); ++ parent) {
              if (new_tuple.insert(*parent).second == false) {
                unique = false;
                break;
              }
            }
          }
          else {
            if (new_tuple.insert(*particle).second == false)
              continue;
          }

          if (unique)
            NewTuples.insert(new_tuple);
        }
      }
    }

    UniqueTuples = NewTuples;
  }

  // Loop over UniqueTuples to make vectors
  for (std::set<std::set<ParticlePtr> >::iterator current_tuple = UniqueTuples.begin();
       current_tuple != UniqueTuples.end(); ++current_tuple) {
    float new_charge = 0.0;
    HAL::ParticlePtr new_particle = new HAL::Particle(GetName());
    HAL::ParticlePtrs new_parents;
    TLorentzVector *vec = new TLorentzVector();
    for (std::set<ParticlePtr>::iterator particle = current_tuple->begin();
         particle != current_tuple->end(); ++particle) {
      new_charge += (*particle)->GetCharge();
      vec->operator+=(*((*particle)->GetP()));
      new_parents.push_back(*particle);
    }
    new_particle->SetCharge(new_charge);
    new_particle->SetP(vec);
    new_particle->SetParticles("parents", new_parents);
    gen_data->AddParticle(new_particle);
    new_particle->SetOwnerIndex(gen_data->GetNParticles() - 1);
    new_particle->SetOriginIndex(gen_data->GetNParticles() - 1);
  }
  gen_data->SetRefType("none");
}

void Algorithms::VecAddReco::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}

} /* HAL */ 
