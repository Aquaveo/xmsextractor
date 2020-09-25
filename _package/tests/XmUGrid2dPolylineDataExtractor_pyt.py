"""Test UGrid2dPolylineDataExtractor.cpp."""
import unittest

import numpy as np

from xms.grid.ugrid import UGrid

from xms.extractor import UGrid2dPolylineDataExtractor


class TestUGrid2dPolylineDataExtractor(unittest.TestCase):
    """UGrid2dPolylineDataExtractor tests."""

    def test_one_cell_one_segment(self):
        """Test extractor with point scalars only."""
        # clang-format off
        #     3-------2
        #     |       |
        # 0===============1
        #     |       |
        #     0-------1
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(-1, 0.5, 0), (2, 0.5, 0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), 0.5, 1.5, 2.5, float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(-1, 0.5, 0), (0.0, 0.5, 0.0), (0.5, 0.5, 0.0), (1.0, 0.5, 0.0), (2, 0.5, 0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_segment_all_in_cell(self):
        """Test extractor with single segment all inside of cell."""
        # clang-format off
        # (1)  3--------2  (3)
        #      |        |
        #      | 0====1 |
        #      |        |
        # (0)  0--------1  (2)
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(0.25, 0.50, 0.0), (0.75, 0.50, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [1.0, 1.5, 2.0]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(0.25, 0.5, 0.0), (0.5, 0.5, 0.0), (0.75, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_segment_along_edge(self):
        """Test extractor with single segment along an edge."""
        # clang-format off
        #  0===3========2===1
        #      |        |
        #      |        |
        #      |        |
        #      0--------1
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(-0.5, 1.0, 0.0), (1.55, 1.0, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), 1.0, 3.0, float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(-0.5, 1.0, 0.0), (0.0, 1.0, 0.0), (1.0, 1.0, 0.0), (1.55, 1.0, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_segment_all_outside(self):
        """Test extractor with single segment all outside of cell."""
        # clang-format off
        #      3========2
        #      |        |
        # 0==1 |        |
        #      |        |
        #      0--------1
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(-0.5, 0.5, 0.0), (-0.25, 0.5, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(-0.5, 0.5, 0.0), (-0.25, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_segment_out_to_touch(self):
        """Test extractor with single segment with endpoint touching cell."""
        # clang-format off
        #      3========2
        #      |        |
        # 0====1        |
        #      |        |
        #      0--------1
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(-0.5, 0.5, 0.0), (0.0, 0.5, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), 0.5]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(-0.5, 0.5, 0.0), (0.0, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_segment_touch_to_out(self):
        """Test extractor with single segment with first point touching edge."""
        # clang-format off
        #      3========2
        #      |        |
        #      |        0===1
        #      |        |
        #      0--------1
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(1.0, 0.5, 0.0), (1.5, 0.5, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [2.5, float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(1.0, 0.5, 0.0), (1.5, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_segment_cross_cell_point(self):
        """Test extractor with single segment touching cell point."""
        # clang-format off
        #        1
        #       /
        #      3========2
        #    / |        |
        #   0  |        |
        #      |        |
        #      0--------1
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(-0.5, 0.5, 0.0), (0.0, 1.0, 0.0), (0.5, 1.5, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), 1.0, float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(-0.5, 0.5, 0.0), (0.0, 1.0, 0.0), (0.5, 1.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_segment_across_cell_into_second(self):
        """Test extractor with single segment crossing first cell into second."""
        # clang-format off
        #      3========2========5
        #      |        |        |
        #   0===============1    |
        #      |        |        |
        #      0--------1--------4
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0), (2, 0, 0), (2, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3, UGrid.cell_type_enum.QUAD, 4, 1, 4, 5, 2]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1, 4, 5]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(-0.5, 0.5, 0.0), (1.5, 0.5, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), 0.5, 1.5, 2.5, 3.5]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(-0.5, 0.5, 0.0), (0.0, 0.5, 0.0), (0.5, 0.5, 0.0),
                              (1.0, 0.5, 0.0), (1.5, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_segment_across_split_cells(self):
        """Test extractor with single segment going across unconnected cells."""
        # clang-format off
        #       3-------2       7-------6
        #       |       |       |       |
        #  0========================1   |
        #       |       |       |       |
        #       0-------1       4-------5
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0),
                  (2, 0, 0), (3, 0, 0), (3, 1, 0), (2, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3, UGrid.cell_type_enum.QUAD, 4, 4, 5, 6, 7]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1, 4, 6, 7, 5]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(-1, 0.5, 0), (2.5, 0.5, 0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), 0.5, 1.5, 2.5, float('nan'), 4.5, 5.5]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(-1.0, 0.5, 0.0), (0.0, 0.5, 0.0), (0.5, 0.5, 0.0),
                              (1.0, 0.5, 0.0), (1.5, 0.5, 0.0), (2.0, 0.5, 0.0),
                              (2.5, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_two_segments_across_one_cell(self):
        """Test extractor with two segments going across a single cell."""
        # clang-format off
        #      3--------2
        #      |        |
        # 0=========1=========2
        #      |        |
        #      0--------1
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(-1, 0.5, 0), (0.5, 0.5, 0.0), (2, 0.5, 0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), 0.5, 1.5, 2.5, float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [
            (-1.0, 0.5, 0.0), (0.0, 0.5, 0.0), (0.5, 0.5, 0.0), (1.0, 0.5, 0.0), (2.0, 0.5, 0.0)
        ]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_two_segments_all_outside(self):
        """Test extractor with two segments all outside of cell."""
        # clang-format off
        #      3--------2
        #      |        |
        #      |        | 0====1====2
        #      |        |
        #      0--------1
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(2.0, 0.5, 0), (3.0, 0.5, 0.0), (4.0, 0.5, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), float('nan'), float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(2.0, 0.5, 0), (3.0, 0.5, 0.0), (4.0, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_two_segments_first_exiting(self):
        """Test extractor with two segments: first in to outside, second outside."""
        # clang-format off
        #      3--------2
        #      |        |
        #      |   0========1=======2
        #      |        |
        #      0--------1
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(0.5, 0.5, 0), (3.0, 0.5, 0.0), (4.0, 0.5, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [1.5, 2.5, float('nan'), float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(0.5, 0.5, 0), (1.0, 0.5, 0), (3.0, 0.5, 0.0), (4.0, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_two_segments_join_in_cell(self):
        """Test extractor with  two segments joining in a cell."""
        # clang-format off
        #      3========2========5
        #      |        |        |
        #      |   0========1=========2
        #      |        |        |
        #      0--------1--------4
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0), (2, 0, 0), (2, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3, UGrid.cell_type_enum.QUAD, 4, 1, 4, 5, 2]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1, 4, 5]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(0.5, 0.5, 0.0), (1.5, 0.5, 0.0), (2.5, 0.5, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [1.5, 2.5, 3.5, 4.5, float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(0.5, 0.5, 0.0), (1.0, 0.5, 0.0), (1.5, 0.5, 0.0),
                              (2.0, 0.5, 0.0), (2.5, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_two_segments_join_on_boundary(self):
        """Test extractor with two segments joining on two cell boundary."""
        # clang-format off
        #      3========2========5
        #      |        |        |
        #      |   0====1=============2
        #      |        |        |
        #      0--------1--------4
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0), (2, 0, 0), (2, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3, UGrid.cell_type_enum.QUAD, 4, 1, 4, 5, 2]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1, 4, 5]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(0.5, 0.5, 0.0), (1.0, 0.5, 0.0), (2.5, 0.5, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [1.5, 2.5, 3.5, 4.5, float('nan')]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(0.5, 0.5, 0.0), (1.0, 0.5, 0.0), (1.5, 0.5, 0.0),
                              (2.0, 0.5, 0.0), (2.5, 0.5, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_three_segments_cross_on_boundary(self):
        """Test extractor with three segments two crossing at boundary."""
        # clang-format off
        #      3----3---2
        #      |      \ |
        #      |   0========1
        #      |        | \ |
        #      0--------1   2
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        point_scalars = [0, 2, 3, 1]
        extractor.set_grid_scalars(point_scalars, [], 'points')

        polyline = [(0.5, 0.5, 0), (1.5, 0.5, 0.0), (1.5, 0.0, 0.0), (0.5, 1.0, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [1.5, 2.5, float('nan'), float('nan'), 2.5, 2.25, 2.0]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(0.5, 0.5, 0), (1.0, 0.5, 0), (1.5, 0.5, 0.0),
                              (1.5, 0.0, 0.0), (1.0, 0.5, 0), (0.75, 0.75, 0.0), (0.5, 1.0, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_cell_scalars(self):
        """Test extractor with cell scalars."""
        # clang-format off
        #      3========2========5
        #      |        |        |
        #   0===============1    |
        #      |        |        |
        #      0--------1--------4
        # clang-format on

        points = [(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0), (2, 0, 0), (2, 1, 0)]
        cells = [UGrid.cell_type_enum.QUAD, 4, 0, 1, 2, 3, UGrid.cell_type_enum.QUAD, 4, 1, 4, 5, 2]
        ugrid = UGrid(points, cells)
        extractor = UGrid2dPolylineDataExtractor(ugrid, "cells")

        cell_scalars = [1, 2]
        extractor.set_grid_scalars(cell_scalars, [], "cells")

        polyline = [(-0.5, 0.75, 0.0), (1.5, 0.75, 0.0)]
        extractor.set_polyline(polyline)
        extracted_locations = extractor.extract_locations
        extracted_data = extractor.extract_data()

        expected_data = [float('nan'), 1.0, 1.0, 1.25, 1.5, 1.75, 1.875]
        np.testing.assert_array_equal(expected_data, extracted_data)
        expected_locations = [(-0.5, 0.75, 0.0), (0.0, 0.75, 0.0), (0.25, 0.75, 0.0),
                              (0.75, 0.75, 0.0), (1., 0.75, 0.0), (1.25, 0.75, 0.0), (1.5, 0.75, 0.0)]
        np.testing.assert_array_equal(expected_locations, extracted_locations)

    def test_transient_tutorial(self):
        """Test UGrid2dPolylineDataExtractor for tutorial with transient data."""
        # build 2x3 grid
        points = [
            (288050, 3907770, 0), (294050, 3907770, 0), (300050, 3907770, 0),
            (306050, 3907770, 0), (288050, 3901770, 0), (294050, 3901770, 0),
            (300050, 3901770, 0), (306050, 3901770, 0), (288050, 3895770, 0),
            (294050, 3895770, 0), (300050, 3895770, 0), (306050, 3895770, 0)
        ]
        cells = [
            UGrid.cell_type_enum.QUAD, 4, 0, 4, 5, 1,
            UGrid.cell_type_enum.QUAD, 4, 1, 5, 6, 2,
            UGrid.cell_type_enum.QUAD, 4, 2, 6, 7, 3,
            UGrid.cell_type_enum.QUAD, 4, 4, 8, 9, 5,
            UGrid.cell_type_enum.QUAD, 4, 5, 9, 10, 6,
            UGrid.cell_type_enum.QUAD, 4, 6, 10, 11, 7
        ]
        ugrid = UGrid(points, cells)
        # Step 1. Create an extractor for an UGrid giving the mapped location of the scalar values
        extractor = UGrid2dPolylineDataExtractor(ugrid, 'points')

        # Step 2. Optionally set the "no data" value.
        extractor.no_data_value = -999.0

        # Step 3. Set the polyline to be extracted along.
        polyline = [
            (290764, 3895106, 0), (291122, 3909108, 0),
            (302772, 3909130, 0), (302794, 3895775, 0)
        ]
        extractor.set_polyline(polyline)

        # Step 4. Optionally get the locations used for extraction along the polyline.
        extracted_locations = extractor.extract_locations
        expected_locations = [
            (290764.0, 3895106.0, 0.0),
            (290780.9, 3895770.0, 0.0),
            (290862.4, 3898957.5, 0.0),
            (290934.3, 3901770.0, 0.0),
            (291012.0, 3904807.9, 0.0),
            (291087.7, 3907770.0, 0.0),
            (291122.0, 3909108.0, 0.0),
            (302772.0, 3909130.0, 0.0),
            (302774.2, 3907770.0, 0.0),
            (302778.7, 3905041.2, 0.0),
            (302784.1, 3901770.0, 0.0),
            (302788.6, 3899031.3, 0.0),
            (302794.0, 3895775.0, 0.0),
        ]
        np.testing.assert_allclose(expected_locations, extracted_locations, atol=0.15)

        # time step 1
        # Step 5. Set the point scalars for the first time step.
        point_scalars = [
            730.787, 1214.54, 1057.145, 629.2069, 351.1153, 631.6649,
            1244.366, 449.9133, 64.04247, 240.9716, 680.0491, 294.9547
        ]
        extractor.set_grid_scalars(point_scalars, [], "cells")
        # Step 6. Extract the data.
        extracted_data = extractor.extract_data()
        expected_data = [-999.0, 144.5, 299.4, 485.9, 681.8,
                         975.7, -999.0, -999.0, 862.8, 780.9,
                         882.3, 811.0, 504.4]
        np.testing.assert_allclose(expected_data, extracted_data, atol=0.2)

        # time step 2
        # Step 7. Continue using steps 5 and 6 for remaining time steps.
        point_scalars = [-999.0, 1220.5, 1057.1, 613.2, 380.1, 625.6, 722.2, 449.9, 51.0, 240.9,
                         609.0, 294.9]
        extractor.set_grid_scalars(point_scalars, [], "cells")
        extracted_data = extractor.extract_data()
        expected_data = [-999.0, 137.4, 314.8, 498.1, -196.9, 124.7, -999.0, -999.0, 855.5,
                         780.9, 598.1, 527.1, 465.4]
        np.testing.assert_allclose(expected_data, extracted_data, atol=0.2)
