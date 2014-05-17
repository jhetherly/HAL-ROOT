/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 * \version 0.0.26
 *
 * \section LICENSE
 * 
 * \section Description
 *
 * These classes are part of the generic algorithm framework. They do
 * common tasks in H.E.P. analysis and aid in fast development of
 * an analysis. Only importing and reconstruction algorithms will
 * create particles; the others algorithms just suffle pointers to
 * particles around.
 */

/// \todo Generic Algorithms: Add chi-squared minimization algorithm
/// \todo Generic Algorithms: Add parent selection algorithm
/// \todo Generic Algorithms: Add merging algorithm
/// \todo Generic Algorithms: Add monitor for UserData algorithm
/// \todo Generic Algorithms: Add parent/child traversal algorithms
/// \todo Generic Algorithms: Make error messages more informative

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

  bool      fIsCart, fIsE, fIsM,
            fIsCartMET, fIsPhiEtMET,
            fHasCharge, fHasID;
  TString   fCartX0, fCartX1, fCartX2, fCartX3, fPt, fEt, 
            fEta, fPhi, fM, fE, 
            fCharge, fID,
            fNEntriesName;
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
 * Algorithm for importing a scalar value and attaching it to an 
 * existing particle.
 * */
class AugmentValueAlgo : public HAL::Algorithm {
public:
  AugmentValueAlgo (TString name, TString title, TString input, TString attribute_name);
  virtual ~AugmentValueAlgo () {}

protected:
  virtual void  Exec (Option_t* /*option*/);
  virtual void  Clear (Option_t* /*option*/);
  virtual void  StoreValue (HAL::AnalysisTreeReader*, HAL::ParticlePtr, long long) = 0;

  TString   fInput, fAttributeLabel;
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
 * by their differences
 * */
class FilterRefParticleAlgo : public Algorithm {
public:
  FilterRefParticleAlgo (TString name, TString title, TString input, TString others) :
    Algorithm(name, title), fInput(input), fOthers(others) {}
  virtual ~FilterRefParticleAlgo () {}

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
 * several particles' TLVs and attributes
 * */
class ParticlesTLVStore : public Algorithm {
public:
  ParticlesTLVStore (TString name, TString title, TString input, TString bname);
  virtual ~ParticlesTLVStore () {}


protected:
  virtual void    Exec (Option_t* /*option*/);
  virtual void    StoreValue (HAL::AnalysisTreeWriter*, long long, HAL::ParticlePtr) = 0;

  bool            fSearchedForAttributes;
  TString         fBranchName, fInput, fNParticles;
  std::map<TString, bool>     fAttributeFlags;
  std::map<TString, TString>  fAttributeLabels;
};

} /* internal */ 



namespace Algorithms
{


/*
 * Importing algorithms
 * */

//! Generic algorithm class that builds particles from information in a TTree.
/*!
 * This algorithm imports the information to build particles from specified branches in a TTree.
 * It may use the branches to build particles with either full 4-vector momentum or transverse 
 * momentum. It can optionally read in the charge, particle ID, and number of particles to 
 * import. It determines how to read in the particles through the specified branch maps given 
 * to the Analysis object. It can also determine how many particles to read in based on whether 
 * you give a number to read, branchmap to scan, or implicitly gather it from the length of the
 * other required branches. The necessary branch maps are given below. The particles from this 
 * algorithm are stored in a GenericData object in the UserData under the algorithm's 
 * name.\n\n
 * __Explaination of the branch maps:__\n
 * The required maps are those needed to construct the TLorentzVectors, either complete vectors
 * or just transverse vectors. Any set of the branch maps given below will do. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Required Branch Maps:_
 * | Cartesian components | \f$ p_T,\eta,\phi,E\f$ | \f$ p_T,\eta,\phi,m\f$ | Transverse Cartesian | \f$ p_T,\phi\f$ | \f$ E_T,\phi\f$ |
 * | :------------------: | :-------: | :-------: | :------------------: | :--------------: | :--------------: |
 * |  <name>:x0           | <name>:pt | <name>:pt |   <name>:x1          |    <name>:pt     |    <name>:et     |
 * |  <name>:x1           |<name>:eta |<name>:eta |   <name>:x2          |    <name>:phi    |    <name>:phi    |
 * |  <name>:x2           |<name>:phi |<name>:phi |                      |                  |                  |
 * |  <name>:x3           | <name>:e  | <name>:m  |                      |                  |                  |
 * _Optional Branch Maps:_
 * | Number of particle | Charge | ID |
 * | :----------------: | :----: | :--:|
 * |  <name>:nentries   |<name>:charge | <name>:id |
 * __Examples:__\n
 * In your analysis file, do the following to import Monte Carlo particles with complete 4-vectors:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("mc", "import basic Monte Carlo particles"));
 *
 * //...
 * 
 * a.MapBranch("mc_pt",     "mc:pt");
 * a.MapBranch("mc_eta",    "mc:eta");
 * a.MapBranch("mc_phi",    "mc:phi");
 * a.MapBranch("mc_m",      "mc:m");
 * a.MapBranch("mc_pdgId",  "mc:id");
 * a.MapBranch("mc_charge", "mc:charge");
 * ~~~~~~~~~~~~~~~~~~~~~~
 * Likewise, you can import the MET vector like so:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("met", "import the MET tranverse vector"));
 *
 * //...
 *
 * a.MapBranch("MET_Truth_Int_etx", "met:x1");
 * a.MapBranch("MET_Truth_Int_ety", "met:x2");
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class ImportParticle : public HAL::internal::ImportParticleAlgo {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] n_max Maximum number of particles to import. If n=0 then all the particles are imported.
   * \sa ImportBool, ImportInteger, ImportCounting, ImportDecimal
   */
  ImportParticle (TString name, TString title, unsigned n_max = 0);
  virtual ~ImportParticle () {}

protected:
  using ImportParticleAlgo::Exec;
  virtual void Exec (Option_t* /*option*/);
  virtual TLorentzVector* MakeTLV (unsigned);

private:
  unsigned  fN;
};


//! Generic algorithm class that stores a boolean value from information in a TTree.
/*!
 * This algorithm imports the information to store a boolean value from a specified branch in 
 * a TTree. The value from this algorithm is stored in a GenericData object in the 
 * UserData under the algorithm's name and <name>:value.\n\n
 * __Explaination of the branch map:__\n
 * The required map is just one that points to the relavent boolean value. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Required Branch Map:_
 * || Boolean value |
 * || :-----------: |
 * ||  <name>:bool  |
 * __Example:__\n
 * In your analysis file, do the following to import a trigger flag:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportBool("lepton trigger", "import the lepton trigger"));
 *
 * //...
 * 
 * a.MapBranch("some_trigger_branch", "lepton trigger:bool");
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class ImportBool : public HAL::internal::ImportValueAlgo {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa ImportParticle, ImportInteger, ImportCounting, ImportDecimal
   */
  ImportBool (TString name, TString title);
  virtual ~ImportBool () {}

  virtual void  StoreValue (HAL::GenericData*);

private:
  TString   fValue;
};


//! Generic algorithm class that stores an integer value from information in a TTree.
/*!
 * This algorithm imports the information to store an integer value from a specified branch in 
 * a TTree. The value from this algorithm is stored in a GenericData object in the 
 * UserData under the algorithm's name and <name>:value.\n\n
 * __Explaination of the branch map:__\n
 * The required map is just one that points to the relavent integer value. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Required Branch Map:_
 * || Integer value |
 * || :-----------: |
 * ||  <name>:integer  |
 * __Example:__\n
 * In your analysis file, do the following for an integer value:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportInteger("integer value", "import an integer value"));
 *
 * //...
 * 
 * a.MapBranch("some_integer_branch", "integer value:integer");
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class ImportInteger : public HAL::internal::ImportValueAlgo {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa ImportBool, ImportParticle, ImportCounting, ImportDecimal
   */
  ImportInteger (TString name, TString title);
  virtual ~ImportInteger () {}

  virtual void  StoreValue (HAL::GenericData*);

private:
  TString   fValue;
};


//! Generic algorithm class that stores a counting value from information in a TTree.
/*!
 * This algorithm imports the information to store a counting value from a specified branch in 
 * a TTree. The value from this algorithm is stored in a GenericData object in the 
 * UserData under the algorithm's name and <name>:value.\n\n
 * __Explaination of the branch map:__\n
 * The required map is just one that points to the relavent counting value. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Required Branch Map:_
 * || Counting value |
 * || :-----------: |
 * ||  <name>:counting  |
 * __Example:__\n
 * In your analysis file, do the following for a counting value:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportCounting("counting value", "import a counting value"));
 *
 * //...
 * 
 * a.MapBranch("some_counting_branch", "counting value:counting");
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class ImportCounting : public HAL::internal::ImportValueAlgo {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa ImportBool, ImportInteger, ImportParticle, ImportDecimal
   */
  ImportCounting (TString name, TString title);
  virtual ~ImportCounting () {}

  virtual void  StoreValue (HAL::GenericData*);

private:
  TString   fValue;
};


//! Generic algorithm class that stores a decimal value from information in a TTree.
/*!
 * This algorithm imports the information to store a decimal value from a specified branch in 
 * a TTree. The value from this algorithm is stored in a GenericData object in the 
 * UserData under the algorithm's name and <name>:value.\n\n
 * __Explaination of the branch map:__\n
 * The required map is just one that points to the relavent decimal value. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Required Branch Map:_
 * || Decimal value |
 * || :-----------: |
 * ||  <name>:decimal  |
 * __Example:__\n
 * In your analysis file, do the following for a decimal value:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportDecimal("decimal value", "import a decimal value"));
 *
 * //...
 * 
 * a.MapBranch("some_decimal_branch", "decimal value:decimal");
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class ImportDecimal : public HAL::internal::ImportValueAlgo {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa ImportBool, ImportInteger, ImportCounting, ImportParticle
   */
  ImportDecimal (TString name, TString title);
  virtual ~ImportDecimal () {}

  virtual void  StoreValue (HAL::GenericData*);

private:
  TString   fValue;
};




//! Generic algorithm class that attaches a decimal value to an existing set of particles
/*!
 * This algorithm attaches information to a set of particles from either a branch in a TTree, a
 * specified property, or a value the user gives. The input particles are copied and the value is 
 * attached to the new particles. This algorithm needs one branch map to locate the value to 
 * attach if attaching from a branch. The output from this algorithm may be accessed by this 
 * algorithm's name.\n\n
 * __Explaination of the branch map:__\n
 * The branch map is just one that points to the relavent value. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Optional Branch Map:_
 * || Value |
 * || :-----------: |
 * ||  <name>:value  |
 * _Available Properties:_
 * | Rank in \f$ p_T \f$ | Rank in \f$ E_T \f$ | Rank in Mass | Rank in Energy | Rank in \f$ \left|\overrightarrow{p}\right| \f$ |
 * | :-----------------: | :---------------: | :--: | :----: | :------------------: |
 * |  rank_pT   | rank_eT | rank_m | rank_e | rank_p3 |
 * __Example:__\n
 * In your analysis file, do the following to attach charge to each muon (the easier 
 * way would be through ImportParticle):
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("muons", "import basic muons"));
 *
 * a.AddAlgo(new HAL::Algorithms::AttachAttribute("muon charge att", "attach charge to muons", 
 *                                                "muons", 
 *                                                "muon charge"));
 *
 * //...
 * 
 * a.MapBranch("mu_charge", "muon charge att:value");
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class AttachAttribute : public HAL::internal::AugmentValueAlgo {
public:
  //! Constructor
  /*!
   * Initializes the algorithm for a given user value
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] input Name of algorithm to attach values to.
   * \param[in] attribute_name Name to give the attribute.
   * \param[in] value Value to store in the particles' attributes lists
   * \sa ImportParticle, SelectParticle
   */
  AttachAttribute (TString name, TString title, TString input, TString attribute_name, double value) :
    AugmentValueAlgo(name, title, input, attribute_name), 
    fUserValue(true), fBranchValue(false), fPropertyValue(false), 
    fValue(value) {}
  //! Constructor
  /*!
   * Initializes the algorithm to read from a TTree branch
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] input Name of algorithm to attach values to.
   * \param[in] attribute_name Name to give the attribute.
   * \sa ImportParticle, SelectParticle
   */
  AttachAttribute (TString name, TString title, TString input, TString attribute_name) :
    AugmentValueAlgo(name, title, input, attribute_name), 
    fUserValue(false), fBranchValue(true), fPropertyValue(false), 
    fBranchLabel(TString::Format("%s:value", name.Data())) {}
  //! Constructor
  /*!
   * Initializes the algorithm for a specified property
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] input Name of algorithm to attach values to.
   * \param[in] attribute_name Name to give the attribute.
   * \param[in] property Property to store in the particles' attribute lists.
   * \param[in] ref_particles Name of particles to compare to.
   * \sa ImportParticle, SelectParticle
   */
  AttachAttribute (TString name, TString title, TString input, TString attribute_name, 
      TString property, TString ref_particles = "") :
    AugmentValueAlgo(name, title, input, attribute_name), 
    fUserValue(false), fBranchValue(false), fPropertyValue(true), 
    fPtRank(property.EqualTo("rank_pt", TString::kIgnoreCase)), 
    fMRank(property.EqualTo("rank_m", TString::kIgnoreCase)), 
    fERank(property.EqualTo("rank_e", TString::kIgnoreCase)), 
    fEtRank(property.EqualTo("rank_et", TString::kIgnoreCase)), 
    fP3Rank(property.EqualTo("rank_p3", TString::kIgnoreCase)),
    fRefCompare(!ref_particles.EqualTo("", TString::kIgnoreCase)) {}
  virtual ~AttachAttribute () {}

  bool          operator() (ParticlePtr lhs, ParticlePtr rhs);

protected:
  virtual void  StoreValue (HAL::AnalysisTreeReader*, HAL::ParticlePtr, long long);
  void          Sort (ParticlePtrs &sl);

private:
  bool      fUserValue, fBranchValue, fPropertyValue;
  bool      fPtRank, fMRank, fERank, fEtRank, fP3Rank;
  bool      fRefCompare;
  double    fValue;
  TString   fBranchLabel, fRefParticles;
};




/*
 * Reconstruction Algorithms
 * */

//! Generic algorithm class that combines tuples of particles from any number of alogrithms.
/*!
 * This algorithm builds particles from the vector addition of other particles from an arbitrary
 * number of other algorithms. Special care is taken to only add unique vectors (parents are 
 * also checked). Also, the tuples will be made up of as many particles as there are input 
 * algorithms (i.e. "length"). This algorithm may return no particles if no unique combination 
 * can be found. The particles from this algorithm are stored in a GenericData object in the 
 * UserData under the algorithm's name.\n\n
 * __Example:__\n
 * In your analysis file, do the following for the vector addition of two muons:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("muons", "import basic muons"));
 *
 * //...
 * 
 * a.AddAlgo(new HAL::Algorithms::VecAddReco("di-muons", "combine muons pairwise", 
 *                                           2,
 *                                           "muons", "muons"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class VecAddReco : public Algorithm {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] length Number of algorithms to combine.
   * \param[in] ... Comma separated list of string literals representing the algorithms to combine.
   * \sa ImportParticle
   */
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

//! Generic algorithm class that selects the particle with the highest or lowest property.
/*!
 * This algorithm selects the particle with highest or lowest property. The list of
 * available properties is given below. A property is input as a string to the 
 * constructor of this algorithm. The particle from this algorithm is stored in a 
 * GenericData object in the UserData under the algorithm's name.\n\n
 * _Available Properties:_
 * | \f$ p_T \f$ | \f$ E_T \f$ | Mass | Energy | \f$ \left|\overrightarrow{p}\right| \f$ |
 * | :-----------------: | :---------------: | :--: | :----: | :------------------: |
 * |  pT   | eT | m | e | p3 |
 * __Example:__\n
 * In your analysis file, do the following to select the highest \f$ p_T \f$ muon:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("muons", "import basic muons"));
 *
 * //...
 * 
 * a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("highest pT muon", "find highest pT muon", 
 *                                                      "muons", 1, "pT", "high"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class ParticleRankSelection : public internal::NthElementAlgo {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] input Name of algorithm to select from.
   * \param[in] rank Rank of particle.
   * \param[in] property Property to rank particle by.
   * \param[in] end Either "high" for highest rank property or "low" for lowest rank property.
   * \sa ImportParticle, SelectParticle
   */
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


//! Generic algorithm class that selects particles with specified properties.
/*!
 * This algorithm selects particles relative to a specified property. This algorithm can
 * select particles by a simple relationship, windowed between two values, or a list of 
 * values of a specified property. How the selection occurs is determined by the 
 * constructor used to initialize the algorithm. The list of available properties and
 * relationships are given below. Properties and relationships are input as strings to 
 * the constructor of this algorithm. The particle from this algorithm is stored in a 
 * GenericData object in the UserData under the algorithm's name.\n\n
 * _Available Properties:_
 * | \f$ p_T \f$ | \f$ E_T \f$ | Mass | Energy | \f$ \left|\overrightarrow{p}\right| \f$ | \f$ \eta \f$ | \f$ \phi \f$ | ID | Charge | Custom Attribute |
 * | :-----------------: | :---------------: | :--: | :----: | :------------------: | :-: | :-: | :-: | :-: | :-: |
 * |  pT   | eT | m | e | p3 | eta | phi | id | charge | <attribute> |
 * _Available Logical Relationships:_
 * | Equal | Not Equal | Greater Than | Greater Than or Equal To | Less Than | Less Than or Equal To |
 * | :-----------------: | :---------------: | :--: | :----: | :------------------: | :-: |
 * |  ==   | != | > | >= | < | <= |
 * _Note:_ = may also be used in place of ==.\n\n
 * __Examples:__\n
 * In your analysis file, do the following to select the muons with \f$ p_T \f$ greater than or equal to 50GeV:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("muons", "import basic muons"));
 *
 * //...
 * 
 * a.AddAlgo(new HAL::Algorithms::SelectParticle("muons pT", "select muons with pt >= 50GeV", 
 *                                               "muons",
 *                                               "pt", ">=", 50000));
 * ~~~~~~~~~~~~~~~~~~~~~~
 * To select the muons within an \f$ \eta \f$ value between -2.0 and 2.0 but not between -0.5 and 0.5:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("muons", "import basic muons"));
 *
 * //...
 * 
 * a.AddAlgo(new HAL::Algorithms::SelectParticle("muons |eta| <= 2.0", "select muons with |eta| <= 2.0", 
 *                                               "muons",
 *                                               "eta", "inclusive", -2.0, 2.0));
 * a.AddAlgo(new HAL::Algorithms::SelectParticle("muons eta", "select muons with |eta| >= 0.5", 
 *                                               "muons |eta| <= 2.0",
 *                                               "eta", "exclusive", -0.5, 0.5));
 * ~~~~~~~~~~~~~~~~~~~~~~
 * To select the Monte Carlo particles with PDG ID's of neutrinos:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("mc", "import basic Monte Carlo particles"));
 *
 * //...
 * 
 * a.AddAlgo(new HAL::Algorithms::SelectParticle("mc_neutrinos", "filter on mc id to get neutrinos", 
 *                                               "mc",
 *                                               "id", 6,
 *                                               -16, -14, -12, 12, 14, 16));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class SelectParticle : public internal::FilterParticleAlgo {
public:
  //! Constructor
  /*!
   * Initializes the algorithm for simple relational selection
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] input Name of algorithm to select from.
   * \param[in] property Property by which to select particles.
   * \param[in] logic How to select the particles.
   * \param[in] value Value of property.
   * \sa ImportParticle, ParticleRankSelection
   */
  SelectParticle (TString name, TString title, TString input, TString property, 
      TString logic, double value);
  //! Constructor
  /*!
   * Initializes the algorithm for a window selection
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] input Name of algorithm to select from.
   * \param[in] property Property by which to select particles.
   * \param[in] inclusion How to select the particles. Either inclusive (or in) or exclusive (or out).
   * \param[in] low Lower value of property.
   * \param[in] high Higher value of property.
   * \sa ImportParticle, ParticleRankSelection
   */
  SelectParticle (TString name, TString title, TString input, TString property, 
      TString inclusion, double low, double high);
  //! Constructor
  /*!
   * Initializes the algorithm for specific values selection
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] input Name of algorithm to select from.
   * \param[in] property Property by which to select particles.
   * \param[in] length Number of values to compare against.
   * \param[in] ... Comma separated list of values. If property is id use integer values, otherwise 
   * use decimal values.
   * \sa ImportParticle, ParticleRankSelection
   */
  SelectParticle (TString name, TString title, TString input, TString property, 
      int length, ...);
  virtual ~SelectParticle () {}

protected:
  virtual bool FilterPredicate(HAL::ParticlePtr);

private:
  void      Setup ();

  double    fHighLimit, fLowLimit;
  bool      fPt, fM, fE, fEt, fP3, fEta, fPhi, fCharge, fID, fAttribute;
  bool      fEqual, fNotEqual, fLessThan, fGreaterThan, fLessThanEqual, fGreaterThanEqual;
  bool      fIn, fOut;
  bool      fSingleEnd, fWindow, fList;
  TString   fProperty;
  std::vector<double> fListValues;
};


//! Generic algorithm class that selects particles with respect to a set of reference particles
/*!
 * This algorithm selects particles relative to a set of reference particles. This algorithm 
 * makes sure to not compare the same particle to itself. An inclusive, exclusive, or windowed
 * selection is determined by the constructor used to initialize the algorithm. The
 * properties are listed below. The particle from this algorithm is stored in a 
 * GenericData object in the UserData under the algorithm's name.\n\n
 * _Available Properties:_
 * | \f$ \Delta R \f$ | \f$ \Delta\phi \f$ |
 * | :-------------: | :----------------: |
 * |  dr   | dphi |
 * __Example:__\n
 * In your analysis file, do the following to select the jets within a \f$ \Delta R \f$ of 0.4 from the highest
 * \f$ p_T \f$ jet:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("jets", "import basic jets"));
 *
 * //...
 * 
 * a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("leading pt jet", "find highest pt jet", 
 *                                                      "jets",
 *                                                      1, "pt"));
 *
 * a.AddAlgo(new HAL::Algorithms::SelectRefParticle("jets close", "filter on jets within deltaR of di-jet", 
 *                                                  "leading pt jet", "jets",
 *                                                  0.4, "dr"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class SelectRefParticle : public internal::FilterRefParticleAlgo {
public:
  //! Constructor
  /*!
   * Initializes the algorithm for a simple selection
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] reference Name of algorithm the reference particles.
   * \param[in] input Name of algorithm to select from.
   * \param[in] value Value of property
   * \param[in] property Property by which to select particles.
   * \param[in] inclusion How to select the particles. Either inclusive (or in) or exclusive (or out).
   * \sa ImportParticle, ParticleRankSelection
   */
  SelectRefParticle (TString name, TString title, TString reference, TString input, 
      double value, TString property = "dr", TString inclusion = "inclusive");
  //! Constructor
  /*!
   * Initializes the algorithm for a window selection
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] reference Name of algorithm the reference particles.
   * \param[in] input Name of algorithm to select from.
   * \param[in] low Lower value of property.
   * \param[in] high Higher value of property.
   * \param[in] property Property by which to select particles.
   * \param[in] inclusion How to select the particles. Either inclusive (or in) or exclusive (or out).
   * \sa ImportParticle, ParticleRankSelection
   */
  SelectRefParticle (TString name, TString title, TString reference, TString input, 
      double low, double high, TString property = "dr", TString inclusion = "inclusive");
  virtual ~SelectRefParticle () {}

protected:
  virtual bool FilterPredicate (HAL::ParticlePtr, HAL::ParticlePtr);

private:
  double    fHighLimit, fLowLimit;
  bool      fIn, fOut, fWindow, fDeltaR, fDeltaPhi;
};


//! Generic algorithm class that performs a chi-squared minimization
/*!
 * This algorithm selects particles based on a chi-squared minimization. This algorithm 
 * properties are listed below. The particles from this algorithm are stored in many
 * GenericData object in the UserData under output names given to this algorithm's 
 * constructor.\n\n
 * _Available Properties:_
 * | \f$ \Delta R \f$ | \f$ \Delta\phi \f$ |
 * | :-------------: | :----------------: |
 * |  dr   | dphi |
 * __Example:__\n
 * In your analysis file, do the following to select the jets within a \f$ \Delta R \f$ of 0.4 from the highest
 * \f$ p_T \f$ jet:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("jets", "import basic jets"));
 *
 * //...
 * 
 * a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("leading pt jet", "find highest pt jet", 
 *                                                      "jets",
 *                                                      1, "pt"));
 *
 * a.AddAlgo(new HAL::Algorithms::SelectRefParticle("jets close", "filter on jets within deltaR of di-jet", 
 *                                                  "leading pt jet", "jets",
 *                                                  0.4, "dr"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class MinChiSquaredSelection : public HAL::Algorithm {
public:
  //! Constructor
  /*!
   * Initializes the algorithm. The variable length argument at the end should conform 
   * to the following rules:\n
   * - It should be given in sets of seven.
   * - The first 
   * - The second 
   * - The third 
   * - The fourth 
   * - The fifth 
   * - The sixth 
   * - The seventh 
   *
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] nterms Number of cuts to make.
   * \param[in] ... Set of seven values per term as explained above.
   * \sa 
   */
  MinChiSquaredSelection (TString name, TString title, long long nterms, ...);
  virtual ~MinChiSquaredSelection () {}
};




/*
 * Cutting Algorithms
 * */

//! Generic algorithm class that serves as a baseline algorithm for any subsequent Cut algorithms
/*!
 * This algorithm passes all events. It can serve as a baseline cut in an analysis.
 * __Example:__\n
 * In your analysis file, do the following to create a baseline cut:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 * 
 * a.AddAlgo(new HAL::Algorithms::EmptyCut("number of events", "baseline event number"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class EmptyCut : public HAL::CutAlgorithm {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa Cut
   */
  EmptyCut (TString name, TString title) : CutAlgorithm(name, title) {}
  virtual ~EmptyCut () {}

protected:
  virtual void Exec (Option_t* /*option*/) {Passed();}
};


//! Generic algorithm class that cuts on particle multiplicity, bool, integer, counting, and decimal values
/*!
 * This algorithm provides a general mechanism to cut on several different algorithms. An important
 * aspect of this algorithm is its ability to either "and" or "or" different predicates together.
 * Combined with the proper selection algorithms, a cut on particle multiplicity is sufficient for
 * most situations. The relational operators and types needed to specify a cut are listed below.\n
 * _Available Types of Cuts:_
 * | Boolean | Integer | Counting | Decimal | Particle Multiplicity |
 * | :-----: | :-----: | :------: | :-----: | :-------------------: |
 * |  bool   | integer | counting | decimal | particle |
 * _Available Logical Relationships:_
 * | Equal | Not Equal | Greater Than | Greater Than or Equal To | Less Than | Less Than or Equal To |
 * | :-----------------: | :---------------: | :--: | :----: | :------------------: | :-: |
 * |  ==   | != | > | >= | < | <= |
 * _Note:_ = may also be used in place of ==.\n\n
 * __Example:__\n
 * In your analysis file, do the following to create a cut:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 * 
 * //...
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("jets", "import basic jet objects"));
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("mc", "import basic Monte Carlo objects"));
 *
 * a.AddAlgo(new HAL::Algorithms::EmptyCut("number of events", "baseline event number"));
 *
 * a.AddAlgo(new HAL::Algorithms::SelectParticle("mc_neutrinos", "filter on mc id to get neutrinos", 
 *                                               "mc",
 *                                               "id", 6,
 *                                               -16, -14, -12, 12, 14, 16));
 * 
 * a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("leading pt jet", "find highest pt jet", 
 *                                                      "jets",
 *                                                      1, "pt"));
 * a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("subleading pt jet", "find 2nd highest pt jet", 
 *                                                      "jets",
 *                                                      2, "pt"));
 * 
 * a.AddAlgo(new HAL::Algorithms::VecAddReco("di-jet", "reconstruct a di-jet object from highest pt", 
 *                                           2, "leading pt jet", "subleading pt jet"));
 * 
 * a.AddAlgo(new HAL::Algorithms::Cut("di-jet and neutrino cut", "make sure dijet and neutrino(s) exist", 
 *                                    "and", 2,
 *                                    "mc_neutrinos", "particle", ">=", 1,
 *                                    "di-jet", "particle", "==", 1));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class Cut : public CutAlgorithm {
public:
  //! Constructor
  /*!
   * Initializes the algorithm. The variable length argument at the end should conform 
   * to the following rules:\n
   * - It should be given in sets of four.
   * - The first should be the name of the algorithm to cut
   * - The second should be the type of cut
   * - The third should be the relational operator
   * - The fourth should be the value of the algorithm
   *
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] logic Determines how to logically string together the cuts. Can be "and" or "or."
   * \param[in] ncuts Number of cuts to make.
   * \param[in] ... Set of four values per cut as explained above.
   * \sa EmptyCut
   */
  Cut (TString name, TString title, TString logic, long long ncuts, ...);
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

//! Generic algorithm class that prints the content of an algorithm to a given ostream.
/*!
 * This algorithm is helpful in monitoring the particles produced or value stored by another
 * generic algorithm.
 * __Example:__\n
 * In your analysis file, do the following to monitor the output of an algorithm:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 * 
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("jets", "import basic jet objects"));
 *
 * a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("leading pt jet", "find highest pt jet", 
 *                                                      "jets",
 *                                                      1, "pt"));
 * 
 * a.AddAlgo(new HAL::Algorithms::MonitorAlgorithm("leading jet monitor", "look at the leading jet", 
 *                                                 "leading pt jet", 100));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class MonitorAlgorithm : public HAL::Algorithm {
public:
  //! Constructor
  /*!
   * Initializes the algorithm.
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] input Name of algorithm to select from.
   * \param[in] period Sets the period of output.
   * \param[in] os Stream to pipe all output to.
   */
  MonitorAlgorithm (TString name, TString title, TString input, long long period = 1, std::ostream &os = std::cout) :
    Algorithm(name, title), fN(period), fInput(input), fOS(&os) {}
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

//! Generic algorithm class that stores the properties of particles to a TTree
/*!
 * This algorithm stores particles or specific parts of particles in a specified TTree. 
 * The user needs only supply a branch name, property, and a tree name (optional). If the 
 * "all" property is given, the branch name given will act as the base for all other 
 * properties, i.e. if branch name is "jets", the branches written will be "jets_pt", 
 * "jets_eta", etc.
 * _Available Properties:_
 * | \f$ p_T \f$ | \f$ E_T \f$ | Mass | Energy | \f$ \left|\overrightarrow{p}\right| \f$ | \f$ \eta \f$ | \f$ \phi \f$ | ID | Charge | Full Particle | All Attributes |
 * | :-----------------: | :---------------: | :--: | :----: | :------------------: | :-: | :-: | :-: | :-: | :-: | :-: |
 * |  pT   | eT | m | e | p3 | eta | phi | id | charge | all | attributes |
 * __Example:__\n
 * In your analysis file, do the following to store jet particles:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 * 
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("basic jets", "import basic jet objects"));
 *
 * a.AddAlgo(new HAL::Algorithms::StoreParticle("store jets", "store jets", 
 *                                              "basic jets", "all", "jets"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class StoreParticle : public internal::ParticlesTLVStore {
public:
  //! Constructor
  /*!
   * Initializes the algorithm.
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] input Name of algorithm to store.
   * \param[in] property Property to store.
   * \param[in] bname Branch to store as.
   * \param[in] tname Tree to store in.
   */
  StoreParticle (TString name, TString title, TString input, TString property, 
                 TString bname, TString tname = "");
  virtual ~StoreParticle () {}

protected:
  virtual void  Init (Option_t* /*option*/);
  virtual void  StoreValue (HAL::AnalysisTreeWriter*, long long, HAL::ParticlePtr);

private:
  bool            fAll, fAttributes, fPt, fM, fE, fEt, fP3, fEta, fPhi, fID, fCharge;
  TString         fPtLabel, fP3Label, fEtaLabel, fEtLabel, fPhiLabel, fMLabel, fELabel, fIDLabel, fChargeLabel,
                  fTreeName;
};

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHMS */

