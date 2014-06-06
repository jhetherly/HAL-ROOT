#include <HAL/Algorithms/SelectParticle.h>

namespace HAL
{

/*
 * Generic class
 * */
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




Algorithms::SelectParticle::SelectParticle (TString name, TString title, TString input, 
    TString property, TString op, double value) : 
  FilterParticleAlgo(name, title, input), 
  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
  fPhi(false), fCharge(false), fID(false), fAttribute(false), fEqual(false), fNotEqual(false), 
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
  fPhi(false), fCharge(false), fID(false), fAttribute(false), fEqual(false), fNotEqual(false), 
  fLessThan(false), fGreaterThan(false), fLessThanEqual(false), 
  fGreaterThanEqual(false), fIn(false), fOut(false), fSingleEnd(false), fWindow(true), 
  fList(false), fProperty(property) {

  Setup();

  if (inclusion.EqualTo("inclusive", TString::kIgnoreCase) || 
      inclusion.EqualTo("in", TString::kIgnoreCase))
    fIn = true;
  else if (inclusion.EqualTo("exclusive", TString::kIgnoreCase) || 
           inclusion.EqualTo("out", TString::kIgnoreCase))
    fOut = true;
}

Algorithms::SelectParticle::SelectParticle (TString name, TString title, TString input, 
    TString property, int length, ...) : 
  FilterParticleAlgo(name, title, input), 
  fPt(false), fM(false), fE(false), fEt(false), fP3(false), fEta(false), 
  fPhi(false), fCharge(false), fID(false), fAttribute(false), fEqual(false), fNotEqual(false), 
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
  else if (fProperty.EqualTo("m", TString::kIgnoreCase))
    fM = true;
  else if (fProperty.EqualTo("e", TString::kIgnoreCase))
    fE = true;
  else if (fProperty.EqualTo("et", TString::kIgnoreCase))
    fEt = true;
  else if (fProperty.EqualTo("p3", TString::kIgnoreCase))
    fP3 = true;
  else if (fProperty.EqualTo("eta", TString::kIgnoreCase))
    fEta = true;
  else if (fProperty.EqualTo("phi", TString::kIgnoreCase))
    fPhi = true;
  else if (fProperty.EqualTo("charge", TString::kIgnoreCase))
    fCharge = true;
  else if (fProperty.EqualTo("id", TString::kIgnoreCase))
    fID = true;
  else
    fAttribute = true;
}

bool Algorithms::SelectParticle::FilterPredicate(ParticlePtr particle) {
  TLorentzVector *vec = particle->GetP();
  double property = 0.0;

  if (fPt)
    property = vec->Pt();
  else if (fM)
    property = vec->M();
  else if (fE)
    property = vec->E();
  else if (fEt)
    property = vec->Et();
  else if (fP3)
    property = vec->P();
  else if (fEta)
    property = vec->Eta();
  else if (fPhi)
    property = vec->Phi();
  else if (fID)
    property = (double)particle->GetID();
  else if (fCharge)
    property = (double)particle->GetCharge();
  else if (fAttribute && particle->HasAttribute(fProperty))
    property = particle->GetAttribute(fProperty);
  else 
    throw HAL::HALException(GetName().Prepend("Couldn't determine property to filter: "));

  if (fSingleEnd) {
    if (fEqual) 
      return (property == fLowLimit);
    else if (fNotEqual) 
      return (property != fLowLimit);
    else if (fGreaterThan)
      return (property > fLowLimit);
    else if (fLessThan)
      return (property < fHighLimit);
    else if (fGreaterThanEqual)
      return (property >= fLowLimit);
    else if (fLessThanEqual)
      return (property <= fHighLimit);
  }

  else if (fWindow) {
    if (fIn)
      return (property <= fHighLimit && property >= fLowLimit);
    else if (fOut)
      return (property >= fHighLimit && property <= fLowLimit);
  }

  else if (fList) {
    for (std::vector<double>::iterator val = fListValues.begin();
        val != fListValues.end(); ++val) {
      if (property == *val)
        return true;
    }
    return false;
  }

  throw HAL::HALException(GetName().Prepend("Couldn't determine how to filter: "));
}

} /* HAL */ 
