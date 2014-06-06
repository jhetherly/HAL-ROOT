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

#ifndef HAL_ALGORITHM_SELECT_PARTICLE
#define HAL_ALGORITHM_SELECT_PARTICLE

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

} /* internal */ 



namespace Algorithms
{


//! Algorithm that selects particles with specified properties.
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

} /* Algorithms */ 

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_SELECT_PARTICLE */

