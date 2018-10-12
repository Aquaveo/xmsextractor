//------------------------------------------------------------------------------
/// \file
/// \brief root module for xmsgrid Python bindings.
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------
#include <pybind11/pybind11.h>
#include <xmsextractor/python/extractor/extractor_py.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------
std::string version() {
    return "1.0.0";
}


//------ Primary Module --------------------------------------------------------
PYBIND11_MODULE(xmsextractor_py, m) {
    m.doc() = "Python bindings for xmsextractor"; // optional module docstring
    m.def("version", &version,
          "Get current version of xmsextractor Python bindings.");

    // Extractor module
    py::module modExtractor = m.def_submodule("extractor");
    initExtractor(modExtractor);
}

