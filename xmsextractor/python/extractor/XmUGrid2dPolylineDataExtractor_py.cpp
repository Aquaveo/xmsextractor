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
    py::class_<xms::XmUGrid2dPolylineDataExtractor, 
        boost::shared_ptr<xms::XmUGrid2dPolylineDataExtractor>> 
        extractor(m, "UGrid2dPolylineDataExtractor");
    // -------------------------------------------------------------------------
    // function: init
    // -------------------------------------------------------------------------
    const char* init_doc = R"pydoc(
        Create a new XmUGrid2dPolylineDataExtractor

        Args:
            ugrid (XmUGrid): The UGrid
            scalar_location (data_location_enum): Location
    )pydoc";
    extractor.def(py::init([](boost::shared_ptr<xms::XmUGrid> ugrid,
        xms::DataLocationEnum scalar_location) {
            return boost::shared_ptr<xms::XmUGrid2dPolylineDataExtractor>
                (xms::XmUGrid2dPolylineDataExtractor::New(
                    ugrid, scalar_location));
        }),init_doc,py::arg("ugrid"),py::arg("scalar_location"));
    // ---------------------------------------------------------------------------
    // attribute: __repr__
    // ---------------------------------------------------------------------------
    extractor.def("__repr__",[](const xms::XmUGrid2dPolylineDataExtractor &extractor)
           {
             return PyReprStringFromXmUGrid2dPolylineDataExtractor(extractor);
           }
    );
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
        [](xms::XmUGrid2dPolylineDataExtractor &self, py::iterable scalars,
                py::iterable activity, xms::DataLocationEnum activity_type) {
            boost::shared_ptr<xms::VecFlt> _scalars =
                xms::VecFltFromPyIter(scalars);
            xms::DynBitset _activity = xms::DynamicBitsetFromPyIter(activity);
            self.SetGridScalars(*_scalars, _activity, activity_type);
        },set_grid_scalars_doc, py::arg("scalars"),py::arg("activity"),
            py::arg("activity_type"));
    // -------------------------------------------------------------------------
    // function: set_polyline
    // -------------------------------------------------------------------------
    const char* set_polyline = R"pydoc(
        Set a polyline

        Args:
            a_polyline (iterable): The polyline.
    )pydoc";
    extractor.def("set_polyline", 
        [](xms::XmUGrid2dPolylineDataExtractor &self, py::iterable polyline) {
            boost::shared_ptr<xms::VecPt3d> _polyline =
                xms::VecPt3dFromPyIter(polyline);
            self.SetPolyline(*_polyline);
        },set_polyline,py::arg("polyline"));
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
            use_idw (bool): Whether to turn IDW on or off.
    )pydoc";
    extractor.def("set_use_idw_for_point_data", 
        &xms::XmUGrid2dPolylineDataExtractor::SetUseIdwForPointData,
          set_use_idw_for_point_data_doc, py::arg("use_idw"));
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
