//------------------------------------------------------------------------------
/// \file
/// \brief
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------
#include <pybind11/pybind11.h>

#include <xmsextractor/python/extractor/extractor_py.h>

#include <sstream>

#include <xmscore/misc/StringUtil.h>
#include <xmscore/python/misc/PyUtils.h>

#include <xmsextractor/python/extractor/extractor_py.h>
#include <xmsextractor/extractor/XmUGrid2dDataExtractor.h>
#include <xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------

void initExtractor(py::module &m) {
    initXmUGrid2dDataExtractor(m);
    initXmUGrid2dPolylineDataExtractor(m);
}
// ---------------------------------------------------------------------------
/// \brief creates a python __repr__ string from an XmUGrid2dDataExtractor class
/// \param[in] a_extractor: the data extractor
/// \return string for the __repr__ python member
// ---------------------------------------------------------------------------
std::string PyReprStringFromXmUGrid2dDataExtractor(const xms::XmUGrid2dDataExtractor& a_extractor)
{
  std::stringstream ss;
  ss << "ugrid: <class: UGrid>\n";
  ss << "scalars: " << xms::StringFromVecFlt(a_extractor.GetScalars());
  std::string dLoc[3] = {"points", "cells", "unknown"};
  ss << "scalar_locations: " << dLoc[(int)a_extractor.GetScalarLocation()] << "\n";
  ss << "extraction_locations: " << xms::StringFromVecPt3d(a_extractor.GetExtractLocations());
  std::string offOn[2] = {"False", "True"};
  ss << "use_idw_for_point_data: " << offOn[(int)a_extractor.GetUseIdwForPointData()] << "\n";
  ss << "no_data_value: " << xms::STRstd(a_extractor.GetNoDataValue()) << "\n";
  return ss.str();
} // PyReprStringFromXmUGrid2dDataExtractor
// ---------------------------------------------------------------------------
/// \brief creates a python __repr__ string from an XmUGrid2dPolylineDataExtractor class
/// \param[in] a_extractor: the data extractor
/// \return string for the __repr__ python member
// ---------------------------------------------------------------------------
std::string PyReprStringFromXmUGrid2dPolylineDataExtractor(
  const xms::XmUGrid2dPolylineDataExtractor& a_extractor)
{
  std::stringstream ss;
  ss << "ugrid: <class: UGrid>\n";
  ss << "scalars: " << xms::StringFromVecFlt(a_extractor.GetScalars());
  std::string dLoc[3] = {"points", "cells", "unknown"};
  ss << "scalar_locations: " << dLoc[(int)a_extractor.GetScalarLocation()] << "\n";
  ss << "extraction_locations: " << xms::StringFromVecPt3d(a_extractor.GetExtractLocations());
  std::string offOn[2] = {"False", "True"};
  ss << "use_idw_for_point_data: " << offOn[(int)a_extractor.GetUseIdwForPointData()] << "\n";
  ss << "no_data_value: " << xms::STRstd(a_extractor.GetNoDataValue()) << "\n";
  return ss.str();
} // PyReprStringFromXmUGrid2dDataExtractor

