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
    const char* extractor_doc = R"pydoc(
        The misc module of the xmsextractor python library contains classes and
        functions that are shared between all of the xms family of libraries.
        These functions included in this class include polyline scalar 
        extraction from any XmUGrid, location scalar extraction from any 
        XmUGrid, polyline extraction locations can be obtained, and xmUGrids may 
        have scalars assigned to either the points or to the cells.
    )pydoc";
    // Extractor module
    py::module modExtractor = m.def_submodule("extractor", extractor_doc);
    initExtractor(modExtractor);
}

