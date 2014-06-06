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

#ifndef HAL_ALGORITHM_STORE_PARTICLE
#define HAL_ALGORITHM_STORE_PARTICLE

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
 * Exporting Algorithms
 * */

//! Algorithm that stores the properties of particles to a TTree
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

#endif /* end of include guard: HAL_ALGORITHM_STORE_PARTICLE */

