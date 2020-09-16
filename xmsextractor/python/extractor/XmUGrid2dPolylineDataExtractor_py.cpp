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
#include <xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h>
#include <xmsextractor/python/extractor/extractor_py.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------
PYBIND11_DECLARE_HOLDER_TYPE(T, boost::shared_ptr<T>);

void initXmUGrid2dPolylineDataExtractor(py::module &m) {
    py::class_<xms::XmUGrid2dPolylineDataExtractor, BSHP<xms::XmUGrid2dPolylineDataExtractor>>
        extractor(m, "UGrid2dPolylineDataExtractor");

    // -------------------------------------------------------------------------
    // function: init
    // -------------------------------------------------------------------------
    extractor.def(py::init([](std::shared_ptr<xms::XmUGrid> ugrid, xms::DataLocationEnum scalar_location) {
            BSHP<xms::XmUGrid2dPolylineDataExtractor> rval(xms::XmUGrid2dPolylineDataExtractor::New(ugrid, scalar_location));
            rval->SetNoDataValue(std::numeric_limits<float>::quiet_NaN());
            return rval;
        }),py::arg("ugrid"),py::arg("scalar_location"));

    // -------------------------------------------------------------------------
    // function: GetDataExtractor
    // -------------------------------------------------------------------------
    extractor.def("GetDataExtractor", [](xms::XmUGrid2dPolylineDataExtractor &self) {
      BSHP<xms::XmUGrid2dDataExtractor> rval(self.GetDataExtractor());
      return rval;
    });

    // -------------------------------------------------------------------------
    // function: SetGridScalars
    // -------------------------------------------------------------------------
    extractor.def("SetGridScalars", [](xms::XmUGrid2dPolylineDataExtractor &self,
                     py::iterable scalars, py::iterable activity, xms::DataLocationEnum activity_type) {
      boost::shared_ptr<xms::VecFlt> _scalars = xms::VecFltFromPyIter(scalars);
      xms::DynBitset _activity = xms::DynamicBitsetFromPyIter(activity);
      self.SetGridScalars(*_scalars, _activity, activity_type);
    }, py::arg("scalars"),py::arg("activity"), py::arg("activity_type"));

    // -------------------------------------------------------------------------
    // function: SetPolyline
    // -------------------------------------------------------------------------
    extractor.def("SetPolyline", [](xms::XmUGrid2dPolylineDataExtractor &self, py::iterable polyline) {
      boost::shared_ptr<xms::VecPt3d> _polyline = xms::VecPt3dFromPyIter(polyline);
      self.SetPolyline(*_polyline);
    },py::arg("polyline"));

    // -------------------------------------------------------------------------
    // function: GetExtractLocations
    // -------------------------------------------------------------------------
    extractor.def("GetExtractLocations", [](xms::XmUGrid2dPolylineDataExtractor &self) -> py::iterable {
      xms::VecPt3d locations = self.GetExtractLocations();
      return xms::PyIterFromVecPt3d(locations);
    });

    // -------------------------------------------------------------------------
    // function: GetCellIndexes
    // -------------------------------------------------------------------------
    extractor.def("GetCellIndexes", [](xms::XmUGrid2dPolylineDataExtractor &self) -> py::iterable {
      return xms::PyIterFromVecInt(self.GetCellIndexes());
    });

    // -------------------------------------------------------------------------
    // function: ExtractData
    // -------------------------------------------------------------------------
    extractor.def("ExtractData", [](xms::XmUGrid2dPolylineDataExtractor &self) -> py::iterable {
      xms::VecFlt outData;
      self.ExtractData(outData);
      return xms::PyIterFromVecFlt(outData);
    });

    // -------------------------------------------------------------------------
    // function: ComputeLocationsAndExtractData
    // -------------------------------------------------------------------------
    extractor.def("ComputeLocationsAndExtractData", [](xms::XmUGrid2dPolylineDataExtractor &self,
                     py::iterable polyline) -> py::iterable {
      boost::shared_ptr<xms::VecPt3d> line(xms::VecPt3dFromPyIter(polyline));
      xms::VecFlt extracted_data;
      xms::VecPt3d extracted_locations;
      self.ComputeLocationsAndExtractData(*line, extracted_data, extracted_locations);
      return py::make_tuple(extracted_data, extracted_locations);
    });

    // -------------------------------------------------------------------------
    // function: SetUseIdwForPointData
    // -------------------------------------------------------------------------
    extractor.def("SetUseIdwForPointData", &xms::XmUGrid2dPolylineDataExtractor::SetUseIdwForPointData, py::arg("use_idw"));

    // -------------------------------------------------------------------------
    // function: GetUseIdwForPointData
    // -------------------------------------------------------------------------
    extractor.def("GetUseIdwForPointData", &xms::XmUGrid2dPolylineDataExtractor::GetUseIdwForPointData);

    // -------------------------------------------------------------------------
    // function: SetNoDataValue
    // -------------------------------------------------------------------------
    extractor.def("SetNoDataValue", &xms::XmUGrid2dPolylineDataExtractor::SetNoDataValue, py::arg("no_data_value"));

    // -------------------------------------------------------------------------
    // function: GetNoDataValue
    // -------------------------------------------------------------------------
    extractor.def("GetNoDataValue", &xms::XmUGrid2dPolylineDataExtractor::GetNoDataValue);
}
