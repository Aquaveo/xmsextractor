//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsextractor/ugrid/XmUGridTriangulator.h>

// 3. Standard library headers

// 4. External library headers

// 5. Shared code headers
#include <xmsgrid/ugrid/XmUGrid.h>

// 6. Non-shared code headers

//----- Forward declarations ---------------------------------------------------

//----- External globals -------------------------------------------------------

//----- Namespace declaration --------------------------------------------------

/// XMS Namespace
namespace xms
{
//----- Constants / Enumerations -----------------------------------------------

//----- Classes / Structs ------------------------------------------------------

//----- Internal functions -----------------------------------------------------

//----- Class / Function definitions -------------------------------------------

namespace
{
class XmUGridTriangulatorImpl : public XmUGridTriangulator
{
public:
  XmUGridTriangulatorImpl(const XmUGrid& a_ugrid);

  virtual int AddCentroidPoint(const int a_cellIdx, const Pt3d& a_pt) final;
  virtual void AddTriangle(const int a_cellIdx,
                           const int a_pt1,
                           const int a_pt2,
                           const int a_pt3) final;
  virtual const VecPt3d& GetPoints() const final;
  virtual const VecInt& GetTriangles() const final;
  virtual BSHP<VecPt3d> GetPointsPtr() final;
  virtual BSHP<VecInt> GetTrianglesPtr() final;
  virtual int GetNumTriangles() const final;
  virtual int GetCellFromTriangle(const int a_triangleIdx) const final;
  virtual int GetCellCentroid(int a_cellIdx) const final;
  virtual void InitMidpoints() final;
  virtual int AddMidPoint(const int a_cellIdx, const int a_ptIdx0, const int a_ptIdx1) final;

private:
  XmUGridTriangulatorImpl();
  const XmUGrid& m_ugrid;
  BSHP<VecPt3d> m_points;     ///< Triangle points for the UGrid
  BSHP<VecInt> m_triangles;   ///< Triangles for the UGrid
  VecInt m_centroidIdxs;      ///< Index of each cell centroid or -1 if none
  VecInt m_triangleToCellIdx; ///< The cell index for each triangle
};

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangulatorImpl
/// \brief Class to store XmUGrid triangles. Tracks where midpoints and
///        triangles came from.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Constructor
//------------------------------------------------------------------------------
XmUGridTriangulatorImpl::XmUGridTriangulatorImpl(const XmUGrid& a_ugrid)
: m_ugrid(a_ugrid)
, m_points(new VecPt3d(a_ugrid.GetLocations()))
, m_triangles(new VecInt)
, m_centroidIdxs(a_ugrid.GetCellCount(), -1)
, m_triangleToCellIdx()
{
} // XmUGridTriangulatorImpl::XmUGridTriangulatorImpl
//------------------------------------------------------------------------------
/// \brief Add a cell point (for the cell centroid).
/// \param a_cellIdx The cell index for the centroid point
/// \param a_point The centroid point
/// \return The index of the added point
//------------------------------------------------------------------------------
int XmUGridTriangulatorImpl::AddCentroidPoint(const int a_cellIdx, const Pt3d& a_point)
{
  int centroidIdx = (int)m_points->size();
  m_points->push_back(a_point);
  m_centroidIdxs[a_cellIdx] = centroidIdx;
  return centroidIdx;
} // XmUGridTriangulatorImpl::AddCentroidPoint
//------------------------------------------------------------------------------
/// \brief Add a triangle cell.
/// \param[in] a_cellIdx The cell index the triangle is from
/// \param[in] a_idx1 The first triangle point index (counter clockwise)
/// \param[in] a_idx2 The second triangle point index (counter clockwise)
/// \param[in] a_idx3 The third triangle point index (counter clockwise)
//------------------------------------------------------------------------------
void XmUGridTriangulatorImpl::AddTriangle(const int a_cellIdx,
                                          const int a_idx1,
                                          const int a_idx2,
                                          const int a_idx3)
{
  m_triangles->push_back(a_idx1);
  m_triangles->push_back(a_idx2);
  m_triangles->push_back(a_idx3);
  m_triangleToCellIdx.push_back(a_cellIdx);
} // XmUGridTriangulatorImpl::AddTriangle
//------------------------------------------------------------------------------
/// \brief Get the centroid of a cell.
/// \param[in] a_cellIdx The cell index.
/// \return The index of the cell point.
//------------------------------------------------------------------------------
int XmUGridTriangulatorImpl::GetCellCentroid(int a_cellIdx) const
{
  int pointIdx = -1;
  if (a_cellIdx >= 0 && a_cellIdx < (int)m_centroidIdxs.size())
    pointIdx = m_centroidIdxs[a_cellIdx];
  return pointIdx;
} // XmUGridTriangulatorImpl::GetCellCentroid
//------------------------------------------------------------------------------
/// \brief Sets the option to create midpoints and sets up the required data.
//------------------------------------------------------------------------------
void XmUGridTriangulatorImpl::InitMidpoints()
{
  BSHP<FlatMapEdgeMidpointInfo> midPoints(new FlatMapEdgeMidpointInfo());
  VecInt attachedPts;
  int numPts = (int)m_points->size();
  for (int ii = 0; ii < numPts; ++ii)
  {
    m_ugrid.GetPointAdjacentPoints(ii, attachedPts);
    if (!attachedPts.empty())
    {
      XmElementMidpointInfo info = {-1, -1};
      for (auto&& ptIdx : attachedPts)
        midPoints->insert(midPoints->end(), std::pair<XmElementEdge, XmElementMidpointInfo>(
                                              XmElementEdge(ii, ptIdx), info));
    }
  }
  SetMidpoints(midPoints);
} // XmUGridTriangulatorImpl::InitMidpoints
//------------------------------------------------------------------------------
/// \brief Add a cell point at the midpoint between the given point indices.
/// \param a_cellIdx The cell index for the centroid point
/// \param a_ptIdx0 The index of the first cell point
/// \param a_ptIdx1 The index of the second cell point
/// \return The index of the added point
//------------------------------------------------------------------------------
int XmUGridTriangulatorImpl::AddMidPoint(const int a_cellIdx,
                                         const int a_ptIdx0,
                                         const int a_ptIdx1)
{
  XmElementEdge edgePair(a_ptIdx0, a_ptIdx1);
  XmElementMidpointInfo& info = FindMidPoint(XmElementEdge(a_ptIdx0, a_ptIdx1));
  auto&& cellMid = info.cellId;
  auto&& idMid = info.midPtId;
  if (idMid < 0)
  {
    Pt3d p0 = m_points->at(a_ptIdx0);
    Pt3d p1 = m_points->at(a_ptIdx1);
    double midPt[3] = {(p0[0] + p1[0]) / 2.0, (p0[1] + p1[1]) / 2.0, (p0[2] + p1[2]) / 2.0};
    cellMid = a_cellIdx;
    idMid = (long)m_points->size();
    m_points->emplace_back(midPt[0], midPt[1], midPt[2]);
  }
  return (int)idMid;
} // XmUGridTriangulatorImpl::AddMidPoint
//------------------------------------------------------------------------------
/// \brief Get the generated triangle points.
/// \return The triangle points
//------------------------------------------------------------------------------
const VecPt3d& XmUGridTriangulatorImpl::GetPoints() const
{
  return *m_points;
} // XmUGridTriangulatorImpl::GetPoints
//------------------------------------------------------------------------------
/// \brief Get the generated triangles.
/// \return a vector of indices for the triangles.
//------------------------------------------------------------------------------
const VecInt& XmUGridTriangulatorImpl::GetTriangles() const
{
  return *m_triangles;
} // XmUGridTriangulatorImpl::GetTriangles
//------------------------------------------------------------------------------
/// \brief Get the generated triangle points as a shared pointer.
/// \return The triangle points
//------------------------------------------------------------------------------
BSHP<VecPt3d> XmUGridTriangulatorImpl::GetPointsPtr()
{
  return m_points;
} // XmUGridTriangulatorImpl::GetPointsPtr
//------------------------------------------------------------------------------
/// \brief Get the generated triangles as a shared pointer.
/// \return a vector of indices for the triangles.
//------------------------------------------------------------------------------
BSHP<VecInt> XmUGridTriangulatorImpl::GetTrianglesPtr()
{
  return m_triangles;
} // XmUGridTriangulatorImpl::GetTrianglesPtr
//------------------------------------------------------------------------------
/// \brief Get the number of triangles generated.
/// \return the number of triangles.
//------------------------------------------------------------------------------
int XmUGridTriangulatorImpl::GetNumTriangles() const
{
  return (int)m_triangleToCellIdx.size();
} // XmUGridTriangulatorImpl::GetNumTriangles
//------------------------------------------------------------------------------
/// \brief Gets the cell ID associated with the given triangle index.
/// \return the cell ID.
//------------------------------------------------------------------------------
int XmUGridTriangulatorImpl::GetCellFromTriangle(const int a_triangleIdx) const
{
  return m_triangleToCellIdx[a_triangleIdx];
} // XmUGridTriangulatorImpl::GetCellFromTriangle
} // namespace

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangulator
/// \brief Class to triangulate.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Build an instance of XmUGridTriangulator
/// \return The new instance.
//------------------------------------------------------------------------------
BSHP<XmUGridTriangulator> XmUGridTriangulator::New(const XmUGrid& a_ugrid)
{
  BSHP<XmUGridTriangulator> triangles(new XmUGridTriangulatorImpl(a_ugrid));
  return triangles;
} // XmUGridTriangulator::New
//------------------------------------------------------------------------------
/// \brief Default contructor.
//------------------------------------------------------------------------------
XmUGridTriangulator::XmUGridTriangulator()
{
} // XmUGridTriangulator::XmUGridTriangulator
//------------------------------------------------------------------------------
/// \brief Destructor.
//------------------------------------------------------------------------------
XmUGridTriangulator::~XmUGridTriangulator()
{
} // XmUGridTriangulator::XmUGridTriangulator
} // namespace xms
