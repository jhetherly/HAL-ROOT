#include <HAL/Algorithms/Cut.h>

namespace HAL
{

/*
 * Cutting Algorithm
 * */

Algorithms::Cut::Cut (TString name, TString title, TString logic, 
    long long length, ...) :
  CutAlgorithm(name, title), fAnd(false), fOr(false) {
  va_list arguments;  // store the variable list of arguments

  if (logic.EqualTo("and", TString::kIgnoreCase))
    fAnd = true;
  else if (logic.EqualTo("or", TString::kIgnoreCase))
    fOr = true;
  va_start (arguments, length); // initializing arguments to store all values after length
  for (long long i = 0; i < length; ++i) {
    const char* algo_name = va_arg(arguments, const char*);
    TString Type(va_arg(arguments, const char*));
    TString Op(va_arg(arguments, const char*));

    if (Type.EqualTo("bool", TString::kIgnoreCase)) {
      internal::BoolAlgoInfo *Algo = new internal::BoolAlgoInfo();
      Algo->fValue = (bool)va_arg(arguments, int);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
    else if (Type.EqualTo("integer", TString::kIgnoreCase)) {
      internal::IntegerAlgoInfo *Algo = new internal::IntegerAlgoInfo();
      Algo->fValue = (long long)va_arg(arguments, int);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
    else if (Type.EqualTo("counting", TString::kIgnoreCase)) {
      internal::CountingAlgoInfo *Algo = new internal::CountingAlgoInfo();
      Algo->fValue = (unsigned long long)va_arg(arguments, unsigned int);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
    else if (Type.EqualTo("decimal", TString::kIgnoreCase)) {
      internal::DecimalAlgoInfo *Algo = new internal::DecimalAlgoInfo();
      Algo->fValue = (long double)va_arg(arguments, double);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
    else if (Type.EqualTo("particle", TString::kIgnoreCase)) {
      internal::NParticlesAlgoInfo *Algo = new internal::NParticlesAlgoInfo();
      Algo->fValue = (long long)va_arg(arguments, int);
      Algo->fName = algo_name;
      if (Op.EqualTo("==") || Op.EqualTo("="))
        Algo->fEqual = true;
      else if (Op.EqualTo("!="))
        Algo->fNotEqual = true;
      else if (Op.EqualTo(">"))
        Algo->fGreaterThan = true;
      else if (Op.EqualTo("<"))
        Algo->fLessThan = true;
      else if (Op.EqualTo(">="))
        Algo->fGreaterThanEqual = true;
      else if (Op.EqualTo("<="))
        Algo->fLessThanEqual = true;
      fAlgorithms.push_back(Algo);
    }
  }
  va_end(arguments); // cleans up the list
}

Algorithms::Cut::~Cut () {
  for (std::vector<internal::AlgoInfo*>::iterator it = fAlgorithms.begin();
      it != fAlgorithms.end(); ++it) {
    delete *it;
  }
}

void Algorithms::Cut::Exec (Option_t* /*option*/) {
  AnalysisData *data = GetUserData();
  HAL::GenericData *input_data = NULL;

  if (fAnd) {
    for (std::vector<internal::AlgoInfo*>::iterator it = fAlgorithms.begin();
         it != fAlgorithms.end(); ++it) {
      if (!data->Exists((*it)->fName)) {
        Abort();
        return;
      }
      input_data = (GenericData*)data->GetTObject((*it)->fName);
      if (!(*it)->Eval(data, input_data)) {
        Abort();
        return;
      }
    }
    Passed();
  }
  else if (fOr) {
    for (std::vector<internal::AlgoInfo*>::iterator it = fAlgorithms.begin();
         it != fAlgorithms.end(); ++it) {
      if (data->Exists((*it)->fName)) {
        input_data = (GenericData*)data->GetTObject((*it)->fName);
        if ((*it)->Eval(data, input_data)) {
          Passed();
          return;
        }
      }
    }
    Abort();
  }
}

bool  internal::BoolAlgoInfo::Eval (HAL::AnalysisData *data, HAL::GenericData *gen_data) {
  bool current_value = data->GetBool(gen_data->GetRefName());

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  return false;
}

bool  internal::IntegerAlgoInfo::Eval (HAL::AnalysisData *data, HAL::GenericData *gen_data) {
  long long current_value = data->GetInteger(gen_data->GetRefName());

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  if (fLessThan && current_value < fValue)
    return true;
  if (fGreaterThan && current_value > fValue)
    return true;
  if (fLessThanEqual && current_value <= fValue)
    return true;
  if (fGreaterThanEqual && current_value >= fValue)
    return true;
  return false;
}

bool  internal::CountingAlgoInfo::Eval (HAL::AnalysisData *data, HAL::GenericData *gen_data) {
  unsigned long long current_value = data->GetCounting(gen_data->GetRefName());

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  if (fLessThan && current_value < fValue)
    return true;
  if (fGreaterThan && current_value > fValue)
    return true;
  if (fLessThanEqual && current_value <= fValue)
    return true;
  if (fGreaterThanEqual && current_value >= fValue)
    return true;
  return false;
}

bool  internal::DecimalAlgoInfo::Eval (HAL::AnalysisData *data, HAL::GenericData *gen_data) {
  long double current_value = data->GetDecimal(gen_data->GetRefName());

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  if (fLessThan && current_value < fValue)
    return true;
  if (fGreaterThan && current_value > fValue)
    return true;
  if (fLessThanEqual && current_value <= fValue)
    return true;
  if (fGreaterThanEqual && current_value >= fValue)
    return true;
  return false;
}

bool  internal::NParticlesAlgoInfo::Eval (HAL::AnalysisData * /*data*/, HAL::GenericData *gen_data) {
  long long current_value = gen_data->GetNParticles();

  if (fEqual && current_value == fValue)
    return true;
  if (fNotEqual && current_value != fValue)
    return true;
  if (fLessThan && current_value < fValue)
    return true;
  if (fGreaterThan && current_value > fValue)
    return true;
  if (fLessThanEqual && current_value <= fValue)
    return true;
  if (fGreaterThanEqual && current_value >= fValue)
    return true;
  return false;
}

} /* HAL */ 
