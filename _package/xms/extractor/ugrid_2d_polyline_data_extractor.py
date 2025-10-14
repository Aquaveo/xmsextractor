"""Extract data from a UGrid2d along a polyline."""

# 1. Standard Python modules

# 2. Third party modules

# 3. Aquaveo modules

# 4. Local modules
from ._xmsextractor import extractor


class UGrid2dPolylineDataExtractor(object):
    """Class for extracting data from a UGrid2d along a polyline."""
    data_locations = {
        'points': extractor.data_location_enum.LOC_POINTS,
        'cells': extractor.data_location_enum.LOC_CELLS,
        'unknown': extractor.data_location_enum.LOC_UNKNOWN,
    }

    def __init__(self, ugrid=None, scalar_location=None, **kwargs):
        """Constructor.

        Args:
            ugrid (UGrid2d): The ugrid to extract data from
            scalar_location (str): Location of the data to extract. One of: 'points', 'cells', 'unknown'
            **kwargs (dict): Generic keyword arguments
        """
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
        """Raise an exception if the specified location string is invalid.

        Args:
            location_str: Location of the data to extract. One of: 'points', 'cells', 'unknown'
        """
        if location_str not in self.data_locations.keys():
            raise ValueError('location must be one of {}, not {}.'.format(
                ", ".join(self.data_locations.keys()), location_str
            ))

    def get_data_extractor(self):
        """Get an instance of a UGrid2dDataExtractor class from this instance.

        Returns:
            (UGrid2dDataExtractor): instance of the UGrid2dDataExtractor class
        """
        instance = self._instance.GetDataExtractor()
        from .ugrid_2d_data_extractor import UGrid2dDataExtractor
        rval = UGrid2dDataExtractor(instance=instance)
        return rval

    def set_grid_scalars(self, scalars, activity, scalar_location):
        """Setup cell scalars to be used to extract interpolated data.

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
        """Set the polyline along which to extract the scalar data.

        Locations crossing cell boundaries are computed along the polyline.

        Args:
            polyline (iterable): The polyline.
        """
        self._instance.SetPolyline(polyline)

    def extract_data(self):
        """Extract interpolated data for the previously set locations.

        Returns:
            The interpolated scalars.
        """
        return self._instance.ExtractData()

    def compute_locations_and_extract_data(self, polyline):
        """Extract data for given polyline.

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
    def cell_indexes(self):
        """Cell indexes for the extract location."""
        return self._instance.GetCellIndexes()

    @property
    def use_idw_for_point_data(self):
        """Use IDW to calculate point scalar values from cell scalars."""
        return self._instance.GetUseIdwForPointData()

    @use_idw_for_point_data.setter
    def use_idw_for_point_data(self, value):
        """Set whether to use IDW to calculate point scalar values from cell scalars."""
        self._instance.SetUseIdwForPointData(value)

    @property
    def no_data_value(self):
        """Value to use when extracted value is in inactive cell or doesn't intersect with the grid."""
        return self._instance.GetNoDataValue()

    @no_data_value.setter
    def no_data_value(self, value):
        """Set value to use when extracted value is in inactive cell or doesn't intersect with the grid."""
        self._instance.SetNoDataValue(value)
