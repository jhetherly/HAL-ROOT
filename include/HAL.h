#include <HAL/Common.h>
#include <HAL/Algorithms.h>
#include <HAL/Algorithm.h>
#include <HAL/Analysis.h>
#include <HAL/AnalysisData.h>
#include <HAL/AnalysisTreeReader.h>
#include <HAL/AnalysisTreeWriter.h>
#include <HAL/AnalysisSelector.h>
#include <HAL/AnalysisUtils.h>
#include <HAL/CutAlgorithm.h>
#include <HAL/CutOptimizer.h>
#include <HAL/Integrator.h>
#include <HAL/Interpolator.h>
#include <HAL/PlotUtils.h>
#include <HAL/PythonReconstructionAlgorithm.h>
#include <HAL/ReconstructionAlgorithm.h>

/*!
 * \mainpage The H.E.P. %Analysis Library
 *
 * \section intro_sec Introduction
 * This a custom ROOT library that provides a framework for physics analysis. It contains several functions and
 * classes that aid in analyzing high energy physics data. To aid in the compilation of all the source code, a macro
 * entitled `CompileSource.C` can be executed from the command line. All functions and classes are available in
 * python from the `lib/HAL.py` module. Furthermore, everything lives in the 'HAL' namespace. HAL stands for\n
 * H - H.E.P.\n
 * A - Analysis\n
 * L - Library\n
 * Of course, this is also a thinly veiled reference to the villian in the classic "2001: A Space Odyssey." But, I
 * personally think this bit of software is far less pernicious.
 * */

