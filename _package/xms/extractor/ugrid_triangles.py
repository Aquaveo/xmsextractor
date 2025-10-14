""""""

__copyright__ = "(C) Copyright Aquaveo 2025"
__license__ = "All rights reserved"
__all__ = ['triangulate_ugrid']

from xms.grid.ugrid import UGrid
from xms.constraint.ugrid_2d import UGrid2d as CoGrid

# 1. Standard Python modules

# 2. Third party modules

# 3. Aquaveo modules

# 4. Local modules
from ._xmsextractor.extractor import triangulate_ugrid as internal_triangulate_ugrid

from xms.grid.ugrid.ugrid import XmUGrid


def triangulate_ugrid(ugrid: UGrid | CoGrid) -> UGrid:

    if isinstance(ugrid, CoGrid):
        ugrid = ugrid.ugrid

    # Would be nice to avoid ._instance, but we can't send the Python grid to C++.
    xm_ugrid = internal_triangulate_ugrid(ugrid._instance)
    # ...and now we have to wrap the C++ instance back up for Python.
    ugrid = UGrid(instance=xm_ugrid)

    return ugrid
