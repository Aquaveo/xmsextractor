#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief Contains the XmUGrid Class and supporting data types.
/// \ingroup ugrid
/// \copyright (C) Copyright Aquaveo 2018. Distributed under the xmsng
///  Software License, Version 1.0. (See accompanying file
///  LICENSE_1_0.txt or copy at http://www.aquaveo.com/xmsng/LICENSE_1_0.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 3. Standard library headers

// 4. External library headers

// 5. Shared code headers
#include <xmscore/misc/base_macros.h>
#include <xmscore/misc/boost_defines.h>
#include <xmscore/stl/vector.h>

//----- Forward declarations ---------------------------------------------------

//----- Namespace declaration --------------------------------------------------

/// XMS Namespace
namespace xms
{
//----- Forward declarations ---------------------------------------------------
class XmUGrid;

//----- Constants / Enumerations -----------------------------------------------

//----- Structs / Classes ------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
class XmUGridTriangles
{
public:
  XmUGridTriangles();

  void Initialize(const XmUGrid& a_ugrid);
  void GenerateTriangles(const XmUGrid& a_ugrid, bool a_earcutTriangles = false);

  const VecPt3d& GetTrianglePoints();
  const VecInt& GetTriangles();
  int AddCellCentroid(int a_cellIdx, const Pt3d& a_point);
  void AddCellTriangle(int a_cellIdx, int a_idx1, int a_idx2, int a_idx3);

private:
  BSHP<VecPt3d> m_trianglePoints; ///< Triangle points for the UGrid
  BSHP<VecInt> m_triangles;       ///< Triangles for the UGrid
  int m_firstCellPoint;           ///< Index of first point that is a centroid
  VecInt m_pointCell;             ///< The cell index for each centroid
  VecInt m_triangleCell;          ///< The cell index for each triangle
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
