#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief Contains the XmUGrid Class and supporting data types.
/// \ingroup ugrid
/// \copyright (C) Copyright Aquaveo 2022. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 3. Standard library headers

// 4. External library headers
#include <boost/container/flat_map.hpp>

#include <xmscore/misc/base_macros.h>   // XM_DISALLOW_COPY_AND_ASSIGN
#include <xmscore/misc/boost_defines.h> // BSHP
#include <xmscore/stl/vector.h>         // VecInt

// 5. Shared code headers
#include <xmsextractor/ugrid/XmElementEdge.h>
#include <xmsextractor/ugrid/XmElementMidpointInfo.h>

//----- Forward declarations ---------------------------------------------------

//----- Namespace declaration --------------------------------------------------

/// XMS Namespace
namespace xms
{
//----- Forward declarations ---------------------------------------------------
typedef boost::container::flat_map<XmElementEdge, XmElementMidpointInfo> FlatMapEdgeMidpointInfo;

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
  virtual const VecPt3d& GetPoints() const = 0;
  void SetMidpoints(BSHP<FlatMapEdgeMidpointInfo> a_midPoints);
  XmElementMidpointInfo& FindMidPoint(const XmElementEdge& a_edge);
  bool GenerateCentroidTriangles(int a_cellIdx, const VecInt& a_cellPointIdxs);
  void BuildEarcutTriangles(int a_cellIdx, const VecInt& a_cellPointIdxs);

private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGridTriangulatorBase)
  class impl;
  BSCP<impl> m_impl;
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
