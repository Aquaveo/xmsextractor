from .._xmsextractor import extractor


class UGrid2dPolylineDataExtractor(object):

    data_locations = {
        'points': extractor.data_location_enum.LOC_POINTS,
        'cells': extractor.data_location_enum.LOC_CELLS,
        'unknown': extractor.data_location_enum.LOC_UNKNOWN,
    }

    def __init__(self, ugrid=None, scalar_locations=None, **kwargs):
        if 'instance' in kwargs:
            self._instance = kwargs['instance']
        else:
            if ugrid is None:
                raise ValueError("ugrid is a required argument")
            if scalar_locations is None:
                raise ValueError("scalar_locations is a required argument")
            self._instance = extractor.UGrid2dPolylineDataExtractor(ugrid, scalar_locations)

    def _check_data_locations(self, location_str):
        if location_str not in self.data_locations.keys():
            raise ValueError('location must be one of {}, not {}.'.format(
                ", ".join(self.data_locations.keys()), location_str
            ))

    def set_grid_scalars(self, scalars, activity, scalar_location):
        self._check_data_locations(scalar_location)
        data_location = self.data_locations[scalar_location]
        self._instance.SetGridScalars(scalars, activity, data_location)

    def set_polyline(self, polyline):
        self._instance.SetPolyline(polyline)

    def extract_data(self):
        return self._instance.ExtractData()

    def compute_locations_and_extract_data(self, polyline):
        return self._instance.ComputeLocationsAndExtractLocations(polyline)

    @property
    def extract_locations(self):
        return self._instance.GetExtractLocations()

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
