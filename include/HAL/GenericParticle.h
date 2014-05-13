#ifndef HAL_GENERICPARTICLE
#define HAL_GENERICPARTICLE

#include <TNamed.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <vector>
#include <map>
#include <iostream>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>

namespace HAL
{

/*
 * Particle class for generic algorithms
 * */

class GenericParticle;

typedef GenericParticle               Particle;
typedef Particle*                     ParticlePtr;
typedef std::vector<ParticlePtr>      ParticlePtrs;
typedef ParticlePtrs::iterator        ParticlePtrsIt;
typedef ParticlePtrs::const_iterator  ParticlePtrsConstIt;

class GenericParticle : public TNamed {
public:
  GenericParticle (const TString &owner, const TString &origin = "", const TString &name = "");
  GenericParticle (const GenericParticle &particle);
  ~GenericParticle ();

  void            SetOwner (const TString &owner) {fOwner = owner;}
  void            SetOrigin (const TString &origin) {fOrigin = origin;}
  void            SetOwnerIndex (const size_t &oi) {fOwnerIndex = oi;}
  void            SetOriginIndex (const size_t &oi) {fOriginIndex = oi;}
  void            SetID (const int &id) {fID = id;}
  void            SetCharge (const float &charge) {fCharge = charge;}
  void            SetP (TLorentzVector *p) {fP = p;}
  void            SetVector (TLorentzVector *vec) {fP = vec;}
  void            SetAttribute (const TString &name, const long double &value);
  void            SetParticle (const TString &name, GenericParticle *particle, const long long &index = -1);
  void            SetParticles (const TString &name, std::vector<GenericParticle*> &particles);
  inline TString  GetOwner () {return fOwner;}
  inline TString  GetOrigin () {return fOrigin;}
  inline size_t   GetOwnerIndex () {return fOriginIndex;}
  inline size_t   GetOriginIndex () {return fOriginIndex;}
  inline int      GetID () {return fID;}
  inline float    GetCharge () {return fCharge;}
  inline TLorentzVector*    GetP () {return fP;}
  inline TLorentzVector*    GetVector () {return fP;}
  inline long double        GetAttribute (const TString &name) {return fScalarAttributes[name];}
  inline std::map<TString, long double, HAL::internal::string_cmp>&  GetAttributes () {return fScalarAttributes;}
  inline ParticlePtr        GetParticle (const TString &name, const long long &index) {return f1DParticles[name][index];}
  inline ParticlePtrs&      GetParticles (const TString &name) {return f1DParticles[name];}

  inline bool     HasAttribute (const TString &name) {return (fScalarAttributes.count(name) == 0) ? false : true;}
  inline bool     HasParticles (const TString &name) {return (f1DParticles.count(name) == 0) ? false : true;}
  inline size_t   GetNParticles (const TString &name) {return HasParticles(name) ? f1DParticles[name].size() : 0;}
  bool            HasSameParticles (const TString &name, ParticlePtr particle);

  friend std::ostream& operator<<(std::ostream& os, const GenericParticle &particle);
  
  ClassDef(GenericParticle, 0);

private:
  TString                                      fOwner; // what algorithm made this particle
  TString                                      fOrigin; // what algorithm first made this particle
  size_t                                       fOwnerIndex, fOriginIndex;
  int                                          fID;
  float                                        fCharge;
  TLorentzVector                              *fP;
  std::map<TString, long double, internal::string_cmp>    fScalarAttributes;
  // the following is for parent/child lists etc...
  std::map<TString, ParticlePtrs, internal::string_cmp>   f1DParticles;
};

std::ostream& operator<<(std::ostream& os, const HAL::GenericParticle &particle);

} /* HAL */ 

#endif
