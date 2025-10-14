//------------------------------------------------------------------------------
/// \file
/// \brief
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <boost/shared_ptr.hpp>

#include <xmscore/python/misc/PyUtils.h>
#include <xmscore/misc/DynBitset.h>
#include <xmscore/stl/vector.h>
#include <xmsgrid/ugrid/XmUGrid.h>

#include <xmsextractor/extractor/XmUGrid2dDataExtractor.h>
#include <xmsextractor/python/extractor/extractor_py.h>
#include <xmsextractor/ugrid/XmUGridTriangles2d.h>
#include <xmsgrid/ugrid/XmUGrid.h>
#include <xmsextractor/ugrid/XmUGridTriangles2d.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------
PYBIND11_DECLARE_HOLDER_TYPE(T, boost::shared_ptr<T>);


void initXmUGridTriangles2d(py::module& m)
{
  // -------------------------------------------------------------------------
  // function: triangulate_ugrid
  // -------------------------------------------------------------------------
  m.def("triangulate_ugrid", &xms::XmTriangulateUGrid);
}
