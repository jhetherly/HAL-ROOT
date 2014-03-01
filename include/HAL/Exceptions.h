#include <TString.h>
#include <exception>

#ifndef HAL_EXCEPTIONS
#define HAL_EXCEPTIONS

namespace HAL
{

class HALException : public std::exception {
public:
  HALException (TString m) : message(m) {}
  ~HALException () throw() {}
  virtual const char* what() const throw() {
    return message.Data();
  }

private:
  TString message;
};

} /* HAL */ 
#endif
