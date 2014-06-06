#include <HAL/Algorithms/ImportValue.h>

namespace HAL
{

namespace Algorithms
{

typedef ImportBoolValue<HAL::AnalysisTreeReader> ImportBool;

typedef ImportIntegerValue<HAL::AnalysisTreeReader> ImportInteger;

typedef ImportCountingValue<HAL::AnalysisTreeReader> ImportCounting;

typedef ImportDecimalValue<HAL::AnalysisTreeReader> ImportDecimal;

}

} /* HAL */ 
