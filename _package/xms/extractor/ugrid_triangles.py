"""Module for UGrid triangulation."""

__copyright__ = "(C) Copyright Aquaveo 2025"
__license__ = "All rights reserved"
__all__ = ['triangulate_ugrid']


# 1. Standard Python modules

# 2. Third party modules

# 3. Aquaveo modules
from xms.grid.ugrid import UGrid

# 4. Local modules
from ._xmsextractor.extractor import triangulate_ugrid as internal_triangulate_ugrid


def triangulate_ugrid(ugrid: UGrid) -> UGrid:
    """
    Triangulate a UGrid.

    Triangulation inserts cell centroids and side centers into all non-triangle cells, then splits those cells into
    triangles. Cells which are already triangles are left unchanged.

    Args:
        ugrid: The UGrid to triangulate.

    Returns:
        A triangulated UGrid.
    """
    # Would be nice to avoid ._instance, but we can't send the Python grid to C++.
    xm_ugrid = internal_triangulate_ugrid(ugrid._instance)
    # ...and now we have to wrap the C++ instance back up for Python.
    ugrid = UGrid(instance=xm_ugrid)

    return ugrid
