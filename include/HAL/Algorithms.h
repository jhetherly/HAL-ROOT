/*
 * Generic algorithms for fast analysis development
 *
 * DEV NOTE:
 * Importing and Reconstruction Algorithms create and destroy objects in 'data'
 *  - creates <name>:4-vec 1D array
 * All other Algorithms should never create or destroy objects in 'data'
 *  - creates <name>:ref_name scalar that points to the actual data (index refers to ref_name indices)
 * ref_names should always reference direct access data, never reference a reference
 * */

#ifndef HAL_ALGORITHMS
#define HAL_ALGORITHMS

#include <TRandom3.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <cstdarg>
#include <string>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/AnalysisUtils.h>
#include <HAL/Algorithm.h>
#include <HAL/CutAlgorithm.h>
#include <HAL/AnalysisData.h>
#include <HAL/AnalysisTreeReader.h>
#include <HAL/AnalysisTreeWriter.h>

#include <iostream>

namespace HAL
{

/*
 * Generic base class algorithms and functions
 * Most of these are ABC's
 * */

namespace internal
{

/*
 * General function for determining how to access information
 * stored in AnalysisData (either reference or direct access)
 * */
bool determineAccessProtocol(HAL::AnalysisData *data, TString &RawInput, TString &RealInput);

/*
 * Algorithm for importing an array of TLorentzVecotr's from and TTree.
 * */
class ImportTLVAlgo : public HAL::Algorithm {
public:
  ImportTLVAlgo (TString name, TString title) : HAL::Algorithm(name, title) {}
  virtual ~ImportTLVAlgo () {}

protected:
  virtual void Exec (Option_t* /*option*/) {}
  virtual void Exec (unsigned n);
  virtual void Clear (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned) = 0;
};

/*
 * Algorithm for finding the nth highest/lowest element in an AnalysisData object
 * */
class NthElementAlgo : public HAL::Algorithm {
public:
  NthElementAlgo (TString name, TString title, TString input, unsigned n) :
    HAL::Algorithm(name, title), fN(n), fInput(input) {}
  virtual ~NthElementAlgo () {}

  virtual TString   SortTag () = 0;
  virtual bool      operator() (long long, long long) = 0;
  virtual void      Sort (std::vector<long long>&) = 0;

protected:
  virtual void      Exec (Option_t* /*option*/);
  virtual void      Clear (Option_t* /*option*/);

  unsigned  fN;
  TString   fInput, fElementName;
};

/*
 * Algorithm for filtering many particle by their TLV
 * */
class FilterTLVAlgo : public Algorithm {
public:
  FilterTLVAlgo (TString name, TString title, TString input) :
    Algorithm(name, title), fInput(input) {}
  virtual ~FilterTLVAlgo () {}

  virtual bool FilterPredicate (TLorentzVector*) = 0;
protected:
  virtual void Exec (Option_t* /*option*/);
  virtual void Clear (Option_t* /*option*/);

  TString fInput;
};

/*
 * Algorithm for cutting many particle on their TLV
 * */
class ParticlesTLVCut : public CutAlgorithm {
public:
  ParticlesTLVCut (TString name, TString title, TString input) :
    CutAlgorithm(name, title), fInput(input) {}
  virtual ~ParticlesTLVCut () {}

  // this should return true if particle passed cut
  virtual bool CutPredicate (TLorentzVector*) = 0;

protected:
  virtual void Exec (Option_t* /*option*/);

  TString   fInput;
};

/*
 * Algorithm for the exporting of simple quantities from a
 * particle's TLV
 * */
class SingleParticleTLVStore : public Algorithm {
public:
  SingleParticleTLVStore (TString name, TString title, TString input, TString bname) :
    Algorithm(name, title), fBranchName(bname), fInput(input) {}
  virtual ~SingleParticleTLVStore () {}


protected:
  virtual void    Exec (Option_t* /*option*/);
  virtual double  StoreValue (TLorentzVector*) = 0;

  TString         fBranchName, fInput;
};

/*
 * Algorithm for the exporting of simple quantities from
 * several particles' TLVs
 * */
class ParticlesTLVStore : public Algorithm {
public:
  ParticlesTLVStore (TString name, TString title, TString input, TString bname, TString num = "") :
    Algorithm(name, title), fMany(num.EqualTo("many", TString::kIgnoreCase)), 
    fBranchName(bname), fInput(input) {}
  virtual ~ParticlesTLVStore () {}


protected:
  virtual void    Exec (Option_t* /*option*/);
  virtual double  StoreValue (TLorentzVector*) = 0;

  bool            fMany;
  TString         fBranchName, fInput;
};

} /* internal */ 



namespace Algorithms
{


/*
 * Importing algorithms
 * */

/*
 * Import algorithm to build particles from cartesian,
 * (pt,eta,phi,energy), or (pt,eta,phi,mass) components of a
 * 4-vector. It can also determine how many elements to read
 * in based on whether you give a number to read, branchmap
 * to scan, or implicitly gather it from the length of the
 * other required branches. It will also determine the type
 * of 4-vector to create based on the branchmaps that are 
 * defined.
 *
 * Prerequisites:
 *  None
 * Required Branch Maps:
 *  <name>:x0
 *  <name>:x1
 *  <name>:x2
 *  <name>:x3
 *  OR
 *  <name>:pt
 *  <name>:eta
 *  <name>:phi
 *  <name>:e
 *  OR
 *  <name>:pt
 *  <name>:eta
 *  <name>:phi
 *  <name>:m
 * Optional Branch Maps:
 *  <name>:nentries
 * UserData Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices)
 */
class ImportTLV : public HAL::internal::ImportTLVAlgo {
public:
  ImportTLV (TString name, TString title, unsigned n = 0);
  virtual ~ImportTLV () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Init (Option_t* /*option*/);
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);

private:
  unsigned  fN;
  bool      fIsCart, fIsE, fIsM;
};




/*
 * Reconstruction Algorithms
 * */

/*
 * Reconstruction algorithm to build particles from the
 * vector addition of other particles.
 * The unknown parameters should be a series of string literals
 * representing the algorithms to combine
 *
 * Prerequisites:
 *  Stored Particles
 * Required Branch Maps:
 *  None
 * UserData Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:nparents (scalar: number of parent particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices)
 *  <name>:parent_ref_name (1D array: ref_names of parents)
 *  <name>:parent_index (1D array: indices of parents)
 */
class VecAddReco : public Algorithm {
public:
  VecAddReco (TString name, TString title, long long length, ...);
  virtual ~VecAddReco();

protected:
  virtual void  Exec (Option_t* /*option*/);
  virtual void  Clear (Option_t* /*option*/);

private:
  const char**  fParentNames;
  long long     fLength;
};




/*
 * Filtering Algorithms
 * */

/*
 * Select the particle with nth highest/lowest TLV property.
 * This property can be:
 * transverse momentum, mass, energy, transverse energy, 3-momentum magnitude
 * pt,                  m,    e,      et,                p3
 * The 'end' parameter describes how to rank the property:
 * high (rank = nth highest property)
 * low (rank = nth lowest property)
 *
 * Prerequisites:
 *  Stored particles
 * Required Branch Maps:
 *  None
 * UserData Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:ref_name ((scalar): string name of reference particles to use)
 *  <name>:index (1D array: of indices (points to the nth highest pT of ref_name))
 * */
class RankSelectionTLV : public internal::NthElementAlgo {
public:
  RankSelectionTLV (TString name, TString title, TString input, unsigned rank, 
                    TString property, TString end = "high");
  virtual ~RankSelectionTLV () {}

  virtual TString       SortTag ();
  virtual bool          operator() (long long, long long);
  virtual void          Sort (std::vector<long long>&);

protected:
  bool      fPt, fM, fE, fEt, fP3, fHigh, fLow;
  TString   fTLVProperty, fEnd;
};


/*
 * Select the particles with TLV property less than, greater than,
 * or within a window of given values.
 * This property can be:
 * transverse momentum, mass, energy, transverse energy, 3-momentum magnitude, eta, phi
 * pt,                  m,    e,      et,                p3,                   eta, phi
 * The 'end' parameter describes how to filter the property:
 * high (upper limit)
 * low (lower limit)
 *
 * Prerequisites:
 *  Stored particles
 * Required Branch Maps:
 *  None
 * UserData Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:ref_name ((scalar): string name of reference particles to use)
 *  <name>:index (1D array: of indices)
 * */
class SelectTLV : public internal::FilterTLVAlgo {
public:
  SelectTLV (TString name, TString title, TString input, TString property, 
      double value, TString end = "low");
  SelectTLV (TString name, TString title, TString input, TString property, 
      double low, double high);
  virtual ~SelectTLV () {}

  virtual bool FilterPredicate(TLorentzVector*);

private:
  void      Setup ();

  double    fHighLimit, fLowLimit;
  bool      fPt, fM, fE, fEt, fP3, fEta, fPhi, fHigh, fLow, fWindow;
  TString   fTLVProperty, fEnd;
};




/*
 * Cutting Algorithms
 * */

/*
 * Cut on particles' pT (lower limit)
 * (logical 'and')
 *
 * Prerequisites:
 *  Stored particle
 * Required Branch Maps:
 *  None
 * UserData Output:
 *  None
 * */
class CutTLV : public internal::ParticlesTLVCut {
public:
  CutTLV (TString name, TString title, TString input, TString property, 
      double value, TString end = "low");
  CutTLV (TString name, TString title, TString input, TString property, 
      double low, double high);
  virtual ~CutTLV () {}

  virtual bool CutPredicate (TLorentzVector *vec);

private:
  void      Setup ();

  double    fHighLimit, fLowLimit;
  bool      fPt, fM, fE, fEt, fP3, fEta, fPhi, fHigh, fLow, fWindow;
  TString   fTLVProperty, fEnd;
};

/*
 * Cut on number of objects
 * (logical 'and' or 'or')
 *
 * Prerequisites:
 *  Stored objects
 * Required Branch Maps:
 *  None
 * UserData Output:
 *  None
 * */
class CutNObjects : public CutAlgorithm {
public:
  CutNObjects (TString name, TString title, TString logic, long long n, long long length, ...);
  virtual ~CutNObjects () {}

protected:
  virtual void Exec (Option_t* /*option*/);

private:
  bool          fAnd, fOr;
  long long     fLength, fN;
  const char**  fParticleNames;
};




/*
 * Exporting Algorithms
 * */

/*
 * Store a TLV property of a particle or particles
 *
 * Prerequisites:
 *  Stored particle
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class StoreTLV : public internal::ParticlesTLVStore {
public:
  StoreTLV (TString name, TString title, TString input, TString property, TString bname, TString num = "");
  virtual ~StoreTLV () {}

protected:
  virtual double  StoreValue (TLorentzVector*);

  bool            fPt, fM, fE, fEt, fP3, fEta, fPhi;
};


///*
// * Store a TLV property of particles
// *
// * Prerequisites:
// *  Stored particles
// * Branch Maps Needed:
// *  None
// * Output:
// *  None
// * */
//class StoreManyTLV : public internal::SingleParticleTLVStore {
//public:
//  StoreManyTLV (TString name, TString title, TString input, TString property, TString bname) :
//    ParticlesTLVStore(name, title, input, bname) {}
//  virtual ~StoreSingleTLV () {}
//
//protected:
//  virtual double  StoreValue (TLorentzVector*);
//};

} /* Algorithms */ 



/*
 * Importing algorithms
 * */




/*
 * Reconstruction Algorithms
 * */




/*
 * Filtering Algorithms
 * */




/*
 * Cutting Algorithms
 * */




/*
 * Exporting Algorithms
 * */

/*
 * Store the pT of a particle
 *
 * Prerequisites:
 *  Stored particle (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0000 : public internal::SingleParticleTLVStore {
public:
  EA0000 (TString name, TString title, TString input, TString bname) :
    SingleParticleTLVStore(name, title, input, bname) {}
  virtual ~EA0000 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the eta of a particle
 *
 * Prerequisites:
 *  Stored particle (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0001 : public internal::SingleParticleTLVStore {
public:
  EA0001 (TString name, TString title, TString input, TString bname) :
    SingleParticleTLVStore(name, title, input, bname) {}
  virtual ~EA0001 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the phi of a particle
 *
 * Prerequisites:
 *  Stored particle (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0002 : public internal::SingleParticleTLVStore {
public:
  EA0002 (TString name, TString title, TString input, TString bname) :
    SingleParticleTLVStore(name, title, input, bname) {}
  virtual ~EA0002 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the mass of a particle
 *
 * Prerequisites:
 *  Stored particles (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0003 : public internal::SingleParticleTLVStore {
public:
  EA0003 (TString name, TString title, TString input, TString bname) :
    SingleParticleTLVStore(name, title, input, bname) {}
  virtual ~EA0003 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the energy of a particle
 *
 * Prerequisites:
 *  Stored particles (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0004 : public internal::SingleParticleTLVStore {
public:
  EA0004 (TString name, TString title, TString input, TString bname) :
    SingleParticleTLVStore(name, title, input, bname) {}
  virtual ~EA0004 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the pT of particles
 *
 * Prerequisites:
 *  Stored particles (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0010 : public internal::ParticlesTLVStore {
public:
  EA0010 (TString name, TString title, TString input, TString bname) :
    ParticlesTLVStore(name, title, input, bname) {}
  virtual ~EA0010 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the eta of particles
 *
 * Prerequisites:
 *  Stored particles (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0011 : public internal::ParticlesTLVStore {
public:
  EA0011 (TString name, TString title, TString input, TString bname) :
    ParticlesTLVStore(name, title, input, bname) {}
  virtual ~EA0011 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the phi of particles
 *
 * Prerequisites:
 *  Stored particles (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0012 : public internal::ParticlesTLVStore {
public:
  EA0012 (TString name, TString title, TString input, TString bname) :
    ParticlesTLVStore(name, title, input, bname) {}
  virtual ~EA0012 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the mass of particles
 *
 * Prerequisites:
 *  Stored particles (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0013 : public internal::ParticlesTLVStore {
public:
  EA0013 (TString name, TString title, TString input, TString bname) :
    ParticlesTLVStore(name, title, input, bname) {}
  virtual ~EA0013 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the energy of particles
 *
 * Prerequisites:
 *  Stored particles (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0014 : public internal::ParticlesTLVStore {
public:
  EA0014 (TString name, TString title, TString input, TString bname) :
    ParticlesTLVStore(name, title, input, bname) {}
  virtual ~EA0014 () {}

protected:
  virtual double  StoreValue (TLorentzVector*);
};

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHMS */

