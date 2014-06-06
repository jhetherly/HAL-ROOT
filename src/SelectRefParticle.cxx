#include <HAL/Algorithms/SelectRefParticle.h>

namespace HAL
{

/*
 * Generic class
 * */
void internal::FilterRefParticleAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::GenericData *gen_data = new GenericData(GetName());
  HAL::GenericData *input_data = NULL;
  HAL::GenericData *others_data = NULL;
  HAL::ParticlePtr  reference;

  data->SetValue(GetName(), gen_data);

  if (data->Exists(fInput) && data->Exists(fOthers)) {
    input_data = (GenericData*)data->GetTObject(fInput);
    others_data = (GenericData*)data->GetTObject(fOthers);
  }
  else
    return;

  for (ParticlePtrsIt particle = others_data->GetParticleBegin(); 
       particle != others_data->GetParticleEnd(); ++ particle) {
    bool add_particle = true;
    for (ParticlePtrsIt ref_particle = input_data->GetParticleBegin(); 
         ref_particle != input_data->GetParticleEnd(); ++ ref_particle) {
      reference = *ref_particle;
      if (reference == *particle || reference->HasSameParticles("parents", *particle))
        continue;
      if (!FilterPredicate(reference, *particle)) {
        add_particle = false;
        break;
      }
    }
    if (add_particle)
      gen_data->AddParticle(*particle);
  }

  IncreaseCounter(gen_data->GetNParticles());
}

void internal::FilterRefParticleAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}








/*
 * Actual classes
 * */

Algorithms::SelectRefParticle::SelectRefParticle (TString name, TString title, TString input, TString others, 
    double value, TString type, TString inclusion) : 
  FilterRefParticleAlgo(name, title, input, others), fIn(false), fOut(false), 
  fWindow(false), fDeltaR(false), fDeltaPhi(false) {

  if (inclusion.EqualTo("in", TString::kIgnoreCase) || 
      inclusion.EqualTo("inclusive", TString::kIgnoreCase)) {
    fIn = true;
    fHighLimit = value;
  }
  else if (inclusion.EqualTo("out", TString::kIgnoreCase) || 
           inclusion.EqualTo("exclusive", TString::kIgnoreCase)) {
    fOut = true;
    fLowLimit = value;
  }

  if (type.EqualTo("dr", TString::kIgnoreCase)) 
    fDeltaR = true;
  else if (type.EqualTo("dphi", TString::kIgnoreCase)) 
    fDeltaPhi = true;
}

Algorithms::SelectRefParticle::SelectRefParticle (TString name, TString title, TString input, TString others, 
    double low, double high, TString type, TString inclusion) :
  FilterRefParticleAlgo(name, title, input, others), fHighLimit(high), fLowLimit(low), fIn(false), fOut(false), 
  fWindow(true), fDeltaR(false), fDeltaPhi(false) {

  if (inclusion.EqualTo("in", TString::kIgnoreCase) || 
      inclusion.EqualTo("inclusive", TString::kIgnoreCase))
    fIn = true;
  else if (inclusion.EqualTo("out", TString::kIgnoreCase) || 
           inclusion.EqualTo("exclusive", TString::kIgnoreCase))
    fOut = true;

  if (type.EqualTo("dr", TString::kIgnoreCase)) 
    fDeltaR = true;
  else if (type.EqualTo("dphi", TString::kIgnoreCase)) 
    fDeltaPhi = true;
}

bool Algorithms::SelectRefParticle::FilterPredicate (HAL::ParticlePtr p_ref, HAL::ParticlePtr particle) {
  TLorentzVector *ref = p_ref->GetP();
  TLorentzVector *vec = particle->GetP();
  double val = 0.0;

  if (fDeltaR)
    val = ref->DeltaR(*vec);
  else if (fDeltaPhi)
    val = ref->DeltaPhi(*vec);

  if (fWindow) {
    if (fIn)
      return (val >= fLowLimit && val <= fHighLimit);
    else if (fOut)
      return (val <= fLowLimit && val >= fHighLimit);
  }
  else if (fIn)
    return (val <= fHighLimit);
  else if (fOut)
    return (val >= fLowLimit);
  throw HAL::HALException(GetName().Prepend("Couldn't determine how to filter: "));
}

} /* HAL */ 
