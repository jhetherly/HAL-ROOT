#include <vector>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ defined_in "HAL/Algorithm.h";
#pragma link C++ defined_in "HAL/Algorithms.h";
#pragma link C++ defined_in "HAL/Analysis.h";
#pragma link C++ defined_in "HAL/AnalysisData.h";
#pragma link C++ defined_in "HAL/AnalysisTreeReader.h";
#pragma link C++ defined_in "HAL/AnalysisTreeWriter.h";
#pragma link C++ defined_in "HAL/AnalysisSelector.h";
#pragma link C++ defined_in "HAL/AnalysisUtils.h";
#pragma link C++ defined_in "HAL/Common.h";
#pragma link C++ defined_in "HAL/CutAlgorithm.h";
#pragma link C++ defined_in "HAL/CutOptimizer.h";
#pragma link C++ defined_in "HAL/GenericData.h";
#pragma link C++ defined_in "HAL/GenericParticle.h";
#pragma link C++ defined_in "HAL/Integrator.h";
#pragma link C++ defined_in "HAL/Interpolator.h";
#pragma link C++ defined_in "HAL/PlotUtils.h";
#pragma link C++ defined_in "HAL/PythonAlgorithm.h";

// These are needed for the AnalysisTreeWriter class
#pragma link C++ class vector<bool>+;
#pragma link C++ class vector<long double>+;
#pragma link C++ class vector<long long>+;
#pragma link C++ class vector<unsigned long long>+;
#pragma link C++ class vector<vector<bool> >+;
#pragma link C++ class vector<vector<long double> >+;
#pragma link C++ class vector<vector<long long> >+;
#pragma link C++ class vector<vector<unsigned long long> >+;

#endif
