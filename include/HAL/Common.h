#ifndef HAL_Common
#define HAL_Common

#ifndef __CINT__
#include <aux/boost/config.hpp>
#endif
#include <cstring>
#include <string>
#include <tuple>
#include <type_traits>
#include <RVersion.h>
#include <TString.h>

#if ROOT_VERSION_CODE < ROOT_VERSION(5,34,17)
typedef double  LongDouble_t;
#endif

#ifdef BOOST_NO_CXX11_NULLPTR
// this is in Rtypes.h in certain versions of ROOT
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
const                        // this is a const object...
class
{
public:
  template<class T>          // convertible to any type
    operator T*() const      // of null non-member
    { return 0; }            // pointer...
  template<class C, class T> // or any type of null
    operator T C::*() const  // member pointer...
    { return 0; }
private:
  void operator&() const;    // whose address can't be taken
} nullptr = {};              // and whose name is nullptr
#endif
#endif

namespace HAL
{

namespace internal
{

// CHANGE TO fast_string_cmp
class string_cmp {
public:
  bool operator() (const char *a, const char *b) const {
    unsigned n = strlen(a);
    unsigned m = strlen(b);

    if (n < m)
      return true;
    if (m < n)
      return false;
    return memcmp(a, b, n) < 0 ? true : false;
  }
  bool operator() (const std::string &a, const std::string &b) const {
    unsigned n = a.size();
    unsigned m = b.size();

    if (n < m)
      return true;
    if (m < n)
      return false;
    return memcmp(a.c_str(), b.c_str(), n) < 0 ? true : false;
  }
  bool operator() (const TString &a, const TString &b) const {
    unsigned n = a.Length();
    unsigned m = b.Length();

    if (n < m)
      return true;
    if (m < n)
      return false;
    return memcmp(a.Data(), b.Data(), n) < 0 ? true : false;
  }
};

// ---------------------------------------------
// tuple for_each (iterate over an std::tuple))
// ---------------------------------------------
// for_each begin
// ---------------------------------------------
template<typename FuncT, std::size_t I = 0, typename ... Tp>
inline typename std::enable_if<I == sizeof ... (Tp), void>::type
for_each (std::tuple<Tp ...>&, FuncT&&)
{}

template<typename FuncT, std::size_t I = 0, typename ... Tp>
inline typename std::enable_if<I == sizeof ... (Tp), void>::type
for_each (std::tuple<Tp ...>&, const FuncT&)
{}

template<typename FuncT, std::size_t I = 0, typename ... Tp>
inline typename std::enable_if < I<sizeof ... (Tp), void>::type
for_each (std::tuple<Tp ...> &t, FuncT &&f)
{
  f(std::get<I>(t));
  ::HAL::internal::for_each<FuncT, I + 1, Tp ...>(t, std::forward<FuncT>(f));
}

template<typename FuncT, std::size_t I = 0, typename ... Tp>
inline typename std::enable_if < I<sizeof ... (Tp), void>::type
for_each (std::tuple<Tp ...> &t, const FuncT &f)
{
  f(std::get<I>(t));
  ::HAL::internal::for_each<FuncT, I + 1, Tp ...>(t, f);
}
// ---------------------------------------------
// for_each end
// ---------------------------------------------

} /* internal */

} /* HAL */

#endif
