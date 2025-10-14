//------------------------------------------------------------------------------
/// \file
/// \brief
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------
#include <pybind11/pybind11.h>

#include <xmsextractor/python/ugrid/ugrid_py.h>

#include <sstream>

#include <xmscore/misc/StringUtil.h>
#include <xmscore/python/misc/PyUtils.h>

#include <xmsextractor/python/extractor/extractor_py.h>
#include <xmsextractor/extractor/XmUGrid2dDataExtractor.h>
#include <xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------

void initUGrid(py::module& m)
{
  initXmUGridTriangles2d(m);
}
