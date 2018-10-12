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
#include <xmsextractor/extractor/XmUGrid2dDataExtractor.h>
#include <xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h>
#include <xmsgrid/ugrid/XmUGrid.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------
PYBIND11_DECLARE_HOLDER_TYPE(T, boost::shared_ptr<T>);

void initXmUGrid2dPolylineDataExtractor(py::module &m) {
    py::class_<xms::XmUGrid2dPolylineDataExtractor, 
        boost::shared_ptr<xms::XmUGrid2dPolylineDataExtractor>> 
        extractor(m, "XmUGrid2dPolylineDataExtractor");
    // -------------------------------------------------------------------------
    // function: init
    // -------------------------------------------------------------------------
    const char* init_doc = R"pydoc(
        Create a new XmUGrid2dPolylineDataExtractor

        Args:
            a_ugrid (XmUGrid): The UGrid
            a_scalarLocation (data_location_enum): Location
    )pydoc";
    extractor.def(py::init([](boost::shared_ptr<xms::XmUGrid> a_ugrid, 
        xms::DataLocationEnum a_scalarLocation) {
            return boost::shared_ptr<xms::XmUGrid2dPolylineDataExtractor>
                (xms::XmUGrid2dPolylineDataExtractor::New(
                    a_ugrid, a_scalarLocation));
        }),init_doc,py::arg("a_ugrid"),py::arg("a_scalarLocation"));
    // -------------------------------------------------------------------------
    // function: set_grid_scalars
    // -------------------------------------------------------------------------
    const char* set_grid_scalars_doc = R"pydoc(
        Set the grid scalars

        Args:
            a_scalars (iterable): The cell or point scalars.
            a_activity (iterable): The activity of the points or cells.
            a_activityType (data_location_enum): The location of the activity 
                (points or cells).
    )pydoc";
    extractor.def("set_grid_scalars", 
        [](xms::XmUGrid2dPolylineDataExtractor &self, py::iterable a_scalars,
                py::iterable a_activity, xms::DataLocationEnum a_activityType) {
            boost::shared_ptr<xms::VecFlt> scalars = 
                xms::VecFltFromPyIter(a_scalars);
            xms::DynBitset activity = xms::DynamicBitsetFromPyIter(a_activity);
            self.SetGridScalars(*scalars, activity, a_activityType);
        },set_grid_scalars_doc, py::arg("a_scalars"),py::arg("a_activity"),
            py::arg("a_activityType"));
    // -------------------------------------------------------------------------
    // function: set_polyline
    // -------------------------------------------------------------------------
    const char* set_polyline = R"pydoc(
        Set a polyline

        Args:
            a_polyline (iterable): The polyline.
    )pydoc";
    extractor.def("set_polyline", 
        [](xms::XmUGrid2dPolylineDataExtractor &self, py::iterable a_polyline) {
            boost::shared_ptr<xms::VecPt3d> polyline = 
                xms::VecPt3dFromPyIter(a_polyline);
            self.SetPolyline(*polyline);
        },set_polyline,py::arg("a_polyline"));
    // -------------------------------------------------------------------------
    // function: get_extract_locations
    // -------------------------------------------------------------------------
    const char* get_extract_locations_doc = R"pydoc(
        Get the extract locations.

        Returns:
            iterable: The locations.
    )pydoc";
    extractor.def("get_extract_locations", 
        [](xms::XmUGrid2dPolylineDataExtractor &self) -> py::iterable {
            xms::VecPt3d locations = self.GetExtractLocations();
            return xms::PyIterFromVecPt3d(locations);
    }, get_extract_locations_doc);
    // -------------------------------------------------------------------------
    // function: extract_data
    // -------------------------------------------------------------------------
    const char* extract_data_doc = R"pydoc(
        Extracts the data from the Grid

        Returns:
            iterable: The extracted values interpolated from the scalar values.
    )pydoc";
    extractor.def("extract_data", 
        [](xms::XmUGrid2dPolylineDataExtractor &self) -> py::iterable {
            xms::VecFlt outData;
            self.ExtractData(outData);
            return xms::PyIterFromVecFlt(outData);
        },extract_data_doc);
    // -------------------------------------------------------------------------
    // function: set_use_idw_for_point_data
    // -------------------------------------------------------------------------
    const char* set_use_idw_for_point_data_doc = R"pydoc(
        Set to use IDW to calculate point scalar values from cell scalars.

        Args:
            a_useIdw (bool): Whether to turn IDW on or off.
    )pydoc";
    extractor.def("set_use_idw_for_point_data", 
        &xms::XmUGrid2dPolylineDataExtractor::SetUseIdwForPointData,
          set_use_idw_for_point_data_doc, py::arg("a_useIdw"));
    // -------------------------------------------------------------------------
    // function: set_no_data_value
    // -------------------------------------------------------------------------
    const char* set_no_data_value_doc = R"pydoc(
        Set the no data value

        Args:
            no_data_value (float): The no data value
    )pydoc";
    extractor.def("set_no_data_value", 
        &xms::XmUGrid2dPolylineDataExtractor::SetNoDataValue,
      set_no_data_value_doc,py::arg("no_data_value"));
}
