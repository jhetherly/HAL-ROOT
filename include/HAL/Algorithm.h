/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 */

#ifndef HAL_Algorithm
#define HAL_Algorithm

#include <list>
#include <TString.h>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>

// forward declaration(s)
class TObject;
class TList;

namespace HAL
{
class AnalysisData;
class AnalysisTreeReader;
class AnalysisTreeWriter;
}
// end forward declaration(s)

namespace HAL
{

//! Base class for all analysis algorithms
/*!
 * This class serves as the parent class for all HAL analysis and user-defined
 * algorithms. It contains hooks for all parts of the calling TSelector, i.e.
 * SlaveBegin, Process (through Exec and Clear), Terminate, etc. It also has
 * methods for keeping track of how many objects it creates. Most importantly,
 * this class can pass information to other algorithms through a common data 
 * store accessed through string names and a few specilized methods.
 */
class Algorithm {
  // (Almost) carbon copy from the TTask class
  // (Since user doesn't inherit from TObject, user's shared 
  //  lib could be more easily imported to python using
  //  rootcint/rootcling)
  
private:

  Bool_t                fPrintCounter;    //Track counting statistics
  std::list<Algorithm*> fAlgorithms;      //List of sub-algorithms
  TString               fOption,          //Option specified in ExecuteAlgo
                        fName,            //Name of algorithm
                        fTitle,           //Description of algorithm (may be empty)
                        fOutputFileName;  //Name of file output is send to
  Bool_t                fHasExecuted;     //True if algo has executed
  Bool_t                fAbort;           //True if algo has signaled an abort

  void       PrintAlgorithmHierarchy (TString indention);
  void       CounterSummaryHelper (TString indention);
  void       CutReportHelper (TString indention, Long64_t &base_number, Long64_t &prev_number);

protected:

  TList     *fDataList;       //Borrowed pointer to the shared data storage
  TString    fAlgorithmType;  //Type of algorithm ("Cut" signals efficiency computation)
  Long64_t   fCounter;        //Object creation counter

  //! Hook into the TSelector::Begin method
  /*!
   * This method allows the algorithm operate at the 'Begin' stage 
   * of processing a TTree.
   * \param[in] option Option string passed to all algorithms from the 
   *                   Analysis::Process call
   */
  virtual void  Begin (Option_t * /*options*/ = "") {}

  //! Hook into the TSelector::SlaveBegin method
  /*!
   * This method allows the algorithm operate at the 'SlaveBegin' stage 
   * of processing a TTree.
   * \param[in] option Option string passed to all algorithms from the 
   *                   Analysis::Process call
   */
  virtual void  SlaveBegin (Option_t * /*options*/ = "") {}

  //! Hook into the TSelector::Init method
  /*!
   * This method allows the algorithm operate at the 'Init' stage 
   * of processing a TTree. This method is called when a new TTree
   * is being prepared for processing.
   * \param[in] option Option string passed to all algorithms from the 
   *                   Analysis::Process call
   */
  virtual void  Init (Option_t * /*options*/ = "") {}
  
  //! Hook into the TSelector::Notify method
  /*!
   * This method allows the algorithm operate at the 'Notify' stage 
   * of processing a TTree. This method is called when a new TTree
   * has been loaded for processing.
   * \param[in] option Option string passed to all algorithms from the 
   *                   Analysis::Process call
   */
  virtual void  Notify (Option_t * /*options*/ = "") {}

  //! Hook into the TSelector::Process method
  /*!
   * This method allows the algorithm operate at the first part of the 
   * 'Process' stage of processing a TTree. This where most of the
   * logic of an algorithm will likely be contained.
   * \param[in] option Option string passed to all algorithms from the 
   *                   Analysis::Process call
   */
  virtual void  Exec (Option_t * /*options*/ = "") {}

  //! Hook into the TSelector::Process method
  /*!
   * This method allows the algorithm operate at the second part of the 
   * 'Process' stage of processing a TTree. This is meant for any 
   * per-event memory management.
   * \param[in] option Option string passed to all algorithms from the 
   *                   Analysis::Process call
   */
  virtual void  Clear (Option_t * /*options*/ = "") {}

  //! Hook into the TSelector::SlaveTerminate method
  /*!
   * This method allows the algorithm operate at the 'SlaveTerminate' stage 
   * of processing a TTree.
   * \param[in] option Option string passed to all algorithms from the 
   *                   Analysis::Process call
   */
  virtual void  SlaveTerminate (Option_t * /*options*/ = "") {}

  //! Hook into the TSelector::Terminate method
  /*!
   * This method allows the algorithm operate at the 'Terminate' stage 
   * of processing a TTree.
   * \param[in] option Option string passed to all algorithms from the 
   *                   Analysis::Process call
   */
  virtual void  Terminate (Option_t * /*options*/ = "") {}

public:

  // Constructor(s)/Destructor ------------------------------------------
  
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
  // --------------------------------------------------------------------

  // Setup --------------------------------------------------------------
  
  //! Attach another algorithm to this one.
  /*!
   * Attaches an algorithm to this algorithm's list of sub-algorithms.
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   */
  void          Add (Algorithm *algo);

  //! \cond NODOC
  void          ls ();
  void          CounterSummary ();
  void          CutReport ();
  void          DeleteAlgos ();
  void          SetName (TString name) {fName = name;}
  void          SetTitle (TString title) {fTitle = title;}
  void          SetOutputFileName (TString filename);
  //! \endcond
  // --------------------------------------------------------------------
  
  // Flow control -------------------------------------------------------
  
  //! Cease further sub-algorithm execution.
  /*!
   * This function flags the algorithm flow to cease and calls the Clear
   * method for the previously executed algorithms. Calling algorithm should
   * take care to clean up its own memory if necessary.
   */
  void          Abort ();

  //! \cond NODOC
  void          CleanAlgos ();
  void          ExecuteAlgo (Option_t *option);
  void          ExecuteAlgos (Option_t *option);
  void          InitializeAlgo (Option_t *option);
  void          BeginAlgo (Option_t *option);
  void          SlaveBeginAlgo (Option_t *option);
  void          NotifyAlgo (Option_t *option);
  void          SlaveTerminateAlgo (Option_t *option);
  void          TerminateAlgo (Option_t *option);
  //! \endcond
  // --------------------------------------------------------------------
  
  // DataList related ---------------------------------------------------
  
  //! Attach an object to the common data store
  /*!
   * Attach an object inheriting from TObject to the common data store.
   * \param[in] name Name of the data. This can be used to later retrieve 
   * the object.
   * \param[in] obj Pointer to object to store.
   */
  void          AddData (TString name, TObject *obj);

  //! Retrieve an object to the common data store
  /*!
   * Retrieve an object inheriting from TObject to the common data store.
   * \param[in] name Name of the data to retrieve. Remember to cast object
   * from TObject.
   */
  TObject*      GetData (TString name);

  //! Check the on object of a given name is stored in the common data store
  /*!
   * \param[in] name Name of the data to check.
   */
  Bool_t        CheckData (TString name);

  //! Remove an object from the common data store
  /*!
   * \param[in] name Name of the data to remove.
   */
  void          DeleteData (TString name);

  //! \cond NODOC
  void          AssignDataList (TList *list); 
  //! \endcond
  
  //! Convenience function for retrieving the AnalysisTreeReader object
  /*!
   * Convenience function for retrieving the AnalysisTreeReader object 
   * from the common data store. This is meant as an easy, universal
   * way of accessing the data in a TTree.
   */
  AnalysisTreeReader*   GetRawData ();

  //! Convenience function for retrieving the AnalysisData object
  /*!
   * Convenience function for retrieving the AnalysisData object 
   * from the common data store. This is meant as an easy, universal 
   * way to pass data from one algorithm to another.
   */
  AnalysisData*         GetUserData ();

  //! Convenience function for retrieving the AnalysisTreeWriter object
  /*!
   * Convenience function for retrieving the AnalysisTreeWriter object 
   * from the common data store. This is meant as an easy, universal way
   * of writing the output of an algorithm to a TTree in a bin-less 
   * fashion.
   */
  AnalysisTreeWriter*   GetUserOutput ();
  // --------------------------------------------------------------------
  
  // Data related -------------------------------------------------------

  //! Retrieves the name of this algorithm
  TString       GetName () {return fName;}

  //! Retrieves the title of this algorithm
  TString       GetTitle () {return fTitle;}

  //! Retrieves the algorithm or sub-algorithm of given name
  /*!
   * \param[in] name Name of algorithm to return.
   */
  Algorithm*    GetAlgorithm (TString name);

  //! Increments the object creation counter for this algorithm
  /*!
   * \param[in] n Number by which to increment.
   */
  void          IncreaseCounter (Long64_t n = 1) {fPrintCounter = true; fCounter += n;}

  //! \cond NODOC
  TString       GetOutputFileName () {return fOutputFileName;}
  Long64_t      GetCounter () {return fCounter;}
  //! \endcond
  // --------------------------------------------------------------------

  ClassDef(Algorithm, 0);

};

} /* HAL */ 

#endif
