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

#ifndef HAL_ALGORITHM_SELECT_RANK
#define HAL_ALGORITHM_SELECT_RANK

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
 * Generic base class algorithm
 * */

namespace internal
{

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

} /* internal */ 



namespace Algorithms
{



/*
 * Filtering Algorithms
 * */

//! Algorithm that selects the particle with the highest or lowest property.
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
 * a.AddAlgo(new HAL::Algorithms::SelectRank("highest pT muon", "find highest pT muon", 
 *                                           "muons", 1, "pT", "high"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class SelectRank : public internal::NthElementAlgo {
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
  SelectRank (TString name, TString title, TString input, unsigned rank, 
              TString property, TString end = "high");
  virtual ~SelectRank () {}

  virtual TString       SortTag ();
  virtual bool          operator() (ParticlePtr, ParticlePtr);
  virtual void          Sort (ParticlePtrs&);

protected:
  bool      fPt, fM, fE, fEt, fP3, fHigh, fLow;
  TString   fTLVProperty, fEnd;
};

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_SELECT_RANK */

