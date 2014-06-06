// @(#)hal
// Author: Jeff Hetherly   5/6/14

/*************************************************************************
 * Copyright (C) 2014, Jeff Hetherly.                                    *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 *************************************************************************/

// Doxygen
/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 *
 * \section LICENSE
 */

#ifndef HAL_Algorithm
#define HAL_Algorithm

#include <TString.h>
#include <TNamed.h>
#include <TObject.h>
#include <TList.h>
#include <TMath.h>
#include <list>
#include <iostream>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/AnalysisData.h>

// forward declaration(s)
namespace HAL
{
class AnalysisTreeReader;
class AnalysisTreeWriter;
} /* HAL */ 

namespace HAL
{

//! Base class for all analysis algorithms
/*!
 * This class serves as the parent class for all HAL::Analysis and user-defined
 * algorithms. It contains hooks for all parts of the calling TSelector, i.e.
 * SlaveBegin, Process (through Exec and Clear), Terminate, etc. It also has
 * methods for keeping track of how many objects it creates. Most importantly,
 * this class can pass information to other algorithms through a common data 
 * store accessed through string names and a few specilized methods.
 */
class Algorithm {
public:
  // (Almost) carbon copy from the TTask class
  // (Since user doesn't inherit from TObject, user's shared 
  //  lib could be more easily imported to python using
  //  rootcint)
  
  // Constructor(s)/Destructor -----------------
  
  //! Constructor
  /*!
   * Initializes the algorithm with a name and optional title
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa CutAlgorithm
   */
  Algorithm(TString name = "", TString title = "");
  //! Copy Constructor
  /*!
   * Copies relavent data from another Algorithm
   * \param[in] algo Algorithm to be copied
   */
  Algorithm (const Algorithm &algo);
  virtual ~Algorithm();
  // -------------------------------------------

  // Setup -------------------------------------
  
  //! Attach another algorithm to this one.
  /*!
   * Attaches an algorithm to this algorithm's list of sub-algorithms.
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa CutAlgorithm
   */
  void          Add (Algorithm *algo) { fAlgorithms.push_back(algo); }
  void          ls ();
  void          CounterSummary ();
  void          CutReport ();
  void          DeleteAlgos ();
  void          SetName (TString name) {fName = name;}
  void          SetTitle (TString title) {fTitle = title;}
  void          SetOutputFileName (TString filename);
  // -------------------------------------------
  
  // Flow control ------------------------------
  void          Abort ();
  void          CleanAlgos ();
  void          ExecuteAlgo (Option_t *option = "0");
  void          ExecuteAlgos (Option_t *option);
  void          InitializeAlgo (Option_t *option);
  void          BeginAlgo (Option_t *option);
  void          SlaveBeginAlgo (Option_t *option);
  void          NotifyAlgo (Option_t *option);
  void          SlaveTerminateAlgo (Option_t *option);
  void          TerminateAlgo (Option_t *option);
  // -------------------------------------------
  
  // DataList related --------------------------
  void          AddData (TString name, TObject *obj);
  TObject*      GetData (TString name);
  Bool_t        CheckData (TString name);
  void          DeleteData (TString name);
  void          AssignDataList (TList *list); 
  AnalysisTreeReader*   GetRawData () {return ((AnalysisTreeReader*)fDataList->FindObject("RawData"));}
  AnalysisData*         GetUserData () {return ((AnalysisData*)fDataList->FindObject("UserData"));}
  AnalysisTreeWriter*   GetUserOutput () {return ((AnalysisTreeWriter*)fDataList->FindObject("UserOutput"));}
  // -------------------------------------------
  
  // Data related ------------------------------
  TString       GetName () {return fName;}
  TString       GetTitle () {return fTitle;}
  TString       GetOutputFileName () {return fOutputFileName;}
  void          IncreaseCounter (long long n = 1) {fPrintCounter = true; fCounter += n;}
  long long     GetCounter () {return fCounter;}
  // -------------------------------------------

protected:
  // User can override these -------------------
  virtual void  Begin (Option_t * /*options*/ = "") {}
  virtual void  SlaveBegin (Option_t * /*options*/ = "") {}
  virtual void  Init (Option_t * /*options*/ = "") {}
  virtual void  Notify (Option_t * /*options*/ = "") {}
  virtual void  Exec (Option_t * /*options*/ = "") {}
  virtual void  Clear (Option_t * /*options*/ = "") {}
  virtual void  SlaveTerminate (Option_t * /*options*/ = "") {}
  virtual void  Terminate (Option_t * /*options*/ = "") {}
  // -------------------------------------------

  TList     *fDataList; // borrowed ptr
  TString    fAlgorithmType;
  long long  fCounter;

private:
  void       ls(TString indention);
  void       counter_summary(TString indention);
  void       cut_report(TString indention, long long &base_number, long long &prev_number);

  TString    fName, fTitle, fOutputFileName;
  bool       fPrintCounter;

  std::list<Algorithm*> fAlgorithms;
  TString               fOption;       //Option specified in ExecuteAlgo
  Bool_t                fHasExecuted;  //True if algo has executed
  Bool_t                fAbort;        //True if algo has signaled an abort

  ClassDef(Algorithm, 0);

};

} /* HAL */ 

#endif
