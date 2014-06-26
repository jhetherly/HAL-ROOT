#include <HAL/Algorithms/ImportParticle.h>
#include <iostream>

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
    if (fHasCharge) particle->SetCharge(tr->GetDecimal(fCharge, i));
    if (fHasID) particle->SetID(tr->GetInteger(fID, i));
    gen_data->AddParticle(particle);
    particle->SetOriginIndex(gen_data->GetNParticles() - 1);
    particle->SetOwnerIndex(gen_data->GetNParticles() - 1);
  }

  gen_data->SetRefType("none");
  IncreaseCounter(gen_data->GetNParticles());
}

void internal::ImportParticleAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
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
  else {
    if (tr->CheckBranchMapNickname(fNEntriesName) && 
        tr->GetInteger(fNEntriesName) < n) {
      n = tr->GetInteger(fNEntriesName);
    }
    else if (fIsCart || fIsCartMET) {
      if (tr->GetRank(fCartX1) == 1 && 
          tr->GetDim(fCartX1) < n)
        n = tr->GetDim(fCartX1);
      else if (tr->GetRank(fCartX1) == 0)
        n = 1;
    }
    else if (fIsE || fIsM || fIsPhiEtMET) {
      if (tr->GetRank(fPhi) == 1 && 
          tr->GetDim(fPhi) < n)
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
  throw HAL::HALException("Couldn't identify type in ImportParticle");
}

} /* HAL */ 
