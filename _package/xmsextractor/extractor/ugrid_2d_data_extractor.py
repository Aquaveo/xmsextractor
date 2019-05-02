from .._xmsextractor import extractor


class UGrid2dDataExtractor(object):
    """

    """

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
        self._check_data_locations(activity_type)
        data_location = self.data_locations[activity_type]
        self._instance.SetGridPointScalars(point_scalars, activity, data_location)

    def set_grid_cell_scalars(self, cell_scalars, activity, activity_type):
        self._check_data_locations(activity_type)
        data_location = self.data_locations[activity_type]
        self._instance.SetGridCellScalars(cell_scalars, activity, data_location)

    def extract_data(self):
        return self._instance.ExtractData()

    def extract_at_location(self, location):
        return self._instance.ExtractAtLocaction(location)

    @property
    def extract_locations(self):
        return self._instance.GetExtractLocations()

    @extract_locations.setter
    def extract_locations(self, value):
        self._instance.SetExtractLocations(value)

    @property
    def use_idw_for_point_data(self):
        return self._instance.GetUseIdwForPointData()

    @use_idw_for_point_data.setter
    def use_idw_for_point_data(self, value):
        self._instance.SetUseIdwForPointData(value)

    @property
    def no_data_value(self):
        return self._instance.GetNoDataValue()

    @no_data_value.setter
    def no_data_value(self, value):
        self._instance.SetNoDataValue(value)


