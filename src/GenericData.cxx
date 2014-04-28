#include <HAL/GenericData.h>

ClassImp(HAL::GenericData);

namespace HAL
{

HAL::GenericData::GenericData (const TString &name, bool is_owner) : 
  fIsOwner(is_owner), fUserDataRefName(""), fUserDataRefType("") {
  fParticles.reserve(20);
  SetName(name.Data());
}

HAL::GenericData::GenericData (const GenericData &data) : 
  TNamed() {
  fUserDataRefName = data.fUserDataRefName; 
  fUserDataRefType = data.fUserDataRefType; 
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
  size_t  np = data.GetNParticles();
  if (data.IsOwner())
    os << "This algorithm owns " << np << " particle(s)." << std::endl;
  else
    os << "This algorithm references " << np << " particle(s)." << std::endl;
  if (np > 0) {
    for (HAL::ParticlePtrsIt particle = data.GetParticleBegin();
        particle != data.GetParticleEnd(); ++particle) {
      os << **particle << std::endl;
    }
  }
  if (!data.GetRefName().EqualTo(""))
    os << "This algorithm references " << data.GetRefName() << " of type " 
       << data.GetRefType() << " in \"UserData\"" << std::endl;
  if (!data.f1DParticles.empty()) {
    os << "Particle arrays:" << std::endl;
    for (std::map<TString, ParticlePtrs>::iterator it = data.f1DParticles.begin();
         it != data.f1DParticles.end(); ++it) {
      size_t count = 0, 
             n = it->second.size();
      os << "\tName: " << it->first << "\t\tNumber of particles: " << n;
      if (n > 0)
        os << "\n\tList of particles: ";
      for (ParticlePtrsIt part = it->second.begin(); part != it->second.end(); ++part) {
        os << (*part)->GetOrigin() << "  " << (*part)->GetOriginIndex();
        if (++count < n)
          os << ",    ";
      }
      os << "\n";
    }
  }
  os.flush();
  return os;
}

} /* HAL */ 
