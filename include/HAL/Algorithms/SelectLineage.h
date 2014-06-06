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

#ifndef HAL_ALGORITHM_SELECT_LINEAGE
#define HAL_ALGORITHM_SELECT_LINEAGE

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

} /* internal */ 



namespace Algorithms
{


} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_SELECT_LINEAGE */

