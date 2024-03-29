#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief Contains the XmUGrid Class and supporting data types.
/// \ingroup ugrid
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 3. Standard library headers

// 4. External library headers

// 5. Shared code headers
#include <xmscore/misc/base_macros.h>
#include <xmscore/misc/boost_defines.h>
#include <xmscore/misc/DynBitset.h>
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
class XmUGridTriangles2d
{
public:
  static BSHP<XmUGridTriangles2d> New();
  virtual ~XmUGridTriangles2d();

  /// \brief Generate triangles for the UGrid.
  /// \param[in] a_ugrid The UGrid for which triangles are generated.
  /// \param[in] a_pointOption Whether to add no points, add centroids only, or add centroids and
  /// midpoints.
  enum PointOptionEnum : int { PO_NO_POINTS, PO_CENTROIDS_ONLY, PO_CENTROIDS_AND_MIDPOINTS };
  virtual void BuildTriangles(const XmUGrid& a_ugrid, PointOptionEnum a_pointOption) = 0;
  /// \brief Generate triangles for the UGrid using earcut algorithm.
  /// \param[in] a_ugrid The UGrid for which triangles are generated.
  virtual void BuildEarcutTriangles(const XmUGrid& a_ugrid) = 0;
  /// \brief Set triangle activity based on each triangles cell.
  /// \param[in] a_cellActivity The cell activity to set on the triangles.
  virtual void SetCellActivity(const DynBitset& a_cellActivity) = 0;
  /// \brief Set to force triangles to be split at cell center with cell data
  /// \param[in] a_splitTrisWithCellData
  virtual void SetSplitTrisWithCellData(bool a_splitTrisWithCellData) = 0;

  /// \brief Get the generated triangle points.
  /// \return The triangle points
  virtual const VecPt3d& GetPoints() const = 0;
  /// \brief Get the generated triangles.
  /// \return a vector of indices for the triangles.
  virtual const VecInt& GetTriangles() const = 0;
  /// \brief Get the generated triangle points as a shared pointer.
  /// \return The triangle points
  virtual BSHP<VecPt3d> GetPointsPtr() = 0;
  /// \brief Get the generated triangles as a shared pointer.
  /// \return a vector of indices for the triangles.
  virtual BSHP<VecInt> GetTrianglesPtr() = 0;

  /// \brief Get the point index for the centroid of a cell.
  /// \param[in] a_cellIdx The cell index.
  /// \return The point index of the cell centroid.
  virtual int GetCellCentroid(int a_cellIdx) const = 0;

  /// \brief Get the cell index and interpolation values intersected by a point.
  /// \param[in] a_point The point to intersect with the UGrid.
  /// \param[out] a_idxs The interpolation points.
  /// \param[out] a_weights The interpolation weights.
  /// \return The cell intersected by the point or -1 if outside of the UGrid.
  virtual int GetIntersectedCell(const Pt3d& a_point, VecInt& a_idxs, VecDbl& a_weights) = 0;

protected:
  XmUGridTriangles2d();

private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGridTriangles2d)
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
