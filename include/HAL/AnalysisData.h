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
  // Decimal values
  virtual void SetValue (std::string, long double);
  virtual void SetValue (std::string n, double v) {SetValue(n, (long double)v);}
  virtual void SetValue (std::string n, float v) {SetValue(n, (long double)v);}
  // Integer values
  virtual void SetValue (std::string, long long);
  virtual void SetValue (std::string n, long v) {SetValue(n, (long long)v);}
  virtual void SetValue (std::string n, int v) {SetValue(n, (long long)v);}
  virtual void SetValue (std::string n, signed char v) {SetValue(n, (long long)v);}
  // Counting values
  virtual void SetValue (std::string, unsigned long long);
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
  // Decimal values
  virtual void SetValue (std::string, long double, Int_t);
  virtual void SetValue (std::string n, double v, Int_t i) {SetValue(n, (long double)v, i);}
  virtual void SetValue (std::string n, float v, Int_t i) {SetValue(n, (long double)v, i);}
  // Integer values
  virtual void SetValue (std::string, long long, Int_t);
  virtual void SetValue (std::string n, long v, Int_t i) {SetValue(n, (long long)v, i);}
  virtual void SetValue (std::string n, int v, Int_t i) {SetValue(n, (long long)v, i);}
  virtual void SetValue (std::string n, signed char v, Int_t i) {SetValue(n, (long long)v, i);}
  // Counting values
  virtual void SetValue (std::string, unsigned long long, Int_t);
  virtual void SetValue (std::string n, unsigned long v, Int_t i) {SetValue(n, (unsigned long long)v, i);}
  virtual void SetValue (std::string n, unsigned int v, Int_t i) {SetValue(n, (unsigned long long)v, i);}
  virtual void SetValue (std::string n, unsigned char v, Int_t i) {SetValue(n, (unsigned long long)v, i);}
  // String values
  virtual void SetValue (std::string, std::string, Int_t);
  virtual void SetValue (std::string n, TString v, Int_t i) {SetValue(n, (std::string)v.Data(), i);}
  virtual void SetValue (std::string n, char v, Int_t i) {SetValue(n, std::string(1, v), i);}
  // TObjects
  virtual void SetValue (std::string, TObject*, Int_t);

  long double         GetDecimal (std::string, Int_t i = -1);
  long long           GetInteger (std::string, Int_t i = -1);
  unsigned long long  GetCounting (std::string, Int_t i = -1);
  std::string         GetString (std::string, Int_t i = -1);
  TObject*            GetTObject (std::string, Int_t i = -1);

public:
  std::map<std::string, long double>                          fDecimalMap;
  std::map<std::string, long long>                            fIntegerMap;
  std::map<std::string, unsigned long long>                   fCountingMap;
  std::map<std::string, std::string>                          fStringMap;
  std::map<std::string, TObject*>                             fTObjectMap;
  // the integer maps are the "index" of the element
  // this allows for disjoint indices without wasted space
  std::map<std::string, std::map<Int_t, long double> >        fDecimalIntMap;
  std::map<std::string, std::map<Int_t, long long> >          fIntegerIntMap;
  std::map<std::string, std::map<Int_t, unsigned long long> > fCountingIntMap;
  std::map<std::string, std::map<Int_t, std::string> >        fStringIntMap;
  std::map<std::string, std::map<Int_t, TObject*> >           fTObjectIntMap;

  ClassDefNV(AnalysisData, 0);
};

} /*  HAL */ 

#endif
