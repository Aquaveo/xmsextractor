cmake_minimum_required(VERSION 3.1.2)
cmake_policy(SET CMP0015 NEW) # Link Directory Pathing
set(CMAKE_DEBUG_POSTFIX _d)

if (APPLE OR WIN32)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 11)
endif()

if (APPLE)
  set(CMAKE_POSITION_INDEPENDENT_CODE False)
else()
  set(CMAKE_POSITION_INDEPENDENT_CODE True)
endif()

project(xmsextractorlib C CXX)

set(BUILD_TESTING NO CACHE BOOL "Enable/Disable testing")
set(IS_CONDA_BUILD NO CACHE BOOL "Set this if you want to make a conda package.")
set(PYTHON_TARGET_VERSION 3.6 CACHE STRING "Version of python to link to for python wrapping.")
set(CONDA_PREFIX "" CACHE PATH "Path to the conda environment used to build.")
set(IS_PYTHON_BUILD NO CACHE BOOL "Set this if you want to build the python bindings.")
set(XMS_TEST_PATH ${PROJECT_SOURCE_DIR}/test_files/ CACHE PATH "Path to test files for testing")
set(XMS_VERSION "\"99.99.99\"" CACHE STRING "Library Version")

add_definitions(-DXMS_VERSION=\"${XMS_VERSION}\")

if(IS_CONDA_BUILD)
  include(${CMAKE_CURRENT_LIST_DIR}/condabuildinfo.cmake)
else() # If we are not using conda, we are using conan
  # Conan setup
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
  set(EXT_INCLUDE_DIRS ${CONAN_INCLUDE_DIRS})
  set(EXT_LIB_DIRS ${CONAN_LIB_DIRS})
  set(EXT_LIBS ${CONAN_LIBS})
endif(IS_CONDA_BUILD)

if(WIN32)
    if(USE_NATIVE_WCHAR_T)
        message("Treating wchar_t as a built-in type.")
        add_definitions(/Zc:wchar_t)  # Treat wchar_t as built-in type
    else()
        message("Treating wchar_t as a typedef.")
        add_definitions(/Zc:wchar_t-)  # Treat wchar_t as typedef
    endif()

    add_definitions(/D BOOST_ALL_NO_LIB)
endif()

if(IS_PYTHON_BUILD)

    # Pybind11 module
    if(IS_CONDA_BUILD)
        include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11Targets.cmake")
        include("${CONDA_PREFIX}/share/cmake/pybind11/FindPythonLibsNew.cmake")
        include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11Config.cmake")
        include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11ConfigVersion.cmake")
        include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11Tools.cmake")

    else()
        include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11Targets.cmake")
        include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/FindPythonLibsNew.cmake")
        include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11Config.cmake")
        include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11ConfigVersion.cmake")
        include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11Tools.cmake")
    endif()

    list(APPEND EXT_INCLUDE_DIRS
        ${pybind11_INCLUDE_DIR}
    )

    find_package(PythonLibsNew ${PYTHON_TARGET_VERSION} EXACT REQUIRED)

    message("PYTHON_INCLUDE: ${PYTHON_INCLUDE_DIRS}")
    message("PYTHON_LIBS: ${PYTHON_LIBRARIES}")

    list(APPEND EXT_INCLUDE_DIRS
      ${PYTHON_INCLUDE_DIRS}
    )
endif()

message("External Include Dirs: ${EXT_INCLUDE_DIRS}")
message("External Lib Dirs: ${EXT_LIB_DIRS}")
message("Extneral Libs: ${EXT_LIBS}")

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${EXT_INCLUDE_DIRS})
link_directories(${EXT_LIB_DIRS})

# Static library sources
set(xmsextractor_sources
  xmsextractor/extractor/XmUGrid2dDataExtractor.cpp
  xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.cpp
  xmsextractor/ugrid/XmUGridTriangles2d.cpp
  xmsextractor/ugrid/XmUGridTriangulator.cpp
  xmsextractor/ugrid/XmUGridTriangulatorBase.cpp
)

set(xmsextractor_headers
  xmsextractor/extractor/XmUGrid2dDataExtractor.h
  xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h
  xmsextractor/ugrid/XmUGridTriangles2d.h
  xmsextractor/ugrid/XmUGridTriangulator.h
  xmsextractor/ugrid/XmUGridTriangulatorBase.h
)

# Pybind11 sources
set(xmsextractor_py
  xmsextractor/python/xmsextractor_py.cpp
  #extractor
  xmsextractor/python/extractor/extractor_py.cpp
  xmsextractor/python/extractor/XmUGrid2dDataExtractor_py.cpp
  xmsextractor/python/extractor/XmUGrid2dPolylineDataExtractor_py.cpp
)

set(xmsextractor_py_headers
  xmsextractor/python/extractor/extractor_py.h
)

# Tests
if (BUILD_TESTING)
 add_definitions(-DXMS_TEST_PATH="${XMS_TEST_PATH}/")
    add_definitions(-DCXX_TEST -DCXXTEST4)

 list(APPEND xmsextractor_sources
   xmsextractor/extractor/XmUGrid2dDataExtractor.t.h
   xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.t.h
   xmsextractor/ugrid/XmUGridTriangles2d.t.h
 )

 find_package(CxxTest)
 if(CXXTEST_FOUND)
   include_directories(${CXXTEST_INCLUDE_DIR})
   enable_testing()

   set(CXXTEST_TESTGEN_ARGS --xunit-printer --have-eh)
   file(GLOB_RECURSE test_headers ${CMAKE_CURRENT_LIST_DIR}/xmsextractor/*.t.h)
      CXXTEST_ADD_TEST(
        runner runner.cpp ${test_headers}
      )
      target_link_libraries(runner ${PROJECT_NAME})
 endif()
endif ()

# Static library
add_library(${PROJECT_NAME} STATIC
  ${xmsextractor_sources} ${xmsextractor_headers}
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/>
    ${Boost_INCLUDE_DIR}
 )
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME}
  ${EXT_LIBS}
)
target_link_libraries(${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
)
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} rt)
endif()

if(IS_PYTHON_BUILD)
    pybind11_add_module(_xmsextractor
      ${xmsextractor_py} ${xmsextractor_py_headers}
    )
    target_include_directories(_xmsextractor
      PRIVATE
          ${EXT_LIBS}
          ${PYTHON_INCLUDE_DIRS}
    )
    target_link_libraries(_xmsextractor
    PRIVATE
      ${EXT_LIBS}
      ${PROJECT_NAME}
  )
    set_target_properties(_xmsextractor PROPERTIES
    LINKER_LANGUAGE CXX
  )

    # Install recipe
    install(
      TARGETS _xmsextractor
      ARCHIVE DESTINATION "_package/xms/extractor"
      LIBRARY DESTINATION "_package/xms/extractor"
    )

    install(DIRECTORY "_package/" DESTINATION "_package"
             FILES_MATCHING PATTERN "*.py"
             PATTERN "_package/tests" EXCLUDE)

endif()

# Disable Optimization for GCC6 for now... Report issue to gcc and pybind
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND IS_PYTHON_BUILD)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
    Message("Disabling Optimization for GCC 6...")
    STRING(REPLACE "-O3" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    STRING(REPLACE "-O3" "" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
  endif()
endif()

# Install recipe
install(
  TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION "lib"
  LIBRARY DESTINATION "lib"
)
foreach (header IN LISTS xmsextractor_headers xmsextractor_py_headers)
  get_filename_component(subdir "${header}" DIRECTORY)
  install(
    FILES "${header}"
    DESTINATION "include/${subdir}"
  )
endforeach ()
