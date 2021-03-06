#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief initializer functions for members of ugrid python module.
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------
#include <pybind11/pybind11.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

namespace xms {
 class XmUGrid2dDataExtractor;
 class XmUGrid2dPolylineDataExtractor;
}; // namepace xms

//----- Function declarations --------------------------------------------------
void initExtractor(py::module &);

void initXmUGrid2dDataExtractor(py::module &);
void initXmUGrid2dPolylineDataExtractor(py::module &);

std::string PyReprStringFromXmUGrid2dDataExtractor(const xms::XmUGrid2dDataExtractor& a_);
std::string PyReprStringFromXmUGrid2dPolylineDataExtractor(
  const xms::XmUGrid2dPolylineDataExtractor& a_extractor);
