#include <HAL/Algorithms.h>

ClassImp(HAL::GenericParticle);
ClassImp(HAL::GenericData);

namespace HAL
{

/*
 * Generic algorithm data containers
 * */
HAL::GenericParticle::GenericParticle (const TString &origin, const TString &name) : 
  fOrigin(origin), fP(NULL) {

  if(!name.EqualTo("")) 
    SetName(name.Data());
}

HAL::GenericParticle::GenericParticle (const GenericParticle &particle) {
  fID = particle.fID;
  fCharge = particle.fCharge;
  fP = new TLorentzVector(*particle.fP);
  fScalarAttributes = particle.fScalarAttributes;
  f1DParticles = particle.f1DParticles;
}

HAL::GenericParticle::~GenericParticle () {
  if (fP) delete fP;
}

void HAL::GenericParticle::SetAttribute (const TString &name, const long double &value) {
  fScalarAttributes[name] = value;
}

void HAL::GenericParticle::SetParticle (const TString &name, 
                                        GenericParticle *particle, 
                                        const long long &index) {
  if (index == -1)
    f1DParticles[name].push_back(particle);
  else
    f1DParticles[name][index] = particle;
  std::stable_sort(f1DParticles[name].begin(), f1DParticles[name].end());
}

void HAL::GenericParticle::Set1DParticle (const TString &name, std::vector<GenericParticle*> &particles) {
  f1DParticles[name] = particles;
  std::stable_sort(f1DParticles[name].begin(), f1DParticles[name].end());
}

bool HAL::GenericParticle::HasSameParticles (const TString &name, ParticlePtr particle) {
  size_t n = GetNParticles(name);
  size_t m = particle->GetNParticles(name);

  if (n == m) {
    if (n != 0)
      return (f1DParticles[name] == particle->GetParticles(name));
    else
      return true;
  }
  return false;
}

std::ostream& operator<<(std::ostream& os, const HAL::GenericParticle &particle) {
  os << particle.fOrigin << std::endl;
  if (particle.fP != NULL) {
    os << "4-vector properties:\tpT\teta\tphi\tmass\tenergy\n";
    os << "                    " << particle.fP->Pt() << "\t" << particle.fP->Eta() << "\t" << particle.fP->Phi()
       << "\t" << particle.fP->M() << "\t" << particle.fP->E() << std::endl;
  }
  // TODO: loop over attributes and particles
  //       print out the ID and charge
  return os;
}

HAL::GenericData::GenericData (const TString &name, bool is_owner) : fIsOwner(is_owner) {
  fParticles.reserve(20);
  SetName(name.Data());
}

HAL::GenericData::GenericData (const GenericData &data) {
  fUserDataRefName = data.fUserDataRefName; 
  fParticles.reserve(20);
  for (ParticlePtrsConstIt particle = data.fParticles.begin(); 
       particle != data.fParticles.end(); ++particle) {
    fParticles.push_back(new Particle(*(*particle)));
  }
  if (fParticles.size() > 0)
    fIsOwner = true;
  f1DParticles = data.f1DParticles;
}

HAL::GenericData::~GenericData () {
  if (fIsOwner) {
    for (ParticlePtrsIt particle = GetParticleBegin(); particle != GetParticleEnd(); ++particle) 
      delete (*particle);
  }
}

std::ostream& operator<<(std::ostream& os, HAL::GenericData &data) {
  // TODO: print out other members
  for (HAL::ParticlePtrsIt particle = data.GetParticleBegin();
       particle != data.GetParticleEnd(); ++particle) {
    os << **particle << std::endl;
  }
  return os;
}

/*
 * Generic classes
 * */
internal::ImportTLVAlgo::ImportTLVAlgo (TString name, TString title) : 
  HAL::Algorithm(name, title) {
}

void internal::ImportTLVAlgo::Exec (unsigned n) {
  HAL::AnalysisData *data = GetUserData();
  HAL::GenericData *gen_data = new GenericData(GetName(), true);

  data->SetValue(GetName(), gen_data);

  for (unsigned i = 0; i < n; ++i) {
    HAL::ParticlePtr particle = new HAL::Particle(GetName());
    TLorentzVector *vec = MakeTLV(i);

    particle->SetP (vec);
    gen_data->AddParticle(particle);
  }

  IncreaseCounter(gen_data->GetNParticles());
}

void internal::ImportTLVAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}

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
  else
    original_data = (GenericData*)data->GetTObject(input_data->GetOwner());

  n = input_data->GetNParticles();
  norigin = original_data->GetNParticles();
  if (n < fN)
    return;

  // create sorted list for original data
  if (!original_data->HasParticles(SortTag())) {
    HAL::ParticlePtrs sorted_particles;

    for (unsigned i = 0; i < norigin; ++i) {
      sorted_particles.push_back(original_data->GetParticle(i));
    }
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

void internal::FilterParticleAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::GenericData *gen_data = new GenericData(GetName());
  HAL::GenericData *input_data = NULL;

  data->SetValue(GetName(), gen_data);

  if (data->Exists(fInput)) 
    input_data = (GenericData*)data->GetTObject(fInput);
  else
    return;

  for (ParticlePtrsIt particle = input_data->GetParticleBegin(); 
       particle != input_data->GetParticleEnd(); ++ particle) {
    if (FilterPredicate(*particle))
      gen_data->AddParticle(*particle);
  }

  IncreaseCounter(gen_data->GetNParticles());
}

void internal::FilterParticleAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}

void internal::FilterRefTLVAlgo::Exec (Option_t* /*option*/) {
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

  if (input_data->GetNParticles() != 1)
    return;

  reference = input_data->GetParticle(0);
  for (ParticlePtrsIt particle = others_data->GetParticleBegin(); 
       particle != others_data->GetParticleEnd(); ++ particle) {
    if (reference == *particle || reference->HasSameParticles("parents", *particle))
      continue;
    if (FilterPredicate(reference, *particle))
      gen_data->AddParticle(*particle);
  }
}

void internal::FilterRefTLVAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}

internal::FilterParentAlgo::FilterParentAlgo (TString name, TString title, 
    TString input, TString logic, long long length, ...) :
  Algorithm(name, title), fExclude(logic.EqualTo("ex", TString::kIgnoreCase)), fInput(input) {

  fRefNames = new const char*[length];
  va_list arguments;  // store the variable list of arguments

  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < length; ++i)
    fRefNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

void internal::FilterParentAlgo::Exec (Option_t* /*option*/) {
}

void internal::FilterParentAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}

void internal::ParticlesTLVStore::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeWriter *output = GetUserOutput();
  HAL::GenericData *input_data = NULL;

  if (data->Exists(fInput))
    input_data = (GenericData*)data->GetTObject(fInput);
  else
    return;

  if (input_data->GetNParticles() == 0)
    return;

  if (fMany) {
    long long i = 0;
    for (ParticlePtrsIt particle = input_data->GetParticleBegin(); 
        particle != input_data->GetParticleEnd(); ++ particle) {
      output->SetValue(fBranchName, StoreValue(*particle), i++);
    }
  }
  else
    output->SetValue(fBranchName, StoreValue(input_data->GetParticle(0)));
}








/*
 * Actual classes
 * */

/*
 * Importing Algorithms
 * */

Algorithms::ImportTLV::ImportTLV (TString name, TString title, unsigned n) : 
  ImportTLVAlgo(name, title), fN(n), fIsCart(false), fIsE(false), fIsM(false) {}

void Algorithms::ImportTLV::Init (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = GetRawData();
  TString CartEntriesName = TString::Format("%s:x1", GetName().Data());
  TString MassEntriesName = TString::Format("%s:m", GetName().Data());
  TString EnergyEntriesName = TString::Format("%s:e", GetName().Data());

  if (tr->CheckBranchMapNickname(CartEntriesName))
    fIsCart = true;
  else if (tr->CheckBranchMapNickname(EnergyEntriesName))
    fIsE = true;
  else if (tr->CheckBranchMapNickname(MassEntriesName))
    fIsM = true;
  if (!fIsCart && !fIsE && !fIsM)
    HAL::HALException(GetName().Prepend("Couldn't determine how to import data: ").Data());

  fCartX0 = TString::Format("%s:x0", GetName().Data());
  fCartX1 = TString::Format("%s:x1", GetName().Data());
  fCartX2 = TString::Format("%s:x2", GetName().Data());
  fCartX3 = TString::Format("%s:x3", GetName().Data());
  fPt = TString::Format("%s:pt", GetName().Data());
  fEta = TString::Format("%s:eta", GetName().Data());
  fPhi = TString::Format("%s:phi", GetName().Data());
  fM = TString::Format("%s:m", GetName().Data());
  fE = TString::Format("%s:e", GetName().Data());
  fNEntriesName = TString::Format("%s:nentries", GetName().Data());
  fCartEntriesName = TString::Format("%s:x1", GetName().Data());
  fPtEntriesName = TString::Format("%s:pt", GetName().Data());
}

void Algorithms::ImportTLV::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = GetRawData();
  long long n = fN;

  // determine number of elements to read in
  if (n == 0) {
    if (tr->CheckBranchMapNickname(fNEntriesName))
      n = tr->GetInteger(fNEntriesName);
    else if (fIsCart)
      n = tr->GetDim(fCartEntriesName);
    else if (fIsE || fIsM)
      n = tr->GetDim(fPtEntriesName);
  }
  // call actual Exec algo
  ImportTLVAlgo::Exec(n);
}

TLorentzVector* Algorithms::ImportTLV::MakeTLV (unsigned i) {
  HAL::AnalysisTreeReader *tr = GetRawData();

  if (fIsCart) {
    long double x0 = tr->GetDecimal(fCartX0, i),
                x1 = tr->GetDecimal(fCartX1, i),
                x2 = tr->GetDecimal(fCartX2, i),
                x3 = tr->GetDecimal(fCartX3, i);
    return new TLorentzVector(x1, x2, x3, x0);
  }
  else if (fIsE) {
    long double e = tr->GetDecimal(fE, i),
                pT = tr->GetDecimal(fPt, i),
                eta = tr->GetDecimal(fEta, i),
                phi = tr->GetDecimal(fPhi, i);
    return HAL::makeTLVFromPtEtaPhiE(pT, eta, phi, e);
  }
  else if (fIsM) {
    long double m = tr->GetDecimal(fM, i),
                pT = tr->GetDecimal(fPt, i),
                eta = tr->GetDecimal(fEta, i),
                phi = tr->GetDecimal(fPhi, i);
    return HAL::makeTLVFromPtEtaPhiM(pT, eta, phi, m);
  }
  throw HAL::HALException("Couldn't identify type in ImportTLV");
}

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
  HAL::GenericData *gen_data = new GenericData(GetName());
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
    int new_charge = 0;
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
    new_particle->Set1DParticle("parents", new_parents);
    gen_data->AddParticle(new_particle);
  }
}

void Algorithms::VecAddReco::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}

Algorithms::ParticleRankSelection::ParticleRankSelection (TString name, TString title, 
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

TString Algorithms::ParticleRankSelection::SortTag () {
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

bool Algorithms::ParticleRankSelection::operator() (ParticlePtr lhs, ParticlePtr rhs) {
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

void Algorithms::ParticleRankSelection::Sort (ParticlePtrs &sl) {
  std::stable_sort(sl.begin(), sl.end(), *this);
}

Algorithms::SelectParticle::SelectParticle (TString name, TString title, TString input, TString property, 
    double value, TString end) : 
  FilterParticleAlgo(name, title, input), 
  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
  fPhi(false), fHigh(false), fLow(false), fWindow(false),
  fTLVProperty(property), fEnd(end) {

  if (end.EqualTo("low", TString::kIgnoreCase)) {
    fLow = true;
    fLowLimit = value;
  }
  else if (end.EqualTo("high", TString::kIgnoreCase)) {
    fHigh = true;
    fHighLimit = value;
  }
  Setup();
}

Algorithms::SelectParticle::SelectParticle (TString name, TString title, TString input, TString property, 
    double low, double high) : 
  FilterParticleAlgo(name, title, input), 
  fHighLimit(high), fLowLimit(low), 
  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
  fPhi(false), fHigh(false), fLow(false), fWindow(true),
  fTLVProperty(property), fEnd("window") {
  Setup();
}

void Algorithms::SelectParticle::Setup () {
  if (fTLVProperty.EqualTo("pt", TString::kIgnoreCase))
    fPt = true;
  if (fTLVProperty.EqualTo("m", TString::kIgnoreCase))
    fM = true;
  if (fTLVProperty.EqualTo("e", TString::kIgnoreCase))
    fE = true;
  if (fTLVProperty.EqualTo("et", TString::kIgnoreCase))
    fEt = true;
  if (fTLVProperty.EqualTo("p3", TString::kIgnoreCase))
    fP3 = true;
  if (fTLVProperty.EqualTo("eta", TString::kIgnoreCase))
    fEta = true;
  if (fTLVProperty.EqualTo("phi", TString::kIgnoreCase))
    fPhi = true;
}

bool Algorithms::SelectParticle::FilterPredicate(ParticlePtr particle) {
  TLorentzVector *vec = particle->GetP();
  if (!fWindow) {
    if (fLow) {
      if (fPt)
        return (vec->Pt() >= fLowLimit);
      else if (fM)
        return (vec->M() >= fLowLimit);
      else if (fE)
        return (vec->E() >= fLowLimit);
      else if (fEt)
        return (vec->Et() >= fLowLimit);
      else if (fP3)
        return (vec->P() >= fLowLimit);
      else if (fEta)
        return (vec->Eta() >= fLowLimit);
      else if (fPhi)
        return (vec->Phi() >= fLowLimit);
    }
    else if (fHigh) {
      if (fPt)
        return (vec->Pt() <= fHighLimit);
      else if (fM)
        return (vec->M() <= fHighLimit);
      else if (fE)
        return (vec->E() <= fHighLimit);
      else if (fEt)
        return (vec->Et() <= fHighLimit);
      else if (fP3)
        return (vec->P() <= fHighLimit);
      else if (fEta)
        return (vec->Eta() <= fHighLimit);
      else if (fPhi)
        return (vec->Phi() <= fHighLimit);
    }
  }
  else { // window cut
    if (fPt)
      return (vec->Pt() <= fHighLimit && vec->Pt() >= fLowLimit);
    else if (fM)
      return (vec->M() <= fHighLimit && vec->M() >= fLowLimit);
    else if (fE)
      return (vec->E() <= fHighLimit && vec->E() >= fLowLimit);
    else if (fEt)
      return (vec->Et() <= fHighLimit && vec->Et() >= fLowLimit);
    else if (fP3)
      return (vec->P() <= fHighLimit && vec->P() >= fLowLimit);
    else if (fEta)
      return (vec->Eta() <= fHighLimit && vec->Eta() >= fLowLimit);
    else if (fPhi)
      return (vec->Phi() <= fHighLimit && vec->Phi() >= fLowLimit);
  }
  throw HAL::HALException(GetName().Prepend("Couldn't determine how to filter: "));
}

Algorithms::SelectDeltaTLV::SelectDeltaTLV (TString name, TString title, TString input, TString others, 
    double value, TString topo, TString type) : 
  FilterRefTLVAlgo(name, title, input, others), fIn(false), fOut(false), 
  fWindow(false), fDeltaR(false), fDeltaPhi(false) {

  if (topo.EqualTo("in", TString::kIgnoreCase)) {
    fIn = true;
    fHighLimit = value;
  }
  else if (topo.EqualTo("out", TString::kIgnoreCase)) {
    fOut = true;
    fLowLimit = value;
  }
  if (type.EqualTo("r", TString::kIgnoreCase)) 
    fDeltaR = true;
  else if (type.EqualTo("phi", TString::kIgnoreCase)) 
    fDeltaPhi = true;
}

Algorithms::SelectDeltaTLV::SelectDeltaTLV (TString name, TString title, TString input, TString others, 
    double low, double high, TString type) :
  FilterRefTLVAlgo(name, title, input, others), fHighLimit(high), fLowLimit(low), fIn(false), fOut(false), 
  fWindow(true), fDeltaR(false), fDeltaPhi(false) {

  if (type.EqualTo("r", TString::kIgnoreCase)) 
    fDeltaR = true;
  else if (type.EqualTo("phi", TString::kIgnoreCase)) 
    fDeltaPhi = true;
}

bool Algorithms::SelectDeltaTLV::FilterPredicate (HAL::ParticlePtr p_ref, HAL::ParticlePtr particle) {
  TLorentzVector *ref = p_ref->GetP();
  TLorentzVector *vec = particle->GetP();
  if (fDeltaR) {
    double dR = ref->DeltaR(*vec);
    if (fIn) {
      if (dR <= fHighLimit)
        return true;
    }
    else if (fOut) {
      if (dR >= fLowLimit)
        return true;
    }
    else if (fWindow) {
      if (dR >= fLowLimit && dR <= fHighLimit)
        return true;
    }
  }
  else if (fDeltaPhi) {
    double dPhi = ref->DeltaPhi(*vec);
    if (fIn) {
      if (dPhi <= fHighLimit)
        return true;
    }
    else if (fOut) {
      if (dPhi >= fLowLimit)
        return true;
    }
    else if (fWindow) {
      if (dPhi >= fLowLimit && dPhi <= fHighLimit)
        return true;
    }
  }
  return false;
}

/*
 * Cutting Algorithms
 * */

Algorithms::CutNObjects::CutNObjects (TString name, TString title, TString logic, 
    long long n, long long length, ...) :
  CutAlgorithm(name, title), fAnd(false), fOr(false), fLength(length), fN(n) {
  fParticleNames = new const char*[fLength];
  va_list arguments;  // store the variable list of arguments

  if (logic.EqualTo("and", TString::kIgnoreCase))
    fAnd = true;
  else if (logic.EqualTo("or", TString::kIgnoreCase))
    fOr = true;
  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < fLength; ++i)
    fParticleNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

void Algorithms::CutNObjects::Exec (Option_t* /*option*/) {
  AnalysisData *data = GetUserData();
  HAL::GenericData *input_data = NULL;

  if (fAnd) {
    for (long long i = 0; i < fLength; ++i) {
      if (!data->Exists(fParticleNames[i])) {
        Abort();
        return;
      }
      input_data = (GenericData*)data->GetTObject(fParticleNames[i]);
      if (input_data->GetNParticles() < (size_t)fN) {
        Abort();
        return;
      }
    }

    Passed();
  }
  else if (fOr) {
    for (long long i = 0; i < fLength; ++i) {
      if (data->Exists(fParticleNames[i])) {
        input_data = (GenericData*)data->GetTObject(fParticleNames[i]);
        if (input_data->GetNParticles() >= (size_t)fN) {
          Passed();
          return;
        }
      }
    }

    Abort();
  }
}

/*
 * Monitoring Algorithms
 * */

void Algorithms::MonitorAlgorithm::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::GenericData *input_data = NULL;

  (*fOS) << "Algorithm: " << fInput.Data() << std::endl;

  if (data->Exists(fInput))
    input_data = (GenericData*)data->GetTObject(fInput);
  else
    return;

  (*fOS) << *input_data << std::endl;
}

void Algorithms::MonitorUserData::Exec (Option_t* /*option*/) {
  //HAL::AnalysisData *data = GetUserData();

  (*fOS) << "UserData has the following elements:" << std::endl;
}

/*
 * Exporting Algorithms
 * */

Algorithms::StoreTLV::StoreTLV (TString name, TString title, TString input, 
    TString property, TString bname, TString num) :
  ParticlesTLVStore(name, title, input, bname, num) {

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
}

double Algorithms::StoreTLV::StoreValue (HAL::ParticlePtr particle) {
  TLorentzVector *vec = particle->GetP();

  if (fPt)
    return vec->Pt();
  if (fM)
    return vec->M();
  if (fE)
    return vec->E();
  if (fEt)
    return vec->Et();
  if (fP3)
    return vec->P();
  if (fEta)
    return vec->Eta();
  if (fPhi)
    return vec->Phi();
  throw HAL::HALException(GetName().Prepend("Couldn't determine what to store: "));
}

} /* HAL */ 
