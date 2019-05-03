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


class UGrid2dDataExtractor(object):

    data_locations = {
        'points': extractor.data_location_enum.LOC_POINTS,
        'cells': extractor.data_location_enum.LOC_CELLS,
        'unknown': extractor.data_location_enum.LOC_UNKNOWN,
    }

    def __init__(self, ugrid=None, **kwargs):
        if 'instance' in kwargs:
            self._instance = kwargs['instance']
        else:
            if ugrid is None:
                raise ValueError("ugrid is a required argument")
            self._instance = extractor.UGrid2dDataExtractor(ugrid._instance)

    def _check_data_locations(self, location_str):
        if location_str not in self.data_locations.keys():
            raise ValueError('location must be one of {}, not {}.'.format(
                ", ".join(self.data_locations.keys()), location_str
            ))

    def set_grid_point_scalars(self, point_scalars, activity, activity_type):
        """
        Setup point scalars to be used to extract interpolated data.

        Args:
            point_scalars (iterable): The point scalars.
            activity (iterable): The activity of the cells.
            activity_type (string): The location at which the data is currently stored. One of 'points', 'cells',
                or 'unknown'
        """
        self._check_data_locations(activity_type)
        data_location = self.data_locations[activity_type]
        self._instance.SetGridPointScalars(point_scalars, activity, data_location)

    def set_grid_cell_scalars(self, cell_scalars, activity, activity_type):
        """
        Setup cell scalars to be used to extract interpolated data.

        Args:
            cell_scalars (iterable): The cell scalars.
            activity (iterable): The activity of the cells.
            activity_type (string): The location at which the data is currently stored. One of 'points', 'cells',
                or 'unknown'
        """
        self._check_data_locations(activity_type)
        data_location = self.data_locations[activity_type]
        self._instance.SetGridCellScalars(cell_scalars, activity, data_location)

    def extract_data(self):
        """
        Extract interpolated data for the previously set locations.

        Returns:
            The interpolated scalars.
        """
        return self._instance.ExtractData()

    def extract_at_location(self, location):
        """
        Extract interpolated data for the previously set locations.

        Args:
            location: The location to get the interpolated scalar.

        Returns:
            The interpolated value.
        """
        return self._instance.ExtractAtLocaction(location)

    @property
    def extract_locations(self):
        """Locations of points to extract interpolated scalar data from."""
        return self._instance.GetExtractLocations()

    @extract_locations.setter
    def extract_locations(self, value):
        self._instance.SetExtractLocations(value)

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


