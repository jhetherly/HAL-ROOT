#include <HAL/AnalysisTreeReader.h>

namespace HAL {

AnalysisTreeReader::AnalysisTreeReader (TTree *t) {fChain = t;}
AnalysisTreeReader::~AnalysisTreeReader () {}

} /* HAL */ 
