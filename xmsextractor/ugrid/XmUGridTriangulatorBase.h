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
#include <xmscore/stl/vector.h>         // VecInt

// 5. Shared code headers

//----- Forward declarations ---------------------------------------------------

//----- Namespace declaration --------------------------------------------------

/// XMS Namespace
namespace xms
{
//----- Forward declarations ---------------------------------------------------

//----- Constants / Enumerations -----------------------------------------------

//----- Structs / Classes ------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
class XmUGridTriangulatorBase
{
public:
  XmUGridTriangulatorBase();
  virtual ~XmUGridTriangulatorBase();
  virtual int AddCentroidPoint(const int a_cellIdx, const Pt3d& a_pt) = 0;
  virtual void AddTriangle(const int a_cellIdx,
                           const int a_pt1,
                           const int a_pt2,
                           const int a_pt3) = 0;
  void Triangulate(const VecInt& a_pts);

protected:
private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGridTriangulatorBase)
  class impl;
  BSCP<impl> m_impl;
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
