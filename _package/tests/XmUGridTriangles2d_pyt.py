"""For testing."""

__copyright__ = "(C) Copyright Aquaveo 2025"
__license__ = "All rights reserved"


# 1. Standard Python modules

# 2. Third party modules
import numpy as np

# 3. Aquaveo modules
from xms.grid.ugrid import UGrid

# 4. Local modules
from xms.extractor.ugrid_triangles import triangulate_ugrid


def test_triangulator():
    r"""
    Test triangulating a grid.

    6    7----8
    |\   |    |
    |  \ |    |
    3----4----5
    |    |    |
    |    |    |
    0----1----2
    """
    points = [
        (0.0, 0.0, 0.0),
        (1.0, 0.0, 0.0),
        (2.0, 0.0, 0.0),
        (0.0, 1.0, 0.0),
        (1.0, 1.0, 0.0),
        (2.0, 1.0, 0.0),
        (0.0, 2.0, 0.0),
        (1.0, 2.0, 0.0),
        (2.0, 2.0, 0.0),
    ]

    cells = [
        UGrid.cell_type_enum.QUAD,
        4,
        0,
        1,
        4,
        3,
        UGrid.cell_type_enum.QUAD,
        4,
        1,
        2,
        5,
        4,
        UGrid.cell_type_enum.QUAD,
        4,
        4,
        5,
        8,
        7,
        UGrid.cell_type_enum.TRIANGLE,
        3,
        3,
        4,
        6,
    ]
    grid = UGrid(points, cells)
    triangulated_grid = triangulate_ugrid(grid)

    expected_points = [
        (0.0, 0.0, 0.0),
        (1.0, 0.0, 0.0),
        (2.0, 0.0, 0.0),
        (0.0, 1.0, 0.0),
        (1.0, 1.0, 0.0),
        (2.0, 1.0, 0.0),
        (0.0, 2.0, 0.0),
        (1.0, 2.0, 0.0),
        (2.0, 2.0, 0.0),
        (0.5, 0.0, 0.0),
        (1.0, 0.5, 0.0),
        (0.5, 1.0, 0.0),
        (0.0, 0.5, 0.0),
        (0.5, 0.5, 0.0),
        (1.5, 0.0, 0.0),
        (2.0, 0.5, 0.0),
        (1.5, 1.0, 0.0),
        (1.5, 0.5, 0.0),
        (2.0, 1.5, 0.0),
        (1.5, 2.0, 0.0),
        (1.0, 1.5, 0.0),
        (1.5, 1.5, 0.0),
    ]
    assert np.array_equal(triangulated_grid.locations, expected_points)

    expected_cells = [
        UGrid.cell_type_enum.TRIANGLE, 3, 0, 9, 13, UGrid.cell_type_enum.TRIANGLE, 3, 9, 1, 13,
        UGrid.cell_type_enum.TRIANGLE, 3, 1, 10, 13, UGrid.cell_type_enum.TRIANGLE, 3, 10, 4, 13,
        UGrid.cell_type_enum.TRIANGLE, 3, 4, 11, 13, UGrid.cell_type_enum.TRIANGLE, 3, 11, 3, 13,
        UGrid.cell_type_enum.TRIANGLE, 3, 3, 12, 13, UGrid.cell_type_enum.TRIANGLE, 3, 12, 0, 13,
        UGrid.cell_type_enum.TRIANGLE, 3, 1, 14, 17, UGrid.cell_type_enum.TRIANGLE, 3, 14, 2, 17,
        UGrid.cell_type_enum.TRIANGLE, 3, 2, 15, 17, UGrid.cell_type_enum.TRIANGLE, 3, 15, 5, 17,
        UGrid.cell_type_enum.TRIANGLE, 3, 5, 16, 17, UGrid.cell_type_enum.TRIANGLE, 3, 16, 4, 17,
        UGrid.cell_type_enum.TRIANGLE, 3, 4, 10, 17, UGrid.cell_type_enum.TRIANGLE, 3, 10, 1, 17,
        UGrid.cell_type_enum.TRIANGLE, 3, 4, 16, 21, UGrid.cell_type_enum.TRIANGLE, 3, 16, 5, 21,
        UGrid.cell_type_enum.TRIANGLE, 3, 5, 18, 21, UGrid.cell_type_enum.TRIANGLE, 3, 18, 8, 21,
        UGrid.cell_type_enum.TRIANGLE, 3, 8, 19, 21, UGrid.cell_type_enum.TRIANGLE, 3, 19, 7, 21,
        UGrid.cell_type_enum.TRIANGLE, 3, 7, 20, 21, UGrid.cell_type_enum.TRIANGLE, 3, 20, 4, 21,
        UGrid.cell_type_enum.TRIANGLE, 3, 3, 4, 6
    ]
    expected_cells = [int(cell) for cell in expected_cells]
    assert np.array_equal(triangulated_grid.cellstream, expected_cells)
