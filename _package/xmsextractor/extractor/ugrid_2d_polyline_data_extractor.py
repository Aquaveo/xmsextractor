"""
********************************************************************************
* Name: interp_anisotropic.py
* Author: Gage Larsen, Matt LeBaron
* Created On: May 2nd, 2019
* Copyright: (c)
* License: BSD 2-Clause
********************************************************************************
"""
from .._xmsextractor import extractor


class UGrid2dPolylineDataExtractor(object):

    data_locations = {
        'points': extractor.data_location_enum.LOC_POINTS,
        'cells': extractor.data_location_enum.LOC_CELLS,
        'unknown': extractor.data_location_enum.LOC_UNKNOWN,
    }

    def __init__(self, ugrid=None, scalar_location=None, **kwargs):
        if 'instance' in kwargs:
            self._instance = kwargs['instance']
        else:
            if ugrid is None:
                raise ValueError("ugrid is a required argument")
            if scalar_location is None:
                raise ValueError("scalar_location is a required argument")
            self._check_data_locations(scalar_location)
            data_location = self.data_locations[scalar_location]
            self._instance = extractor.UGrid2dPolylineDataExtractor(ugrid._instance, data_location)

    def _check_data_locations(self, location_str):
        if location_str not in self.data_locations.keys():
            raise ValueError('location must be one of {}, not {}.'.format(
                ", ".join(self.data_locations.keys()), location_str
            ))

    def set_grid_scalars(self, scalars, activity, scalar_location):
        """
        Setup cell scalars to be used to extract interpolated data.

        Args:
            scalars (iterable): The cell scalars.
            activity (iterable): The activity of the cells.
            scalar_location (string): The location at which the data is currently stored. One of 'points', 'cells',
                or 'unknown'
        """
        self._check_data_locations(scalar_location)
        data_location = self.data_locations[scalar_location]
        self._instance.SetGridScalars(scalars, activity, data_location)

    def set_polyline(self, polyline):
        """
        Set the polyline along which to extract the scalar data. Locations
        crossing cell boundaries are computed along the polyline.

        Args:
            polyline (iterable): The polyline.
        """
        self._instance.SetPolyline(polyline)

    def extract_data(self):
        """
        Extract interpolated data for the previously set locations.

        Returns:
            The interpolated scalars.
        """
        return self._instance.ExtractData()

    def compute_locations_and_extract_data(self, polyline):
        """
        Extract data for given polyline.

        Args:
            polyline (iterable): The polyline.

        Returns:
            A tuple of the extracted data and their locations
        """
        return self._instance.ComputeLocationsAndExtractLocations(polyline)

    @property
    def extract_locations(self):
        """Locations of points to extract interpolated scalar data from."""
        return self._instance.GetExtractLocations()

    @property
    def use_idw_for_point_data(self):
        """Use IDW to calculate point scalar values from cell scalars."""
        return self._instance.GetUseIdwForPointData()

    @use_idw_for_point_data.setter
    def use_idw_for_point_data(self, value):
        self._instance.SetUseIdwForPointData(value)

    @property
    def no_data_value(self):
        """Value to use when extracted value is in inactive cell or doesn't intersect with the grid."""
        return self._instance.GetNoDataValue()

    @no_data_value.setter
    def no_data_value(self, value):
        self._instance.SetNoDataValue(value)
