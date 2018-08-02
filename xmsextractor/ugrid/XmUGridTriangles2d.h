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
#include <xmscore/misc/DynBitset.h>
#include <xmscore/stl/vector.h>

//----- Forward declarations ---------------------------------------------------

//----- Namespace declaration --------------------------------------------------

/// XMS Namespace
namespace xms
{
//----- Forward declarations ---------------------------------------------------
class GmTriSearch;
class XmUGrid;

//----- Constants / Enumerations -----------------------------------------------

//----- Structs / Classes ------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
class XmUGridTriangles
{
public:
  static BSHP<XmUGridTriangles> New();
  virtual ~XmUGridTriangles();

  virtual void BuildTriangles(const XmUGrid& a_ugrid, bool a_addTriangleCenters) = 0;
  virtual void BuildEarcutTriangles(const XmUGrid& a_ugrid) = 0;
  virtual void SetCellActivity(const DynBitset& a_cellActivity) = 0;

  virtual const VecPt3d& GetPoints() const = 0;
  virtual const VecInt& GetTriangles() const = 0;
  virtual BSHP<VecPt3d> GetPointsPtr() = 0;
  virtual BSHP<VecInt> GetTrianglesPtr() = 0;

  virtual int GetCellCentroid(int a_cellIdx) const = 0;
  virtual int GetIntersectedCell(const Pt3d& a_point, VecInt& a_idxs, VecDbl& a_weights) = 0;

protected:
  XmUGridTriangles();

private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGridTriangles)

};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
