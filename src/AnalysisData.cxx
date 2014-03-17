#include <HAL/AnalysisData.h>

ClassImp(HAL::AnalysisData);

namespace HAL
{

void AnalysisData::SetValue (std::string name, LongDouble_t v) {
  if (fDecimalMap.count(name) != 0)
    fDecimalMap[name] = v;
  else
    fDecimalMap.insert(std::pair<std::string, LongDouble_t>(name, v));
}

void AnalysisData::SetValue (std::string name, long long v) {
  if (fIntegerMap.count(name) != 0)
    fIntegerMap[name] = v;
  else
    fIntegerMap.insert(std::pair<std::string, long long>(name, v));
}

void AnalysisData::SetValue (std::string name, unsigned long long v) {
  if (fCountingMap.count(name) != 0)
    fCountingMap[name] = v;
  else 
    fCountingMap.insert(std::pair<std::string, unsigned long long>(name, v));
}

void AnalysisData::SetValue (std::string name, std::string v) {
  if (fStringMap.count(name) != 0)
    fStringMap[name] = v;
  else 
    fStringMap.insert(std::pair<std::string, std::string>(name, v));
}

void AnalysisData::SetValue (std::string name, TObject *v) { 
  if (fTObjectMap.count(name) != 0)
    fTObjectMap[name] = v;
  else 
    fTObjectMap.insert(std::pair<std::string, TObject*>(name, v));
}

void AnalysisData::SetValue (std::string name, LongDouble_t v, Int_t i) {
  if (fDecimalIntMap.count(name) != 0 && fDecimalIntMap[name].count(i) != 0)
    fDecimalIntMap[name][i] = v;
  else if (fDecimalIntMap.count(name) != 0)
    fDecimalIntMap[name].insert(std::pair<Int_t, LongDouble_t>(i, v));
  else {
    std::map<Int_t, LongDouble_t>   temp;
    temp.insert(std::pair<Int_t, LongDouble_t>(i, v));
    fDecimalIntMap.insert(std::pair<std::string, std::map<Int_t, LongDouble_t> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, long long v, Int_t i) {
  if (fIntegerIntMap.count(name) != 0 && fIntegerIntMap[name].count(i) != 0)
    fIntegerIntMap[name][i] = v;
  else if (fIntegerIntMap.count(name) != 0)
    fIntegerIntMap[name].insert(std::pair<Int_t, long long>(i, v));
  else {
    std::map<Int_t, long long>   temp;
    temp.insert(std::pair<Int_t, long long>(i, v));
    fIntegerIntMap.insert(std::pair<std::string, std::map<Int_t, long long> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, unsigned long long v, Int_t i) {
  if (fCountingIntMap.count(name) != 0 && fCountingIntMap[name].count(i) != 0)
    fCountingIntMap[name][i] = v;
  else if (fCountingIntMap.count(name) != 0)
    fCountingIntMap[name].insert(std::pair<Int_t, unsigned long long>(i, v));
  else {
    std::map<Int_t, unsigned long long>   temp;
    temp.insert(std::pair<Int_t, unsigned long long>(i, v));
    fCountingIntMap.insert(std::pair<std::string, std::map<Int_t, unsigned long long> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, std::string v, Int_t i) {
  if (fStringIntMap.count(name) != 0 && fStringIntMap[name].count(i) != 0)
    fStringIntMap[name][i] = v;
  else if (fStringIntMap.count(name) != 0)
    fStringIntMap[name].insert(std::pair<Int_t, std::string>(i, v));
  else {
    std::map<Int_t, std::string>   temp;
    temp.insert(std::pair<Int_t, std::string>(i, v));
    fStringIntMap.insert(std::pair<std::string, std::map<Int_t, std::string> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, TObject *v, Int_t i) {
  if (fTObjectIntMap.count(name) != 0 && fTObjectIntMap[name].count(i) != 0)
    fTObjectIntMap[name][i] = v;
  else if (fTObjectIntMap.count(name) != 0)
    fTObjectIntMap[name].insert(std::pair<Int_t, TObject*>(i, v));
  else {
    std::map<Int_t, TObject*>   temp;
    temp.insert(std::pair<Int_t, TObject*>(i, v));
    fTObjectIntMap.insert(std::pair<std::string, std::map<Int_t, TObject*> >(name, temp));
  }
}

LongDouble_t AnalysisData::GetDecimal (std::string name, Int_t i) {
  if (fDecimalMap.count(name) != 0)
    return fDecimalMap[name];
  if (fDecimalIntMap.count(name) != 0)
    return fDecimalIntMap[name][i];
  if (fIntegerMap.count(name) != 0)
    return fIntegerMap[name];
  if (fIntegerIntMap.count(name) != 0)
    return fIntegerIntMap[name][i];
  if (fCountingMap.count(name) != 0)
    return fCountingMap[name];
  if (fCountingIntMap.count(name) != 0)
    return fCountingIntMap[name][i];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

long long AnalysisData::GetInteger (std::string name, Int_t i) {
  if (fDecimalMap.count(name) != 0)
    return fDecimalMap[name];
  if (fDecimalIntMap.count(name) != 0)
    return fDecimalIntMap[name][i];
  if (fIntegerMap.count(name) != 0)
    return fIntegerMap[name];
  if (fIntegerIntMap.count(name) != 0)
    return fIntegerIntMap[name][i];
  if (fCountingMap.count(name) != 0)
    return fCountingMap[name];
  if (fCountingIntMap.count(name) != 0)
    return fCountingIntMap[name][i];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

unsigned long long AnalysisData::GetCounting (std::string name, Int_t i) {
  if (fDecimalMap.count(name) != 0)
    return fDecimalMap[name];
  if (fDecimalIntMap.count(name) != 0)
    return fDecimalIntMap[name][i];
  if (fIntegerMap.count(name) != 0)
    return fIntegerMap[name];
  if (fIntegerIntMap.count(name) != 0)
    return fIntegerIntMap[name][i];
  if (fCountingMap.count(name) != 0)
    return fCountingMap[name];
  if (fCountingIntMap.count(name) != 0)
    return fCountingIntMap[name][i];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

std::string AnalysisData::GetString (std::string name, Int_t i) {
  if (fStringMap.count(name) != 0)
    return fStringMap[name];
  if (fStringIntMap.count(name) != 0)
    return fStringIntMap[name][i];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

TObject* AnalysisData::GetTObject (std::string name, Int_t i) {
  if (fTObjectMap.count(name) != 0)
    return fTObjectMap[name];
  if (fTObjectIntMap.count(name) != 0)
    return fTObjectIntMap[name][i];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

} /*  HAL */ 

