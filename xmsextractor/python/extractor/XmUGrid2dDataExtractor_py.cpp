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

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------

void initXmUGrid2dDataExtractor(py::module &m) {
    py::class_<xms::XmUGrid2dDataExtractor, std::shared_ptr<xms::XmUGrid2dDataExtractor>> extractor(m, "UGrid2dDataExtractor");

    // -------------------------------------------------------------------------
    // function: init
    // -------------------------------------------------------------------------
    extractor.def(py::init([](std::shared_ptr<xms::XmUGrid> ugrid) -> std::shared_ptr<xms::XmUGrid2dDataExtractor> {
      std::shared_ptr<xms::XmUGrid2dDataExtractor> rval(xms::XmUGrid2dDataExtractor::New(ugrid));
      rval->SetNoDataValue(std::numeric_limits<float>::quiet_NaN());
      return rval;
    }), py::arg("ugrid"));

    // -------------------------------------------------------------------------
    // function: SetGridPointScalars
    // -------------------------------------------------------------------------
    extractor.def("SetGridPointScalars", [](xms::XmUGrid2dDataExtractor &self, py::iterable a_pointScalars,
                     py::iterable a_activity, xms::DataLocationEnum a_activityType) {
      boost::shared_ptr<xms::VecFlt> pointScalars = xms::VecFltFromPyIter(a_pointScalars);
      xms::DynBitset activity = xms::DynamicBitsetFromPyIter(a_activity);
      self.SetGridPointScalars(*pointScalars, activity, a_activityType);
    }, py::arg("point_scalars"), py::arg("activity"), py::arg("activity_type"));

    // -------------------------------------------------------------------------
    // function: SetGridCellScalars
    // -------------------------------------------------------------------------
    extractor.def("SetGridCellScalars", [](xms::XmUGrid2dDataExtractor &self, py::iterable a_cellScalars,
                     py::iterable a_activity, xms::DataLocationEnum a_activityType) {
      boost::shared_ptr<xms::VecFlt> cellScalars = xms::VecFltFromPyIter(a_cellScalars);
      xms::DynBitset activity = xms::DynamicBitsetFromPyIter(a_activity);
      self.SetGridCellScalars(*cellScalars, activity, a_activityType);
    }, py::arg("point_scalars"), py::arg("activity") ,py::arg("activity_type"));

    // -------------------------------------------------------------------------
    // function: SetExtractLocations
    // -------------------------------------------------------------------------
    extractor.def("SetExtractLocations", [](xms::XmUGrid2dDataExtractor &self, py::iterable locations) {
      boost::shared_ptr<xms::VecPt3d> _locations = xms::VecPt3dFromPyIter(locations);
      self.SetExtractLocations(*_locations);
    }, py::arg("locations"));

    // -------------------------------------------------------------------------
    // function: SetExtractLocations
    // -------------------------------------------------------------------------
    extractor.def("GetExtractLocations", [](xms::XmUGrid2dDataExtractor &self) {
      return xms::PyIterFromVecPt3d(self.GetExtractLocations());
    });

    // -------------------------------------------------------------------------
    // function: ExtractData
    // -------------------------------------------------------------------------
    extractor.def("ExtractData", [](xms::XmUGrid2dDataExtractor &self) -> py::iterable {
      xms::VecFlt outData;
      self.ExtractData(outData);
      return xms::PyIterFromVecFlt(outData);
    });

    // -------------------------------------------------------------------------
    // function: ExtractAtLocation
    // -------------------------------------------------------------------------
    extractor.def("ExtractAtLocation", [](xms::XmUGrid2dDataExtractor &self, py::iterable location) -> float {
      xms::Pt3d _location = xms::Pt3dFromPyIter(location);
      return self.ExtractAtLocation(_location);
    }, py::arg("location"));

    // -------------------------------------------------------------------------
    // function: SetUseIdwForPointData
    // -------------------------------------------------------------------------
    extractor.def("SetUseIdwForPointData", &xms::XmUGrid2dDataExtractor::SetUseIdwForPointData, py::arg("use_idw"));

    // -------------------------------------------------------------------------
    // function: GetUseIdwForPointData
    // -------------------------------------------------------------------------
    extractor.def("GetUseIdwForPointData", &xms::XmUGrid2dDataExtractor::GetUseIdwForPointData);

    // -------------------------------------------------------------------------
    // function: SetNoDataValue
    // -------------------------------------------------------------------------
    extractor.def("SetNoDataValue", &xms::XmUGrid2dDataExtractor::SetNoDataValue, py::arg("no_data_value"));

    // -------------------------------------------------------------------------
    // function: GetNoDataValue
    // -------------------------------------------------------------------------
    extractor.def("GetNoDataValue", &xms::XmUGrid2dDataExtractor::GetNoDataValue);

    // DataLocationEnum
    py::enum_<xms::DataLocationEnum>(m, "data_location_enum",
                    "data_location_enum location mapping for dataset values")
        .value("LOC_POINTS", xms::LOC_POINTS)
        .value("LOC_CELLS", xms::LOC_CELLS)
        .value("LOC_UNKNOWN", xms::LOC_UNKNOWN);
}
