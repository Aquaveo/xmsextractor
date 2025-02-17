"""Extract data from a UGrid2d at specified locations."""
from xms.grid.ugrid import UGrid

from ._xmsextractor import extractor


class UGrid2dDataExtractor(object):
    """Class for extracting data from a UGrid2d at specified locations."""
    data_locations = {
        'points': extractor.data_location_enum.LOC_POINTS,
        'cells': extractor.data_location_enum.LOC_CELLS,
        'unknown': extractor.data_location_enum.LOC_UNKNOWN,
    }

    def __init__(self, ugrid: UGrid = None, **kwargs):
        """
        Constructor.

        Typical use is to set the points you want data for with self.extract_locations, set the scalar values for
        the grid with self.set_grid_point_scalars or self.set_grid_cell_scalars, then call self.extract data to get the
        extracted data.

        If scalar values are not set, then the grid's elevation data will be used by default.

        Args:
            ugrid (UGrid2d): The ugrid to extract data from
            **kwargs (dict): Generic keyword arguments
        """
        if 'instance' in kwargs:
            self._instance = kwargs['instance']
        else:
            if ugrid is None:
                raise ValueError("ugrid is a required argument")
            self._instance = extractor.UGrid2dDataExtractor(ugrid._instance)
            self._ugrid = ugrid

    def _check_data_locations(self, location_str):
        """Raise an exception if the specified location string is invalid.

        Args:
            location_str: Location of the data to extract. One of: 'points', 'cells', 'unknown'
        """
        if location_str not in self.data_locations.keys():
            raise ValueError('location must be one of {}, not {}.'.format(
                ", ".join(self.data_locations.keys()), location_str
            ))

    def set_grid_point_scalars(self, point_scalars, activity, activity_type):
        """Setup point scalars to be used to extract interpolated data.

        There must be as many scalars as the grid has points.

        Args:
            point_scalars (iterable): The point scalars.
            activity (iterable): The activity of the cells.
            activity_type (string): The location at which the data is currently stored. One of 'points', 'cells',
                or 'unknown'
        """
        if len(point_scalars) != self._ugrid.point_count:
            raise ValueError('Number of scalars must match number of grid points')

        self._check_data_locations(activity_type)
        data_location = self.data_locations[activity_type]
        self._instance.SetGridPointScalars(point_scalars, activity, data_location)

    def set_grid_cell_scalars(self, cell_scalars, activity, activity_type):
        """Setup cell scalars to be used to extract interpolated data.

        There must be as many scalars as the grid has cells.

        Args:
            cell_scalars (iterable): The cell scalars.
            activity (iterable): The activity of the cells.
            activity_type (string): The location at which the data is currently stored. One of 'points', 'cells',
                or 'unknown'
        """
        if len(cell_scalars) != self._ugrid.cell_count:
            raise ValueError('Number of scalars must match number of grid points')

        self._check_data_locations(activity_type)
        data_location = self.data_locations[activity_type]
        self._instance.SetGridCellScalars(cell_scalars, activity, data_location)

    def extract_data(self):
        """Extract interpolated data for the previously set locations.

        Returns:
            The interpolated scalars.
        """
        return self._instance.ExtractData()

    def extract_at_location(self, location):
        """Extract interpolated data for a single location.

        Args:
            location: The location to get the interpolated scalar.

        Returns:
            The interpolated value.
        """
        return self._instance.ExtractAtLocation(location)

    @property
    def extract_locations(self):
        """Locations of points to extract interpolated scalar data from."""
        return self._instance.GetExtractLocations()

    @extract_locations.setter
    def extract_locations(self, value):
        """Set locations of points to extract interpolated scalar data from."""
        self._instance.SetExtractLocations(value)

    @property
    def cell_indexes(self):
        """
        Cell indexes for the extract location.

        This is initialized by self.extract_data.
        """
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
