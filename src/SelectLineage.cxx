#include <HAL/Algorithms/SelectLineage.h>

namespace HAL
{

/*
 * Generic class
 * */
internal::FilterParentAlgo::FilterParentAlgo (TString name, TString title, 
    TString input, TString logic, long long length, ...) :
  Algorithm(name, title), fExclude(logic.EqualTo("ex", TString::kIgnoreCase)), fInput(input) {

  fRefNames = new const char*[length];
  va_list arguments;  // store the variable list of arguments

  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < length; ++i)
    fRefNames[i] = va_arg(arguments, const char*);
  va_end(arguments); // cleans up the list
}

void internal::FilterParentAlgo::Exec (Option_t* /*option*/) {
}

void internal::FilterParentAlgo::Clear (Option_t* /*option*/) {
  delete GetUserData()->GetTObject(GetName());
}

} /* HAL */ 
