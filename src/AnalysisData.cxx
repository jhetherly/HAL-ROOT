#include <HAL/AnalysisData.h>

ClassImp(HAL::AnalysisData);

namespace HAL
{

void AnalysisData::SetValue (std::string name, bool v) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kB;
  else if (fNameTypeMap[name] != kB)
    throw HALException(name.insert(0, "Cannot reassign bool storage container for "));

  if (fBoolMap.count(name) != 0)
    fBoolMap[name] = v;
  else
    fBoolMap.insert(std::pair<std::string, bool>(name, v));
}

void AnalysisData::SetValue (std::string name, long double v) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kD;
  else if (fNameTypeMap[name] == kI)
    SetValue(name, (long long)v);
  else if (fNameTypeMap[name] == kC && v >= 0.0)
    SetValue(name, (unsigned long long)v);
  else if (fNameTypeMap[name] != kD)
    throw HALException(name.insert(0, "Cannot reassign decimal storage container for "));

  if (fDecimalMap.count(name) != 0)
    fDecimalMap[name] = v;
  else
    fDecimalMap.insert(std::pair<std::string, long double>(name, v));
}

void AnalysisData::SetValue (std::string name, long long v) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kI;
  else if (fNameTypeMap[name] == kD)
    SetValue(name, (long double)v);
  else if (fNameTypeMap[name] == kC && v >= 0)
    SetValue(name, (unsigned long long)v);
  else if (fNameTypeMap[name] != kI)
    throw HALException(name.insert(0, "Cannot reassign integer storage container for "));

  if (fIntegerMap.count(name) != 0)
    fIntegerMap[name] = v;
  else
    fIntegerMap.insert(std::pair<std::string, long long>(name, v));
}

void AnalysisData::SetValue (std::string name, unsigned long long v) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kC;
  else if (fNameTypeMap[name] == kD)
    SetValue(name, (long double)v);
  else if (fNameTypeMap[name] == kI)
    SetValue(name, (long long)v);
  else if (fNameTypeMap[name] != kC)
    throw HALException(name.insert(0, "Cannot reassign counting storage container for "));

  if (fCountingMap.count(name) != 0)
    fCountingMap[name] = v;
  else 
    fCountingMap.insert(std::pair<std::string, unsigned long long>(name, v));
}

void AnalysisData::SetValue (std::string name, std::string v) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kS;
  else if (fNameTypeMap[name] != kS)
    throw HALException(name.insert(0, "Cannot reassign string storage container for "));

  if (fStringMap.count(name) != 0)
    fStringMap[name] = v;
  else 
    fStringMap.insert(std::pair<std::string, std::string>(name, v));
}

void AnalysisData::SetValue (std::string name, TObject *v) { 
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kO;
  else if (fNameTypeMap[name] != kO)
    throw HALException(name.insert(0, "Cannot reassign TObject storage container for "));

  if (fTObjectMap.count(name) != 0)
    fTObjectMap[name] = v;
  else 
    fTObjectMap.insert(std::pair<std::string, TObject*>(name, v));
}

void AnalysisData::SetValue (std::string name, bool v, long long i) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIB;
  else if (fNameTypeMap[name] != kIB)
    throw HALException(name.insert(0, "Cannot reassign 1D bool storage container for "));

  if (fBoolIntMap.count(name) != 0 && fBoolIntMap[name].count(i) != 0)
    fBoolIntMap[name][i] = v;
  else if (fBoolIntMap.count(name) != 0)
    fBoolIntMap[name].insert(std::pair<long long, bool>(i, v));
  else {
    std::map<long long, bool>   temp;
    temp.insert(std::pair<long long, bool>(i, v));
    fBoolIntMap.insert(std::pair<std::string, std::map<long long, bool> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, long double v, long long i) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kID;
  else if (fNameTypeMap[name] == kII)
    SetValue(name, (long long)v, i);
  else if (fNameTypeMap[name] == kIC && v >= 0.0)
    SetValue(name, (unsigned long long)v, i);
  else if (fNameTypeMap[name] != kID)
    throw HALException(name.insert(0, "Cannot reassign 1D decimal storage container for "));

  if (fDecimalIntMap.count(name) != 0 && fDecimalIntMap[name].count(i) != 0)
    fDecimalIntMap[name][i] = v;
  else if (fDecimalIntMap.count(name) != 0)
    fDecimalIntMap[name].insert(std::pair<long long, long double>(i, v));
  else {
    std::map<long long, long double>   temp;
    temp.insert(std::pair<long long, long double>(i, v));
    fDecimalIntMap.insert(std::pair<std::string, std::map<long long, long double> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, long long v, long long i) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kII;
  else if (fNameTypeMap[name] == kID)
    SetValue(name, (long double)v, i);
  else if (fNameTypeMap[name] == kIC && v >= 0)
    SetValue(name, (unsigned long long)v, i);
  else if (fNameTypeMap[name] != kII)
    throw HALException(name.insert(0, "Cannot reassign 1D integer storage container for "));

  if (fIntegerIntMap.count(name) != 0 && fIntegerIntMap[name].count(i) != 0)
    fIntegerIntMap[name][i] = v;
  else if (fIntegerIntMap.count(name) != 0)
    fIntegerIntMap[name].insert(std::pair<long long, long long>(i, v));
  else {
    std::map<long long, long long>   temp;
    temp.insert(std::pair<long long, long long>(i, v));
    fIntegerIntMap.insert(std::pair<std::string, std::map<long long, long long> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, unsigned long long v, long long i) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIC;
  else if (fNameTypeMap[name] == kID)
    SetValue(name, (long double)v, i);
  else if (fNameTypeMap[name] == kII)
    SetValue(name, (long long)v, i);
  else if (fNameTypeMap[name] != kIC)
    throw HALException(name.insert(0, "Cannot reassign 1D counting storage container for "));

  if (fCountingIntMap.count(name) != 0 && fCountingIntMap[name].count(i) != 0)
    fCountingIntMap[name][i] = v;
  else if (fCountingIntMap.count(name) != 0)
    fCountingIntMap[name].insert(std::pair<long long, unsigned long long>(i, v));
  else {
    std::map<long long, unsigned long long>   temp;
    temp.insert(std::pair<long long, unsigned long long>(i, v));
    fCountingIntMap.insert(std::pair<std::string, std::map<long long, unsigned long long> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, std::string v, long long i) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIS;
  else if (fNameTypeMap[name] != kIS)
    throw HALException(name.insert(0, "Cannot reassign 1D string storage container for "));

  if (fStringIntMap.count(name) != 0 && fStringIntMap[name].count(i) != 0)
    fStringIntMap[name][i] = v;
  else if (fStringIntMap.count(name) != 0)
    fStringIntMap[name].insert(std::pair<long long, std::string>(i, v));
  else {
    std::map<long long, std::string>   temp;
    temp.insert(std::pair<long long, std::string>(i, v));
    fStringIntMap.insert(std::pair<std::string, std::map<long long, std::string> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, TObject *v, long long i) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIO;
  else if (fNameTypeMap[name] != kIO)
    throw HALException(name.insert(0, "Cannot reassign 1D TObject storage container for "));

  if (fTObjectIntMap.count(name) != 0 && fTObjectIntMap[name].count(i) != 0)
    fTObjectIntMap[name][i] = v;
  else if (fTObjectIntMap.count(name) != 0)
    fTObjectIntMap[name].insert(std::pair<long long, TObject*>(i, v));
  else {
    std::map<long long, TObject*>   temp;
    temp.insert(std::pair<long long, TObject*>(i, v));
    fTObjectIntMap.insert(std::pair<std::string, std::map<long long, TObject*> >(name, temp));
  }
}

void AnalysisData::SetValue (std::string name, bool v, long long i, long long j) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIIB;
  else if (fNameTypeMap[name] != kIIB)
    throw HALException(name.insert(0, "Cannot reassign 2D bool storage container for "));

  if (fBoolIntIntMap.count(name) != 0 && fBoolIntIntMap[name].count(i) != 0 && fBoolIntIntMap[name][i].count(j) != 0)
    fBoolIntIntMap[name][i][j] = v;
  else if (fBoolIntIntMap.count(name) != 0 && fBoolIntIntMap[name].count(i) != 0)
    fBoolIntIntMap[name][i].insert(std::pair<long long, bool>(i, v));
  else if (fBoolIntIntMap.count(name) != 0) {
    std::map<long long, bool>   temp;
    temp.insert(std::pair<long long, bool>(j, v));
    fBoolIntIntMap[name].insert(std::pair<long long, std::map<long long, bool> >(i, temp));
  }
  else {
    std::map<long long, bool>                       temp1;
    std::map<long long, std::map<long long, bool> > temp2;
    temp1.insert(std::pair<long long, bool>(j, v));
    temp2.insert(std::pair<long long, std::map<long long, bool> >(i, temp1));
    fBoolIntIntMap.insert(std::pair<std::string, std::map<long long, std::map<long long, bool> > >(name, temp2));
  }
}

void AnalysisData::SetValue (std::string name, long double v, long long i, long long j) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIID;
  else if (fNameTypeMap[name] == kIII)
    SetValue(name, (long long)v, i, j);
  else if (fNameTypeMap[name] == kIIC && v >= 0.0)
    SetValue(name, (unsigned long long)v, i, j);
  else if (fNameTypeMap[name] != kIID)
    throw HALException(name.insert(0, "Cannot reassign 2D decimal storage container for "));

  if (fDecimalIntIntMap.count(name) != 0 && fDecimalIntIntMap[name].count(i) != 0 && fDecimalIntIntMap[name][i].count(j) != 0)
    fDecimalIntIntMap[name][i][j] = v;
  else if (fDecimalIntIntMap.count(name) != 0 && fDecimalIntIntMap[name].count(i) != 0)
    fDecimalIntIntMap[name][i].insert(std::pair<long long, long double>(i, v));
  else if (fDecimalIntIntMap.count(name) != 0) {
    std::map<long long, long double>   temp;
    temp.insert(std::pair<long long, long double>(j, v));
    fDecimalIntIntMap[name].insert(std::pair<long long, std::map<long long, long double> >(i, temp));
  }
  else {
    std::map<long long, long double>                       temp1;
    std::map<long long, std::map<long long, long double> > temp2;
    temp1.insert(std::pair<long long, long double>(j, v));
    temp2.insert(std::pair<long long, std::map<long long, long double> >(i, temp1));
    fDecimalIntIntMap.insert(std::pair<std::string, std::map<long long, std::map<long long, long double> > >(name, temp2));
  }
}

void AnalysisData::SetValue (std::string name, long long v, long long i, long long j) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIII;
  else if (fNameTypeMap[name] == kIID)
    SetValue(name, (long double)v, i, j);
  else if (fNameTypeMap[name] == kIIC && v >= 0)
    SetValue(name, (unsigned long long)v, i, j);
  else if (fNameTypeMap[name] != kIII)
    throw HALException(name.insert(0, "Cannot reassign 2D integer storage container for "));

  if (fIntegerIntIntMap.count(name) != 0 && fIntegerIntIntMap[name].count(i) != 0 && fIntegerIntIntMap[name][i].count(j) != 0)
    fIntegerIntIntMap[name][i][j] = v;
  else if (fIntegerIntIntMap.count(name) != 0 && fIntegerIntIntMap[name].count(i) != 0)
    fIntegerIntIntMap[name][i].insert(std::pair<long long, long long>(i, v));
  else if (fIntegerIntIntMap.count(name) != 0) {
    std::map<long long, long long>   temp;
    temp.insert(std::pair<long long, long long>(j, v));
    fIntegerIntIntMap[name].insert(std::pair<long long, std::map<long long, long long> >(i, temp));
  }
  else {
    std::map<long long, long long>                       temp1;
    std::map<long long, std::map<long long, long long> > temp2;
    temp1.insert(std::pair<long long, long long>(j, v));
    temp2.insert(std::pair<long long, std::map<long long, long long> >(i, temp1));
    fIntegerIntIntMap.insert(std::pair<std::string, std::map<long long, std::map<long long, long long> > >(name, temp2));
  }
}

void AnalysisData::SetValue (std::string name, unsigned long long v, long long i, long long j) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIIC;
  else if (fNameTypeMap[name] == kIID)
    SetValue(name, (long double)v, i, j);
  else if (fNameTypeMap[name] == kIII)
    SetValue(name, (long long)v, i, j);
  else if (fNameTypeMap[name] != kIIC)
    throw HALException(name.insert(0, "Cannot reassign 2D counting storage container for "));

  if (fCountingIntIntMap.count(name) != 0 && fCountingIntIntMap[name].count(i) != 0 && fCountingIntIntMap[name][i].count(j) != 0)
    fCountingIntIntMap[name][i][j] = v;
  else if (fCountingIntIntMap.count(name) != 0 && fCountingIntIntMap[name].count(i) != 0)
    fCountingIntIntMap[name][i].insert(std::pair<long long, unsigned long long>(i, v));
  else if (fCountingIntIntMap.count(name) != 0) {
    std::map<long long, unsigned long long>   temp;
    temp.insert(std::pair<long long, unsigned long long>(j, v));
    fCountingIntIntMap[name].insert(std::pair<long long, std::map<long long, unsigned long long> >(i, temp));
  }
  else {
    std::map<long long, unsigned long long>                       temp1;
    std::map<long long, std::map<long long, unsigned long long> > temp2;
    temp1.insert(std::pair<long long, unsigned long long>(j, v));
    temp2.insert(std::pair<long long, std::map<long long, unsigned long long> >(i, temp1));
    fCountingIntIntMap.insert(std::pair<std::string, std::map<long long, std::map<long long, unsigned long long> > >(name, temp2));
  }
}

void AnalysisData::SetValue (std::string name, std::string v, long long i, long long j) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIIS;
  else if (fNameTypeMap[name] != kIIS)
    throw HALException(name.insert(0, "Cannot reassign 2D string storage container for "));

  if (fStringIntIntMap.count(name) != 0 && fStringIntIntMap[name].count(i) != 0 && fStringIntIntMap[name][i].count(j) != 0)
    fStringIntIntMap[name][i][j] = v;
  else if (fStringIntIntMap.count(name) != 0 && fStringIntIntMap[name].count(i) != 0)
    fStringIntIntMap[name][i].insert(std::pair<long long, std::string>(i, v));
  else if (fStringIntIntMap.count(name) != 0) {
    std::map<long long, std::string>   temp;
    temp.insert(std::pair<long long, std::string>(j, v));
    fStringIntIntMap[name].insert(std::pair<long long, std::map<long long, std::string> >(i, temp));
  }
  else {
    std::map<long long, std::string>                       temp1;
    std::map<long long, std::map<long long, std::string> > temp2;
    temp1.insert(std::pair<long long, std::string>(j, v));
    temp2.insert(std::pair<long long, std::map<long long, std::string> >(i, temp1));
    fStringIntIntMap.insert(std::pair<std::string, std::map<long long, std::map<long long, std::string> > >(name, temp2));
  }
}

void AnalysisData::SetValue (std::string name, TObject *v, long long i, long long j) {
  // check if 'name' already has a container
  if (!NameAlreadyStored(name))
    fNameTypeMap[name] = kIIO;
  else if (fNameTypeMap[name] != kIIO)
    throw HALException(name.insert(0, "Cannot reassign 2D TObject storage container for "));

  if (fTObjectIntIntMap.count(name) != 0 && fTObjectIntIntMap[name].count(i) != 0 && fTObjectIntIntMap[name][i].count(j) != 0)
    fTObjectIntIntMap[name][i][j] = v;
  else if (fTObjectIntIntMap.count(name) != 0 && fTObjectIntIntMap[name].count(i) != 0)
    fTObjectIntIntMap[name][i].insert(std::pair<long long, TObject*>(i, v));
  else if (fTObjectIntIntMap.count(name) != 0) {
    std::map<long long, TObject*>   temp;
    temp.insert(std::pair<long long, TObject*>(j, v));
    fTObjectIntIntMap[name].insert(std::pair<long long, std::map<long long, TObject*> >(i, temp));
  }
  else {
    std::map<long long, TObject*>                       temp1;
    std::map<long long, std::map<long long, TObject*> > temp2;
    temp1.insert(std::pair<long long, TObject*>(j, v));
    temp2.insert(std::pair<long long, std::map<long long, TObject*> >(i, temp1));
    fTObjectIntIntMap.insert(std::pair<std::string, std::map<long long, std::map<long long, TObject*> > >(name, temp2));
  }
}



bool AnalysisData::GetBool (std::string name, long long i, long long j) {
  if (fBoolMap.count(name) != 0)
    return fBoolMap[name];
  if (fBoolIntMap.count(name) != 0)
    return fBoolIntMap[name][i];
  if (fBoolIntIntMap.count(name) != 0)
    return fBoolIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

long double AnalysisData::GetDecimal (std::string name, long long i, long long j) {
  if (fDecimalMap.count(name) != 0)
    return fDecimalMap[name];
  if (fDecimalIntMap.count(name) != 0)
    return fDecimalIntMap[name][i];
  if (fDecimalIntIntMap.count(name) != 0)
    return fDecimalIntIntMap[name][i][j];
  if (fIntegerMap.count(name) != 0)
    return fIntegerMap[name];
  if (fIntegerIntMap.count(name) != 0)
    return fIntegerIntMap[name][i];
  if (fIntegerIntIntMap.count(name) != 0)
    return fIntegerIntIntMap[name][i][j];
  if (fCountingMap.count(name) != 0)
    return fCountingMap[name];
  if (fCountingIntMap.count(name) != 0)
    return fCountingIntMap[name][i];
  if (fCountingIntIntMap.count(name) != 0)
    return fCountingIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

long long AnalysisData::GetInteger (std::string name, long long i, long long j) {
  if (fDecimalMap.count(name) != 0)
    return fDecimalMap[name];
  if (fDecimalIntMap.count(name) != 0)
    return fDecimalIntMap[name][i];
  if (fDecimalIntIntMap.count(name) != 0)
    return fDecimalIntIntMap[name][i][j];
  if (fIntegerMap.count(name) != 0)
    return fIntegerMap[name];
  if (fIntegerIntMap.count(name) != 0)
    return fIntegerIntMap[name][i];
  if (fIntegerIntIntMap.count(name) != 0)
    return fIntegerIntIntMap[name][i][j];
  if (fCountingMap.count(name) != 0)
    return fCountingMap[name];
  if (fCountingIntMap.count(name) != 0)
    return fCountingIntMap[name][i];
  if (fCountingIntIntMap.count(name) != 0)
    return fCountingIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

unsigned long long AnalysisData::GetCounting (std::string name, long long i, long long j) {
  if (fDecimalMap.count(name) != 0)
    return fDecimalMap[name];
  if (fDecimalIntMap.count(name) != 0)
    return fDecimalIntMap[name][i];
  if (fDecimalIntIntMap.count(name) != 0)
    return fDecimalIntIntMap[name][i][j];
  if (fIntegerMap.count(name) != 0)
    return fIntegerMap[name];
  if (fIntegerIntMap.count(name) != 0)
    return fIntegerIntMap[name][i];
  if (fIntegerIntIntMap.count(name) != 0)
    return fIntegerIntIntMap[name][i][j];
  if (fCountingMap.count(name) != 0)
    return fCountingMap[name];
  if (fCountingIntMap.count(name) != 0)
    return fCountingIntMap[name][i];
  if (fCountingIntIntMap.count(name) != 0)
    return fCountingIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

std::string AnalysisData::GetString (std::string name, long long i, long long j) {
  if (fStringMap.count(name) != 0)
    return fStringMap[name];
  if (fStringIntMap.count(name) != 0)
    return fStringIntMap[name][i];
  if (fStringIntIntMap.count(name) != 0)
    return fStringIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

TObject* AnalysisData::GetTObject (std::string name, long long i, long long j) {
  if (fTObjectMap.count(name) != 0)
    return fTObjectMap[name];
  if (fTObjectIntMap.count(name) != 0)
    return fTObjectIntMap[name][i];
  if (fTObjectIntIntMap.count(name) != 0)
    return fTObjectIntIntMap[name][i][j];

  throw HALException(name.insert(0, "Error retrieving data: ").c_str());
}

} /*  HAL */ 

