#include <HAL/Algorithms/MinChiSquaredSelection.h>

namespace HAL
{

Algorithms::MinChiSquaredSelection::MinChiSquaredSelection (TString name, TString title, 
    long long length, ...) :
  Algorithm(name, title) {
  va_list arguments;  // store the variable list of arguments

  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < length; ++i) {
    // input_algo, property, ref_algo, mean_value, sigma_value, group_num, output_name
    TString InputAlgo(va_arg(arguments, const char*));
    TString Property(va_arg(arguments, const char*));
    TString RefAlgo(va_arg(arguments, const char*));
    double MeanValue = va_arg(arguments, double);
    double SigmaValue = va_arg(arguments, double);
    int GroupNumber = va_arg(arguments, int);
    TString OutputName(va_arg(arguments, const char*));

  }
  va_end(arguments); // cleans up the list
}

} /* HAL */ 
