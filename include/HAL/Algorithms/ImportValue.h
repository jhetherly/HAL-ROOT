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

/// \todo Generic Algorithms: Make error messages more informative

#ifndef HAL_ALGORITHM_IMPORT_VALUE
#define HAL_ALGORITHM_IMPORT_VALUE

#include <TString.h>
#include <iostream>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>
#include <HAL/Algorithm.h>
#include <HAL/AnalysisData.h>
#include <HAL/AnalysisTreeReader.h>
#include <HAL/GenericData.h>


namespace HAL
{

/*
 * Generic base class algorithm
 * */

namespace internal
{

/*
 * Algorithm for importing a scalar value from a TTree.
 * */
template<typename ValueType>
class ImportValueAlgo : public HAL::Algorithm {
public:
  ImportValueAlgo (TString name, TString title);
  virtual ~ImportValueAlgo () {}

protected:
  virtual void  Exec (Option_t* /*option*/);
  virtual void  Clear (Option_t* /*option*/);

  virtual ValueType   GetValue () = 0;

  TString   fValueLabel, fUserDataLabel, fRefName;
};

} /* internal */ 



namespace Algorithms
{

//! Algorithm that stores a boolean value from information in a TTree.
/*!
 * This algorithm imports the information to store a boolean value from a specified branch in 
 * a TTree. The value from this algorithm is stored in a GenericData object in the 
 * UserData under the algorithm's name and <name>:value.\n\n
 * __Explaination of the branch map:__\n
 * The required map is just one that points to the relavent boolean value. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Required Branch Map:_
 * || Boolean value |
 * || :-----------: |
 * ||  <name>:bool  |
 * __Note:__ A custom value fetching class may be passed as a template arguement. This class
 * must have constructor that accepts the TTree as an arguement and overloads the ()
 * operator that accepts the entry number as an arguement. For example:
 * ~~~~~~~~~~~~~~{.cpp}
 * #include "TTreeReader.h" // ROOT 6 tree reading class
 * #include "TTreeReaderValue.h"
 *
 * class MyTreeReader {
 * public:
 *  // Needs a constructor like this
 *  MyTreeReader (TTree *t) : tr(t), value(tr, "my_branch") {}
 *  // Needs operator() overloaded like this
 *  bool operator (Long64_t entry) {
 *    tr.SetEntry(entry);
 *    return *value;
 *  }
 * private:
 *  TTreeReader tr;
 *  TTreeReaderValue<Bool_t> value;
 * };
 * ~~~~~~~~~~~~~~
 * __Examples:__\n
 * In your analysis file, do the following to import a trigger flag:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportBool("lepton trigger", "import the lepton trigger"));
 *
 * //...
 * 
 * a.MapBranch("some_trigger_branch", "lepton trigger:bool");
 * ~~~~~~~~~~~~~~~~~~~~~~
 * To import a value with a custom tree reading class, do the following:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * #include "MyTreeReader.h"
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportBoolValue<MyTreeReader>("lepton trigger", 
 *                                                              "import the lepton trigger"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
template<class ValueGetter = HAL::AnalysisTreeReader>
class ImportBoolValue : public HAL::internal::ImportValueAlgo<bool> {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa ImportParticle, ImportInteger, ImportCounting, ImportDecimal
   */
  ImportBoolValue (TString name, TString title);
  virtual ~ImportBoolValue () {if (fValueGetterPtr != NULL) delete fValueGetterPtr;}

protected:
  virtual bool   GetValue ();

  ValueGetter   *fValueGetterPtr;
};


//! Algorithm that stores an integer value from information in a TTree.
/*!
 * This algorithm imports the information to store an integer value from a specified branch in 
 * a TTree. The value from this algorithm is stored in a GenericData object in the 
 * UserData under the algorithm's name and <name>:value.\n\n
 * __Explaination of the branch map:__\n
 * The required map is just one that points to the relavent integer value. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Required Branch Map:_
 * || Integer value |
 * || :-----------: |
 * ||  <name>:integer  |
 * __Note:__ A custom value fetching class may be passed as a template arguement. This class
 * must have constructor that accepts the TTree as an arguement and overloads the ()
 * operator that accepts the entry number as an arguement. For example:
 * ~~~~~~~~~~~~~~{.cpp}
 * #include "TTreeReader.h" // ROOT 6 tree reading class
 * #include "TTreeReaderValue.h"
 *
 * class MyTreeReader {
 * public:
 *  // Needs a constructor like this
 *  MyTreeReader (TTree *t) : tr(t), value(tr, "my_branch") {}
 *  // Needs operator() overloaded like this
 *  long long operator (Long64_t entry) {
 *    tr.SetEntry(entry);
 *    return *value;
 *  }
 * private:
 *  TTreeReader tr;
 *  TTreeReaderValue<Int_t> value;
 * };
 * ~~~~~~~~~~~~~~
 * __Example:__\n
 * In your analysis file, do the following for an integer value:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportInteger("integer value", "import an integer value"));
 *
 * //...
 * 
 * a.MapBranch("some_integer_branch", "integer value:integer");
 * ~~~~~~~~~~~~~~~~~~~~~~
 * To import a value with a custom tree reading class, do the following:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * #include "MyTreeReader.h"
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportIntegerValue<MyTreeReader>("integer value", 
 *                                                                 "import an integer value"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
template<class ValueGetter = HAL::AnalysisTreeReader>
class ImportIntegerValue : public HAL::internal::ImportValueAlgo<long long> {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa ImportBool, ImportParticle, ImportCounting, ImportDecimal
   */
  ImportIntegerValue (TString name, TString title);
  virtual ~ImportIntegerValue () {if (fValueGetterPtr != NULL) delete fValueGetterPtr;}

protected:
  virtual long long  GetValue ();

  ValueGetter   *fValueGetterPtr;
};


//! Algorithm that stores a counting value from information in a TTree.
/*!
 * This algorithm imports the information to store a counting value from a specified branch in 
 * a TTree. The value from this algorithm is stored in a GenericData object in the 
 * UserData under the algorithm's name and <name>:value.\n\n
 * __Explaination of the branch map:__\n
 * The required map is just one that points to the relavent counting value. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Required Branch Map:_
 * || Counting value |
 * || :-----------: |
 * ||  <name>:counting  |
 * __Note:__ A custom value fetching class may be passed as a template arguement. This class
 * must have constructor that accepts the TTree as an arguement and overloads the ()
 * operator that accepts the entry number as an arguement. For example:
 * ~~~~~~~~~~~~~~{.cpp}
 * #include "TTreeReader.h" // ROOT 6 tree reading class
 * #include "TTreeReaderValue.h"
 *
 * class MyTreeReader {
 * public:
 *  // Needs a constructor like this
 *  MyTreeReader (TTree *t) : tr(t), value(tr, "my_branch") {}
 *  // Needs operator() overloaded like this
 *  unsigned long long operator (Long64_t entry) {
 *    tr.SetEntry(entry);
 *    return *value;
 *  }
 * private:
 *  TTreeReader tr;
 *  TTreeReaderValue<UInt_t> value;
 * };
 * ~~~~~~~~~~~~~~
 * __Example:__\n
 * In your analysis file, do the following for a counting value:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportCounting("counting value", "import a counting value"));
 *
 * //...
 * 
 * a.MapBranch("some_counting_branch", "counting value:counting");
 * ~~~~~~~~~~~~~~~~~~~~~~
 * To import a value with a custom tree reading class, do the following:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * #include "MyTreeReader.h"
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportCountingValue<MyTreeReader>("counting value", 
 *                                                                  "import an counting value"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
template<class ValueGetter = HAL::AnalysisTreeReader>
class ImportCountingValue : public HAL::internal::ImportValueAlgo<unsigned long long> {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa ImportBool, ImportInteger, ImportParticle, ImportDecimal
   */
  ImportCountingValue (TString name, TString title);
  virtual ~ImportCountingValue () {}

protected:
  virtual unsigned long long  GetValue ();

  ValueGetter   *fValueGetterPtr;
};


//! Algorithm that stores a decimal value from information in a TTree.
/*!
 * This algorithm imports the information to store a decimal value from a specified branch in 
 * a TTree. The value from this algorithm is stored in a GenericData object in the 
 * UserData under the algorithm's name and <name>:value.\n\n
 * __Explaination of the branch map:__\n
 * The required map is just one that points to the relavent decimal value. <name> refers to 
 * the name given to this algorithm's constructor.\n
 * _Required Branch Map:_
 * || Decimal value |
 * || :-----------: |
 * ||  <name>:decimal  |
 * __Note:__ A custom value fetching class may be passed as a template arguement. This class
 * must have constructor that accepts the TTree as an arguement and overloads the ()
 * operator that accepts the entry number as an arguement. For example:
 * ~~~~~~~~~~~~~~{.cpp}
 * #include "TTreeReader.h" // ROOT 6 tree reading class
 * #include "TTreeReaderValue.h"
 *
 * class MyTreeReader {
 * public:
 *  // Needs a constructor like this
 *  MyTreeReader (TTree *t) : tr(t), value(tr, "my_branch") {}
 *  // Needs operator() overloaded like this
 *  long double operator (Long64_t entry) {
 *    tr.SetEntry(entry);
 *    return *value;
 *  }
 * private:
 *  TTreeReader tr;
 *  TTreeReaderValue<Float_t> value;
 * };
 * ~~~~~~~~~~~~~~
 * __Example:__\n
 * In your analysis file, do the following for a decimal value:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportDecimal("decimal value", "import a decimal value"));
 *
 * //...
 * 
 * a.MapBranch("some_decimal_branch", "decimal value:decimal");
 * ~~~~~~~~~~~~~~~~~~~~~~
 * To import a value with a custom tree reading class, do the following:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * #include "MyTreeReader.h"
 * HAL::Analysis a("sample analysis", "", "truth");
 *
 * a.AddAlgo(new HAL::Algorithms::ImportDecimalValue<MyTreeReader>("decimal value", 
 *                                                                 "import an decimal value"));
 * ~~~~~~~~~~~~~~~~~~~~~~
 */
template<class ValueGetter = HAL::AnalysisTreeReader>
class ImportDecimalValue : public HAL::internal::ImportValueAlgo<long double> {
public:
  //! Constructor
  /*!
   * Initializes the algorithm
   * \param[in] name Name of the algorithm. This can be used as the input to other 
   * algorithms.
   * \param[in] title Description of the algorithm. Can be an empty string.
   * \sa ImportBool, ImportInteger, ImportCounting, ImportParticle
   */
  ImportDecimalValue (TString name, TString title);
  virtual ~ImportDecimalValue () {}

protected:
  virtual long double  GetValue ();

  ValueGetter   *fValueGetterPtr;
};

} /* Algorithms */ 


#ifndef __CINT__

/*
 * Generic class
 * */
template<typename ValueType>
internal::ImportValueAlgo<ValueType>::ImportValueAlgo (TString name, TString title) :
  HAL::Algorithm(name, title) {

  fUserDataLabel = TString::Format("%s:value", name.Data());
}

template<typename ValueType>
void  internal::ImportValueAlgo<ValueType>::Exec (Option_t* /*option*/) {
  HAL::AnalysisData *data = GetUserData();
  HAL::GenericData *gen_data = new GenericData(GetName());

  data->SetValue(GetName(), gen_data);

  // Save actual value to user data
  data->SetValue(fUserDataLabel, GetValue());

  gen_data->SetRefName(fUserDataLabel);
  gen_data->SetRefType(fRefName.Data());
}

template<typename ValueType>
void  internal::ImportValueAlgo<ValueType>::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}




template<class ValueGetter>
Algorithms::ImportBoolValue<ValueGetter>::ImportBoolValue (TString name, TString title) : 
  ImportValueAlgo<bool>(name, title), fValueGetterPtr(NULL) {
  fRefName = "bool";
  fValueLabel = TString::Format("%s:%s", name.Data(), fRefName.Data());
}

template<class ValueGetter>
inline bool Algorithms::ImportBoolValue<ValueGetter>::GetValue () {
  if (fValueGetterPtr == NULL) 
    fValueGetterPtr = new ValueGetter(GetRawData()->GetTree());
  return (*fValueGetterPtr)(GetRawData()->GetEntryNumber());
}

template<>
inline bool Algorithms::ImportBoolValue<HAL::AnalysisTreeReader>::GetValue () {
  return GetRawData()->GetBool(fValueLabel);
}

template<class ValueGetter>
Algorithms::ImportIntegerValue<ValueGetter>::ImportIntegerValue (TString name, TString title) : 
  ImportValueAlgo<long long>(name, title), fValueGetterPtr(NULL) {
  fRefName = "integer";
  fValueLabel = TString::Format("%s:%s", name.Data(), fRefName.Data());
}

template<class ValueGetter>
inline long long Algorithms::ImportIntegerValue<ValueGetter>::GetValue () {
  if (fValueGetterPtr == NULL) 
    fValueGetterPtr = new ValueGetter(GetRawData()->GetTree());
  return (*fValueGetterPtr)(GetRawData()->GetEntryNumber());
}

template<>
inline long long Algorithms::ImportIntegerValue<HAL::AnalysisTreeReader>::GetValue () {
  return GetRawData()->GetInteger(fValueLabel);
}

template<class ValueGetter>
Algorithms::ImportCountingValue<ValueGetter>::ImportCountingValue (TString name, TString title) : 
  ImportValueAlgo<unsigned long long>(name, title), fValueGetterPtr(NULL) {
  fRefName = "counting";
  fValueLabel = TString::Format("%s:%s", name.Data(), fRefName.Data());
}

template<class ValueGetter>
inline unsigned long long Algorithms::ImportCountingValue<ValueGetter>::GetValue () {
  if (fValueGetterPtr == NULL) 
    fValueGetterPtr = new ValueGetter(GetRawData()->GetTree());
  return (*fValueGetterPtr)(GetRawData()->GetEntryNumber());
}

template<>
inline unsigned long long Algorithms::ImportCountingValue<HAL::AnalysisTreeReader>::GetValue () {
  return GetRawData()->GetCounting(fValueLabel);
}

template<class ValueGetter>
Algorithms::ImportDecimalValue<ValueGetter>::ImportDecimalValue (TString name, TString title) : 
  ImportValueAlgo<long double>(name, title), fValueGetterPtr(NULL) {
  fRefName = "decimal";
  fValueLabel = TString::Format("%s:%s", name.Data(), fRefName.Data());
}

template<class ValueGetter>
inline long double Algorithms::ImportDecimalValue<ValueGetter>::GetValue () {
  if (fValueGetterPtr == NULL) 
    fValueGetterPtr = new ValueGetter(GetRawData()->GetTree());
  return (*fValueGetterPtr)(GetRawData()->GetEntryNumber());
}

template<>
inline long double Algorithms::ImportDecimalValue<HAL::AnalysisTreeReader>::GetValue () {
  return GetRawData()->GetDecimal(fValueLabel);
}

#endif

} /* HAL */ 

#endif /* end of include guard: HAL_ALGORITHM_IMPORT_VALUE */

