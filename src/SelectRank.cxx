#include <HAL/Algorithms/SelectRank.h>

namespace HAL
{

/*
 * Generic class
 * */
void internal::NthElementAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::GenericData *gen_data = new GenericData(GetName());
  HAL::GenericData *input_data = NULL;
  HAL::GenericData *original_data = NULL;
  long long n, norigin;

  data->SetValue(GetName(), gen_data);

  if (data->Exists(fInput)) 
    input_data = (GenericData*)data->GetTObject(fInput);
  else
    return;

  // Need to look for actual owner for sorting purposes
  if (input_data->IsOwner())
    original_data = input_data;
  else if (input_data->GetNParticles() > 0)
    original_data = (GenericData*)data->GetTObject(input_data->GetOwner());
  else return;

  n = input_data->GetNParticles();
  norigin = original_data->GetNParticles();
  if (n < fN)
    return;

  // Create sorted list for original data
  if (!original_data->HasParticles(SortTag())) {
    HAL::ParticlePtrs sorted_particles;

    for (unsigned i = 0; i < norigin; ++i) 
      sorted_particles.push_back(original_data->GetParticle(i));
    Sort(sorted_particles);
    original_data->SetParticles(SortTag(), sorted_particles);
  }

  // loop over the sorted list and find the fN-th ranked member in fInput
  ParticlePtr result = NULL;
  ParticlePtrs sorted_list = original_data->GetParticles(SortTag());
  long long count = 0;
  for (ParticlePtrsIt particle = sorted_list.begin(); particle != sorted_list.end(); ++ particle) {
    for (ParticlePtrsIt my_particle = input_data->GetParticleBegin(); 
         my_particle != input_data->GetParticleEnd(); ++ my_particle) {
      if (*particle == *my_particle) {
        if (++count == fN) {
          result = *particle;
          break;
        }
      }
    }
    if (result != NULL)
      break;
  }

  gen_data->AddParticle(result);
  IncreaseCounter(gen_data->GetNParticles());
}

void internal::NthElementAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}





/*
 * Actual class
 * */

Algorithms::SelectRank::SelectRank (TString name, TString title, 
    TString input, unsigned rank, TString property, TString end) : 
  NthElementAlgo(name, title, input, rank), fPt(false), fM(false), fE(false),
    fEt(false), fP3(false), fHigh(false), fLow(false),
    fTLVProperty(property), fEnd(end) {
  if (fTLVProperty.EqualTo("pt", TString::kIgnoreCase))
    fPt = true;
  else if (fTLVProperty.EqualTo("m", TString::kIgnoreCase))
    fM = true;
  else if (fTLVProperty.EqualTo("e", TString::kIgnoreCase))
    fE = true;
  else if (fTLVProperty.EqualTo("et", TString::kIgnoreCase))
    fEt = true;
  else if (fTLVProperty.EqualTo("p3", TString::kIgnoreCase))
    fP3 = true;

  if (fEnd.EqualTo("high", TString::kIgnoreCase))
    fHigh = true;
  else if (fEnd.EqualTo("low", TString::kIgnoreCase))
    fLow = true;
}

TString Algorithms::SelectRank::SortTag () {
  if (fPt)
    return "4v_pt";
  if (fM)
    return "4v_m";
  if (fE)
    return "4v_e";
  if (fEt)
    return "4v_et";
  if (fP3)
    return "4v_p3";
  throw HALException(GetName().Prepend("Couldn't determine sorting type: "));
}

bool Algorithms::SelectRank::operator() (ParticlePtr lhs, ParticlePtr rhs) {
  TLorentzVector *lhs_vec = lhs->GetP();
  TLorentzVector *rhs_vec = rhs->GetP();

  if (fHigh) {
    if (fPt)
      return (lhs_vec->Pt() > rhs_vec->Pt());
    if (fM)
      return (lhs_vec->M() > rhs_vec->M());
    if (fE)
      return (lhs_vec->E() > rhs_vec->E());
    if (fEt)
      return (lhs_vec->Et() > rhs_vec->Et());
    if (fP3)
      return (lhs_vec->P() > rhs_vec->P());
  }
  else if (fLow) {
    if (fPt)
      return (lhs_vec->Pt() < rhs_vec->Pt());
    if (fM)
      return (lhs_vec->M() < rhs_vec->M());
    if (fE)
      return (lhs_vec->E() < rhs_vec->E());
    if (fEt)
      return (lhs_vec->Et() < rhs_vec->Et());
    if (fP3)
      return (lhs_vec->P() < rhs_vec->P());
  }
  throw HALException(GetName().Prepend("Couldn't determine sorting information: "));
}

void Algorithms::SelectRank::Sort (ParticlePtrs &sl) {
  std::stable_sort(sl.begin(), sl.end(), *this);
}

} /* HAL */ 
