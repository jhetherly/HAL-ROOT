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
#include <TNamed.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <cstdarg>
#include <string>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/AnalysisUtils.h>
#include <HAL/Algorithm.h>
#include <HAL/CutAlgorithm.h>
#include <HAL/AnalysisUtils.h>
#include <HAL/AnalysisData.h>
#include <HAL/AnalysisTreeReader.h>
#include <HAL/AnalysisTreeWriter.h>
#include <HAL/GenericParticle.h>
#include <HAL/GenericData.h>


namespace HAL
{

/*
 * Generic base class algorithms
 * Most of these are ABC's
 * */

namespace internal
{

/*
 * Algorithm for importing an array of TLorentzVecotr's from a TTree.
 * */
class ImportParticleAlgo : public HAL::Algorithm {
public:
  ImportParticleAlgo (TString name, TString title);
  virtual ~ImportParticleAlgo () {}

protected:
  virtual void Init (Option_t* /*option*/);
  virtual void Exec (Option_t* /*option*/) {}
  virtual void Exec (unsigned n);
  virtual void Clear (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned) = 0;

  bool      fIsCart, fIsE, fIsM;
  bool      fIsCartMET, fIsPhiEtMET;
  bool      fHasCharge, fHasID;
  TString   fCartX0, fCartX1, fCartX2, fCartX3, fPt, fEt;
  TString   fEta, fPhi, fM, fE;
  TString   fCharge, fID;
  TString   fNEntriesName;
};

/*
 * Algorithm for importing a scalar value from a TTree.
 * */
class ImportValueAlgo : public HAL::Algorithm {
public:
  ImportValueAlgo (TString name, TString title);
  virtual ~ImportValueAlgo () {}

  virtual void  StoreValue (HAL::GenericData*) = 0;

protected:
  virtual void  Exec (Option_t* /*option*/);
  virtual void  Clear (Option_t* /*option*/);

  TString   fUserDataLabel;
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
  virtual bool      operator() (HAL::ParticlePtr, HAL::ParticlePtr) = 0;
  virtual void      Sort (HAL::ParticlePtrs&) = 0;

protected:
  virtual void      Exec (Option_t* /*option*/);
  virtual void      Clear (Option_t* /*option*/);

  unsigned           fN;
  TString            fInput;
};

/*
 * Algorithm for filtering particles by their TLV
 * */
class FilterParticleAlgo : public Algorithm {
public:
  FilterParticleAlgo (TString name, TString title, TString input) :
    Algorithm(name, title), fInput(input) {}
  virtual ~FilterParticleAlgo () {}

  virtual bool FilterPredicate (HAL::ParticlePtr) = 0;
  
protected:
  virtual void Exec (Option_t* /*option*/);
  virtual void Clear (Option_t* /*option*/);

  TString fInput;
};

/*
 * Algorithm for filtering particles compared to a reference particle
 * by their TLVs
 * */
class FilterRefTLVAlgo : public Algorithm {
public:
  FilterRefTLVAlgo (TString name, TString title, TString input, TString others) :
    Algorithm(name, title), fInput(input), fOthers(others) {}
  virtual ~FilterRefTLVAlgo () {}

  virtual bool FilterPredicate (HAL::ParticlePtr, HAL::ParticlePtr) = 0;

protected:
  virtual void Exec (Option_t* /*option*/);
  virtual void Clear (Option_t* /*option*/);

  TString fInput, fOthers;
};

/*
 * Algorithm for filtering if a particle is a parent of the input refernece
 * logic: 'ex' or 'in'
 * */
class FilterParentAlgo : public Algorithm {
public:
  FilterParentAlgo (TString name, TString title, TString input, TString logic, long long length, ...);
  virtual ~FilterParentAlgo () {}

  virtual bool FilterPredicate (long long in, long long ref) = 0;
  
protected:
  virtual void Exec (Option_t* /*option*/);
  virtual void Clear (Option_t* /*option*/);

  const char          **fRefNames;
  bool                  fExclude;
  std::set<long long>   fRefIndices;
  TString               fInput;
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
  virtual void    StoreValue (HAL::AnalysisTreeWriter*, long long, HAL::ParticlePtr) = 0;

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
 *  OR
 *  <name>:x1
 *  <name>:x2
 *  OR
 *  <name>:pt
 *  <name>:phi
 *  OR
 *  <name>:et
 *  <name>:phi
 * Optional Branch Maps:
 *  <name>:nentries
 *  <name>:charge
 *  <name>:id
 * UserData Output:
 *  <name>
 */
class ImportParticle : public HAL::internal::ImportParticleAlgo {
public:
  ImportParticle (TString name, TString title, unsigned n = 0);
  virtual ~ImportParticle () {}

protected:
  using ImportParticleAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);

private:
  unsigned  fN;
};


/*
 * Import algorithm to store a bool value from a TTree
 *
 * Prerequisites:
 *  None
 * Required Branch Maps:
 *  <name>:bool
 * UserData Output:
 *  <name>
 *  <name>:value
 */
class ImportBool : public HAL::internal::ImportValueAlgo {
public:
  ImportBool (TString name, TString title);
  virtual ~ImportBool () {}

  virtual void  StoreValue (HAL::GenericData*);

private:
  TString   fValue;
};


/*
 * Import algorithm to store a integer value from a TTree
 *
 * Prerequisites:
 *  None
 * Required Branch Maps:
 *  <name>:integer
 * UserData Output:
 *  <name>
 *  <name>:value
 */
class ImportInteger : public HAL::internal::ImportValueAlgo {
public:
  ImportInteger (TString name, TString title);
  virtual ~ImportInteger () {}

  virtual void  StoreValue (HAL::GenericData*);

private:
  TString   fValue;
};


/*
 * Import algorithm to store a counting value from a TTree
 *
 * Prerequisites:
 *  None
 * Required Branch Maps:
 *  <name>:counting
 * UserData Output:
 *  <name>
 *  <name>:value
 */
class ImportCounting : public HAL::internal::ImportValueAlgo {
public:
  ImportCounting (TString name, TString title);
  virtual ~ImportCounting () {}

  virtual void  StoreValue (HAL::GenericData*);

private:
  TString   fValue;
};


/*
 * Import algorithm to store a decimal value from a TTree
 *
 * Prerequisites:
 *  None
 * Required Branch Maps:
 *  <name>:decimal
 * UserData Output:
 *  <name>
 *  <name>:value
 */
class ImportDecimal : public HAL::internal::ImportValueAlgo {
public:
  ImportDecimal (TString name, TString title);
  virtual ~ImportDecimal () {}

  virtual void  StoreValue (HAL::GenericData*);

private:
  TString   fValue;
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
class ParticleRankSelection : public internal::NthElementAlgo {
public:
  ParticleRankSelection (TString name, TString title, TString input, unsigned rank, 
                         TString property, TString end = "high");
  virtual ~ParticleRankSelection () {}

  virtual TString       SortTag ();
  virtual bool          operator() (ParticlePtr, ParticlePtr);
  virtual void          Sort (ParticlePtrs&);

protected:
  bool      fPt, fM, fE, fEt, fP3, fHigh, fLow;
  TString   fTLVProperty, fEnd;
};


/*
 * Select particles with TLV property less than, greater than,
 * or within a window of given values.
 * This property can be:
 * transverse momentum, mass, energy, transverse energy, 3-momentum magnitude, eta, phi
 * pt,                  m,    e,      et,                p3,                   eta, phi
 * The 'op' parameter describes how to filter the property: 
 * == or =, !=, >, <, >=, <=
 * The 'inclusion' parameter describes how to filter the property: 
 * inclusive or in, exclusive or out
 * The varargs constructor needs integers if property is id, doubles otherwise
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
class SelectParticle : public internal::FilterParticleAlgo {
public:
  SelectParticle (TString name, TString title, TString input, TString property, 
      TString op, double value);
  SelectParticle (TString name, TString title, TString input, TString property, 
      TString inclusion, double low, double high);
  SelectParticle (TString name, TString title, TString input, TString property, 
      int length, ...);
  virtual ~SelectParticle () {}

  virtual bool FilterPredicate(HAL::ParticlePtr);

private:
  void      Setup ();

  double    fHighLimit, fLowLimit;
  bool      fPt, fM, fE, fEt, fP3, fEta, fPhi, fCharge, fID;
  bool      fEqual, fNotEqual, fLessThan, fGreaterThan, fLessThanEqual, fGreaterThanEqual;
  bool      fIn, fOut;
  bool      fSingleEnd, fWindow, fList;
  TString   fProperty;
  std::vector<double> fListValues;
};


/*
 * Select particles with TLV within, without, or windowed between
 * given R/Phi value(s) from reference particle
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
class SelectDeltaTLV : public internal::FilterRefTLVAlgo {
public:
  SelectDeltaTLV (TString name, TString title, TString input, TString others, 
      double value, TString topo = "in", TString type = "r");
  SelectDeltaTLV (TString name, TString title, TString input, TString others, 
      double low, double high, TString type = "r");
  virtual ~SelectDeltaTLV () {}

  virtual bool FilterPredicate (HAL::ParticlePtr, HAL::ParticlePtr);

private:
  double    fHighLimit, fLowLimit;
  bool      fIn, fOut, fWindow, fDeltaR, fDeltaPhi;
};




/*
 * Cutting Algorithms
 * */

/*
 * Empty cut. This is useful for giving the total number of 
 * events processed.
 *
 * Prerequisites:
 *  None
 * Required Branch Maps:
 *  None
 * UserData Output:
 *  None
 * */
class EmptyCut : public HAL::CutAlgorithm {
public:
  EmptyCut (TString name, TString title) : CutAlgorithm(name, title) {}
  virtual ~EmptyCut () {}

protected:
  virtual void Exec (Option_t* /*option*/) {Passed();}
};


/*
 * Cut on value and/or number of particles in the given algorithms
 * (logical 'and' or 'or')
 * Requires: algorithms - literal string name of algorithm
 *           type - literal string type: bool, integer, counting, decimal, particle
 *           relational operator - literal string of: ==, !=, <, >, <=, >=
 *           value - literal value of the above types
 *           
 *
 * Prerequisites:
 *  Stored data
 * Required Branch Maps:
 *  None
 * UserData Output:
 *  None
 * */
class Cut : public CutAlgorithm {
public:
  Cut (TString name, TString title, TString logic, long long length, ...);
  virtual ~Cut ();

protected:
  virtual void Exec (Option_t* /*option*/);

private:
  bool          fAnd, fOr;

  struct AlgoInfo {
    AlgoInfo () 
      : fEqual(false), fNotEqual(false), fLessThan(false), 
        fGreaterThan(false), fLessThanEqual(false), fGreaterThanEqual(false) {}
    virtual ~AlgoInfo () {}
    const char    *fName;
    bool           fEqual, fNotEqual, fLessThan, fGreaterThan, fLessThanEqual, fGreaterThanEqual;
    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*) = 0;
  };

  struct BoolAlgoInfo : public AlgoInfo {
    bool           fValue;
    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
  };
  
  struct IntegerAlgoInfo : public AlgoInfo {
    long long      fValue;
    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
  };

  struct CountingAlgoInfo : public AlgoInfo {
    unsigned long long fValue;
    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
  };

  struct DecimalAlgoInfo : public AlgoInfo {
    long double    fValue;
    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
  };

  struct NParticlesAlgoInfo : public AlgoInfo {
    long long      fValue;
    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
  };

  std::vector<AlgoInfo*>    fAlgorithms;
};




/*
 * Monitoring Algorithms
 * */

/*
 * Prints the output of an algorithm to the given ostream
 * */
class MonitorAlgorithm : public HAL::Algorithm {
public:
  MonitorAlgorithm (TString name, TString title, TString input, long long n = 1, std::ostream &os = std::cout) :
    Algorithm(name, title), fN(n), fInput(input), fOS(&os) {}
  virtual ~MonitorAlgorithm () {}

protected:
  virtual void Exec (Option_t* /*option*/);

private:
  long long      fN;
  TString        fInput;
  std::ostream  *fOS;
};


/*
 * Prints the contents of "UserData" to the given ostream
 * */
class MonitorUserData : public HAL::Algorithm {
public:
  MonitorUserData (TString name, TString title, std::ostream &os = std::cout) :
    Algorithm(name, title), fOS(&os) {}
  virtual ~MonitorUserData () {}

protected:
  virtual void Exec (Option_t* /*option*/);

private:
  std::ostream  *fOS;
};




/*
 * Exporting Algorithms
 * */

/*
 * Store a TLV property of a particle or particles
 * This property can be:
 * transverse momentum, mass, energy, transverse energy, 3-momentum magnitude, eta, phi
 * pt,                  m,    e,      et,                p3,                   eta, phi
 *
 * Prerequisites:
 *  Stored particle
 * Required Branch Maps:
 *  None
 * UserData Output:
 *  None
 * */
class StoreTLV : public internal::ParticlesTLVStore {
public:
  StoreTLV (TString name, TString title, TString input, TString property, TString bname, TString num = "");
  virtual ~StoreTLV () {}

protected:
  virtual void  StoreValue (HAL::AnalysisTreeWriter*, long long, HAL::ParticlePtr);

private:
  bool            fAll, fPt, fM, fE, fEt, fP3, fEta, fPhi, fID, fCharge;
  TString         fPtLabel, fEtaLabel, fPhiLabel, fMLabel, fELabel, fIDLabel, fChargeLabel;
};

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHMS */

