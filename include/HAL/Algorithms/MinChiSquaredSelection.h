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

#ifndef HAL_ALGORITHM_MINCHISQUAREDSELECTION
#define HAL_ALGORITHM_MINCHISQUAREDSELECTION

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

//! Algorithm that performs a chi-squared minimization
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

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_MINCHISQUAREDSELECTION */

