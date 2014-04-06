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
 * Algorithm for a single particle cut on its TLV
 * */
class ParticlesTLVCut : public CutAlgorithm {
public:
  ParticlesTLVCut (TString name, TString title, TString input, double cut) :
    CutAlgorithm(name, title), fInput(input), fCutValue(cut) {}
  virtual ~ParticlesTLVCut () {}

  // this should return true if particle passed cut
  virtual bool CutPredicate (TLorentzVector*) = 0;

protected:
  virtual void Exec (Option_t* /*option*/);

  TString   fInput;
  double    fCutValue;
};

/*
 * Algorithm for the exporting of simple quantities from a
 * particle's TLV
 * */
class ParticlesTLVStore : public Algorithm {
public:
  ParticlesTLVStore (TString name, TString title, TString input, TString bname) :
    Algorithm(name, title), fBranchName(bname), fInput(input) {}
  virtual ~ParticlesTLVStore () {}


protected:
  virtual void    Exec (Option_t* /*option*/);
  virtual double  StoreValue (TLorentzVector*) = 0;

  TString         fBranchName, fInput;
};

} /* internal */ 



/*
 * Importing algorithms
 * */

/*
 * Import algorithm to build particles from the cartesian components
 * of a 4-vector.
 *
 * Prerequisites:
 *  None
 * Branch Maps Needed:
 *  <name>:nentries
 *  <name>:x0
 *  <name>:x1
 *  <name>:x2
 *  <name>:x3
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0000 : public internal::ImportTLVAlgo {
public:
  IA0000 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0000 () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);
};


/*
 * Import algorithm to build particles from the cartesian components
 * of a 4-vector. It uses the dimension of the x1 coordinate array
 * as the number to loop over. This saves the user from having to
 * explicitly give an array length.
 *
 * Prerequisites:
 *  None
 * Branch Maps Needed:
 *  <name>:x0
 *  <name>:x1
 *  <name>:x2
 *  <name>:x3
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0001 : public internal::ImportTLVAlgo {
public:
  IA0001 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0001 () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);
};


/*
 * Import algorithm to build particles from the cartesian components
 * of a 4-vector. User must specify number of particles to read in.
 *
 * Prerequisites:
 *  None
 * Branch Maps Needed:
 *  <name>:x0
 *  <name>:x1
 *  <name>:x2
 *  <name>:x3
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0002 : public internal::ImportTLVAlgo {
public:
  IA0002 (TString name, TString title, unsigned n) : ImportTLVAlgo(name, title), fNEntries(n) {}
  virtual ~IA0002 () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);

private:
  unsigned fNEntries;
};

/*
 * Import algorithm to build particles from the pT, eta, phi, and 
 * energy components of a 4-vector.
 *
 * Prerequisites:
 *  None
 * Branch Maps Needed:
 *  <name>:nentries
 *  <name>:pt
 *  <name>:eta
 *  <name>:phi
 *  <name>:e
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0010 : public internal::ImportTLVAlgo {
public:
  IA0010 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0010 () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);
};


/*
 * Import algorithm to build particles from the pT, eta, phi, and 
 * energy components of a 4-vector. It uses the dimension of the pT 
 * array as the number to loop over. This saves the user from having 
 * to explicitly give an array length.
 *
 * Prerequisites:
 *  None
 * Branch Maps Needed:
 *  <name>:pt
 *  <name>:eta
 *  <name>:phi
 *  <name>:e
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0011 : public internal::ImportTLVAlgo {
public:
  IA0011 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0011 () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);
};


/*
 * Import algorithm to build particles from the pT, eta, phi, and 
 * energy components of a 4-vector. User must specify number of 
 * particles to read in.
 *
 * Prerequisites:
 *  None
 * Branch Maps Needed:
 *  <name>:pt
 *  <name>:eta
 *  <name>:phi
 *  <name>:e
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0012 : public internal::ImportTLVAlgo {
public:
  IA0012 (TString name, TString title, unsigned n) : ImportTLVAlgo(name, title), fNEntries(n) {}
  virtual ~IA0012 () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);

private:
  unsigned fNEntries;
};


/*
 * Import algorithm to build particles from the pT, eta, phi, and 
 * mass components of a 4-vector.
 *
 * Prerequisites:
 *  None
 * Branch Maps Needed:
 *  <name>:nentries
 *  <name>:pt
 *  <name>:eta
 *  <name>:phi
 *  <name>:m
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0020 : public internal::ImportTLVAlgo {
public:
  IA0020 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0020 () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);
};


/*
 * Import algorithm to build particles from the pT, eta, phi, and 
 * mass components of a 4-vector. It uses the dimension of the pT 
 * array as the number to loop over. This saves the user from having 
 * to explicitly give an array length.
 *
 * Prerequisites:
 *  None
 * Branch Maps Needed:
 *  <name>:pt
 *  <name>:eta
 *  <name>:phi
 *  <name>:m
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0021 : public internal::ImportTLVAlgo {
public:
  IA0021 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0021 () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);
};


/*
 * Import algorithm to build particles from the pT, eta, phi, and 
 * mass components of a 4-vector. User must specify number of 
 * particles to read in.
 *
 * Prerequisites:
 *  None
 * Branch Maps Needed:
 *  <name>:pt
 *  <name>:eta
 *  <name>:phi
 *  <name>:m
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0022 : public internal::ImportTLVAlgo {
public:
  IA0022 (TString name, TString title, unsigned n) : ImportTLVAlgo(name, title), fNEntries(n) {}
  virtual ~IA0022 () {}

protected:
  using ImportTLVAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);

private:
  unsigned fNEntries;
};




/*
 * Reconstruction Algorithms
 * */

/*
 * Reconstruction algorithm to build particles from the
 * vector addition of other particles.
 * The unknown parameters should be a series of string literals
 *
 * Prerequisites:
 *  Stored Particles
 * Branch Maps Needed:
 *  None
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:nparents (scalar: number of parent particles)
 *  <name>:4-vec (1D array: of TLorentzVectors)
 *  <name>:index (1D array: of indices)
 *  <name>:parent_ref_name (1D array: ref_names of parents)
 *  <name>:parent_index (1D array: indices of parents)
 */
class RA0000: public Algorithm {
public:
  RA0000 (TString name, TString title, long long length, ...);
  virtual ~RA0000();

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
 * Select the particle with nth highest pT
 *
 * Prerequisites:
 *  Stored particles (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:ref_name ((scalar): string name of reference particles to use)
 *  <name>:index (1D array: of indices (just one index that points to the nth highest pT of ref_name))
 * */
class FA0000 : public internal::NthElementAlgo {
public:
  FA0000 (TString name, TString title, TString input, unsigned n) : 
    NthElementAlgo(name, title, input, n) {}
  virtual ~FA0000 () {}

  virtual TString       SortTag ();
  virtual bool          operator() (long long, long long);
  virtual void          Sort (std::vector<long long>&);
};




/*
 * Cutting Algorithms
 * */

/*
 * Cut on particles' pT (lower limit)
 *
 * Prerequisites:
 *  Stored particle (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class CA0000 : public internal::ParticlesTLVCut {
public:
  CA0000 (TString name, TString title, TString input, double cut) :
    ParticlesTLVCut(name, title, input, cut) {}
  virtual ~CA0000 () {}

  virtual bool CutPredicate (TLorentzVector *vec);
};


/*
 * Cut on particles' mass (lower limit)
 *
 * Prerequisites:
 *  Stored particles (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class CA0003 : public internal::ParticlesTLVCut {
public:
  CA0003 (TString name, TString title, TString input, double cut) :
    ParticlesTLVCut(name, title, input, cut) {}
  virtual ~CA0003 () {}

  virtual bool CutPredicate (TLorentzVector *vec);
};




/*
 * Exporting Algorithms
 * */

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
class EA0000 : public internal::ParticlesTLVStore {
public:
  EA0000 (TString name, TString title, TString input, TString bname) :
    ParticlesTLVStore(name, title, input, bname) {}
  virtual ~EA0000 () {}


protected:
  virtual double  StoreValue (TLorentzVector*);
};


/*
 * Store the mass of a particle
 *
 * Prerequisites:
 *  Stored particle (either as references or direct access)
 * Branch Maps Needed:
 *  None
 * Output:
 *  None
 * */
class EA0003 : public internal::ParticlesTLVStore {
public:
  EA0003 (TString name, TString title, TString input, TString bname) :
    ParticlesTLVStore(name, title, input, bname) {}
  virtual ~EA0003 () {}


protected:
  virtual double  StoreValue (TLorentzVector*);
};

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHMS */

