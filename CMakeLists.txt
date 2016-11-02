# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(HAL)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

#---Set compiler flags
#---NOTE: use "cmake -DCMAKE_C_COMPILER=$(which `root-config --cc`) -DCMAKE_CXX_COMPILER=$(which `root-config --cxx`) .."
#---      if you want to use the same compilers as ROOT
set(CMAKE_C_FLAGS ${ROOT_C_FLAGS})
set(CMAKE_CXX_FLAGS ${ROOT_CXX_FLAGS})

#---point to header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#---TEMPORARY!!!
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/aux)

#---Create folder to hold includes and library
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/include")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/include/HAL")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

#---Make ROOT dictionaries
file(GLOB DICT_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/HAL/*.h")
list(REMOVE_ITEM DICT_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/HAL/HAL_LinkDef.h")
ROOT_GENERATE_DICTIONARY("${CMAKE_CURRENT_SOURCE_DIR}/src/HAL_dict" ${CMAKE_CURRENT_SOURCE_DIR}/include/HAL.h LINKDEF "${CMAKE_CURRENT_SOURCE_DIR}/include/HAL/HAL_LinkDef.h" OPTIONS -v -s "${PROJECT_BINARY_DIR}/lib/HAL_dict" -rmf "${PROJECT_BINARY_DIR}/lib/libHAL_dict.rootmap")
#ROOT_GENERATE_DICTIONARY("${CMAKE_CURRENT_SOURCE_DIR}/src/HAL_dict" ${DICT_INCLUDES} LINKDEF "${CMAKE_CURRENT_SOURCE_DIR}/include/HAL/HAL_LinkDef.h" OPTIONS -v -s "${PROJECT_BINARY_DIR}/lib/HAL_dict" -rmf "${PROJECT_BINARY_DIR}/lib/libHAL_dict.rootmap")

# add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/HAL_dict.cxx
#                    COMMAND cd ${PROJECT_BINARY_DIR}/include
#                    COMMAND rootcling -f ${CMAKE_CURRENT_SOURCE_DIR}/src/HAL_dict.cxx -p ${CMAKE_CURRENT_SOURCE_DIR}/include -v -s "${PROJECT_BINARY_DIR}/lib/HAL_dict" -rmf "${PROJECT_BINARY_DIR}/lib/libHAL_dict.rootmap"
#                    MAIN_DEPENDENCY ${DICT_INCLUDES})

#  #---Link to library directory
#  set(PROJECT_LINK_LIBS HAL_dict_rdict.pcm)
#  link_directories(${CMAKE_BINARY_DIR}/lib)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/HAL.h" "${PROJECT_BINARY_DIR}/include/HAL.h" COPYONLY)
#  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/HAL/*h" "${CMAKE_BINARY_DIR}/include/HAL/*" COPYONLY)
file(COPY ${DICT_INCLUDES} DESTINATION "${PROJECT_BINARY_DIR}/include/HAL/")

#---Get source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx")
list(APPEND ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/src/HAL_dict.cxx")

#---Create a shared library with geneated dictionary
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
add_library(HAL SHARED ${SOURCES})
target_link_libraries(HAL ${ROOT_LIBRARIES})

# #---Install library
# install(TARGETS HAL DESTINATION "${CMAKE_BINARY_DIR}/lib")

# #---Create  a main program using the library
# add_executable(Main MainEvent.cxx)
# target_link_libraries(Main Event)
