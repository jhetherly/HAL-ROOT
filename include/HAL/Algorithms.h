/*
 * Generic algorithms for fast analysis development
 *
 * DEV NOTE:
 * Importing and Reconstruction Algorithms create and destroy objects in 'data'
 * All other Algorithms should never create or destroy objects in 'data'
 * */

#ifndef HAL_ALGORITHMS
#define HAL_ALGORITHMS

#include <TString.h>
#include <TLorentzVector.h>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/AnalysisUtils.h>
#include <HAL/Algorithm.h>
#include <HAL/CutAlgorithm.h>
#include <HAL/AnalysisData.h>
#include <HAL/AnalysisTreeReader.h>
#include <HAL/AnalysisTreeWriter.h>

namespace HAL
{

/*
 * Generic base class algorithms
 * Most of these are ABC's
 * */

/*
 * Algorithm for importing an array of TLorentzVecotr's from and TTree.
 * */
class ImportTLVAlgo : public Algorithm {
public:
  ImportTLVAlgo (TString name, TString title) : Algorithm(name, title) {}
  virtual ~ImportTLVAlgo () {}

protected:
  virtual void Exec (Option_t* /*option*/) {}
  virtual void Exec (unsigned n);
  virtual void Clear (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned) = 0;
};



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
 *  <name>:4-vec (1D array: of TLorentzVector's)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0000 : public ImportTLVAlgo {
public:
  IA0000 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0000 () {}

protected:
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
 *  <name>:4-vec (1D array: of TLorentzVector's)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0001 : public ImportTLVAlgo {
public:
  IA0001 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0001 () {}

protected:
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
 *  <name>:4-vec (1D array: of TLorentzVector's)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0002 : public ImportTLVAlgo {
public:
  IA0002 (TString name, TString title, unsigned n) : ImportTLVAlgo(name, title), fNEntries(n) {}
  virtual ~IA0002 () {}

protected:
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
 *  <name>:pT
 *  <name>:eta
 *  <name>:phi
 *  <name>:e
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVector's)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0010 : public ImportTLVAlgo {
public:
  IA0010 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0010 () {}

protected:
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
 *  <name>:pT
 *  <name>:eta
 *  <name>:phi
 *  <name>:e
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVector's)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0011 : public ImportTLVAlgo {
public:
  IA0011 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0011 () {}

protected:
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
 *  <name>:pT
 *  <name>:eta
 *  <name>:phi
 *  <name>:e
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVector's)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0012 : public ImportTLVAlgo {
public:
  IA0012 (TString name, TString title, unsigned n) : ImportTLVAlgo(name, title), fNEntries(n) {}
  virtual ~IA0012 () {}

protected:
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
 *  <name>:pT
 *  <name>:eta
 *  <name>:phi
 *  <name>:m
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVector's)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0020 : public ImportTLVAlgo {
public:
  IA0020 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0020 () {}

protected:
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
 *  <name>:pT
 *  <name>:eta
 *  <name>:phi
 *  <name>:m
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVector's)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0021 : public ImportTLVAlgo {
public:
  IA0021 (TString name, TString title) : ImportTLVAlgo(name, title) {}
  virtual ~IA0021 () {}

protected:
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
 *  <name>:pT
 *  <name>:eta
 *  <name>:phi
 *  <name>:m
 * Output:
 *  <name>:nobjects (scalar: number of particles)
 *  <name>:4-vec (1D array: of TLorentzVector's)
 *  <name>:index (1D array: of indices (used in subsequent algorithms))
 */
class IA0022 : public ImportTLVAlgo {
public:
  IA0022 (TString name, TString title, unsigned n) : ImportTLVAlgo(name, title), fNEntries(n) {}
  virtual ~IA0022 () {}

protected:
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);

private:
  unsigned fNEntries;
};




/*
 * Reconstruction Algorithms
 * */




/*
 * Filtering Algorithms
 * */

/*
 * Select the particle with nth highest pT
 *
 * Prerequisites:
 *  Stored TLorentzVector's
 * Branch Maps Needed:
 *  None
 * Output:
 *  <name>:4-vec (scalar: TLorentzVector)
 *  <name>:index (scalar: index)
 * */
class FA0000 : public Algorithm {
public:
  FA0000 (TString name, TString title, TString input, unsigned n) : 
    Algorithm(name, title), fN(n), fInput(input) {}
  virtual ~FA0000 () {}

protected:
  virtual void Exec (Option_t* /*option*/);

private:
  unsigned  fN;
  TString   fInput;
};




/*
 * Cutting Algorithms
 * */




/*
 * Exporting Algorithms
 * */

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHMS */

