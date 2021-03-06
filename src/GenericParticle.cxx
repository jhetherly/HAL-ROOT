#include <HAL/GenericParticle.h>
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
#include <aux/boost/foreach.hpp>
#endif
#include <TLorentzVector.h>

ClassImp(HAL::GenericParticle);

namespace HAL
{

//______________________________________________________________________________
HAL::GenericParticle::GenericParticle (const TString &owner, const TString &origin, const TString &name) : 
  fOwner(owner), fOwnerIndex(0), fOriginIndex(0), fID(0), fCharge(0.0), fP(nullptr) 
{

  if(origin.EqualTo("")) 
    fOrigin = fOwner;
  else
    fOrigin = origin;
  if(!name.EqualTo("")) 
    SetName(name.Data());
}

//______________________________________________________________________________
HAL::GenericParticle::GenericParticle (const GenericParticle &particle) : 
  TNamed() 
{
  fOwner = particle.fOwner;
  fOrigin = particle.fOrigin;
  fOwnerIndex = particle.fOwnerIndex;
  fOriginIndex = particle.fOriginIndex;
  fID = particle.fID;
  fCharge = particle.fCharge;
  fP = new TLorentzVector(*particle.fP);
  fScalarAttributes = particle.fScalarAttributes;
  f1DParticles = particle.f1DParticles;
}

//______________________________________________________________________________
HAL::GenericParticle::~GenericParticle () 
{
  if (fP) delete fP;
}

//______________________________________________________________________________
void HAL::GenericParticle::SetAttribute (const TString &name, const long double &value) 
{
  fScalarAttributes[name] = value;
}

//______________________________________________________________________________
void HAL::GenericParticle::SetParticle (const TString &name, 
                                        GenericParticle *particle, 
                                        const long long &index) 
{
  if (index == -1)
    f1DParticles[name].push_back(particle);
  else
    f1DParticles[name][index] = particle;
  std::stable_sort(f1DParticles[name].begin(), f1DParticles[name].end());
}

//______________________________________________________________________________
void HAL::GenericParticle::SetParticles (const TString &name, std::vector<GenericParticle*> &particles) 
{
  f1DParticles[name] = particles;
  std::stable_sort(f1DParticles[name].begin(), f1DParticles[name].end());
}

//______________________________________________________________________________
bool HAL::GenericParticle::HasSameParticles (const TString &name, ParticlePtr particle) 
{
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

//______________________________________________________________________________
std::ostream& operator<<(std::ostream& os, const HAL::GenericParticle &particle) 
{
  os << "Origin: " << particle.fOrigin << "\t\tIndex: " << particle.fOriginIndex << std::endl;
  os << "Owner: " << particle.fOwner << "\t\tIndex: " << particle.fOwnerIndex << std::endl;
  os << "ID: " << particle.fID << "\t\tCharge: " << particle.fCharge << std::endl;
  os.precision(4);
  os << std::scientific;
  if (particle.fP != nullptr) {
    os << "4-vector properties:\n\tpT\t\teta\t\tphi\t\tmass\t\tenergy\n"
       << "\t" << particle.fP->Pt() << "\t" << particle.fP->Eta() << "\t" << particle.fP->Phi()
       << "\t" << particle.fP->M() << "\t" << particle.fP->E() << "\n";
  }
  if (!particle.fScalarAttributes.empty()) {
    os << "Scalar attributes:" << std::endl;
    for (std::map<TString, long double>::const_iterator sa = particle.fScalarAttributes.begin();
         sa != particle.fScalarAttributes.end(); ++sa) {
      os << "\tName: " << sa->first << "\t\tValue: " << sa->second << "\n";
    }
  }
  if (!particle.f1DParticles.empty()) {
    os << "Particle arrays:" << std::endl;
    for (std::map<TString, ParticlePtrs>::const_iterator p = particle.f1DParticles.begin();
         p != particle.f1DParticles.end(); ++p) {
      size_t count = 0, 
             n = p->second.size();
      os << "\tName: " << p->first << "\t\tNumber of particles: " << n;
      if (n > 0)
        os << "\n\tList of particles: ";
      for (ParticlePtrsConstIt part = p->second.begin(); part != p->second.end(); ++part) {
        os << (*part)->GetOrigin() << "  " << (*part)->GetOriginIndex();
        if (++count < n)
          os << ",    ";
      }
      os << "\n";
    }
  }
  os.unsetf(std::ios_base::floatfield);
  os.flush();
  return os;
}

} /* HAL */ 
