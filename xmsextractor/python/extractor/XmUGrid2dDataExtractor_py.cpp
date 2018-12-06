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
PYBIND11_DECLARE_HOLDER_TYPE(T, boost::shared_ptr<T>);

void initXmUGrid2dDataExtractor(py::module &m) {
    py::class_<xms::XmUGrid2dDataExtractor, 
        boost::shared_ptr<xms::XmUGrid2dDataExtractor>> 
        extractor(m, "UGrid2dDataExtractor");
    // -------------------------------------------------------------------------
    // function: init
    // -------------------------------------------------------------------------
    const char* init_doc = R"pydoc(
        Create a new XmUGrid2dDataExtractor.

        Args:
            ugrid (XmUGrid): A ugrid
    )pydoc";
    extractor.def(py::init([](boost::shared_ptr<xms::XmUGrid> ugrid) {
            return boost::shared_ptr<xms::XmUGrid2dDataExtractor>
                (xms::XmUGrid2dDataExtractor::New(ugrid));
            }), init_doc, py::arg("ugrid"));
    // -------------------------------------------------------------------------
    // function: init
    // -------------------------------------------------------------------------
    const char* init_doc2 = R"pydoc(
        Copy construct a new XmUGrid2dDataExtractor.

        Args:
            extractor (XmUGrid2dDataExtractor): extractor to copy
    )pydoc";
    extractor.def(py::init(
        [](boost::shared_ptr<xms::XmUGrid2dDataExtractor> extractor) {
            return boost::shared_ptr<xms::XmUGrid2dDataExtractor>
                (xms::XmUGrid2dDataExtractor::New(extractor));
        }), init_doc2, py::arg("extractor"));
    // ---------------------------------------------------------------------------
    // attribute: __repr__
    // ---------------------------------------------------------------------------
    extractor.def("__repr__",[](const xms::XmUGrid2dDataExtractor &extractor)
           {
             return PyReprStringFromXmUGrid2dDataExtractor(extractor);
           }
    );
    // -------------------------------------------------------------------------
    // function: set_grid_point_scalars
    // -------------------------------------------------------------------------
    const char* set_grid_point_scalars_doc = R"pydoc(
        Set the point scalar values

        Args:
            point_scalars (iterable): The point scalars.
            activity (iterable): The activity of the cells.
            activity_type (data_location_enum): The location at which the data 
            is currently stored.
    )pydoc";
    extractor.def("set_grid_point_scalars", 
        [](xms::XmUGrid2dDataExtractor &self, py::iterable a_pointScalars,
                                          py::iterable a_activity,
                                          xms::DataLocationEnum a_activityType) {
            boost::shared_ptr<xms::VecFlt> pointScalars =
                 xms::VecFltFromPyIter(a_pointScalars);
            xms::DynBitset activity = xms::DynamicBitsetFromPyIter(a_activity);
            self.SetGridPointScalars(*pointScalars, activity, a_activityType);
        },set_grid_point_scalars_doc,py::arg("point_scalars"),
        py::arg("activity"),py::arg("activity_type"));
    // -------------------------------------------------------------------------
    // function: set_grid_cell_scalars
    // -------------------------------------------------------------------------
    const char* set_grid_cell_scalars_doc = R"pydoc(
        Set the point scalar values

        Args:
            point_scalars (iterable): The point scalars.
            activity (iterable): The activity of the cells.
            activity_type (data_location_enum): The location at which the data 
            is currently stored.
    )pydoc";
    extractor.def("set_grid_cell_scalars", [](xms::XmUGrid2dDataExtractor &self,
        py::iterable a_cellScalars,
                                        py::iterable a_activity,
                                        xms::DataLocationEnum a_activityType) {
            boost::shared_ptr<xms::VecFlt> cellScalars = 
                xms::VecFltFromPyIter(a_cellScalars);
            xms::DynBitset activity = xms::DynamicBitsetFromPyIter(a_activity);
            self.SetGridCellScalars(*cellScalars, activity, a_activityType);
        },set_grid_cell_scalars_doc,py::arg("point_scalars"),py::arg("activity")
        ,py::arg("activity_type"));
    // -------------------------------------------------------------------------
    // function: set_extract_locations
    // -------------------------------------------------------------------------
    const char* set_extract_locations_doc = R"pydoc(
        Set extraction locations.

        Args:
            locations (iterable): The locations.
    )pydoc";
    extractor.def("set_extract_locations", [](xms::XmUGrid2dDataExtractor &self,
        py::iterable locations) {
            boost::shared_ptr<xms::VecPt3d> _locations =
                xms::VecPt3dFromPyIter(locations);
            self.SetExtractLocations(*_locations);
        }, set_extract_locations_doc, py::arg("locations"));
    // -------------------------------------------------------------------------
    // function: get_extract_locations
    // -------------------------------------------------------------------------
    const char* get_extract_locations_doc = R"pydoc(
        Get extraction locations.

        Returns:
            iterable: The locations.
    )pydoc";
    extractor.def("get_extract_locations", 
        [](xms::XmUGrid2dDataExtractor &self) -> py::iterable {
            xms::VecPt3d locations = self.GetExtractLocations();
            return xms::PyIterFromVecPt3d(locations);
        }, get_extract_locations_doc);
    // -------------------------------------------------------------------------
    // function: extract_data
    // -------------------------------------------------------------------------
    const char* extract_data_doc = R"pydoc(
        Extract interpolated data for the previously set locations.

        Returns:
            iterable: The interpolated scalars.
    )pydoc";
    extractor.def("extract_data", 
        [](xms::XmUGrid2dDataExtractor &self) -> py::iterable {
            xms::VecFlt outData;
            self.ExtractData(outData);
            return xms::PyIterFromVecFlt(outData);
          }, extract_data_doc);
    // -------------------------------------------------------------------------
    // function: extract_at_location
    // -------------------------------------------------------------------------
    const char* extract_at_location_doc = R"pydoc(
        Extract interpolated data for the previously set locations.

        Args:
            location (iterable): The location to get the interpolated scalar.

        Returns:
            float: The interpolated value.
    )pydoc";
    extractor.def("extract_at_location", 
       [](xms::XmUGrid2dDataExtractor &self, py::iterable location) -> float {
            xms::Pt3d _location = xms::Pt3dFromPyIter(location);
            float value = self.ExtractAtLocation(_location);
            return value;
          }, extract_at_location_doc, py::arg("location"));
    // -------------------------------------------------------------------------
    // function: set_use_idw_for_point_data
    // -------------------------------------------------------------------------
    const char* set_use_idw_for_point_data_doc = R"pydoc(
        Set whether to use IDW for point data.

        Args:
            use_idw (bool): Whether to turn IDW on or off.
    )pydoc";
    extractor.def("set_use_idw_for_point_data", 
        &xms::XmUGrid2dDataExtractor::SetUseIdwForPointData,
            set_use_idw_for_point_data_doc, py::arg("use_idw"));
    // -------------------------------------------------------------------------
    // function: set_no_data_value
    // -------------------------------------------------------------------------
    const char* set_no_data_value_doc = R"pydoc(
        Set the no data value

        Args:
            no_data_value (bool): The no data value
    )pydoc";
    extractor.def("set_no_data_value", 
        &xms::XmUGrid2dDataExtractor::SetNoDataValue,
      set_no_data_value_doc,py::arg("no_data_value"));

    // DataLocationEnum
    py::enum_<xms::DataLocationEnum>(m, "data_location_enum",
                    "data_location_enum location mapping for dataset values")
        .value("LOC_POINTS", xms::LOC_POINTS)
        .value("LOC_CELLS", xms::LOC_CELLS)
        .value("LOC_UNKNOWN", xms::LOC_UNKNOWN);
}
