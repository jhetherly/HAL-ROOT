#ifndef HAL_ImportObject
#define HAL_ImportObject

#include <TMath.h>
#include <TString.h>
#include <HAL/Common.h>
#include <HAL/Algorithm.h>
#include <HAL/AnalysisData.h>
#include <HAL/AnalysisTreeReader.h>

namespace HAL
{

// NOTE TO SELF: templates can be partialy set using a
// Type Generator Idiom

/*
 * This host class places no restriction on Object other
 * than it must be a valid template parameter
 * ObjectCreationPolicy.
 *
 * ObjectCreationPolicy must accept the Object class as a
 * template parameter. It must also provide a consrtuctor
 * that accepts a TString, a method named Init that accepts
 * an Option_t* and an AnalysisTreeReader* object, a method 
 * named GetNObjects, and a method named CreateObject that 
 * accepts an unsigned int. Note: Init may be called more 
 * than once with the same arguements.
 *
 * Storage must derive from TObject so that it can be 
 * stored in an AnalysisData object. It must provide a 
 * constructor that accepts 
 * (Option_t*, TString, unsigned, AnalysisData*), a virtual 
 * destructor, and a method named StoreObject that accpets 
 * the output from ObjectCreationPolicy::CreateObject.
 * Finally, the class must have a template parameter that
 * can accept class Object.
 * */
template < class Object, 
           template <class> class Storage,
           template <class> class ObjectCreationPolicy
           >
class ImportObject: public Algorithm, public ObjectCreationPolicy<Object> {
public:
  ImportObject (TString name, TString title) :
    Algorithm(name, title), ObjectCreationPolicy<Object>(name), 
    fOS(nullptr) {}
  virtual ~ImportObject () {}


  virtual void Init (Option_t *option) 
  {
    ObjectCreationPolicy<Object>::Init(option, GetRawData());
  }


  virtual void Notify (Option_t *option) 
  {
    ObjectCreationPolicy<Object>::Init(option, GetRawData());
  }


  virtual void Exec (Option_t* option)
  {
    HAL::AnalysisData *data = GetUserData();
    unsigned int n = ObjectCreationPolicy<Object>::GetNObjects();

    fOS = new ObjectStorage(option, GetName(), n, data);

    for (unsigned int i = 0; i < n; ++i) {
      fOS->StoreObject(ObjectCreationPolicy<Object>::CreateObject(i));
    }

    IncreaseCounter(n);
  }


  virtual void Clear (Option_t* /*option*/) 
  {
    delete GetUserData()->GetTObject(GetName());
  }

private:
  Storage<Object> *fOS;
};




// Policies

class AnalysisTreeReaderPolicy {
private:
  AnalysisTreeReader *fATR;

protected:
  ~AnalysisTreeReaderPolicy () {}

public:
  void PassRawData(AnalysisTreeReader *tr)
  {
    fATR = tr;
  }

  unsigned long ReadDimension(TString &bname)
  {
    return fATR->GetDim(bname);
  }

  unsigned long ReadCountingNumber(TString &bname)
  {
    return fATR->GetCounting(bname);
  }

  long ReadIntegerNumberArray(TString &bname, unsigned long &i)
  {
    return fATR->GetInteger(bname, i);
  }

  long double ReadDecimalNumberArray(TString &bname, unsigned long &i)
  {
    return fATR->GetDecimal(bname, i);
  }
};




//template < class Particle, class TreeReadingPolicy >
//class SimpleAutoParticleFromTree: public TreeReadingPolicy {
//protected:
//  ~SimpleAutoParticleFromTree () {}
//
//  void Init (Option_t*, AnalysisTreeReader*);
//  unsigned int GetNObjects ();
//  Particle* CreateObject (unsigned int);
//};

// Intermediate Policy hosts
// 'Particle' must adhere to a certain interface
// - Must have default constructor
// - Must have SetCartVector that takes the cartesian components of a 4-vector
// - Must have SetEPtEtaPhiVector that takes the energy, pT, eta, and phi components of a 4-vector
// - Must have SetMPtEtaPhiVector that takes the mass, pT, eta, and phi components of a 4-vector
// - Must have SetCharge that sets the charge of the particle
// - Must have SetID that sets the ID number of the particle
template < class Particle, class TreeReadingPolicy = AnalysisTreeReaderPolicy >
class SimpleAutoParticleFromTree: public TreeReadingPolicy {
protected:
  SimpleAutoParticleFromTree (TString name) :
    fIsCart(kFALSE), fIsE(kFALSE), fIsM(kFALSE), fIsCartMET(kFALSE), fIsPhiEtMET(kFALSE),
    fHasNEntries(kFALSE), fHasCharge(kFALSE), fHasID(kFALSE)
  {
    fName = name;
    fCartX0 = TString::Format("%s:x0", fName.Data());
    fCartX1 = TString::Format("%s:x1", fName.Data());
    fCartX2 = TString::Format("%s:x2", fName.Data());
    fCartX3 = TString::Format("%s:x3", fName.Data());
    fM = TString::Format("%s:m", fName.Data());
    fE = TString::Format("%s:e", fName.Data());
    fPt = TString::Format("%s:pt", fName.Data());
    fEta = TString::Format("%s:eta", fName.Data());
    fPhi = TString::Format("%s:phi", fName.Data());
    fCharge = TString::Format("%s:charge", fName.Data());
    fID = TString::Format("%s:id", fName.Data());
    fNEntriesName = TString::Format("%s:nentries", fName.Data());
  }
  ~SimpleAutoParticleFromTree () {}

  void Init (Option_t * /*option*/, AnalysisTreeReader *tr)
  {
    if (tr->CheckBranchMapNickname(fCartX0) && tr->CheckBranchMapNickname(fCartX1) && 
        tr->CheckBranchMapNickname(fCartX2) && tr->CheckBranchMapNickname(fCartX3))
      fIsCart = kTRUE;
    else if (tr->CheckBranchMapNickname(fE) && tr->CheckBranchMapNickname(fPt) &&
             tr->CheckBranchMapNickname(fEta) && tr->CheckBranchMapNickname(fPhi))
      fIsE = kTRUE;
    else if (tr->CheckBranchMapNickname(fM) && tr->CheckBranchMapNickname(fPt) &&
             tr->CheckBranchMapNickname(fEta) && tr->CheckBranchMapNickname(fPhi))
      fIsM = kTRUE;
    else if (!tr->CheckBranchMapNickname(fCartX0) && tr->CheckBranchMapNickname(fCartX1) &&
             tr->CheckBranchMapNickname(fCartX2) && !tr->CheckBranchMapNickname(fCartX3))
      fIsCartMET = kTRUE;
    else if ((tr->CheckBranchMapNickname(fEt) || tr->CheckBranchMapNickname(fPt)) &&
             tr->CheckBranchMapNickname(fPhi) && !tr->CheckBranchMapNickname(fEta) &&
             !tr->CheckBranchMapNickname(fM) && !tr->CheckBranchMapNickname(fE))
      fIsPhiEtMET = kTRUE;
    if (!fIsCart && !fIsE && !fIsM && !fIsCartMET && !fIsPhiEtMET)
      HAL::HALException(GetName().Prepend("Couldn't determine how to import data: ").Data());

    if (tr->CheckBranchMapNickname(fCharge))
      fHasCharge = kTRUE;
    if (tr->CheckBranchMapNickname(fID))
      fHasID = kTRUE;

    fCartX0 = tr->GetBranchName(fCartX0);
    fCartX1 = tr->GetBranchName(fCartX1);
    fCartX2 = tr->GetBranchName(fCartX2);
    fCartX3 = tr->GetBranchName(fCartX3);
    fM = tr->GetBranchName(fM);
    fE = tr->GetBranchName(fE);
    fPt = tr->GetBranchName(fPt);
    fEta = tr->GetBranchName(fEta);
    fPhi = tr->GetBranchName(fPhi);
    fCharge = tr->GetBranchName(fCharge);
    fID = tr->GetBranchName(fID);
    fNEntriesName = tr->GetBranchName(fNEntriesName);

    TreeReadingPolicy::PassRawData(tr);
  }

  unsigned int GetNObjects ()
  {
    if (fHasNEntries)
      return TreeReadingPolicy::ReadCountingNumber(fNEntriesName);
    else if (fIsCart)
      return TreeReadingPolicy::ReadDimension(fCartX1);
    else
      return TreeReadingPolicy::ReadDimension(fPhi);
  }

  Particle* CreateObject (unsigned int i)
  {
    Particle *p = new Particle();

    if (fIsCart) {
      p->SetCartVector(TreeReadingPolicy::ReadDecimalNumberArray(fCartX0, i),
                       TreeReadingPolicy::ReadDecimalNumberArray(fCartX1, i),
                       TreeReadingPolicy::ReadDecimalNumberArray(fCartX2, i),
                       TreeReadingPolicy::ReadDecimalNumberArray(fCartX3, i));
    }
    else if (fIsE) {
    }
    else if (fIsM) {
    }
    else if (fIsCartMET) {
    }
    else if (fIsPhiEtMET) {
    }
    if (fHasCharge)
      p->SetCharge(TreeReadingPolicy::ReadDecimalNumberArray(fCharge, i));
    if (fHasID)
      p->SetID(TreeReadingPolicy::ReadIntegerNumberArray(fID, i));

    return p;
  }

private:
  Bool_t                 fIsCart,
                         fIsE,
                         fIsM,
                         fIsCartMET,
                         fIsPhiEtMET,
                         fHasNEntries,
                         fHasCharge,
                         fHasID;
  TString                fName,
                         fNEntriesName,
                         fCartX0,
                         fCartX1,
                         fCartX2,
                         fCartX3,
                         fM,
                         fE,
                         fPt,
                         fEta,
                         fPhi,
                         fCharge,
                         fID;
};





template < class Particle, class TreeReadingPolicy = AnalysisTreeReaderPolicy >
class SimpleCartParticleFromTree: public TreeReadingPolicy {
protected:
  SimpleCartParticleFromTree (TString name) :
    fHasNEntries(kFALSE), fHasCharge(kFALSE), fHasID(kFALSE)
  {
    fName = name;
    fCartX0 = TString::Format("%s:x0", fName.Data());
    fCartX1 = TString::Format("%s:x1", fName.Data());
    fCartX2 = TString::Format("%s:x2", fName.Data());
    fCartX3 = TString::Format("%s:x3", fName.Data());
    fCharge = TString::Format("%s:charge", fName.Data());
    fID = TString::Format("%s:id", fName.Data());
    fNEntriesName = TString::Format("%s:nentries", fName.Data());
  }
  ~SimpleCartParticleFromTree () {}

  void Init (Option_t * /*option*/, AnalysisTreeReader *tr)
  {
    if (!tr->CheckBranchMapNickname(fCartX0) || !tr->CheckBranchMapNickname(fCartX1) || 
        !tr->CheckBranchMapNickname(fCartX2) || !tr->CheckBranchMapNickname(fCartX3))
      HAL::HALException(fName.Prepend("Couldn't determine how to import cartesian 4-vector in algorithm ").Data());
    if (tr->CheckBranchMapNickname(fNEntriesName))
      fHasNEntries = kTRUE;
    if (tr->CheckBranchMapNickname(fCharge))
      fHasCharge = kTRUE;
    if (tr->CheckBranchMapNickname(fID))
      fHasID = kTRUE;

    fCartX0 = tr->GetBranchName(fCartX0);
    fCartX1 = tr->GetBranchName(fCartX1);
    fCartX2 = tr->GetBranchName(fCartX2);
    fCartX3 = tr->GetBranchName(fCartX3);
    fCharge = tr->GetBranchName(fCharge);
    fID = tr->GetBranchName(fID);
    fNEntriesName = tr->GetBranchName(fNEntriesName);

    TreeReadingPolicy::PassRawData(tr);
  }

  unsigned int GetNObjects ()
  {
    if (fHasNEntries)
      return TreeReadingPolicy::ReadCountingNumber(fNEntriesName);
    else
      return TreeReadingPolicy::ReadDimension(fCartX1);
  }

  Particle* CreateObject (unsigned int i)
  {
    Particle *p = new Particle();

    p->SetCartVector(TreeReadingPolicy::ReadDecimalNumberArray(fCartX0, i),
                     TreeReadingPolicy::ReadDecimalNumberArray(fCartX1, i),
                     TreeReadingPolicy::ReadDecimalNumberArray(fCartX2, i),
                     TreeReadingPolicy::ReadDecimalNumberArray(fCartX3, i));
    if (fHasCharge)
      p->SetCharge(TreeReadingPolicy::ReadDecimalNumberArray(fCharge, i));
    if (fHasID)
      p->SetID(TreeReadingPolicy::ReadIntegerNumberArray(fID, i));

    return p;
  }

private:
  Bool_t                 fIsCart,
                         fIsE,
                         fIsM,
                         fIsCartMET,
                         fIsPhiEtMET,
                         fHasNEntries,
                         fHasCharge,
                         fHasID;
  TString                fName,
                         fNEntriesName,
                         fCartX0,
                         fCartX1,
                         fCartX2,
                         fCartX3,
                         fCharge,
                         fID;
};





template < class Particle, class TreeReadingPolicy = AnalysisTreeReaderPolicy >
class SimpleEPtEtaPhiParticleFromTree: public TreeReadingPolicy {
protected:
  SimpleEPtEtaPhiParticleFromTree (TString name) :
    fHasNEntries(kFALSE), fHasCharge(kFALSE), fHasID(kFALSE)
  {
    fName = name;
    fE = TString::Format("%s:e", fName.Data());
    fPt = TString::Format("%s:pt", fName.Data());
    fEta = TString::Format("%s:eta", fName.Data());
    fPhi = TString::Format("%s:phi", fName.Data());
    fCharge = TString::Format("%s:charge", fName.Data());
    fID = TString::Format("%s:id", fName.Data());
    fNEntriesName = TString::Format("%s:nentries", fName.Data());
  }
  ~SimpleEPtEtaPhiParticleFromTree () {}

  void Init (Option_t * /*option*/, AnalysisTreeReader *tr)
  {
    if (!tr->CheckBranchMapNickname(fE) || !tr->CheckBranchMapNickname(fPt) || 
        !tr->CheckBranchMapNickname(fEta) || !tr->CheckBranchMapNickname(fPhi))
      HAL::HALException(fName.Prepend("Couldn't determine how to import E, pT, eta, phi 4-vector in algorithm ").Data());
    if (tr->CheckBranchMapNickname(fNEntriesName))
      fHasNEntries = kTRUE;
    if (tr->CheckBranchMapNickname(fCharge))
      fHasCharge = kTRUE;
    if (tr->CheckBranchMapNickname(fID))
      fHasID = kTRUE;

    fE = tr->GetBranchName(fE);
    fPt = tr->GetBranchName(fPt);
    fEta = tr->GetBranchName(fEta);
    fPhi = tr->GetBranchName(fPhi);
    fCharge = tr->GetBranchName(fCharge);
    fID = tr->GetBranchName(fID);
    fNEntriesName = tr->GetBranchName(fNEntriesName);

    TreeReadingPolicy::PassRawData(tr);
  }

  unsigned int GetNObjects ()
  {
    if (fHasNEntries)
      return TreeReadingPolicy::ReadCountingNumber(fNEntriesName);
    else
      return TreeReadingPolicy::ReadDimension(fPhi);
  }

  Particle* CreateObject (unsigned int i)
  {
    Particle *p = new Particle();

    p->SetEPtEtaPhiVector(TreeReadingPolicy::ReadDecimalNumberArray(fE, i),
                          TreeReadingPolicy::ReadDecimalNumberArray(fPt, i),
                          TreeReadingPolicy::ReadDecimalNumberArray(fEta, i),
                          TreeReadingPolicy::ReadDecimalNumberArray(fPhi, i));
    if (fHasCharge)
      p->SetCharge(TreeReadingPolicy::ReadDecimalNumberArray(fCharge, i));
    if (fHasID)
      p->SetID(TreeReadingPolicy::ReadIntegerNumberArray(fID, i));

    return p;
  }

private:
  Bool_t                 fHasNEntries,
                         fHasCharge,
                         fHasID;
  TString                fName,
                         fNEntriesName,
                         fE,
                         fPt,
                         fEta,
                         fPhi,
                         fCharge,
                         fID;
};



template < class Particle, class TreeReadingPolicy = AnalysisTreeReaderPolicy >
class SimpleMPtEtaPhiParticleFromTree: public TreeReadingPolicy {
protected:
  SimpleMPtEtaPhiParticleFromTree (TString name) :
    fHasNEntries(kFALSE), fHasCharge(kFALSE), fHasID(kFALSE)
  {
    fName = name;
    fM = TString::Format("%s:m", fName.Data());
    fPt = TString::Format("%s:pt", fName.Data());
    fEta = TString::Format("%s:eta", fName.Data());
    fPhi = TString::Format("%s:phi", fName.Data());
    fCharge = TString::Format("%s:charge", fName.Data());
    fID = TString::Format("%s:id", fName.Data());
    fNEntriesName = TString::Format("%s:nentries", fName.Data());
  }
  ~SimpleMPtEtaPhiParticleFromTree () {}

  void Init (Option_t * /*option*/, AnalysisTreeReader *tr)
  {
    if (!tr->CheckBranchMapNickname(fM) || !tr->CheckBranchMapNickname(fPt) || 
        !tr->CheckBranchMapNickname(fEta) || !tr->CheckBranchMapNickname(fPhi))
      HAL::HALException(fName.Prepend("Couldn't determine how to import M, pT, eta, phi 4-vector in algorithm ").Data());
    if (tr->CheckBranchMapNickname(fNEntriesName))
      fHasNEntries = kTRUE;
    if (tr->CheckBranchMapNickname(fCharge))
      fHasCharge = kTRUE;
    if (tr->CheckBranchMapNickname(fID))
      fHasID = kTRUE;

    fM = tr->GetBranchName(fM);
    fPt = tr->GetBranchName(fPt);
    fEta = tr->GetBranchName(fEta);
    fPhi = tr->GetBranchName(fPhi);
    fCharge = tr->GetBranchName(fCharge);
    fID = tr->GetBranchName(fID);
    fNEntriesName = tr->GetBranchName(fNEntriesName);

    TreeReadingPolicy::PassRawData(tr);
  }

  unsigned int GetNObjects ()
  {
    if (fHasNEntries)
      return TreeReadingPolicy::ReadCountingNumber(fNEntriesName);
    else
      return TreeReadingPolicy::ReadDimension(fPhi);
  }

  Particle* CreateObject (unsigned int i)
  {
    Particle *p = new Particle();

    p->SetMPtEtaPhiVector(TreeReadingPolicy::ReadDecimalNumberArray(fM, i),
                          TreeReadingPolicy::ReadDecimalNumberArray(fPt, i),
                          TreeReadingPolicy::ReadDecimalNumberArray(fEta, i),
                          TreeReadingPolicy::ReadDecimalNumberArray(fPhi, i));
    if (fHasCharge)
      p->SetCharge(TreeReadingPolicy::ReadDecimalNumberArray(fCharge, i));
    if (fHasID)
      p->SetID(TreeReadingPolicy::ReadIntegerNumberArray(fID, i));

    return p;
  }

private:
  Bool_t                 fHasNEntries,
                         fHasCharge,
                         fHasID;
  TString                fName,
                         fNEntriesName,
                         fM,
                         fPt,
                         fEta,
                         fPhi,
                         fCharge,
                         fID;
};



template < class Particle, class TreeReadingPolicy = AnalysisTreeReaderPolicy >
class SimpleCartTransParticleFromTree: public TreeReadingPolicy {
protected:
  SimpleCartTransParticleFromTree (TString name) :
    fHasNEntries(kFALSE), fHasCharge(kFALSE), fHasID(kFALSE)
  {
    fName = name;
    fCartX1 = TString::Format("%s:x1", fName.Data());
    fCartX2 = TString::Format("%s:x2", fName.Data());
    fCharge = TString::Format("%s:charge", fName.Data());
    fID = TString::Format("%s:id", fName.Data());
    fNEntriesName = TString::Format("%s:nentries", fName.Data());
  }
  ~SimpleCartTransParticleFromTree () {}

  void Init (Option_t * /*option*/, AnalysisTreeReader *tr)
  {
    if (!tr->CheckBranchMapNickname(fCartX1) || !tr->CheckBranchMapNickname(fCartX2))
      HAL::HALException(fName.Prepend("Couldn't determine how to import cartesian transverse 4-vector in algorithm ").Data());
    if (tr->CheckBranchMapNickname(fNEntriesName))
      fHasNEntries = kTRUE;
    if (tr->CheckBranchMapNickname(fCharge))
      fHasCharge = kTRUE;
    if (tr->CheckBranchMapNickname(fID))
      fHasID = kTRUE;

    fCartX1 = tr->GetBranchName(fCartX1);
    fCartX2 = tr->GetBranchName(fCartX2);
    fCharge = tr->GetBranchName(fCharge);
    fID = tr->GetBranchName(fID);
    fNEntriesName = tr->GetBranchName(fNEntriesName);

    TreeReadingPolicy::PassRawData(tr);
  }

  unsigned int GetNObjects ()
  {
    if (fHasNEntries)
      return TreeReadingPolicy::ReadCountingNumber(fNEntriesName);
    else
      return TreeReadingPolicy::ReadDimension(fCartX1);
  }

  Particle* CreateObject (unsigned int i)
  {
    Particle *p = new Particle();
    long double x1 = TreeReadingPolicy::ReadDecimalNumberArray(fCartX1, i),
                x2 = TreeReadingPolicy::ReadDecimalNumberArray(fCartX2, i);

    p->SetCartVector(TMath::Sqrt(x1*x1 + x2*x2),
                     x1,
                     x2,
                     0.0);
    if (fHasCharge)
      p->SetCharge(TreeReadingPolicy::ReadDecimalNumberArray(fCharge, i));
    if (fHasID)
      p->SetID(TreeReadingPolicy::ReadIntegerNumberArray(fID, i));

    return p;
  }

private:
  Bool_t                 fHasNEntries,
                         fHasCharge,
                         fHasID;
  TString                fName,
                         fNEntriesName,
                         fCartX1,
                         fCartX2,
                         fCharge,
                         fID;
};



template < class Particle, class TreeReadingPolicy = AnalysisTreeReaderPolicy >
class SimpleEtPhiTransParticleFromTree: public TreeReadingPolicy {
protected:
  SimpleEtPhiTransParticleFromTree (TString name) :
    fHasNEntries(kFALSE), fHasCharge(kFALSE), fHasID(kFALSE)
  {
    fName = name;
    fEt = TString::Format("%s:et", fName.Data());
    fPhi = TString::Format("%s:phi", fName.Data());
    fCharge = TString::Format("%s:charge", fName.Data());
    fID = TString::Format("%s:id", fName.Data());
    fNEntriesName = TString::Format("%s:nentries", fName.Data());
  }
  ~SimpleEtPhiTransParticleFromTree () {}

  void Init (Option_t * /*option*/, AnalysisTreeReader *tr)
  {
    if (!tr->CheckBranchMapNickname(fEt) || !tr->CheckBranchMapNickname(fPhi))
      HAL::HALException(fName.Prepend("Couldn't determine how to import eT, phi transverse 4-vector in algorithm ").Data());
    if (tr->CheckBranchMapNickname(fNEntriesName))
      fHasNEntries = kTRUE;
    if (tr->CheckBranchMapNickname(fCharge))
      fHasCharge = kTRUE;
    if (tr->CheckBranchMapNickname(fID))
      fHasID = kTRUE;

    fEt = tr->GetBranchName(fEt);
    fPhi = tr->GetBranchName(fPhi);
    fCharge = tr->GetBranchName(fCharge);
    fID = tr->GetBranchName(fID);
    fNEntriesName = tr->GetBranchName(fNEntriesName);

    TreeReadingPolicy::PassRawData(tr);
  }

  unsigned int GetNObjects ()
  {
    if (fHasNEntries)
      return TreeReadingPolicy::ReadCountingNumber(fNEntriesName);
    else
      return TreeReadingPolicy::ReadDimension(fPhi);
  }

  Particle* CreateObject (unsigned int i)
  {
    Particle *p = new Particle();
    long double et = TreeReadingPolicy::ReadDecimalNumberArray(fEt, i),
                phi = TreeReadingPolicy::ReadDecimalNumberArray(fPhi, i);

    p->SetCartVector(et,
                     et*TMath::Cos(phi),
                     et*TMath::Sin(phi),
                     0.0);
    if (fHasCharge)
      p->SetCharge(TreeReadingPolicy::ReadDecimalNumberArray(fCharge, i));
    if (fHasID)
      p->SetID(TreeReadingPolicy::ReadIntegerNumberArray(fID, i));

    return p;
  }

private:
  Bool_t                 fHasNEntries,
                         fHasCharge,
                         fHasID;
  TString                fName,
                         fNEntriesName,
                         fEt,
                         fPhi,
                         fCharge,
                         fID;
};



//template < class Particle >
//class AnalysisTreeReaderAutoSimpleParticleCreator: 
//  public SimpleParticleAutoFromTree<Particle, AnalysisTreeReaderPolicy> {
//protected:
//  ~AnalysisTreeReaderAutoSimpleParticleCreator () {}
//};

} /* HAL */

#endif
