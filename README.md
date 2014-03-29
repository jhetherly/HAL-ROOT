# Introduction
This is my personal, custom ROOT library. It contains several functions and classes
that aid in analyzing high energy physics data. All macros are meant to be used in their
compiled form and may be too complex for the CINT interpreter. To aid in the compilation
of all the source code, a macro entitled 'CompileSource.C' can be executed from the
command line by 'root -q -b -l CompileSource.C'. All functions and classes are available
in python from the lib/python/HAL.py module. Furthermore, everything lives in the 'HAL'
namespace. HAL stands for  
H - H.E.P.  
A - Analysis  
L - Library  
Of course, this is also a thinly veiled reference to the villian in the classic 
"2001: A Space Odyssey." But, I personally think this bit of software is far less 
pernicious.

# Requirements and Installation
This software requires a working version of ROOT along with a compatable version of python
if one is to use HAL.py (in other words, you need to be able to use PyROOT from anywhere
in your system). Additionally, it requires a few modifications to your ROOT and python
environments. Here are the steps to installing this software:  

1. Run the CompileSource.C macro as described above
2. Add the 'lib' folder to your Unix.\*.Root.DynamicPath (or Windows equivalent) in .rootrc
3. Add the 'include' folder to your ACLiC include path in .rootrc
(e.g. ACLiC.IncludePaths:   -I$(HOME)/src/HAL-ROOT/include)
3. Add the 'lib/python' folder to your PYTHONPATH  
4. Optionally you can add 'lib' to Unix.\*.Root.MacroPath for easy loading of the library in
an interactive ROOT session (e.g. '.L libHAL.so')

# Documentation
There is no formal documentation for this software, but the header/source files have 
explainatory comments or self-explainatory function names/parameters.

# Testing
To test that everything is correctly there are several macros and python scripts in the
tests folder. They should work like any other macro or python script ('root -b -q test.C'
and 'python test.py').

# TODO
