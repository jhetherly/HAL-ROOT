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
  void SetValue (std::string, LongDouble_t);
#ifdef LONGDOUBLE
  void SetValue (std::string n, double v) {SetValue(n, (LongDouble_t)v);}
#endif
  void SetValue (std::string n, float v) {SetValue(n, (LongDouble_t)v);}
  // Integer values
  void SetValue (std::string, long long);
  void SetValue (std::string n, long v) {SetValue(n, (long long)v);}
  void SetValue (std::string n, int v) {SetValue(n, (long long)v);}
  void SetValue (std::string n, signed char v) {SetValue(n, (long long)v);}
  // Counting values
  void SetValue (std::string, unsigned long long);
  void SetValue (std::string n, unsigned long v) {SetValue(n, (unsigned long long)v);}
  void SetValue (std::string n, unsigned int v) {SetValue(n, (unsigned long long)v);}
  void SetValue (std::string n, unsigned char v) {SetValue(n, (unsigned long long)v);}
  // String values
  void SetValue (std::string, std::string);
  void SetValue (std::string n, TString v) {SetValue(n, (std::string)v.Data());}
  void SetValue (std::string n, char v) {SetValue(n, std::string(1, v));}
  void SetValue (std::string, TObject*);
  // 2D maps
  void SetValue (std::string, LongDouble_t, Int_t);
#ifdef LONGDOUBLE
  //void SetValue (std::string, double, Int_t);
#endif
  void SetValue (std::string, long long, Int_t);
  void SetValue (std::string, unsigned long long, Int_t);
  void SetValue (std::string, std::string, Int_t);
  void SetValue (std::string, TObject*, Int_t);

  LongDouble_t        GetDecimal (std::string, Int_t i = -1);
  long long           GetInteger (std::string, Int_t i = -1);
  unsigned long long  GetCounting (std::string, Int_t i = -1);
  std::string         GetString (std::string, Int_t i = -1);
  TObject*            GetTObject (std::string, Int_t i = -1);

public:
  std::map<std::string, LongDouble_t>                         fDecimalMap;
  std::map<std::string, long long>                            fIntegerMap;
  std::map<std::string, unsigned long long>                   fCountingMap;
  std::map<std::string, std::string>                          fStringMap;
  std::map<std::string, TObject*>                             fTObjectMap;
  // the integer maps are the "index" of the element
  // this allows for disjoint indices without wasted space
  std::map<std::string, std::map<Int_t, LongDouble_t> >       fDecimalIntMap;
  std::map<std::string, std::map<Int_t, long long> >          fIntegerIntMap;
  std::map<std::string, std::map<Int_t, unsigned long long> > fCountingIntMap;
  std::map<std::string, std::map<Int_t, std::string> >        fStringIntMap;
  std::map<std::string, std::map<Int_t, TObject*> >           fTObjectIntMap;

  ClassDefNV(AnalysisData, 0);
};

} /*  HAL */ 

#endif
