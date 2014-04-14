#ifndef HAL_ANALYSISDATA
#define HAL_ANALYSISDATA

#include <TNamed.h>
#include <TString.h>
#include <TRegexp.h>
#include <string>
#include <map>
#include <vector>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>

namespace HAL
{

class AnalysisData : public TNamed {
public:
  // Bool values
  virtual void SetValue (const TString&, const bool&);
  // Decimal values
  virtual void SetValue (const TString&, const long double&);
  virtual void SetValue (const TString n, const double &v) {SetValue(n, (long double)v);}
  virtual void SetValue (const TString &n, const float &v) {SetValue(n, (long double)v);}
  // Integer values
  virtual void SetValue (const TString&, const long long&);
  virtual void SetValue (const TString &n, const short &v) {SetValue(n, (long long)v);}
  virtual void SetValue (const TString &n, const long &v) {SetValue(n, (long long)v);}
  virtual void SetValue (const TString &n, const int &v) {SetValue(n, (long long)v);}
  virtual void SetValue (const TString &n, const signed char &v) {SetValue(n, (long long)v);}
  // Counting values
  virtual void SetValue (const TString&, const unsigned long long&);
  virtual void SetValue (const TString &n, const unsigned short &v) {SetValue(n, (unsigned long long)v);}
  virtual void SetValue (const TString &n, const unsigned long &v) {SetValue(n, (unsigned long long)v);}
  virtual void SetValue (const TString &n, const unsigned int &v) {SetValue(n, (unsigned long long)v);}
  virtual void SetValue (const TString &n, const unsigned char &v) {SetValue(n, (unsigned long long)v);}
  // String values
  virtual void SetValue (const TString &n, const std::string &v);
  virtual void SetValue (const TString &n, const TString &v) {SetValue(n, std::string(v.Data()));}
  virtual void SetValue (const TString &n, const char &v) {SetValue(n, std::string(1, v));}
  // TObjects
  virtual void SetValue (const TString&, TObject*);
  // 2D maps
  // Bool values
  virtual void SetValue (const TString&, const bool&, const long long&);
  // Decimal values
  virtual void SetValue (const TString&, const long double&, const long long&);
  virtual void SetValue (const TString &n, const double &v, const long long &i) {SetValue(n, (long double)v, i);}
  virtual void SetValue (const TString &n, const float &v, const long long &i) {SetValue(n, (long double)v, i);}
  // Integer values
  virtual void SetValue (const TString&, const long long&, const long long&);
  virtual void SetValue (const TString &n, const short &v, const long long &i) {SetValue(n, (long long)v, i);}
  virtual void SetValue (const TString &n, const long &v, const long long &i) {SetValue(n, (long long)v, i);}
  virtual void SetValue (const TString &n, const int &v, const long long &i) {SetValue(n, (long long)v, i);}
  virtual void SetValue (const TString &n, const signed char &v, const long long &i) {SetValue(n, (long long)v, i);}
  // Counting values
  virtual void SetValue (const TString&, const unsigned long long&, const long long&);
  virtual void SetValue (const TString &n, const unsigned short &v, const long long &i) {SetValue(n, (unsigned long long)v, i);}
  virtual void SetValue (const TString &n, const unsigned long &v, const long long &i) {SetValue(n, (unsigned long long)v, i);}
  virtual void SetValue (const TString &n, const unsigned int &v, const long long &i) {SetValue(n, (unsigned long long)v, i);}
  virtual void SetValue (const TString &n, const unsigned char &v, const long long &i) {SetValue(n, (unsigned long long)v, i);}
  // String values
  virtual void SetValue (const TString&, const std::string&, const long long&);
  virtual void SetValue (const TString &n, const TString &v, const long long &i) {SetValue(n, std::string(v.Data()), i);}
  virtual void SetValue (const TString &n, const char &v, const long long &i) {SetValue(n, std::string(1, v), i);}
  // TObjects
  virtual void SetValue (const TString&, TObject*, const long long&);
  // 2D integer maps
  // Bool values
  virtual void SetValue (const TString&, const bool&, const long long&, const long long&);
  // Decimal values
  virtual void SetValue (const TString&, const long double&, const long long&, const long long&);
  virtual void SetValue (const TString &n, const double &v, const long long &i, const long long &j) {SetValue(n, (long double)v, i, j);}
  virtual void SetValue (const TString &n, const float &v, const long long &i, const long long &j) {SetValue(n, (long double)v, i, j);}
  // Integer values
  virtual void SetValue (const TString&, const long long&, const long long&, const long long&);
  virtual void SetValue (const TString &n, const short &v, const long long &i, const long long &j) {SetValue(n, (long long)v, i, j);}
  virtual void SetValue (const TString &n, const long &v, const long long &i, const long long &j) {SetValue(n, (long long)v, i, j);}
  virtual void SetValue (const TString &n, const int &v, const long long &i, const long long &j) {SetValue(n, (long long)v, i, j);}
  virtual void SetValue (const TString &n, const signed char &v, const long long &i, const long long &j) {SetValue(n, (long long)v, i, j);}
  // Counting values
  virtual void SetValue (const TString&, const unsigned long long&, const long long&, const long long&);
  virtual void SetValue (const TString &n, const unsigned short &v, const long long &i, const long long &j) {SetValue(n, (unsigned long long)v, i, j);}
  virtual void SetValue (const TString &n, const unsigned long &v, const long long &i, const long long &j) {SetValue(n, (unsigned long long)v, i , j);}
  virtual void SetValue (const TString &n, const unsigned int &v, const long long &i, const long long &j) {SetValue(n, (unsigned long long)v, i, j);}
  virtual void SetValue (const TString &n, const unsigned char &v, const long long &i, const long long &j) {SetValue(n, (unsigned long long)v, i, j);}
  // String values
  virtual void SetValue (const TString&, const std::string&, const long long&, const long long&);
  virtual void SetValue (const TString &n, const TString &v, const long long &i, const long long &j) {SetValue(n, std::string(v.Data()), i, j);}
  virtual void SetValue (const TString &n, const char &v, const long long &i, const long long &j) {SetValue(n, std::string(1, v), i, j);}
  // TObjects
  virtual void SetValue (const TString&, TObject*, const long long&, const long long&);

  bool                GetBool (const TString&, const long long &i = -1, const long long &j = -1);
  long double         GetDecimal (const TString&, const long long &i = -1, const long long &j = -1);
  long long           GetInteger (const TString&, const long long &i = -1, const long long &j = -1);
  unsigned long long  GetCounting (const TString&, const long long &i = -1, const long long &j = -1);
  TString             GetString (const TString&, const long long &i = -1, const long long &j = -1);
  TObject*            GetTObject (const TString&, const long long &i = -1, const long long &j = -1);

  bool                      Exists (const TString &n, const long long &i = -1, const long long &j = -1);
  unsigned                  TypeDim (std::string n);
  unsigned                  TypeDim (const TString &n) {return TypeDim(std::string(n.Data()));}
  std::vector<TString>      GetSimilarNames (const TString &n, unsigned min_dim);
  // Copy value(s) stored in <map>[from] to <map>[to]
  void                      CopyValues (const TString &from, const TString &to);
  // Swap elements in a 1D or 2D IntMap or IntIntMap
  void                      SwapValues (const TString &n, long long i, long long j);
  void                      Reset ();
  void                      RemoveNameAndData (const TString&);
  void                      RemoveData (const TString&);
  void                      RemoveAllAssociatedData (const TString&);

public:
  std::map<std::string, bool>                                 fBoolMap;
  std::map<std::string, long double>                          fDecimalMap;
  std::map<std::string, long long>                            fIntegerMap;
  std::map<std::string, unsigned long long>                   fCountingMap;
  std::map<std::string, std::string>                          fStringMap;
  std::map<std::string, TObject*>                             fTObjectMap;
  // the integer maps are the "index" of the element
  // this allows for disjoint indices without wasted space
  std::map<std::string, std::map<long long, bool> >               fBoolIntMap;
  std::map<std::string, std::map<long long, long double> >        fDecimalIntMap;
  std::map<std::string, std::map<long long, long long> >          fIntegerIntMap;
  std::map<std::string, std::map<long long, unsigned long long> > fCountingIntMap;
  std::map<std::string, std::map<long long, std::string> >        fStringIntMap;
  std::map<std::string, std::map<long long, TObject*> >           fTObjectIntMap;
  // for 2D "indexing"
  std::map<std::string, std::map<long long, std::map<long long, bool> > >               fBoolIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, long double> > >        fDecimalIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, long long> > >          fIntegerIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, unsigned long long> > > fCountingIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, std::string> > >        fStringIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, TObject*> > >           fTObjectIntIntMap;

  ClassDefNV(AnalysisData, 0);

private:
  enum StorageType {kB, kD, kI, kC, kS, kO,
                    kIB, kID, kII, kIC, kIS, kIO,
                    kIIB, kIID, kIII, kIIC, kIIS, kIIO};

  std::map<std::string, StorageType>  fNameTypeMap;

  inline bool NameAlreadyStored (const std::string &n) {return fNameTypeMap.count(n) != 0 ? true : false;}
};

} /*  HAL */ 

#endif
