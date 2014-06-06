#include <HAL/Common.h>
#include <HAL/Algorithm.h>
#include <HAL/Algorithms/AttachAttribute.h>
#include <HAL/Algorithms/Cut.h>
#include <HAL/Algorithms/ImportParticle.h>
#include <HAL/Algorithms/ImportValue.h>
#include <HAL/Algorithms/MinChiSquaredSelection.h>
#include <HAL/Algorithms/Monitor.h>
#include <HAL/Algorithms/SelectLineage.h>
#include <HAL/Algorithms/SelectParticle.h>
#include <HAL/Algorithms/SelectRank.h>
#include <HAL/Algorithms/SelectRefParticle.h>
#include <HAL/Algorithms/StoreParticle.h>
#include <HAL/Algorithms/VecAddReco.h>
#include <HAL/Analysis.h>
#include <HAL/AnalysisData.h>
#include <HAL/AnalysisTreeReader.h>
#include <HAL/AnalysisTreeWriter.h>
#include <HAL/AnalysisSelector.h>
#include <HAL/AnalysisUtils.h>
#include <HAL/CutAlgorithm.h>
#include <HAL/CutOptimizer.h>
#include <HAL/GenericData.h>
#include <HAL/GenericParticle.h>
#include <HAL/Integrator.h>
#include <HAL/Interpolator.h>
#include <HAL/PlotUtils.h>
//#include <HAL/PythonAlgorithm.h>

/*!
 * \mainpage Welcome, to the HAL code reference.
 *
 * \section intro_sec Introduction
 * This a custom ROOT library that provides a framework for physics analysis. It contains several functions and
 * classes that aid in analyzing high energy physics data. To aid in the compilation of all the source code, a macro
 * entitled `CompileSource.C` can be executed from the command line. All functions and classes are available in
 * python from the `lib/HAL.py` module. Furthermore, everything lives in the 'HAL' namespace. HAL stands for\n
 * H - H.E.P.\n
 * A - Analysis\n
 * L - Library\n
 * Of course, this is also a thinly veiled reference to the villian in the classic "2001: A Space Odyssey." However, 
 * this bit of software is far less pernicious.
 *
 * \section overview_sec Overview
 * HAL's design goals are:\n
 * Flexiblity - Users have choice in how they use HAL. A user can take full advantage of the builtin
 * framework and wide selection of generic algorithms or just add a few helpful HAL classes or functions to an
 * existing analysis.\n
 * Usability - HAL strives to make the user experience as physics-focused as possible. This means providing classes
 * that do a lot of the heavy lifting, including: reading a TTree and assigning branches, looping 
 * over events, making unique tuples from lists of particles, and more. The interfaces for these classes are designed
 * for a smooth user experience.\n
 * Extensibility - The framework in HAL is trivially extended with custom user code. This means that analysis-specific
 * code can couple with generic algorithms with minimal effort.\n
 * Performance - HAL leverages the performance advanages of C++. The framework is competitive with hand-coded analyses
 * but with advanage of ample code reuse.\n
 * */

/// \todo Generic Algorithms: Add chi-squared minimization algorithm
/// \todo Generic Algorithms: Add parent selection algorithm
/// \todo Generic Algorithms: Add merging algorithm
/// \todo Generic Algorithms: Add monitor for UserData algorithm
/// \todo Generic Algorithms: Add parent/child traversal algorithms
/// \todo Generic Algorithms: Make error messages more informative
