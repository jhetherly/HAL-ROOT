#ifndef HAL_COMMON
#define HAL_COMMON

#include <RVersion.h>
#include <TString.h>
#include <cstring>
#include <string>

#if ROOT_VERSION_CODE < ROOT_VERSION(5,34,17)
typedef double     LongDouble_t;
#endif

namespace HAL
{

namespace internal
{

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
