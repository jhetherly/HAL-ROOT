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

#ifndef HAL_ALGORITHM_ATTACH_ATTRIBUTE
#define HAL_ALGORITHM_ATTACH_ATTRIBUTE

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

} /* internal */ 



namespace Algorithms
{


/*
 * Importing algorithm
 * */

//! Algorithm that attaches a decimal value to an existing set of particles
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

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_ATTACH_ATTRIBUTE */

