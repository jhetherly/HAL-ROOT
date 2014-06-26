#include <HAL/Algorithms/ImportValue.h>

namespace HAL
{

namespace Algorithms
{

ImportBoolValue<HAL::AnalysisTreeReader>::ImportBoolValue (TString name, TString title) : 
  HAL::internal::ImportValueAlgo<bool>(name, title) {
  fRefName = "bool";
  fValueLabel = TString::Format("%s:%s", name.Data(), fRefName.Data());
}

bool ImportBoolValue<HAL::AnalysisTreeReader>::GetValue () {
  return GetRawData()->GetBool(fValueLabel);
}

ImportIntegerValue<HAL::AnalysisTreeReader>::ImportIntegerValue (TString name, TString title) : 
  HAL::internal::ImportValueAlgo<long long>(name, title) {
  fRefName = "integer";
  fValueLabel = TString::Format("%s:%s", name.Data(), fRefName.Data());
}

long long ImportIntegerValue<HAL::AnalysisTreeReader>::GetValue () {
  return GetRawData()->GetInteger(fValueLabel);
}

ImportCountingValue<HAL::AnalysisTreeReader>::ImportCountingValue (TString name, TString title) : 
  HAL::internal::ImportValueAlgo<unsigned long long>(name, title) {
  fRefName = "counting";
  fValueLabel = TString::Format("%s:%s", name.Data(), fRefName.Data());
}

ImportDecimalValue<HAL::AnalysisTreeReader>::ImportDecimalValue (TString name, TString title) : 
  HAL::internal::ImportValueAlgo<long double>(name, title) {
  fRefName = "decimal";
  fValueLabel = TString::Format("%s:%s", name.Data(), fRefName.Data());
}

long double ImportDecimalValue<HAL::AnalysisTreeReader>::GetValue () {
  return GetRawData()->GetDecimal(fValueLabel);
}

unsigned long long ImportCountingValue<HAL::AnalysisTreeReader>::GetValue () {
  return GetRawData()->GetCounting(fValueLabel);
}

typedef ImportBoolValue<HAL::AnalysisTreeReader> ImportBool;

typedef ImportIntegerValue<HAL::AnalysisTreeReader> ImportInteger;

typedef ImportCountingValue<HAL::AnalysisTreeReader> ImportCounting;

typedef ImportDecimalValue<HAL::AnalysisTreeReader> ImportDecimal;

}

} /* HAL */ 
