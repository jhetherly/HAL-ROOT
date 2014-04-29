#include <HAL/Algorithms.h>

namespace HAL
{

/*
 * Generic classes
 * */
internal::ImportParticleAlgo::ImportParticleAlgo (TString name, TString title) : 
  HAL::Algorithm(name, title), fIsCart(false), fIsE(false), fIsM(false), 
  fIsCartMET(false), fIsPhiEtMET(false), fHasCharge(false), fHasID(false) {

  fCartX0 = TString::Format("%s:x0", GetName().Data());
  fCartX1 = TString::Format("%s:x1", GetName().Data());
  fCartX2 = TString::Format("%s:x2", GetName().Data());
  fCartX3 = TString::Format("%s:x3", GetName().Data());
  fPt = TString::Format("%s:pt", GetName().Data());
  fEt = TString::Format("%s:et", GetName().Data());
  fEta = TString::Format("%s:eta", GetName().Data());
  fPhi = TString::Format("%s:phi", GetName().Data());
  fM = TString::Format("%s:m", GetName().Data());
  fE = TString::Format("%s:e", GetName().Data());
  fCharge = TString::Format("%s:charge", GetName().Data());
  fID = TString::Format("%s:id", GetName().Data());
  fNEntriesName = TString::Format("%s:nentries", GetName().Data());
}

void internal::ImportParticleAlgo::Init (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = GetRawData();

  if (tr->CheckBranchMapNickname(fCartX0) && tr->CheckBranchMapNickname(fCartX1) && 
      tr->CheckBranchMapNickname(fCartX2) && tr->CheckBranchMapNickname(fCartX3))
    fIsCart = true;
  else if (tr->CheckBranchMapNickname(fE) && tr->CheckBranchMapNickname(fPt) &&
           tr->CheckBranchMapNickname(fEta) && tr->CheckBranchMapNickname(fPhi))
    fIsE = true;
  else if (tr->CheckBranchMapNickname(fM) && tr->CheckBranchMapNickname(fPt) &&
           tr->CheckBranchMapNickname(fEta) && tr->CheckBranchMapNickname(fPhi))
    fIsM = true;
  else if (!tr->CheckBranchMapNickname(fCartX0) && tr->CheckBranchMapNickname(fCartX1) &&
           tr->CheckBranchMapNickname(fCartX2) && !tr->CheckBranchMapNickname(fCartX3))
    fIsCartMET = true;
  else if ((tr->CheckBranchMapNickname(fEt) || tr->CheckBranchMapNickname(fPt)) &&
           tr->CheckBranchMapNickname(fPhi) && !tr->CheckBranchMapNickname(fEta) &&
           !tr->CheckBranchMapNickname(fM) && !tr->CheckBranchMapNickname(fE))
    fIsPhiEtMET = true;
  if (!fIsCart && !fIsE && !fIsM && !fIsCartMET && !fIsPhiEtMET)
    HAL::HALException(GetName().Prepend("Couldn't determine how to import data: ").Data());

  if (tr->CheckBranchMapNickname(fCharge))
    fHasCharge = true;
  if (tr->CheckBranchMapNickname(fID))
    fHasID = true;
  // Use only one since these are synonyms
  if (tr->CheckBranchMapNickname(fEt))
    fPt = fEt;
}

void internal::ImportParticleAlgo::Exec (unsigned n) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeReader *tr = GetRawData();
  HAL::GenericData *gen_data = new GenericData(GetName(), true);

  data->SetValue(GetName(), gen_data);

  for (unsigned i = 0; i < n; ++i) {
    HAL::ParticlePtr particle = new HAL::Particle(GetName());
    TLorentzVector *vec = MakeTLV(i);

    particle->SetP (vec);
    if (fHasCharge) particle->SetCharge(tr->GetInteger(fCharge, i));
    if (fHasID) particle->SetID(tr->GetInteger(fID, i));
    gen_data->AddParticle(particle);
    particle->SetOriginIndex(gen_data->GetNParticles() - 1);
  }

  gen_data->SetRefType("none");
  IncreaseCounter(gen_data->GetNParticles());
}

void internal::ImportParticleAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}

internal::ImportValueAlgo::ImportValueAlgo (TString name, TString title) : 
  HAL::Algorithm(name, title) {

  fUserDataLabel = TString::Format("%s:value", name.Data());
}

void  internal::ImportValueAlgo::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::GenericData *gen_data = new GenericData(GetName());

  data->SetValue(GetName(), gen_data);

  StoreValue(gen_data);
}

void  internal::ImportValueAlgo::Clear (Option_t* /*option*/) {
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
        particle != input_data->GetParticleEnd(); ++ particle)
      //output->SetValue(fBranchName, StoreValue(*particle), i++);
      StoreValue(output, i++, (*particle));
  }
  else
    //output->SetValue(fBranchName, StoreValue(input_data->GetParticle(0)));
    StoreValue(output, 0, input_data->GetParticle(0));
}








/*
 * Actual classes
 * */

/*
 * Importing Algorithms
 * */

Algorithms::ImportParticle::ImportParticle (TString name, TString title, unsigned n) : 
  ImportParticleAlgo(name, title), fN(n) {
}

void Algorithms::ImportParticle::Exec (Option_t* /*option*/) {
  HAL::AnalysisTreeReader *tr = GetRawData();
  long long n = fN;

  // determine number of elements to read in
  if (n == 0) {
    if (tr->CheckBranchMapNickname(fNEntriesName)) {
      n = tr->GetInteger(fNEntriesName);
    }
    else if (fIsCart || fIsCartMET) {
      if (tr->GetRank(fCartX1) == 1)
        n = tr->GetDim(fCartX1);
      else if (tr->GetRank(fCartX1) == 0)
        n = 1;
    }
    else if (fIsE || fIsM || fIsPhiEtMET) {
      if (tr->GetRank(fPhi) == 1)
        n = tr->GetDim(fPhi);
      else if (tr->GetRank(fPhi) == 0)
        n = 1;
    }
  }
  // call actual Exec algo
  ImportParticleAlgo::Exec(n);
}

TLorentzVector* Algorithms::ImportParticle::MakeTLV (unsigned i) {
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
  else if (fIsCartMET) {
    long double x1 = tr->GetDecimal(fCartX1, i),
                x2 = tr->GetDecimal(fCartX2, i);
    return new TLorentzVector(x1, x2, 0.0, TMath::Sqrt(x1*x1 + x2*x2));
  }
  else if (fIsPhiEtMET) {
    long double phi = tr->GetDecimal(fPhi, i),
                pt = tr->GetDecimal(fPt, i);
    return new TLorentzVector(pt*TMath::Cos(phi), pt*TMath::Sin(phi), 0.0, pt);
  }
  throw HAL::HALException("Couldn't identify type in ImportTLV");
}

Algorithms::ImportBool::ImportBool (TString name, TString title) : 
  ImportValueAlgo(name, title) {

  fValue = TString::Format("%s:bool", name.Data());
}

void Algorithms::ImportBool::StoreValue (HAL::GenericData *gen_data) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeReader *tr = GetRawData();

  data->SetValue(fUserDataLabel, tr->GetBool(fValue));
  gen_data->SetRefName(fUserDataLabel);
  gen_data->SetRefType("bool");
}

Algorithms::ImportInteger::ImportInteger (TString name, TString title) : 
  ImportValueAlgo(name, title) {

  fValue = TString::Format("%s:integer", name.Data());
}

void Algorithms::ImportInteger::StoreValue (HAL::GenericData *gen_data) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeReader *tr = GetRawData();

  data->SetValue(fUserDataLabel, tr->GetInteger(fValue));
  gen_data->SetRefName(fUserDataLabel);
  gen_data->SetRefType("integer");
}

Algorithms::ImportCounting::ImportCounting (TString name, TString title) : 
  ImportValueAlgo(name, title) {

  fValue = TString::Format("%s:counting", name.Data());
}

void Algorithms::ImportCounting::StoreValue (HAL::GenericData *gen_data) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeReader *tr = GetRawData();

  data->SetValue(fUserDataLabel, tr->GetCounting(fValue));
  gen_data->SetRefName(fUserDataLabel);
  gen_data->SetRefType("counting");
}

Algorithms::ImportDecimal::ImportDecimal (TString name, TString title) : 
  ImportValueAlgo(name, title) {

  fValue = TString::Format("%s:decimal", name.Data());
}

void Algorithms::ImportDecimal::StoreValue (HAL::GenericData *gen_data) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeReader *tr = GetRawData();

  data->SetValue(fUserDataLabel, tr->GetDecimal(fValue));
  gen_data->SetRefName(fUserDataLabel);
  gen_data->SetRefType("decimal");
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
    new_particle->SetOriginIndex(gen_data->GetNParticles() - 1);
  }
  gen_data->SetRefType("none");
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

Algorithms::SelectParticle::SelectParticle (TString name, TString title, TString input, 
    TString property, TString op, double value) : 
  FilterParticleAlgo(name, title, input), 
  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
  fPhi(false), fCharge(false), fID(false), fEqual(false), fNotEqual(false), 
  fLessThan(false), fGreaterThan(false), fLessThanEqual(false), 
  fGreaterThanEqual(false), fIn(false), fOut(false), fSingleEnd(true), fWindow(false), 
  fList(false), fProperty(property) {

  Setup();

  if (op.EqualTo("==") || op.EqualTo("=")) {
    fEqual = true;
    fLowLimit = value;
  }
  else if (op.EqualTo("!=")) {
    fNotEqual = true;
    fLowLimit = value;
  }
  else if (op.EqualTo(">")) {
    fGreaterThan = true;
    fLowLimit = value;
  }
  else if (op.EqualTo("<")) {
    fLessThan = true;
    fHighLimit = value;
  }
  else if (op.EqualTo(">=")) {
    fGreaterThanEqual = true;
    fLowLimit = value;
  }
  else if (op.EqualTo("<=")) {
    fLessThanEqual = true;
    fHighLimit = value;
  }
}

Algorithms::SelectParticle::SelectParticle (TString name, TString title, TString input, 
    TString property, TString inclusion, double low, double high) : 
  FilterParticleAlgo(name, title, input), 
  fHighLimit(high), fLowLimit(low), 
  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
  fPhi(false), fCharge(false), fID(false), fEqual(false), fNotEqual(false), 
  fLessThan(false), fGreaterThan(false), fLessThanEqual(false), 
  fGreaterThanEqual(false), fIn(false), fOut(false), fSingleEnd(false), fWindow(true), 
  fList(false), fProperty(property) {

  Setup();

  if (inclusion.EqualTo("inclusive") || inclusion.EqualTo("in"))
    fIn = true;
  else if (inclusion.EqualTo("exclusive") || inclusion.EqualTo("out"))
    fOut = true;
}

Algorithms::SelectParticle::SelectParticle (TString name, TString title, TString input, 
    TString property, int length, ...) : 
  FilterParticleAlgo(name, title, input), 
  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
  fPhi(false), fCharge(false), fID(false), fEqual(false), fNotEqual(false), 
  fLessThan(false), fGreaterThan(false), fLessThanEqual(false), 
  fGreaterThanEqual(false), fIn(false), fOut(false), fSingleEnd(false), fWindow(false), 
  fList(true), fProperty(property) {

  Setup();

  va_list arguments;  // store the variable list of arguments
  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < length; ++i) {
    if (fID)
      fListValues.push_back(va_arg(arguments, int));
    else
      fListValues.push_back(va_arg(arguments, double));
  }
  va_end(arguments); // cleans up the list
}

void Algorithms::SelectParticle::Setup () {
  if (fProperty.EqualTo("pt", TString::kIgnoreCase))
    fPt = true;
  if (fProperty.EqualTo("m", TString::kIgnoreCase))
    fM = true;
  if (fProperty.EqualTo("e", TString::kIgnoreCase))
    fE = true;
  if (fProperty.EqualTo("et", TString::kIgnoreCase))
    fEt = true;
  if (fProperty.EqualTo("p3", TString::kIgnoreCase))
    fP3 = true;
  if (fProperty.EqualTo("eta", TString::kIgnoreCase))
    fEta = true;
  if (fProperty.EqualTo("phi", TString::kIgnoreCase))
    fPhi = true;
  if (fProperty.EqualTo("charge", TString::kIgnoreCase))
    fCharge = true;
  if (fProperty.EqualTo("id", TString::kIgnoreCase))
    fID = true;
}

bool Algorithms::SelectParticle::FilterPredicate(ParticlePtr particle) {
  TLorentzVector *vec = particle->GetP();
  int charge = particle->GetCharge();
  int id = particle->GetID();

  if (fSingleEnd) {
    if (fEqual) {
      if (fPt)
        return (vec->Pt() == fLowLimit);
      else if (fM)
        return (vec->M() == fLowLimit);
      else if (fE)
        return (vec->E() == fLowLimit);
      else if (fEt)
        return (vec->Et() == fLowLimit);
      else if (fP3)
        return (vec->P() == fLowLimit);
      else if (fEta)
        return (vec->Eta() == fLowLimit);
      else if (fPhi)
        return (vec->Phi() == fLowLimit);
      else if (fID)
        return (id == fLowLimit);
      else if (fCharge)
        return (charge == fLowLimit);
    }
    else if (fNotEqual) {
      if (fPt)
        return (vec->Pt() != fLowLimit);
      else if (fM)
        return (vec->M() != fLowLimit);
      else if (fE)
        return (vec->E() != fLowLimit);
      else if (fEt)
        return (vec->Et() != fLowLimit);
      else if (fP3)
        return (vec->P() != fLowLimit);
      else if (fEta)
        return (vec->Eta() != fLowLimit);
      else if (fPhi)
        return (vec->Phi() != fLowLimit);
      else if (fID)
        return (id != fLowLimit);
      else if (fCharge)
        return (charge != fLowLimit);
    }
    else if (fGreaterThan) {
      if (fPt)
        return (vec->Pt() > fLowLimit);
      else if (fM)
        return (vec->M() > fLowLimit);
      else if (fE)
        return (vec->E() > fLowLimit);
      else if (fEt)
        return (vec->Et() > fLowLimit);
      else if (fP3)
        return (vec->P() > fLowLimit);
      else if (fEta)
        return (vec->Eta() > fLowLimit);
      else if (fPhi)
        return (vec->Phi() > fLowLimit);
      else if (fID)
        return (id > fLowLimit);
      else if (fCharge)
        return (charge > fLowLimit);
    }
    else if (fLessThan) {
      if (fPt)
        return (vec->Pt() < fHighLimit);
      else if (fM)
        return (vec->M() < fHighLimit);
      else if (fE)
        return (vec->E() < fHighLimit);
      else if (fEt)
        return (vec->Et() < fHighLimit);
      else if (fP3)
        return (vec->P() < fHighLimit);
      else if (fEta)
        return (vec->Eta() < fHighLimit);
      else if (fPhi)
        return (vec->Phi() < fHighLimit);
      else if (fID)
        return (id < fHighLimit);
      else if (fCharge)
        return (charge < fHighLimit);
    }
    else if (fGreaterThanEqual) {
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
      else if (fID)
        return (id >= fLowLimit);
      else if (fCharge)
        return (charge >= fLowLimit);
    }
    else if (fLessThanEqual) {
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
      else if (fID)
        return (id <= fHighLimit);
      else if (fCharge)
        return (charge <= fHighLimit);
    }
  }

  else if (fWindow) {
    if (fIn) {
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
      else if (fID)
        return (id <= fHighLimit && id >= fLowLimit);
      else if (fCharge)
        return (charge <= fHighLimit && charge >= fLowLimit);
    }
    else if (fOut) {
      if (fPt)
        return (vec->Pt() >= fHighLimit && vec->Pt() <= fLowLimit);
      else if (fM)
        return (vec->M() >= fHighLimit && vec->M() <= fLowLimit);
      else if (fE)
        return (vec->E() >= fHighLimit && vec->E() <= fLowLimit);
      else if (fEt)
        return (vec->Et() >= fHighLimit && vec->Et() <= fLowLimit);
      else if (fP3)
        return (vec->P() >= fHighLimit && vec->P() <= fLowLimit);
      else if (fEta)
        return (vec->Eta() >= fHighLimit && vec->Eta() <= fLowLimit);
      else if (fPhi)
        return (vec->Phi() >= fHighLimit && vec->Phi() <= fLowLimit);
      else if (fID)
        return (id >= fHighLimit && id <= fLowLimit);
      else if (fCharge)
        return (charge >= fHighLimit && charge <= fLowLimit);
    }
  }

  else if (fList) {
    if (fPt) {
      for (std::vector<double>::iterator val = fListValues.begin();
           val != fListValues.end(); ++val) {
        if (vec->Pt() == *val)
          return true;
      }
    }
    else if (fM) {
      for (std::vector<double>::iterator val = fListValues.begin();
           val != fListValues.end(); ++val) {
        if (vec->M() == *val)
          return true;
      }
    }
    else if (fE) {
      for (std::vector<double>::iterator val = fListValues.begin();
           val != fListValues.end(); ++val) {
        if (vec->E() == *val)
          return true;
      }
    }
    else if (fEt) {
      for (std::vector<double>::iterator val = fListValues.begin();
           val != fListValues.end(); ++val) {
        if (vec->Et() == *val)
          return true;
      }
    }
    else if (fP3) {
      for (std::vector<double>::iterator val = fListValues.begin();
           val != fListValues.end(); ++val) {
        if (vec->P() == *val)
          return true;
      }
    }
    else if (fEta) {
      for (std::vector<double>::iterator val = fListValues.begin();
           val != fListValues.end(); ++val) {
        if (vec->Eta() == *val)
          return true;
      }
    }
    else if (fPhi) {
      for (std::vector<double>::iterator val = fListValues.begin();
           val != fListValues.end(); ++val) {
        if (vec->Phi() == *val)
          return true;
      }
    }
    else if (fID) {
      for (std::vector<double>::iterator val = fListValues.begin();
           val != fListValues.end(); ++val) {
        if (id == *val)
          return true;
      }
    }
    else if (fCharge) {
      for (std::vector<double>::iterator val = fListValues.begin();
           val != fListValues.end(); ++val) {
        if (charge == *val)
          return true;
      }
    }
    return false;
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

Algorithms::Cut::Cut (TString name, TString title, TString logic, 
    long long length, ...) :
  CutAlgorithm(name, title), fAnd(false), fOr(false) {
  va_list arguments;  // store the variable list of arguments

  if (logic.EqualTo("and", TString::kIgnoreCase))
    fAnd = true;
  else if (logic.EqualTo("or", TString::kIgnoreCase))
    fOr = true;
  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < length; ++i) {
    const char* algo_name = va_arg(arguments, const char*);
    const char* type = va_arg(arguments, const char*);
    TString Type(type);
    const char* op = va_arg(arguments, const char*);
    TString Op(op);

    if (Type.EqualTo("bool")) {
      BoolAlgoInfo *Algo = new BoolAlgoInfo();
      Algo->fValue = (bool)va_arg(arguments, int);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
    else if (Type.EqualTo("integer")) {
      IntegerAlgoInfo *Algo = new IntegerAlgoInfo();
      Algo->fValue = (long long)va_arg(arguments, int);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
    else if (Type.EqualTo("counting")) {
      CountingAlgoInfo *Algo = new CountingAlgoInfo();
      Algo->fValue = (unsigned long long)va_arg(arguments, unsigned int);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
    else if (Type.EqualTo("decimal")) {
      DecimalAlgoInfo *Algo = new DecimalAlgoInfo();
      Algo->fValue = (long double)va_arg(arguments, double);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
    else if (Type.EqualTo("particle")) {
      NParticlesAlgoInfo *Algo = new NParticlesAlgoInfo();
      Algo->fValue = (long long)va_arg(arguments, int);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
  }
  va_end(arguments); // cleans up the list
}

Algorithms::Cut::~Cut () {
  for (std::vector<AlgoInfo*>::iterator it = fAlgorithms.begin();
      it != fAlgorithms.end(); ++it) {
    delete *it;
  }
}

void Algorithms::Cut::Exec (Option_t* /*option*/) {
  AnalysisData *data = GetUserData();
  HAL::GenericData *input_data = NULL;

  if (fAnd) {
    for (std::vector<AlgoInfo*>::iterator it = fAlgorithms.begin();
         it != fAlgorithms.end(); ++it) {
      if (!data->Exists((*it)->fName)) {
        Abort();
        return;
      }
      input_data = (GenericData*)data->GetTObject((*it)->fName);
      if (!(*it)->Eval(data, input_data)) {
        Abort();
        return;
      }
    }
    Passed();
  }
  else if (fOr) {
    for (std::vector<AlgoInfo*>::iterator it = fAlgorithms.begin();
         it != fAlgorithms.end(); ++it) {
      if (data->Exists((*it)->fName)) {
        input_data = (GenericData*)data->GetTObject((*it)->fName);
        if ((*it)->Eval(data, input_data)) {
          Passed();
          return;
        }
      }
    }
    Abort();
  }
}

bool  Algorithms::Cut::BoolAlgoInfo::Eval (HAL::AnalysisData *data, HAL::GenericData *gen_data) {
  bool current_value = data->GetBool(gen_data->GetRefName());

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  return false;
}

bool  Algorithms::Cut::IntegerAlgoInfo::Eval (HAL::AnalysisData *data, HAL::GenericData *gen_data) {
  long long current_value = data->GetInteger(gen_data->GetRefName());

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  if (fLessThan && current_value < fValue)
    return true;
  if (fGreaterThan && current_value > fValue)
    return true;
  if (fLessThanEqual && current_value <= fValue)
    return true;
  if (fGreaterThanEqual && current_value >= fValue)
    return true;
  return false;
}

bool  Algorithms::Cut::CountingAlgoInfo::Eval (HAL::AnalysisData *data, HAL::GenericData *gen_data) {
  unsigned long long current_value = data->GetCounting(gen_data->GetRefName());

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  if (fLessThan && current_value < fValue)
    return true;
  if (fGreaterThan && current_value > fValue)
    return true;
  if (fLessThanEqual && current_value <= fValue)
    return true;
  if (fGreaterThanEqual && current_value >= fValue)
    return true;
  return false;
}

bool  Algorithms::Cut::DecimalAlgoInfo::Eval (HAL::AnalysisData *data, HAL::GenericData *gen_data) {
  long double current_value = data->GetDecimal(gen_data->GetRefName());

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  if (fLessThan && current_value < fValue)
    return true;
  if (fGreaterThan && current_value > fValue)
    return true;
  if (fLessThanEqual && current_value <= fValue)
    return true;
  if (fGreaterThanEqual && current_value >= fValue)
    return true;
  return false;
}

bool  Algorithms::Cut::NParticlesAlgoInfo::Eval (HAL::AnalysisData * /*data*/, HAL::GenericData *gen_data) {
  long long current_value = gen_data->GetNParticles();

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  if (fLessThan && current_value < fValue)
    return true;
  if (fGreaterThan && current_value > fValue)
    return true;
  if (fLessThanEqual && current_value <= fValue)
    return true;
  if (fGreaterThanEqual && current_value >= fValue)
    return true;
  return false;
}

/*
 * Monitoring Algorithms
 * */

void Algorithms::MonitorAlgorithm::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::AnalysisTreeReader *tr = GetRawData();
  HAL::GenericData *input_data = NULL;

  if ((tr->GetEntryNumber() + 1) % fN == 0) {
    (*fOS) << "\nSummary for algorithm - " << fInput.Data() << std::endl;

    if (data->Exists(fInput))
      input_data = (GenericData*)data->GetTObject(fInput);
    else {
      (*fOS) << "Algorithm is empty!" << std::endl;
      return;
    }

    (*fOS) << *input_data << std::endl;
  }
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
  ParticlesTLVStore(name, title, input, bname, num), fAll(false), fPt(false), fM(false), 
  fE(false), fEt(false), fP3(false), fEta(false), fPhi(false), fID(false), fCharge(false) {

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

  fPtLabel = TString::Format("%s.pt", fBranchName.Data());
  fEtaLabel = TString::Format("%s.eta", fBranchName.Data());
  fPhiLabel = TString::Format("%s.phi", fBranchName.Data());
  fMLabel = TString::Format("%s.m", fBranchName.Data());
  fELabel = TString::Format("%s.e", fBranchName.Data());
  fIDLabel = TString::Format("%s.id", fBranchName.Data());
  fChargeLabel = TString::Format("%s.charge", fBranchName.Data());
}

void Algorithms::StoreTLV::StoreValue (HAL::AnalysisTreeWriter *output, long long i, HAL::ParticlePtr particle) {
  TLorentzVector *vec = particle->GetP();

  if (fPt) {
    if (fMany)
      output->SetValue(fBranchName, vec->Pt(), i);
    else
      output->SetValue(fBranchName, vec->Pt());
    return;
  }
  if (fM) {
    if (fMany)
      output->SetValue(fBranchName, vec->M(), i);
    else
      output->SetValue(fBranchName, vec->M());
    return;
  }
  if (fE) {
    if (fMany)
      output->SetValue(fBranchName, vec->E(), i);
    else
      output->SetValue(fBranchName, vec->E());
    return;
  }
  if (fEt) {
    if (fMany)
      output->SetValue(fBranchName, vec->Et(), i);
    else
      output->SetValue(fBranchName, vec->Et());
    return;
  }
  if (fP3) {
    if (fMany)
      output->SetValue(fBranchName, vec->P(), i);
    else
      output->SetValue(fBranchName, vec->P());
    return;
  }
  if (fEta) {
    if (fMany)
      output->SetValue(fBranchName, vec->Eta(), i);
    else
      output->SetValue(fBranchName, vec->Eta());
    return;
  }
  if (fPhi) {
    if (fMany)
      output->SetValue(fBranchName, vec->Phi(), i);
    else
      output->SetValue(fBranchName, vec->Phi());
    return;
  }
  if (fID) {
    if (fMany)
      output->SetValue(fBranchName, particle->GetID(), i);
    else
      output->SetValue(fBranchName, particle->GetID());
    return;
  }
  if (fCharge) {
    if (fMany)
      output->SetValue(fBranchName, particle->GetCharge(), i);
    else
      output->SetValue(fBranchName, particle->GetCharge());
    return;
  }
  if (fAll) {
    if (fMany) {
      output->SetValue(fPtLabel, vec->Pt(), i);
      output->SetValue(fEtaLabel, vec->Eta(), i);
      output->SetValue(fPhiLabel, vec->Phi(), i);
      output->SetValue(fMLabel, vec->M(), i);
      output->SetValue(fELabel, vec->E(), i);
      output->SetValue(fIDLabel, particle->GetID(), i);
      output->SetValue(fChargeLabel, particle->GetCharge(), i);
    }
    else {
      output->SetValue(fPtLabel, vec->Pt());
      output->SetValue(fEtaLabel, vec->Eta());
      output->SetValue(fPhiLabel, vec->Phi());
      output->SetValue(fMLabel, vec->M());
      output->SetValue(fELabel, vec->E());
      output->SetValue(fIDLabel, particle->GetID());
      output->SetValue(fChargeLabel, particle->GetCharge());
    }
    return;
  }
  throw HAL::HALException(GetName().Prepend("Couldn't determine what to store: "));
}

} /* HAL */ 
