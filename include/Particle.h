#include <TLorentzVector.h>
#include <vector>

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
