/*!
 * \file
 * \author  Jeff Hetherly <jhetherly@smu.edu>
 */

#ifndef HAL_Exceptions
#define HAL_Exceptions

#include <TString.h>
#include <exception>
#include <HAL/Common.h>

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
