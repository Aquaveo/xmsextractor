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
#include <xmsgrid/ugrid/XmUGrid.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------
PYBIND11_DECLARE_HOLDER_TYPE(T, boost::shared_ptr<T>);

void initXmUGrid2dDataExtractor(py::module &m) {
    // XmUGrid2dDataExtractor Class
    const char* xmUGrid2dDataExtractor_doc = R"pydoc(
        Class provides ability to extract dataset values at points for an 
        unstructured grid.
    )pydoc";
    py::class_<xms::XmUGrid2dDataExtractor, 
        boost::shared_ptr<xms::XmUGrid2dDataExtractor>> 
        extractor(m, "XmUGrid2dDataExtractor", xmUGrid2dDataExtractor_doc);
    // -------------------------------------------------------------------------
    // function: init
    // -------------------------------------------------------------------------
    const char* init_doc = R"pydoc(
        Create a new XmUGrid2dDataExtractor.

        Args:
            ugrid (XmUGrid): The UGrid geometry to use to extract values from.
    )pydoc";
    extractor.def(py::init([](boost::shared_ptr<xms::XmUGrid> ugrid) {
            return boost::shared_ptr<xms::XmUGrid2dDataExtractor>
                (xms::XmUGrid2dDataExtractor::New(ugrid));
            }), init_doc, py::arg("ugrid"));
    // -------------------------------------------------------------------------
    // function: init
    // -------------------------------------------------------------------------
    const char* init_doc2 = R"pydoc(
        Create a new XmUGrid2dDataExtractor using shallow copy from existing
        extractor.

        Args:
            xm_extractor (XmUGrid2dDataExtractor): The extractor to shallow copy.
            
        Returns:
            iterable: The new XmUGrid2dDataExtractor.
    )pydoc";
    extractor.def(py::init(
        [](boost::shared_ptr<xms::XmUGrid2dDataExtractor> xm_extractor) {
            return boost::shared_ptr<xms::XmUGrid2dDataExtractor>
                (xms::XmUGrid2dDataExtractor::New(xm_extractor));
        }), init_doc2, py::arg("xm_extractor"));
    // -------------------------------------------------------------------------
    // function: set_grid_point_scalars
    // -------------------------------------------------------------------------
    const char* set_grid_point_scalars_doc = R"pydoc(
        Setup point scalars to be used to extract interpolated data.

        Args:
            point_scalars (iterable): The point scalars.

            activity (iterable): The activity of the cells.

            activity_type (data_location_enum): The location at which the data is currently stored.
    )pydoc";
    extractor.def("set_grid_point_scalars", 
        [](xms::XmUGrid2dDataExtractor &self, py::iterable point_scalars,
                                          py::iterable activity,
                                          xms::DataLocationEnum activity_type) {
            boost::shared_ptr<xms::VecFlt> pointScalars =
                 xms::VecFltFromPyIter(point_scalars);
            xms::DynBitset cell_activity = xms::DynamicBitsetFromPyIter(activity);
            self.SetGridPointScalars(*pointScalars, cell_activity, activity_type);
        },set_grid_point_scalars_doc,py::arg("point_scalars"),
        py::arg("activity"),py::arg("activity_type"));
    // -------------------------------------------------------------------------
    // function: set_grid_cell_scalars
    // -------------------------------------------------------------------------
    const char* set_grid_cell_scalars_doc = R"pydoc(
        Setup cell scalars to be used to extract interpolated data.

        Args:
            point_scalars (iterable): The point scalars.

            activity (iterable): The activity of the cells.

            activity_type (data_location_enum): The location at which the data is currently stored.
    )pydoc";
    extractor.def("set_grid_cell_scalars", [](xms::XmUGrid2dDataExtractor &self,
                                        py::iterable point_scalars,
                                        py::iterable activity,
                                        xms::DataLocationEnum activity_type) {
            boost::shared_ptr<xms::VecFlt> cellScalars = 
                xms::VecFltFromPyIter(point_scalars);
            xms::DynBitset cell_activity = xms::DynamicBitsetFromPyIter(activity);
            self.SetGridCellScalars(*cellScalars, cell_activity, activity_type);
        },set_grid_cell_scalars_doc,py::arg("point_scalars"),py::arg("activity")
        ,py::arg("activity_type"));
    // -------------------------------------------------------------------------
    // function: set_extract_locations
    // -------------------------------------------------------------------------
    const char* set_extract_locations_doc = R"pydoc(
        Sets locations of points to extract interpolated scalar data from.

        Args:
            grid_locations (iterable): The locations.
    )pydoc";
    extractor.def("set_extract_locations", [](xms::XmUGrid2dDataExtractor &self,
        py::iterable grid_locations) {
            boost::shared_ptr<xms::VecPt3d> locations = 
                xms::VecPt3dFromPyIter(grid_locations);
            self.SetExtractLocations(*locations);
        }, set_extract_locations_doc, py::arg("grid_locations"));
    // -------------------------------------------------------------------------
    // function: get_extract_locations
    // -------------------------------------------------------------------------
    const char* get_extract_locations_doc = R"pydoc(
        Gets locations of points to extract interpolated scalar data from.

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
            scalar_location (iterable): The location to get the interpolated 
                scalar.

        Returns:
            float: The interpolated value.
    )pydoc";
    extractor.def("extract_at_location", 
       [](xms::XmUGrid2dDataExtractor &self, py::iterable scalar_location) -> 
       float {
            xms::Pt3d location = xms::Pt3dFromPyIter(scalar_location);
            float value = self.ExtractAtLocation(location);
            return value;
          }, extract_at_location_doc, py::arg("scalar_location"));
    // -------------------------------------------------------------------------
    // function: set_use_idw_for_point_data
    // -------------------------------------------------------------------------
    const char* set_use_idw_for_point_data_doc = R"pydoc(
        Set to use IDW to calculate point scalar values from cell scalars.

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
        Set value to use when extracted value is in inactive cell or doesn't
        intersect with the grid.

        Args:
            no_data_value (bool): The no data value.
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
