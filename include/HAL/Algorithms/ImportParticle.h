/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
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

#ifndef HAL_ALGORITHM_IMPORT_PARTICLE
#define HAL_ALGORITHM_IMPORT_PARTICLE

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

} /* internal */ 



namespace Algorithms
{


/*
 * Importing algorithms
 * */

//! Algorithm that builds particles from information in a TTree.
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

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_IMPORT_PARTICLE */

