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

#ifndef HAL_ALGORITHM_VEC_ADD_RECO
#define HAL_ALGORITHM_VEC_ADD_RECO

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

namespace Algorithms
{


/*
 * Reconstruction Algorithms
 * */

//! Algorithm that combines tuples of particles from any number of alogrithms.
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

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_VEC_ADD_RECO */

