#ifndef HAL_PARTICLE
#define HAL_PARTICLE

#include <TLorentzVector.h>
#include <vector>
#include <HAL/Common.h>

namespace HAL {

class Particle {
public:
  Int_t PDG_ID;
  TLorentzVector *P;
  std::vector<Int_t> motherIndexList;
  std::vector<Particle> motherList;
  std::vector<Int_t> daughterIndexList;
  std::vector<Particle> daughterList;
};

}

#endif
