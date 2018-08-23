set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.1.2)
cmake_policy(SET CMP0015 NEW) # Link Directory Pathing
set(CMAKE_DEBUG_POSTFIX _d)

if (APPLE)  
  set(CMAKE_POSITION_INDEPENDENT_CODE False)
else()
  set(CMAKE_POSITION_INDEPENDENT_CODE True)
endif()

set(BUILD_TESTING NO CACHE BOOL "Enable/Disable testing")
set(IS_PYTHON_BUILD NO CACHE BOOL "Enable/Disable PYTHON")
set(IS_CONDA_BUILD NO CACHE BOOL "Set this if you want to make a conda package.")
set(PYTHON_TARGET_VERSION 3.6 CACHE STRING "Version of python to link to for python wrapping.")

if(IS_PYTHON_BUILD AND BUILD_TESTING)
  message(FATAL_ERROR "Cannot build python module when testing is enabled")
endif()


project(xmsextractor C CXX)

if(WIN32)
    if(XMS_BUILD)
        add_definitions(/D _WIN32_WINNT=0x0501)  # Windows XP and higher
        add_definitions(/Zc:wchar_t-)  # Treat wchar_t as built-in type
    else(NOT XMS_BUILD)
        add_definitions(/D BOOST_ALL_NO_LIB)
    endif()
endif()

IF(IS_CONDA_BUILD)
  include(${CMAKE_CURRENT_LIST_DIR}/condabuildinfo.cmake)

ELSE() # If we are not using conda, we are using conan
  # C++11 Support

  # Conan setup
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
  set(EXT_INCLUDE_DIRS ${CONAN_INCLUDE_DIRS})
  set(EXT_LIB_DIRS ${CONAN_LIB_DIRS})
  set(EXT_LIBS ${CONAN_LIBS})
ENDIF(IS_CONDA_BUILD)

message(STATUS "External Include Dirs: ${EXT_INCLUDE_DIRS}")
message(STATUS "External Lib Dirs: ${EXT_LIB_DIRS}")
message(STATUS "Extneral Libs: ${EXT_LIBS}")

if(IS_PYTHON_BUILD)
  # linux and mac builds for conan (on TRAVISCI) use a docker that has python
  # 2.7 as system python. We do not have control over that docker image so we
  # can't change this and it is fine for building conan packages and checking
  # that we don't have errors in the python wrapping. We have conda recipes
  # for building python packages that target other versions of python.
  find_package(PythonLibs ${PYTHON_TARGET_VERSION} EXACT REQUIRED)
  list(APPEND EXT_INCLUDE_DIRS
    ${PYTHON_INCLUDE_DIRS}
  )
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
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${EXT_INCLUDE_DIRS})
link_directories(${EXT_LIB_DIRS})

# Sources
set(xmsextractor_sources
  xmsextractor/extractor/XmUGrid2dDataExtractor.cpp
  xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.cpp
  xmsextractor/ugrid/XmUGridTriangles2d.cpp
)

set(xmsextractor_headers
  xmsextractor/extractor/XmUGrid2dDataExtractor.h
  xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h
  xmsextractor/ugrid/XmUGridTriangles2d.h
)

# Tests
if(BUILD_TESTING)
    add_definitions(-DCXX_TEST -DCXXTEST4)

    list(APPEND xmsextractor_sources
        xmsextractor/extractor/XmUGrid2dDataExtractor.t.h
        xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.t.h
        xmsextractor/ugrid/XmUGridTriangles2d.t.h
    )

#    list(APPEND xmsextractor_sources
#        xmsextractor/testing/TestTools.cpp
#    )

    find_package(CxxTest)
    if(CXXTEST_FOUND)
      include_directories(${CXXTEST_INCLUDE_DIRS})
      enable_testing()

      set(CXXTEST_TESTGEN_ARGS --xunit-printer)
      file(GLOB_RECURSE test_headers ${CMAKE_CURRENT_LIST_DIR}/xmsextractor/*.t.h)
      CXXTEST_ADD_TEST(
        runner runner.cpp ${test_headers}
      )
      target_link_libraries(runner ${PROJECT_NAME})

      if(WIN32)
        set(RUNNERNAME "runner.exe")
      else()
        set(RUNNERNAME "runner")
      endif()
    endif()
endif()

if(IS_PYTHON_BUILD)
  list(APPEND xmsextractor_py
    # add c++ python files
    xmsextractor/python/xmsextractor_py.cpp
	xmsextractor/python/extractor/extractor_py.cpp
	xmsextractor/python/extractor/XmUGrid2dDataExtractor_py.cpp
	xmsextractor/python/extractor/XmUGrid2dPolylineDataExtractor_py.cpp
  )
  list(APPEND xmsextractor_py_headers
    # add python files here
    xmsextractor/python/extractor/extractor_py.h
  )
endif()

# Static library
add_library(${PROJECT_NAME} STATIC
  ${xmsextractor_sources} ${xmsextractor_headers}
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

#Pybind11
if(IS_PYTHON_BUILD)

  message(STATUS "PYTHON_INCLUDE: ${PYTHON_INCLUDE_DIRS}")
  message(STATUS "PYTHON_LIBS: ${PYTHON_LIBRARIES}")

  pybind11_add_module(xmsextractor_py
    ${xmsextractor_py} ${xmsextractor_py_headers}
  )
  target_include_directories(xmsextractor_py
      PRIVATE
        ${EXT_INCLUDE_DIRS}
    )
  target_link_libraries(xmsextractor_py
    PRIVATE
      ${EXT_LIBS}
      ${PROJECT_NAME}
  )
  set_target_properties(xmsextractor_py PROPERTIES
    LINKER_LANGUAGE CXX
  )

  execute_process(COMMAND python -c "import os; print('${PYTHON_SITE_PACKAGES}'.replace('${CONDA_PREFIX}' + os.sep, ''))" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES_TRIMMED OUTPUT_STRIP_TRAILING_WHITESPACE)

  # Install recipe
  install(
    TARGETS xmsextractor_py
    ARCHIVE DESTINATION ${PYTHON_SITE_PACKAGES_TRIMMED}
    LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES_TRIMMED}
  )
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
