#include <HAL/AnalysisData.h>
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
#include <aux/boost/foreach.hpp>
#endif
#include <TRegexp.h>

ClassImp(HAL::AnalysisData);

namespace HAL
{

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const bool &v) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kB;
  else if (fNameTypeMap[name] != kB)
    throw HALException(name.insert(0, "Cannot reassign bool storage container for "));

  fBoolMap[name] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const long double &v) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kD;
  else if (fNameTypeMap[name] == kI)
    SetValue(name, (long long)v);
  else if (fNameTypeMap[name] == kC && v >= 0.0)
    SetValue(name, (unsigned long long)v);
  else if (fNameTypeMap[name] != kD)
    throw HALException(name.insert(0, "Cannot reassign decimal storage container for "));

  fDecimalMap[name] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const long long &v) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kI;
  else if (fNameTypeMap[name] == kD)
    SetValue(name, (long double)v);
  else if (fNameTypeMap[name] == kC && v >= 0)
    SetValue(name, (unsigned long long)v);
  else if (fNameTypeMap[name] != kI)
    throw HALException(name.insert(0, "Cannot reassign integer storage container for "));

  fIntegerMap[name] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const unsigned long long &v) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kC;
  else if (fNameTypeMap[name] == kD)
    SetValue(name, (long double)v);
  else if (fNameTypeMap[name] == kI)
    SetValue(name, (long long)v);
  else if (fNameTypeMap[name] != kC)
    throw HALException(name.insert(0, "Cannot reassign counting storage container for "));

  fCountingMap[name] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const std::string &v) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kS;
  else if (fNameTypeMap[name] != kS)
    throw HALException(name.insert(0, "Cannot reassign string storage container for "));

  fStringMap[name] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, TObject *v) 
{ 
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kO;
  else if (fNameTypeMap[name] != kO)
    throw HALException(name.insert(0, "Cannot reassign TObject storage container for "));

  fTObjectMap[name] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const bool &v, const long long &i) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIB;
  else if (fNameTypeMap[name] != kIB)
    throw HALException(name.insert(0, "Cannot reassign 1D bool storage container for "));

  fBoolIntMap[name][i] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const long double &v, const long long &i) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kID;
  else if (fNameTypeMap[name] == kII)
    SetValue(name, (long long)v, i);
  else if (fNameTypeMap[name] == kIC && v >= 0.0)
    SetValue(name, (unsigned long long)v, i);
  else if (fNameTypeMap[name] != kID)
    throw HALException(name.insert(0, "Cannot reassign 1D decimal storage container for "));

  fDecimalIntMap[name][i] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const long long &v, const long long &i) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kII;
  else if (fNameTypeMap[name] == kID)
    SetValue(name, (long double)v, i);
  else if (fNameTypeMap[name] == kIC && v >= 0)
    SetValue(name, (unsigned long long)v, i);
  else if (fNameTypeMap[name] != kII)
    throw HALException(name.insert(0, "Cannot reassign 1D integer storage container for "));

  fIntegerIntMap[name][i] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const unsigned long long &v, const long long &i) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIC;
  else if (fNameTypeMap[name] == kID)
    SetValue(name, (long double)v, i);
  else if (fNameTypeMap[name] == kII)
    SetValue(name, (long long)v, i);
  else if (fNameTypeMap[name] != kIC)
    throw HALException(name.insert(0, "Cannot reassign 1D counting storage container for "));

  fCountingIntMap[name][i] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const std::string &v, const long long &i) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIS;
  else if (fNameTypeMap[name] != kIS)
    throw HALException(name.insert(0, "Cannot reassign 1D string storage container for "));

  fStringIntMap[name][i] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, TObject *v, const long long &i) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIO;
  else if (fNameTypeMap[name] != kIO)
    throw HALException(name.insert(0, "Cannot reassign 1D TObject storage container for "));

  fTObjectIntMap[name][i] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const bool &v, const long long &i, const long long &j) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIIB;
  else if (fNameTypeMap[name] != kIIB)
    throw HALException(name.insert(0, "Cannot reassign 2D bool storage container for "));

  fBoolIntIntMap[name][i][j] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const long double &v, const long long &i, const long long &j) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIID;
  else if (fNameTypeMap[name] == kIII)
    SetValue(name, (long long)v, i, j);
  else if (fNameTypeMap[name] == kIIC && v >= 0.0)
    SetValue(name, (unsigned long long)v, i, j);
  else if (fNameTypeMap[name] != kIID)
    throw HALException(name.insert(0, "Cannot reassign 2D decimal storage container for "));

  fDecimalIntIntMap[name][i][j] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const long long &v, const long long &i, const long long &j) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIII;
  else if (fNameTypeMap[name] == kIID)
    SetValue(name, (long double)v, i, j);
  else if (fNameTypeMap[name] == kIIC && v >= 0)
    SetValue(name, (unsigned long long)v, i, j);
  else if (fNameTypeMap[name] != kIII)
    throw HALException(name.insert(0, "Cannot reassign 2D integer storage container for "));

  fIntegerIntIntMap[name][i][j] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const unsigned long long &v, 
                             const long long &i, const long long &j) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIIC;
  else if (fNameTypeMap[name] == kIID)
    SetValue(name, (long double)v, i, j);
  else if (fNameTypeMap[name] == kIII)
    SetValue(name, (long long)v, i, j);
  else if (fNameTypeMap[name] != kIIC)
    throw HALException(name.insert(0, "Cannot reassign 2D counting storage container for "));

  fCountingIntIntMap[name][i][j] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, const std::string &v, 
                             const long long &i, const long long &j) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIIS;
  else if (fNameTypeMap[name] != kIIS)
    throw HALException(name.insert(0, "Cannot reassign 2D string storage container for "));

  fStringIntIntMap[name][i][j] = v;
}

//______________________________________________________________________________
void AnalysisData::SetValue (const TString &n, TObject *v, 
                             const long long &i, const long long &j) 
{
  std::string name(n.Data());

  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIIO;
  else if (fNameTypeMap[name] != kIIO)
    throw HALException(name.insert(0, "Cannot reassign 2D TObject storage container for "));

  fTObjectIntIntMap[name][i][j] = v;
}

//______________________________________________________________________________
bool AnalysisData::GetBool (const TString &n, const long long &i, const long long &j) 
{
  std::string name(n.Data());

  if (fBoolMap.count(name) != 0)
    return fBoolMap[name];
  if (fBoolIntMap.count(name) != 0)
    return fBoolIntMap[name][i];
  if (fBoolIntIntMap.count(name) != 0)
    return fBoolIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

//______________________________________________________________________________
long double AnalysisData::GetDecimal (const TString &n, const long long &i, 
                                      const long long &j) 
{
  std::string name(n.Data());

  if (fDecimalMap.count(name) != 0)
    return fDecimalMap[name];
  if (fDecimalIntMap.count(name) != 0)
    return fDecimalIntMap[name][i];
  if (fDecimalIntIntMap.count(name) != 0)
    return fDecimalIntIntMap[name][i][j];
  if (fIntegerMap.count(name) != 0)
    return fIntegerMap[name];
  if (fIntegerIntMap.count(name) != 0)
    return fIntegerIntMap[name][i];
  if (fIntegerIntIntMap.count(name) != 0)
    return fIntegerIntIntMap[name][i][j];
  if (fCountingMap.count(name) != 0)
    return fCountingMap[name];
  if (fCountingIntMap.count(name) != 0)
    return fCountingIntMap[name][i];
  if (fCountingIntIntMap.count(name) != 0)
    return fCountingIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

//______________________________________________________________________________
long long AnalysisData::GetInteger (const TString &n, const long long &i, 
                                    const long long &j) 
{
  std::string name(n.Data());

  if (fDecimalMap.count(name) != 0)
    return fDecimalMap[name];
  if (fDecimalIntMap.count(name) != 0)
    return fDecimalIntMap[name][i];
  if (fDecimalIntIntMap.count(name) != 0)
    return fDecimalIntIntMap[name][i][j];
  if (fIntegerMap.count(name) != 0)
    return fIntegerMap[name];
  if (fIntegerIntMap.count(name) != 0)
    return fIntegerIntMap[name][i];
  if (fIntegerIntIntMap.count(name) != 0)
    return fIntegerIntIntMap[name][i][j];
  if (fCountingMap.count(name) != 0)
    return fCountingMap[name];
  if (fCountingIntMap.count(name) != 0)
    return fCountingIntMap[name][i];
  if (fCountingIntIntMap.count(name) != 0)
    return fCountingIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

//______________________________________________________________________________
unsigned long long AnalysisData::GetCounting (const TString &n, 
                                              const long long &i, const long long &j) 
{
  std::string name(n.Data());

  if (fDecimalMap.count(name) != 0)
    return fDecimalMap[name];
  if (fDecimalIntMap.count(name) != 0)
    return fDecimalIntMap[name][i];
  if (fDecimalIntIntMap.count(name) != 0)
    return fDecimalIntIntMap[name][i][j];
  if (fIntegerMap.count(name) != 0)
    return fIntegerMap[name];
  if (fIntegerIntMap.count(name) != 0)
    return fIntegerIntMap[name][i];
  if (fIntegerIntIntMap.count(name) != 0)
    return fIntegerIntIntMap[name][i][j];
  if (fCountingMap.count(name) != 0)
    return fCountingMap[name];
  if (fCountingIntMap.count(name) != 0)
    return fCountingIntMap[name][i];
  if (fCountingIntIntMap.count(name) != 0)
    return fCountingIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

//______________________________________________________________________________
TString AnalysisData::GetString (const TString &n, const long long &i, 
                                 const long long &j) 
{
  std::string name(n.Data());

  if (fStringMap.count(name) != 0)
    return fStringMap[name];
  if (fStringIntMap.count(name) != 0)
    return fStringIntMap[name][i];
  if (fStringIntIntMap.count(name) != 0)
    return fStringIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

//______________________________________________________________________________
TObject* AnalysisData::GetTObject (const TString &n, const long long &i, 
                                   const long long &j) 
{
  std::string name(n.Data());

  if (fTObjectMap.count(name) != 0)
    return fTObjectMap[name];
  if (fTObjectIntMap.count(name) != 0)
    return fTObjectIntMap[name][i];
  if (fTObjectIntIntMap.count(name) != 0)
    return fTObjectIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

//______________________________________________________________________________
bool AnalysisData::Exists (const TString &name, const long long &i, 
                           const long long &j) 
{
  std::string n(name.Data());

  if (NameAlreadyStored(n) && i == -1 && j == -1)
    return true;
  if (NameAlreadyStored(n) && j == -1) {
    if (fNameTypeMap[n] == kIB)
      return (fBoolIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kID)
      return (fDecimalIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kII)
      return (fIntegerIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kIC)
      return (fCountingIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kIS)
      return (fStringIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kIO)
      return (fTObjectIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kIIB)
      return (fBoolIntIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kIID)
      return (fDecimalIntIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kIII)
      return (fIntegerIntIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kIIC)
      return (fCountingIntIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kIIS)
      return (fStringIntIntMap[n].count(i) == 1) ? true : false;
    if (fNameTypeMap[n] == kIIO)
      return (fTObjectIntIntMap[n].count(i) == 1) ? true : false;
  }
  if (NameAlreadyStored(n)) {
    if (fNameTypeMap[n] == kIIB)
      return (fBoolIntIntMap[n][i].count(j) == 1) ? true : false;
    if (fNameTypeMap[n] == kIID)
      return (fDecimalIntIntMap[n][i].count(j) == 1) ? true : false;
    if (fNameTypeMap[n] == kIII)
      return (fIntegerIntIntMap[n][i].count(j) == 1) ? true : false;
    if (fNameTypeMap[n] == kIIC)
      return (fCountingIntIntMap[n][i].count(j) == 1) ? true : false;
    if (fNameTypeMap[n] == kIIS)
      return (fStringIntIntMap[n][i].count(j) == 1) ? true : false;
    if (fNameTypeMap[n] == kIIO)
      return (fTObjectIntIntMap[n][i].count(j) == 1) ? true : false;
  }
  return false;
}

//______________________________________________________________________________
unsigned AnalysisData::TypeDim (std::string n) 
{
  if (fNameTypeMap[n] == kB || fNameTypeMap[n] == kD ||
      fNameTypeMap[n] == kI || fNameTypeMap[n] == kC ||
      fNameTypeMap[n] == kS || fNameTypeMap[n] == kO)
    return 0;
  if (fNameTypeMap[n] == kIB || fNameTypeMap[n] == kID ||
      fNameTypeMap[n] == kII || fNameTypeMap[n] == kIC ||
      fNameTypeMap[n] == kIS || fNameTypeMap[n] == kIO)
    return 1;
  if (fNameTypeMap[n] == kIIB || fNameTypeMap[n] == kIID ||
      fNameTypeMap[n] == kIII || fNameTypeMap[n] == kIIC ||
      fNameTypeMap[n] == kIIS || fNameTypeMap[n] == kIIO)
    return 2;
  throw HALException("Type dimension couldn't be determined.");
}

//______________________________________________________________________________
std::vector<TString> AnalysisData::GetSimilarNames (const TString &nn, 
                                                    unsigned min_dim) 
{
  std::string n(nn.Data());

  std::vector<TString> names;
  TRegexp pat(n.substr(0, n.find_first_of(':')).insert(0, "^").c_str()); // pick out ^<name>:

#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, StorageType> it;
  BOOST_FOREACH( it, fNameTypeMap )
#else
  for (auto it: fNameTypeMap)
#endif
  {
    TString name(it.first.c_str());
    if (name.Contains(pat) && TypeDim(it.first) >= min_dim && it.first != n)
      names.push_back(it.first);
  }
  
  return names;
}

//______________________________________________________________________________
void AnalysisData::CopyValues (const TString &f, const TString &t) 
{
  std::string from(f.Data());
  std::string to(t.Data());

  if (TypeDim(from) == 0) {
    if (fNameTypeMap[from] == kB)
      fBoolMap[to] = fBoolMap[from];
    else if (fNameTypeMap[from] == kD)
      fDecimalMap[to] = fDecimalMap[from];
    else if (fNameTypeMap[from] == kI)
      fIntegerMap[to] = fIntegerMap[from];
    else if (fNameTypeMap[from] == kC)
      fCountingMap[to] = fCountingMap[from];
    else if (fNameTypeMap[from] == kS)
      fStringMap[to] = fStringMap[from];
    else if (fNameTypeMap[from] == kO)
      fTObjectMap[to] = fTObjectMap[from];
  }
  else if (TypeDim(from) == 1) {
    if (fNameTypeMap[from] == kIB)
      fBoolIntMap[to] = fBoolIntMap[from];
    else if (fNameTypeMap[from] == kID)
      fDecimalIntMap[to] = fDecimalIntMap[from];
    else if (fNameTypeMap[from] == kII)
      fIntegerIntMap[to] = fIntegerIntMap[from];
    else if (fNameTypeMap[from] == kIC)
      fCountingIntMap[to] = fCountingIntMap[from];
    else if (fNameTypeMap[from] == kIS)
      fStringIntMap[to] = fStringIntMap[from];
    else if (fNameTypeMap[from] == kIO)
      fTObjectIntMap[to] = fTObjectIntMap[from];
  }
  else if (TypeDim(from) == 2) {
    if (fNameTypeMap[from] == kIIB)
      fBoolIntIntMap[to] = fBoolIntIntMap[from];
    else if (fNameTypeMap[from] == kIID)
      fDecimalIntIntMap[to] = fDecimalIntIntMap[from];
    else if (fNameTypeMap[from] == kIII)
      fIntegerIntIntMap[to] = fIntegerIntIntMap[from];
    else if (fNameTypeMap[from] == kIIC)
      fCountingIntIntMap[to] = fCountingIntIntMap[from];
    else if (fNameTypeMap[from] == kIIS)
      fStringIntIntMap[to] = fStringIntIntMap[from];
    else if (fNameTypeMap[from] == kIIO)
      fTObjectIntIntMap[to] = fTObjectIntIntMap[from];
  }
}

//______________________________________________________________________________
void AnalysisData::SwapValues (const TString &name, long long i, long long j) 
{
  std::string n(name.Data());

  if (TypeDim(n) == 1) {
    if (fNameTypeMap[n] == kIB) {
      bool temp = fBoolIntMap[n][i];
      fBoolIntMap[n][i] = fBoolIntMap[n][j];
      fBoolIntMap[n][j] = temp;
    }
    else if (fNameTypeMap[n] == kID) {
      long double temp = fDecimalIntMap[n][i];
      fDecimalIntMap[n][i] = fDecimalIntMap[n][j];
      fDecimalIntMap[n][j] = temp;
    }
    else if (fNameTypeMap[n] == kII) {
      long long temp = fIntegerIntMap[n][i];
      fIntegerIntMap[n][i] = fIntegerIntMap[n][j];
      fIntegerIntMap[n][j] = temp;
    }
    else if (fNameTypeMap[n] == kIC) {
      unsigned long long temp = fCountingIntMap[n][i];
      fCountingIntMap[n][i] = fCountingIntMap[n][j];
      fCountingIntMap[n][j] = temp;
    }
    else if (fNameTypeMap[n] == kIS) {
      std::string temp = fStringIntMap[n][i];
      fStringIntMap[n][i] = fStringIntMap[n][j];
      fStringIntMap[n][j] = temp;
    }
    else if (fNameTypeMap[n] == kIO) {
      TObject *temp = fTObjectIntMap[n][i];
      fTObjectIntMap[n][i] = fTObjectIntMap[n][j];
      fTObjectIntMap[n][j] = temp;
    }
  }
  else if (TypeDim(n) == 2) {
    if (fNameTypeMap[n] == kIIB)
      fBoolIntIntMap[n][i].swap(fBoolIntIntMap[n][j]);
    else if (fNameTypeMap[n] == kIID)
      fDecimalIntIntMap[n][i].swap(fDecimalIntIntMap[n][j]);
    else if (fNameTypeMap[n] == kIII)
      fIntegerIntIntMap[n][i].swap(fIntegerIntIntMap[n][j]);
    else if (fNameTypeMap[n] == kIIC)
      fCountingIntIntMap[n][i].swap(fCountingIntIntMap[n][j]);
    else if (fNameTypeMap[n] == kIIS)
      fStringIntIntMap[n][i].swap(fStringIntIntMap[n][j]);
    else if (fNameTypeMap[n] == kIIO)
      fTObjectIntIntMap[n][i].swap(fTObjectIntIntMap[n][j]);
  }
}

//______________________________________________________________________________
void AnalysisData::Reset () 
{
  fBoolMap.clear();
  fDecimalMap.clear();
  fIntegerMap.clear();
  fCountingMap.clear();
  fStringMap.clear();
  fTObjectMap.clear();
  fBoolIntMap.clear();
  fDecimalIntMap.clear();
  fIntegerIntMap.clear();
  fCountingIntMap.clear();
  fStringIntMap.clear();
  fTObjectIntMap.clear();
  fBoolIntIntMap.clear();
  fDecimalIntIntMap.clear();
  fIntegerIntIntMap.clear();
  fCountingIntIntMap.clear();
  fStringIntIntMap.clear();
  fTObjectIntIntMap.clear();
  fNameTypeMap.clear();
}

//______________________________________________________________________________
void AnalysisData::RemoveNameAndData (const TString &name) 
{
  std::string n(name.Data());

  RemoveData(name);
  fNameTypeMap.erase(n);
}

//______________________________________________________________________________
void AnalysisData::RemoveData (const TString &name) 
{
  std::string n(name.Data());

  if (fNameTypeMap[n] == kB)
    fBoolMap.erase(n);
  else if (fNameTypeMap[n] == kD)
    fDecimalMap.erase(n);
  else if (fNameTypeMap[n] == kI)
    fIntegerMap.erase(n);
  else if (fNameTypeMap[n] == kC)
    fCountingMap.erase(n);
  else if (fNameTypeMap[n] == kS)
    fStringMap.erase(n);
  else if (fNameTypeMap[n] == kO)
    fTObjectMap.erase(n);
  else if (fNameTypeMap[n] == kIB)
    fBoolIntMap.erase(n);
  else if (fNameTypeMap[n] == kID)
    fDecimalIntMap.erase(n);
  else if (fNameTypeMap[n] == kII)
    fIntegerIntMap.erase(n);
  else if (fNameTypeMap[n] == kIC)
    fCountingIntMap.erase(n);
  else if (fNameTypeMap[n] == kIS)
    fStringIntMap.erase(n);
  else if (fNameTypeMap[n] == kIO)
    fTObjectIntMap.erase(n);
  else if (fNameTypeMap[n] == kIIB)
    fBoolIntIntMap.erase(n);
  else if (fNameTypeMap[n] == kIID)
    fDecimalIntIntMap.erase(n);
  else if (fNameTypeMap[n] == kIII)
    fIntegerIntIntMap.erase(n);
  else if (fNameTypeMap[n] == kIIC)
    fCountingIntIntMap.erase(n);
  else if (fNameTypeMap[n] == kIIS)
    fStringIntIntMap.erase(n);
  else if (fNameTypeMap[n] == kIIO)
    fTObjectIntIntMap.erase(n);
}

//______________________________________________________________________________
void AnalysisData::RemoveAllAssociatedData (const TString &nn) 
{
  std::string n(nn.Data());

  TRegexp prefix(TString::Format("^%s:*.*", n.c_str()));
  std::vector<std::string> names;

#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  std::pair<std::string, StorageType> name;
  BOOST_FOREACH( name, fNameTypeMap )
#else
  for (auto name: fNameTypeMap)
#endif
  {
    if (TString(name.first.c_str()).Contains(prefix)) {
      RemoveData(name.first);
      names.push_back(name.first);
    }
  }
#ifdef BOOST_NO_CXX11_RANGE_BASED_FOR
  BOOST_FOREACH( std::string name, names )
#else
  for (auto name: names)
#endif
    fNameTypeMap.erase(name);
}

} /*  HAL */ 

