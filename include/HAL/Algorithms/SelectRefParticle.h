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

#ifndef HAL_ALGORITHM_SELECT_REF_PARTICLE
#define HAL_ALGORITHM_SELECT_REF_PARTICLE

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

} /* internal */ 



namespace Algorithms
{


//! Algorithm that selects particles with respect to a set of reference particles
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

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_SELECT_REF_PARTICLE */

