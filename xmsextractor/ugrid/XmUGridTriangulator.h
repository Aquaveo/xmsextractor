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
#include <xmscore/misc/base_macros.h>   // XM_DISALLOW_COPY_AND_ASSIGN
#include <xmscore/misc/boost_defines.h> // BSHP

// 5. Shared code headers
#include <xmsextractor/ugrid/XmUGridTriangulatorBase.h>

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
class XmUGridTriangulator : public XmUGridTriangulatorBase
{
public:
  static BSHP<XmUGridTriangulator> New(const XmUGrid& a_ugrid);
  virtual ~XmUGridTriangulator();
  virtual int AddCentroidPoint(const int a_cellIdx, const Pt3d& a_pt) = 0;
  virtual void AddTriangle(const int a_cellIdx,
                           const int a_pt1,
                           const int a_pt2,
                           const int a_pt3) = 0;
  virtual const VecPt3d& GetPoints() const = 0;
  virtual const VecInt& GetTriangles() const = 0;
  virtual BSHP<VecPt3d> GetPointsPtr() = 0;
  virtual BSHP<VecInt> GetTrianglesPtr() = 0;
  virtual int GetNumTriangles() const = 0;
  virtual int GetCellFromTriangle(const int a_triangleIdx) const = 0;
  virtual int GetCellCentroid(int a_cellIdx) const = 0;

protected:
  XmUGridTriangulator();

private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGridTriangulator)
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
