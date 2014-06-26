#ifndef HAL_Common
#define HAL_Common

#ifndef __CINT__
#include <aux/boost/config.hpp>
#endif
#include <cstring>
#include <string>
#include <RVersion.h>
#include <TString.h>

#if ROOT_VERSION_CODE < ROOT_VERSION(5,34,17)
typedef double  LongDouble_t;
#endif

#ifdef BOOST_NO_CXX11_NULLPTR
const int       nullptr = NULL;
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

} /* internal */ 

} /* HAL */ 

#endif
