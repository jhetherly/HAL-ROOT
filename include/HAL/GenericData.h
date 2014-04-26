#ifndef HAL_GENERICDATA
#define HAL_GENERICDATA

#include <TNamed.h>
#include <TString.h>
#include <vector>
#include <map>
#include <iostream>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/GenericParticle.h>

namespace HAL
{

/*
 * Data container for generic algorithms
 * */

class GenericData : public TNamed {
public:
  GenericData (const TString &name, bool is_owner = false);
  GenericData (const GenericData &data);
  virtual ~GenericData ();

  void          SetRefName (const TString &name) {fUserDataRefName = name;}
  void          AddParticle (ParticlePtr particle) {fParticles.push_back(particle);}
  void          SetParticles (const TString &name, ParticlePtrs &particles) {f1DParticles[name] = particles;}
  inline TString        GetRefName () {return fUserDataRefName;}
  inline ParticlePtr    GetParticle (const long long &index) {return fParticles[index];}
  inline ParticlePtrsIt GetParticleBegin () {return fParticles.begin();}
  inline ParticlePtrsIt GetParticleEnd () {return fParticles.end();}
  inline ParticlePtrs&  GetParticles (const TString &name) {return f1DParticles[name];}

  inline bool       IsOwner () {return fIsOwner;}
  inline TString    GetOwner () {return (fParticles.size() >= 1) ? fParticles[0]->GetOrigin() : "";}
  inline bool       HasParticles (const TString &name) {return (f1DParticles.count(name) != 0) ? true : false;}
  inline size_t     GetNParticles () {return fParticles.size();}
  inline size_t     GetNParticles (const TString &name) {return HasParticles(name) ? f1DParticles[name].size() : 0;}

  friend std::ostream& operator<<(std::ostream& os, GenericData &data);

  ClassDef(GenericData, 0);

private:
  bool                                                  fIsOwner;
  // used if value is stored in 'UserData' (i.e. a trigger boolean)
  TString                                               fUserDataRefName; 
  // list of actual particles (and actual owner of memory)
  ParticlePtrs                                          fParticles;
  // the following is for sorted lists, etc...
  std::map<TString, ParticlePtrs, internal::string_cmp> f1DParticles;
};

std::ostream& operator<<(std::ostream& os, HAL::GenericData &data);

} /* HAL */ 

#endif
