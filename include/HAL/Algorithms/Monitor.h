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

#ifndef HAL_ALGORITHM_MONITOR
#define HAL_ALGORITHM_MONITOR

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
 * Monitoring Algorithms
 * */

//! Algorithm that prints the content of an algorithm to a given ostream.
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

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_MONITOR */

