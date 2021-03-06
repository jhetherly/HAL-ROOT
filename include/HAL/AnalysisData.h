/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 */

#ifndef HAL_AnalysisData
#define HAL_AnalysisData

#include <type_traits>
#include <string>
#include <vector>
#include <map>
#include <TNamed.h>
#include <TString.h>
#include <HAL/Common.h>
#include <HAL/Exceptions.h>

namespace HAL
{

//! Class for storage and sharing of data referenced by strings
/*!
 * This class aids in passing data between algorithms.
 * The common data store is a TList. This causes problems
 * when algorithms must pass plain old data (POD) between
 * each other as a TList may only hold object deriving from
 * TObject. This class provides a way for POD to be stored
 * and retrieved by strings.
 */
class AnalysisData : public TNamed {

private:
  enum StorageType {kB, kD, kI, kC, kS, kO,
                    kIB, kID, kII, kIC, kIS, kIO,
                    kIIB, kIID, kIII, kIIC, kIIS, kIIO};

  std::map<std::string, StorageType, internal::string_cmp>  fNameTypeMap;

  inline bool NameAlreadyStored (const std::string &n) {return fNameTypeMap.count(n) != 0 ? true : false;}

protected:
  std::map<std::string, bool, internal::string_cmp>                                 fBoolMap;
  std::map<std::string, long double, internal::string_cmp>                          fDecimalMap;
  std::map<std::string, long long, internal::string_cmp>                            fIntegerMap;
  std::map<std::string, unsigned long long, internal::string_cmp>                   fCountingMap;
  std::map<std::string, std::string, internal::string_cmp>                          fStringMap;
  std::map<std::string, TObject*, internal::string_cmp>                             fTObjectMap;
  // the integer maps mimic the "index" of the element
  // this allows for disjoint indices without wasted space
  std::map<std::string, std::map<long long, bool>, internal::string_cmp >               fBoolIntMap;
  std::map<std::string, std::map<long long, long double>, internal::string_cmp >        fDecimalIntMap;
  std::map<std::string, std::map<long long, long long>, internal::string_cmp >          fIntegerIntMap;
  std::map<std::string, std::map<long long, unsigned long long>, internal::string_cmp > fCountingIntMap;
  std::map<std::string, std::map<long long, std::string>, internal::string_cmp >        fStringIntMap;
  std::map<std::string, std::map<long long, TObject*>, internal::string_cmp >           fTObjectIntMap;
  // for 2D "indexing"
  std::map<std::string, std::map<long long, std::map<long long, bool> >, internal::string_cmp >               fBoolIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, long double> >, internal::string_cmp >        fDecimalIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, long long> >, internal::string_cmp >          fIntegerIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, unsigned long long> >, internal::string_cmp > fCountingIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, std::string> >, internal::string_cmp >        fStringIntIntMap;
  std::map<std::string, std::map<long long, std::map<long long, TObject*> >, internal::string_cmp >           fTObjectIntIntMap;

public:
  virtual ~AnalysisData () {}

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

  ClassDef(AnalysisData, 0);

};

#ifndef __CLING__

namespace StaticDataMap_internal {
  // keep in case looping over DataMap needs specialization
  template<typename FuncT, std::size_t I = 0, typename ... Tp>
  void
  for_each_DataMap (std::tuple<Tp ...> &t, FuncT &&f)
  {
    f(std::get<I>(t));
    ::HAL::internal::for_each<FuncT, I + 1, Tp ...>(t, std::forward<FuncT>(f));
  }

  template<typename FuncT, std::size_t I = 0, typename ... Tp>
  void
  for_each_DataMap (std::tuple<Tp ...> &t, const FuncT &f)
  {
    f(std::get<I>(t));
    ::HAL::internal::for_each<FuncT, I + 1, Tp ...>(t, f);
  }

  template <class Type> using map = std::map<std::string, Type, ::HAL::internal::string_cmp>;

}



// CRTP: Derived::TupleType_t<Types>
//        Derived type needs to supply a TupleType_t template that takes one parameter (expanded from variatic Types...); this is the type that will be stored
//        ex: for just storing single values of a given type - template <class T> using TupleType_t = T;
//        ex: for storing tuples of values of a given type with a bool - template <class T> using TupleType_t = std::tuple<T, bool>;
template<class Derived, class ... Types>
class StaticDataMap {
private:

  // "type", map
  std::tuple<StaticDataMap_internal::map<typename Derived::template TupleType_t<Types>>...> m_variableMaps;
  unsigned long long m_count     = 0;

public:
  ClassDef(StaticDataMap, 0);

  class StaticDataMap_Proxy {
    StaticDataMap &m_map;
    std::string    m_key;

  public:

    StaticDataMap_Proxy (StaticDataMap &map, const std::string &key) :
      m_map(map), m_key(key) {}

      template<class Ref>
      operator Ref()
      {
        return m_map.get<Ref>(m_key);
      }

      template<class Ref>
      StaticDataMap& operator=(const Ref &val)
      {
        m_map.set(m_key, val);
        return m_map;
      }
    };

  virtual ~StaticDataMap () {}

  // basic things
  template<class Ref>
  auto access(const std::string & name)
  {
    return std::get<std::decay<Ref>::decay_t>(m_variableMaps)[name];
  }

  template<class Ref>
  Ref get (const std::string &name)
  {
    if (!this->exists(name)) Fatal("StaticDataMap::get()" ,"key (%s) does not exist", name.c_str());
    // return std::get<0>(this->access<Ref>(name));
    return this->access<Ref>(name);
  }

  template<class Ref>
  void get (const std::string &name, Ref &val)
  {
    val = this->get<Ref>(name);
  }

  template<class Ref>
  void set (const std::string &name, const Ref &val)
  {
    // std::get<0>(this->access<Ref>(name)) = val;
    this->access<Ref>(name) = val;
  }

  template<class Func>
  void for_each (const Func &f)
  {
    StaticDataMap_internal::for_each_DataMap(m_variableMaps, f);
  }

  bool exists (const std::string &key)
  {
    auto result = false;
    auto keyCheck = [&result, &key](const auto &t) {
      if (result) return;
      for (auto &p : t) {
        result = result || (key == p.first);
        if (result) break;
      }
    };
    this->for_each(keyCheck);
    return result;
  } // exists

  StaticDataMap_Proxy operator[] (const std::string &key)
  {
    return StaticDataMap_Proxy(*this, key);
  }

};

#endif

} /*  HAL */

#endif
