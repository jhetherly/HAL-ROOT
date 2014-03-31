#ifndef HAL_ANALYSISDATA
#define HAL_ANALYSISDATA

#include <TNamed.h>
#include <TString.h>
#include <string>
#include <map>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>

namespace HAL
{

class AnalysisData : public TNamed {
public:
  // Bool values
  virtual void SetValue (std::string, bool);
  // Decimal values
  virtual void SetValue (std::string, long double);
  virtual void SetValue (std::string n, double v) {SetValue(n, (long double)v);}
  virtual void SetValue (std::string n, float v) {SetValue(n, (long double)v);}
  // Integer values
  virtual void SetValue (std::string, long long);
  virtual void SetValue (std::string n, short v) {SetValue(n, (long long)v);}
  virtual void SetValue (std::string n, long v) {SetValue(n, (long long)v);}
  virtual void SetValue (std::string n, int v) {SetValue(n, (long long)v);}
  virtual void SetValue (std::string n, signed char v) {SetValue(n, (long long)v);}
  // Counting values
  virtual void SetValue (std::string, unsigned long long);
  virtual void SetValue (std::string n, unsigned short v) {SetValue(n, (unsigned long long)v);}
  virtual void SetValue (std::string n, unsigned long v) {SetValue(n, (unsigned long long)v);}
  virtual void SetValue (std::string n, unsigned int v) {SetValue(n, (unsigned long long)v);}
  virtual void SetValue (std::string n, unsigned char v) {SetValue(n, (unsigned long long)v);}
  // String values
  virtual void SetValue (std::string, std::string);
  virtual void SetValue (std::string n, TString v) {SetValue(n, (std::string)v.Data());}
  virtual void SetValue (std::string n, char v) {SetValue(n, std::string(1, v));}
  // TObjects
  virtual void SetValue (std::string, TObject*);
  // 2D maps
  // Bool values
  virtual void SetValue (std::string, bool, long long);
  // Decimal values
  virtual void SetValue (std::string, long double, long long);
  virtual void SetValue (std::string n, double v, long long i) {SetValue(n, (long double)v, i);}
  virtual void SetValue (std::string n, float v, long long i) {SetValue(n, (long double)v, i);}
  // Integer values
  virtual void SetValue (std::string, long long, long long);
  virtual void SetValue (std::string n, short v, long long i) {SetValue(n, (long long)v, i);}
  virtual void SetValue (std::string n, long v, long long i) {SetValue(n, (long long)v, i);}
  virtual void SetValue (std::string n, int v, long long i) {SetValue(n, (long long)v, i);}
  virtual void SetValue (std::string n, signed char v, long long i) {SetValue(n, (long long)v, i);}
  // Counting values
  virtual void SetValue (std::string, unsigned long long, long long);
  virtual void SetValue (std::string n, unsigned short v, long long i) {SetValue(n, (unsigned long long)v, i);}
  virtual void SetValue (std::string n, unsigned long v, long long i) {SetValue(n, (unsigned long long)v, i);}
  virtual void SetValue (std::string n, unsigned int v, long long i) {SetValue(n, (unsigned long long)v, i);}
  virtual void SetValue (std::string n, unsigned char v, long long i) {SetValue(n, (unsigned long long)v, i);}
  // String values
  virtual void SetValue (std::string, std::string, long long);
  virtual void SetValue (std::string n, TString v, long long i) {SetValue(n, (std::string)v.Data(), i);}
  virtual void SetValue (std::string n, char v, long long i) {SetValue(n, std::string(1, v), i);}
  // TObjects
  virtual void SetValue (std::string, TObject*, long long);
  // 2D integer maps
  // Bool values
  virtual void SetValue (std::string, bool, long long, long long);
  // Decimal values
  virtual void SetValue (std::string, long double, long long, long long);
  virtual void SetValue (std::string n, double v, long long i, long long j) {SetValue(n, (long double)v, i, j);}
  virtual void SetValue (std::string n, float v, long long i, long long j) {SetValue(n, (long double)v, i, j);}
  // Integer values
  virtual void SetValue (std::string, long long, long long, long long);
  virtual void SetValue (std::string n, short v, long long i, long long j) {SetValue(n, (long long)v, i, j);}
  virtual void SetValue (std::string n, long v, long long i, long long j) {SetValue(n, (long long)v, i, j);}
  virtual void SetValue (std::string n, int v, long long i, long long j) {SetValue(n, (long long)v, i, j);}
  virtual void SetValue (std::string n, signed char v, long long i, long long j) {SetValue(n, (long long)v, i, j);}
  // Counting values
  virtual void SetValue (std::string, unsigned long long, long long, long long);
  virtual void SetValue (std::string n, unsigned short v, long long i, long long j) {SetValue(n, (unsigned long long)v, i, j);}
  virtual void SetValue (std::string n, unsigned long v, long long i, long long j) {SetValue(n, (unsigned long long)v, i , j);}
  virtual void SetValue (std::string n, unsigned int v, long long i, long long j) {SetValue(n, (unsigned long long)v, i, j);}
  virtual void SetValue (std::string n, unsigned char v, long long i, long long j) {SetValue(n, (unsigned long long)v, i, j);}
  // String values
  virtual void SetValue (std::string, std::string, long long, long long);
  virtual void SetValue (std::string n, TString v, long long i, long long j) {SetValue(n, (std::string)v.Data(), i, j);}
  virtual void SetValue (std::string n, char v, long long i, long long j) {SetValue(n, std::string(1, v), i, j);}
  // TObjects
  virtual void SetValue (std::string, TObject*, long long, long long);

  bool                GetBool (std::string, long long i = -1, long long j = -1);
  long double         GetDecimal (std::string, long long i = -1, long long j = -1);
  long long           GetInteger (std::string, long long i = -1, long long j = -1);
  unsigned long long  GetCounting (std::string, long long i = -1, long long j = -1);
  std::string         GetString (std::string, long long i = -1, long long j = -1);
  TObject*            GetTObject (std::string, long long i = -1, long long j = -1);

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

  bool NameAlreadyStored (std::string n) {return fNameTypeMap.count(n) != 0 ? true : false;}
};

} /*  HAL */ 

#endif
