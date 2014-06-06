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

// \todo Generic Algorithms: Make error messages more informative

#ifndef HAL_ALGOIRTHM_CUT
#define HAL_ALGOIRTHM_CUT

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

namespace internal 
{

// forward declare helper class
struct AlgoInfo;

}

namespace Algorithms
{


/*
 * Cutting Algorithms
 * */

//! Algorithm that serves as a baseline algorithm for any subsequent Cut algorithms
/*!
 * This algorithm passes all events. It can serve as a baseline cut in an analysis.
 * __Example:__\n
 * In your analysis file, do the following to create a baseline cut:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 * 
 * a.AddAlgo(new HAL::Algorithms::EmptyCut("number of events", "baseline event number"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class EmptyCut : public HAL::CutAlgorithm {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa Cut
   */
  EmptyCut (TString name, TString title) : CutAlgorithm(name, title) {}
  virtual ~EmptyCut () {}

protected:
  virtual void Exec (Option_t* /*option*/) {Passed();}
};


//! Algorithm that cuts on particle multiplicity, bool, integer, counting, and decimal values
/*!
 * This algorithm provides a general mechanism to cut on several different algorithms. An important
 * aspect of this algorithm is its ability to either "and" or "or" different predicates together.
 * Combined with the proper selection algorithms, a cut on particle multiplicity is sufficient for
 * most situations. The relational operators and types needed to specify a cut are listed below.\n
 * _Available Types of Cuts:_
 * | Boolean | Integer | Counting | Decimal | Particle Multiplicity |
 * | :-----: | :-----: | :------: | :-----: | :-------------------: |
 * |  bool   | integer | counting | decimal | particle |
 * _Available Logical Relationships:_
 * | Equal | Not Equal | Greater Than | Greater Than or Equal To | Less Than | Less Than or Equal To |
 * | :-----------------: | :---------------: | :--: | :----: | :------------------: | :-: |
 * |  ==   | != | > | >= | < | <= |
 * _Note:_ = may also be used in place of ==.\n\n
 * __Example:__\n
 * In your analysis file, do the following to create a cut:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 * 
 * //...
 *
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("jets", "import basic jet objects"));
 * a.AddAlgo(new HAL::Algorithms::ImportParticle("mc", "import basic Monte Carlo objects"));
 *
 * a.AddAlgo(new HAL::Algorithms::EmptyCut("number of events", "baseline event number"));
 *
 * a.AddAlgo(new HAL::Algorithms::SelectParticle("mc_neutrinos", "filter on mc id to get neutrinos", 
 *                                               "mc",
 *                                               "id", 6,
 *                                               -16, -14, -12, 12, 14, 16));
 * 
 * a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("leading pt jet", "find highest pt jet", 
 *                                                      "jets",
 *                                                      1, "pt"));
 * a.AddAlgo(new HAL::Algorithms::ParticleRankSelection("subleading pt jet", "find 2nd highest pt jet", 
 *                                                      "jets",
 *                                                      2, "pt"));
 * 
 * a.AddAlgo(new HAL::Algorithms::VecAddReco("di-jet", "reconstruct a di-jet object from highest pt", 
 *                                           2, "leading pt jet", "subleading pt jet"));
 * 
 * a.AddAlgo(new HAL::Algorithms::Cut("di-jet and neutrino cut", "make sure dijet and neutrino(s) exist", 
 *                                    "and", 2,
 *                                    "mc_neutrinos", "particle", ">=", 1,
 *                                    "di-jet", "particle", "==", 1));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
class Cut : public CutAlgorithm {
public:
  //! Constructor
  /*!
   * Initializes the algorithm. The variable length argument at the end should conform 
   * to the following rules:\n
   * - It should be given in sets of four.
   * - The first should be the name of the algorithm to cut
   * - The second should be the type of cut
   * - The third should be the relational operator
   * - The fourth should be the value of the algorithm
   *
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \param[in] logic Determines how to logically string together the cuts. Can be "and" or "or."
   * \param[in] ncuts Number of cuts to make.
   * \param[in] ... Set of four values per cut as explained above.
   * \sa EmptyCut
   */
  Cut (TString name, TString title, TString logic, long long ncuts, ...);
  virtual ~Cut ();

protected:
  virtual void Exec (Option_t* /*option*/);

private:
  bool          fAnd, fOr;

//public:
//  struct AlgoInfo {
//    AlgoInfo () 
//      : fEqual(false), fNotEqual(false), fLessThan(false), 
//        fGreaterThan(false), fLessThanEqual(false), fGreaterThanEqual(false) {}
//    virtual ~AlgoInfo () {}
//    const char    *fName;
//    bool           fEqual, fNotEqual, fLessThan, fGreaterThan, fLessThanEqual, fGreaterThanEqual;
//    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*) = 0;
//  };
//
//  struct BoolAlgoInfo : public AlgoInfo {
//    bool           fValue;
//    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
//  };
//  
//  struct IntegerAlgoInfo : public AlgoInfo {
//    long long      fValue;
//    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
//  };
//
//  struct CountingAlgoInfo : public AlgoInfo {
//    unsigned long long fValue;
//    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
//  };
//
//  struct DecimalAlgoInfo : public AlgoInfo {
//    long double    fValue;
//    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
//  };
//
//  struct NParticlesAlgoInfo : public AlgoInfo {
//    long long      fValue;
//    virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
//  };
//
//  std::vector<AlgoInfo*>    fAlgorithms;
  std::vector<HAL::internal::AlgoInfo*>    fAlgorithms;
};

} /* Algorithms */ 

namespace internal
{

struct AlgoInfo {
  AlgoInfo () 
    : fEqual(false), fNotEqual(false), fLessThan(false), 
    fGreaterThan(false), fLessThanEqual(false), fGreaterThanEqual(false) {}
  virtual ~AlgoInfo () {}
  const char    *fName;
  bool           fEqual, fNotEqual, fLessThan, fGreaterThan, fLessThanEqual, fGreaterThanEqual;
  virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*) = 0;
};

struct BoolAlgoInfo : public AlgoInfo {
  bool           fValue;
  virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
};

struct IntegerAlgoInfo : public AlgoInfo {
  long long      fValue;
  virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
};

struct CountingAlgoInfo : public AlgoInfo {
  unsigned long long fValue;
  virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
};

struct DecimalAlgoInfo : public AlgoInfo {
  long double    fValue;
  virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
};

struct NParticlesAlgoInfo : public AlgoInfo {
  long long      fValue;
  virtual bool  Eval (HAL::AnalysisData*, HAL::GenericData*);
};

} /* internal */

} /* HAL */ 

#endif /* end of include guard: HAL_ALGOIRTHM_CUT */

