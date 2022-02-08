//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsextractor/ugrid/XmUGridTriangles2d.h>

// 3. Standard library headers

// 4. External library headers

// 5. Shared code headers
#include <xmsgrid/geometry/GmTriSearch.h>
#include <xmsgrid/ugrid/XmUGrid.h>

// 6. Non-shared code headers
#include <xmsextractor/ugrid/XmUGridTriangulator.h>

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
class XmUGridTriangles2dImpl : public XmUGridTriangles2d
{
public:
  XmUGridTriangles2dImpl();

  virtual void BuildTriangles(const XmUGrid& a_ugrid, PointOptionEnum a_pointOption) override;
  virtual void BuildEarcutTriangles(const XmUGrid& a_ugrid) override;
  virtual void SetCellActivity(const DynBitset& a_cellActivity) override;

  virtual const VecPt3d& GetPoints() const override;
  virtual const VecInt& GetTriangles() const override;
  virtual BSHP<VecPt3d> GetPointsPtr() override;
  virtual BSHP<VecInt> GetTrianglesPtr() override;

  virtual int GetCellCentroid(int a_cellIdx) const override;

  virtual int GetIntersectedCell(const Pt3d& a_point, VecInt& a_idxs, VecDbl& a_weights) override;

private:
  void Initialize(const XmUGrid& a_ugrid);
  BSHP<GmTriSearch> GetTriSearch();

  BSHP<XmUGridTriangulator> m_triangulator; ///< Triangulator
  mutable BSHP<GmTriSearch> m_triSearch;    ///< Triangle searcher for triangles
};

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangles2dImpl
/// \brief Class to store XmUGrid triangles. Tracks where midpoints and
///        triangles came from.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Constructor
//------------------------------------------------------------------------------
XmUGridTriangles2dImpl::XmUGridTriangles2dImpl()
: m_triangulator()
, m_triSearch()
{
} // XmUGridTriangles2dImpl::XmUGridTriangles2dImpl
//------------------------------------------------------------------------------
/// \brief Generate triangles for the UGrid.
/// \param[in] a_ugrid The UGrid for which triangles are generated.
/// \param[in] a_addTriangleCenters Whether or not triangle cells get a centroid added.
//------------------------------------------------------------------------------
void XmUGridTriangles2dImpl::BuildTriangles(const XmUGrid& a_ugrid, PointOptionEnum a_pointOption)
{
  Initialize(a_ugrid);

  int numCells = a_ugrid.GetCellCount();
  VecInt cellPoints;
  bool createMidpoints = a_pointOption == PO_CENTROIDS_AND_MIDPOINTS;
  if (createMidpoints)
    m_triangulator->InitMidpoints();
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    if (a_ugrid.GetCellDimension(cellIdx) != 2)
      continue;
    a_ugrid.GetCellPoints(cellIdx, cellPoints);
    if (createMidpoints)
    {
      int nPts = (int)cellPoints.size();
      int i = 0;
      for (auto& it = cellPoints.begin(); it != cellPoints.end(); ++it)
      {
        int ip1 = (i + 1) % nPts;
        int id0 = cellPoints[i];
        int id1 = cellPoints[ip1];
        int idMid = m_triangulator->AddMidPoint(cellIdx, id0, id1);
        it = cellPoints.insert(it + 1, idMid);
        i += 2;
        nPts = (int)cellPoints.size();
      }
    }
    bool builtTriangles = false;
    if (a_pointOption != PO_NO_POINTS)
      builtTriangles = m_triangulator->GenerateCentroidTriangles(cellIdx, cellPoints);
    if (!builtTriangles)
      m_triangulator->BuildEarcutTriangles(cellIdx, cellPoints);
  }
} // XmUGridTriangles2dImpl::BuildTriangles
//------------------------------------------------------------------------------
/// \brief Generate triangles for the UGrid using earcut algorithm.
/// \param[in] a_ugrid The UGrid for which triangles are generated.
//------------------------------------------------------------------------------
void XmUGridTriangles2dImpl::BuildEarcutTriangles(const XmUGrid& a_ugrid)
{
  Initialize(a_ugrid);

  int numCells = a_ugrid.GetCellCount();
  VecInt cellPoints;
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    if (a_ugrid.GetCellDimension(cellIdx) != 2)
      continue;
    a_ugrid.GetCellPoints(cellIdx, cellPoints);
    m_triangulator->BuildEarcutTriangles(cellIdx, cellPoints);
  }
} // XmUGridTriangles2dImpl::BuildEarcutTriangles
//------------------------------------------------------------------------------
/// \brief Set triangle activity based on each triangles cell.
//------------------------------------------------------------------------------
void XmUGridTriangles2dImpl::SetCellActivity(const DynBitset& a_cellActivity)
{
  if (a_cellActivity.empty())
  {
    DynBitset emptyActivity;
    GetTriSearch()->SetTriActivity(emptyActivity);
    return;
  }

  DynBitset triangleActivity;
  int numTriangles = (int)m_triangulator->GetNumTriangles();
  triangleActivity.resize(numTriangles);
  for (int triangleIdx = 0; triangleIdx < numTriangles; ++triangleIdx)
  {
    int cellIdx = m_triangulator->GetCellFromTriangle(triangleIdx);
    triangleActivity[triangleIdx] = cellIdx >= a_cellActivity.size() || a_cellActivity[cellIdx];
  }
  GetTriSearch()->SetTriActivity(triangleActivity);
} // XmUGridTriangles2dImpl::SetCellActivity
//------------------------------------------------------------------------------
/// \brief Get the generated triangle points.
/// \return The triangle points
//------------------------------------------------------------------------------
const VecPt3d& XmUGridTriangles2dImpl::GetPoints() const
{
  return m_triangulator->GetPoints();
} // XmUGridTriangles2dImpl::GetPoints
//------------------------------------------------------------------------------
/// \brief Get the generated triangles.
/// \return a vector of indices for the triangles.
//------------------------------------------------------------------------------
const VecInt& XmUGridTriangles2dImpl::GetTriangles() const
{
  return m_triangulator->GetTriangles();
} // XmUGridTriangles2dImpl::GetTriangles
//------------------------------------------------------------------------------
/// \brief Get the generated triangle points as a shared pointer.
/// \return The triangle points
//------------------------------------------------------------------------------
BSHP<VecPt3d> XmUGridTriangles2dImpl::GetPointsPtr()
{
  return m_triangulator->GetPointsPtr();
} // XmUGridTriangles2dImpl::GetPointsPtr
//------------------------------------------------------------------------------
/// \brief Get the generated triangles as a shared pointer.
/// \return a vector of indices for the triangles.
//------------------------------------------------------------------------------
BSHP<VecInt> XmUGridTriangles2dImpl::GetTrianglesPtr()
{
  return m_triangulator->GetTrianglesPtr();
} // XmUGridTriangles2dImpl::GetTrianglesPtr
//------------------------------------------------------------------------------
/// \brief Get the centroid of a cell.
/// \param[in] a_cellIdx The cell index.
/// \return The index of the cell point.
//------------------------------------------------------------------------------
int XmUGridTriangles2dImpl::GetCellCentroid(int a_cellIdx) const
{
  return m_triangulator->GetCellCentroid(a_cellIdx);
} // XmUGridTriangles2dImpl::GetCellCentroid
//------------------------------------------------------------------------------
/// \brief Get the cell index and interpolation values intersected by a point.
/// \param[in] a_point The point to intersect with the UGrid.
/// \param[out] a_idxs The interpolation points.
/// \param[out] a_weights The interpolation weights.
/// \return The cell intersected by the point or -1 if outside of the UGrid.
//------------------------------------------------------------------------------
int XmUGridTriangles2dImpl::GetIntersectedCell(const Pt3d& a_point,
                                               VecInt& a_idxs,
                                               VecDbl& a_weights)
{
  if (!m_triSearch)
    GetTriSearch();
  int cellIdx = -1;
  int triangleLocation;
  if (m_triSearch->InterpWeightsTriangleIdx(a_point, triangleLocation, a_idxs, a_weights))
  {
    int triangleIdx = triangleLocation / 3;
    cellIdx = m_triangulator->GetCellFromTriangle(triangleIdx);
  }
  return cellIdx;
} // XmUGridTriangles2dImpl::GetIntersectedCell
//------------------------------------------------------------------------------
/// \brief Initialize triangulation for a UGrid.
/// \param[in] a_ugrid The UGrid for which triangles are generated.
//------------------------------------------------------------------------------
void XmUGridTriangles2dImpl::Initialize(const XmUGrid& a_ugrid)
{
  m_triangulator = XmUGridTriangulator::New(a_ugrid);
  m_triSearch.reset();
} // XmUGridTriangles2dImpl::Initialize
//------------------------------------------------------------------------------
/// \brief Get triangle search object.
//------------------------------------------------------------------------------
BSHP<GmTriSearch> XmUGridTriangles2dImpl::GetTriSearch()
{
  if (!m_triSearch)
  {
    m_triSearch = GmTriSearch::New();
    m_triSearch->TrisToSearch(GetPointsPtr(), GetTrianglesPtr());
  }
  return m_triSearch;
} // XmUGridTriangles2dImpl::GetTriSearch

} // namespace

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangles2d
/// \brief Class to store XmUGrid triangles. Tracks where midpoints and
///        triangles came from.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Build an instance of XmUGridTriangles2d
/// \return The new instance.
//------------------------------------------------------------------------------
BSHP<XmUGridTriangles2d> XmUGridTriangles2d::New()
{
  BSHP<XmUGridTriangles2d> triangles(new XmUGridTriangles2dImpl);
  return triangles;
} // XmUGridTriangles2d::New
//------------------------------------------------------------------------------
/// \brief Default contructor.
//------------------------------------------------------------------------------
XmUGridTriangles2d::XmUGridTriangles2d()
{
} // XmUGridTriangles2d::XmUGridTriangles2d
//------------------------------------------------------------------------------
/// \brief Destructor.
//------------------------------------------------------------------------------
XmUGridTriangles2d::~XmUGridTriangles2d()
{
} // XmUGridTriangles2d::XmUGridTriangles2d
} // namespace xms

#ifdef CXX_TEST
//------------------------------------------------------------------------------
// Unit Tests
//------------------------------------------------------------------------------
using namespace xms;
#include <xmsextractor/ugrid/XmUGridTriangles2d.t.h>

#include <xmscore/testing/TestTools.h>

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangles2dUnitTests
/// \brief Class to to test XmUGridTriangles2d
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Test creating triangles using cell centroid.
//------------------------------------------------------------------------------
void XmUGridTriangles2dUnitTests::testBuildCentroidTrianglesOnTriangle()
{
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {0.5, 1, 0}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2};
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  TS_REQUIRE_NOT_NULL(ugrid);
  XmUGridTriangles2dImpl triangles;

  // test building both cetroid and midpoints in triangle cell
  triangles.BuildTriangles(*ugrid, XmUGridTriangles2d::PO_CENTROIDS_AND_MIDPOINTS);

  VecPt3d triPointsOut = triangles.GetPoints();
  VecPt3d triPointsExpected = {{0, 0, 0},      {1, 0, 0},      {0.5, 1, 0},      {0.5, 0, 0},
                               {0.75, 0.5, 0}, {0.25, 0.5, 0}, {0.5, 1 / 3.0, 0}};
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

  VecInt trianglesOut = triangles.GetTriangles();
  VecInt trianglesExpected = {0, 3, 6, 3, 1, 6, 1, 4, 6, 4, 2, 6, 2, 5, 6, 5, 0, 6};
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);

  TS_ASSERT_EQUALS(6, triangles.GetCellCentroid(0));

  VecInt idxs;
  VecDbl weights;
  // point intersecting last triangle
  int cellIdx = triangles.GetIntersectedCell(Pt3d(0.25, 0.25, 0), idxs, weights);
  TS_ASSERT_EQUALS(0, cellIdx);
  VecInt idxsExpected = {5, 0, 6};
  TS_ASSERT_EQUALS(idxsExpected, idxs);
  VecDbl weightsExpected = {0.25, 0.375, 0.375};
  TS_ASSERT_EQUALS(weightsExpected, weights);
  cellIdx = triangles.GetIntersectedCell(Pt3d(0.5, 0.5, 0), idxs, weights);
  TS_ASSERT_EQUALS(0, cellIdx);
  idxsExpected = {4, 2, 6};
  TS_ASSERT_EQUALS(idxsExpected, idxs);
  weightsExpected = {0.0, 0.25, 0.75};
  TS_ASSERT_EQUALS(weightsExpected, weights);
  cellIdx = triangles.GetIntersectedCell(Pt3d(0.5, 0.25, 0), idxs, weights);
  TS_ASSERT_EQUALS(0, cellIdx);
  idxsExpected = {0, 3, 6};
  TS_ASSERT_EQUALS(idxsExpected, idxs);
  weightsExpected = {0.0, 0.25, 0.75};
  TS_ASSERT_EQUALS(weightsExpected, weights);

  // test building cetroid in triangle cell
  triangles.BuildTriangles(*ugrid, XmUGridTriangles2d::PO_CENTROIDS_ONLY);

  triPointsOut = triangles.GetPoints();
  triPointsExpected = {{0, 0, 0}, {1, 0, 0}, {0.5, 1, 0}, {0.5, 1 / 3.0, 0}};
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

  trianglesOut = triangles.GetTriangles();
  trianglesExpected = {0, 1, 3, 1, 2, 3, 2, 0, 3};
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);

  TS_ASSERT_EQUALS(3, triangles.GetCellCentroid(0));

  // point intersecting last triangle
  cellIdx = triangles.GetIntersectedCell(Pt3d(0.25, 0.25, 0), idxs, weights);
  TS_ASSERT_EQUALS(0, cellIdx);
  idxsExpected = {2, 0, 3};
  TS_ASSERT_EQUALS(idxsExpected, idxs);
  weightsExpected = {0.125, 0.5, 0.375};
  TS_ASSERT_EQUALS(weightsExpected, weights);
  cellIdx = triangles.GetIntersectedCell(Pt3d(0.75, 0.25, 0), idxs, weights);
  TS_ASSERT_EQUALS(0, cellIdx);
  idxsExpected = {1, 2, 3};
  TS_ASSERT_EQUALS(idxsExpected, idxs);
  weightsExpected = {0.5, 0.125, 0.375};
  TS_ASSERT_EQUALS(weightsExpected, weights);
  cellIdx = triangles.GetIntersectedCell(Pt3d(0.5, 0.25, 0), idxs, weights);
  TS_ASSERT_EQUALS(0, cellIdx);
  idxsExpected = {0, 1, 3};
  TS_ASSERT_EQUALS(idxsExpected, idxs);
  weightsExpected = {0.125, 0.125, 0.75};
  TS_ASSERT_EQUALS(weightsExpected, weights);

  // test without building centroid in triangle cell
  triangles.BuildTriangles(*ugrid, XmUGridTriangles2d::PO_NO_POINTS);

  triPointsOut = triangles.GetPoints();
  TS_ASSERT_EQUALS(points, triPointsOut);

  trianglesOut = triangles.GetTriangles();
  trianglesExpected = {0, 1, 2};
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);

  TS_ASSERT_EQUALS(-1, triangles.GetCellCentroid(0));

  cellIdx = triangles.GetIntersectedCell(Pt3d(0.5, 0.25, 0), idxs, weights);
  TS_ASSERT_EQUALS(0, cellIdx);
  idxsExpected = {0, 1, 2};
  TS_ASSERT_EQUALS(idxsExpected, idxs);
  weightsExpected = {0.375, 0.375, 0.25};
  TS_ASSERT_EQUALS(weightsExpected, weights);

} // XmUGridTriangles2dUnitTests::testBuildCentroidTrianglesOnTriangle
//------------------------------------------------------------------------------
/// \brief Test creating triangles on a quad using cell centroid.
//------------------------------------------------------------------------------
void XmUGridTriangles2dUnitTests::testBuildCentroidTrianglesOnQuad()
{
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  TS_REQUIRE_NOT_NULL(ugrid);
  XmUGridTriangles2dImpl triangles;
  triangles.BuildTriangles(*ugrid, XmUGridTriangles2d::PO_CENTROIDS_ONLY);
  VecPt3d triPointsOut = triangles.GetPoints();
  VecPt3d triPointsExpected = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0.5, 0.5, 0}};
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

  VecInt trianglesOut = triangles.GetTriangles();
  VecInt trianglesExpected = {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4};
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);

  TS_ASSERT_EQUALS(4, triangles.GetCellCentroid(0));
} // XmUGridTriangles2dUnitTests::testBuildCentroidTrianglesOnQuad
//------------------------------------------------------------------------------
/// \brief Test creating triangles using cell centroid for linear 2D cell types.
//------------------------------------------------------------------------------
void XmUGridTriangles2dUnitTests::testBuildCentroidTriangles2dCellTypes()
{
  // clang-format off
  VecPt3d points = {
    {0, 0, 0},   // 0
    {10, 0, 0},  // 1
    {20, 0, 0},  // 2
    {30, 0, 0},  // 3
    {40, 0, 0},  // 4
    {0, 10, 0},  // 5
    {10, 10, 0}, // 6
    {20, 10, 0}, // 7
    {40, 10, 0}, // 8
    {0, 20, 0},  // 9
    {10, 20, 0}, // 10
    {20, 20, 0}, // 11
    {30, 20, 0}, // 12
    {40, 20, 0}  // 13
  };

  // Cell type (5), number of points (3), point numbers, counterclockwise
  std::vector<int> cells = {(int)XMU_QUAD, 4, 0, 1, 6, 5,             // 0
                            (int)XMU_PIXEL, 4, 1, 2, 6, 7,            // 1
                            (int)XMU_TRIANGLE, 3, 2, 3, 7,            // 2
                            (int)XMU_POLYGON, 6, 3, 4, 8, 13, 12, 7}; // 3
  // clang-format on

  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  TS_REQUIRE_NOT_NULL(ugrid);
  XmUGridTriangles2dImpl triangles;
  triangles.BuildTriangles(*ugrid, XmUGridTriangles2d::PO_CENTROIDS_ONLY);

  VecPt3d triPointsOut = triangles.GetPoints();
  // clang-format off
  VecPt3d triPointsExpected = {
    {0.0, 0.0, 0.0},
    {10.0, 0.0, 0.0},
    {20.0, 0.0, 0.0},
    {30.0, 0.0, 0.0},
    {40.0, 0.0, 0.0},
    {0.0, 10.0, 0.0},
    {10.0, 10.0, 0.0},
    {20.0, 10.0, 0.0},
    {40.0, 10.0, 0.0},
    {0.0, 20.0, 0.0},
    {10.0, 20.0, 0.0},
    {20.0, 20.0, 0.0},
    {30.0, 20.0, 0.0},
    {40.0, 20.0, 0.0},

    // added centroids
    {5.0, 5.0, 0.0},                               // 14 - quad centroid
    {15.0, 5.0, 0.0},                              // 15 - pixel centroid
    {23.333333333333332, 3.3333333333333335, 0.0}, // 16 - triangle centroid
    {33.333333333333336, 10.0, 0.0}                // 17 - polygon centroid
  };
  // clang-format on
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

  VecInt trianglesOut = triangles.GetTriangles();
  // clang-format off
  VecInt trianglesExpected = {
    0, 1, 14, 1, 6, 14, 6, 5, 14, 5, 0, 14, // quad
    1, 2, 15, 2, 7, 15, 7, 6, 15, 6, 1, 15, // pixel
    2, 3, 16, 3, 7, 16, 7, 2, 16,           // triangle
    3, 4, 17, 4, 8, 17, 8, 13, 17, 13, 12, 17, 12, 7, 17, 7, 3, 17}; // polygon
  // clang-format on
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);
} // XmUGridTriangles2dUnitTests::testBuildCentroidTriangles2dCellTypes
//------------------------------------------------------------------------------
/// \brief Test creating plan view 2D triangles using ear cut algorithm.
//------------------------------------------------------------------------------
void XmUGridTriangles2dUnitTests::testBuildEarcutTriangles()
{
  {
    // V shaped cell
    // clang-format off
    VecPt3d points = {
      {0, 0, 0},   // 0
      {10, 0, 0},  // 1
      {1, 1, 0},   // 2
      {0, 10, 0},  // 3
      {10, 10, 0}  // 4
    };

    std::vector<int> cells = {
      XMU_QUAD, 4, 0, 1, 2, 3,
    };
    // clang-format on

    std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
    XmUGridTriangles2dImpl ugridTris;

    ugridTris.BuildTriangles(*ugrid, XmUGridTriangles2d::PO_CENTROIDS_ONLY);

    VecPt3d triPointsOut = ugridTris.GetPoints();
    VecPt3d triPointsExpected = points;
    TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

    VecInt trianglesOut = ugridTris.GetTriangles();
    VecInt trianglesExpected = {2, 3, 0, 0, 1, 2};
    TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);
  }

  {
    // house shaped cell
    // clang-format off
    VecPt3d points = {
      {0, 0, 0},   // 0
      {10, 0, 0},  // 1
      {10, 10, 0}, // 2
      {5, 11, 0},  // 3
      {0, 10, 0},  // 4
    };

    std::vector<int> cells = {(int)XMU_POLYGON, 5, 0, 1, 2, 3, 4};
    // clang-format on

    std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
    XmUGridTriangles2dImpl ugridTris;
    ugridTris.BuildEarcutTriangles(*ugrid);

    VecPt3d triPointsOut = ugridTris.GetPoints();
    VecPt3d triPointsExpected = points;
    TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

    VecInt trianglesOut = ugridTris.GetTriangles();
    VecInt trianglesExpected = {4, 0, 1, 1, 2, 3, 1, 3, 4};
    TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);
  }
} // XmUGridTriangles2dUnitTests::testBuildEarcutTriangles
//------------------------------------------------------------------------------
/// \brief Test creating triangles for centroid and earcut cells.
//------------------------------------------------------------------------------
void XmUGridTriangles2dUnitTests::testBuildCentroidAndEarcutTriangles()
{
  // 7----6---------3----2
  // |    |         |    |
  // |    |         |    |
  // |    |         |    |
  // |    |   (8)   |    |
  // |    |         |    |
  // |    |         |    |
  // |    |         |    |
  // |    5---------4    |
  // |                   |
  // |                   |
  // |                   |
  // 0-------------------1
  // clang-format off
  VecPt3d points = {
    { 0,  0, 0}, // 0
    {15,  0, 0}, // 1
    {15, 15, 0}, // 2
    {10, 15, 0}, // 3
    {10,  5, 0}, // 4
    { 5,  5, 0}, // 5
    { 5, 15, 0}, // 6
    { 0, 15, 0}  // 7
  };

  // Cell type (5), number of points (3), point numbers, counterclockwise
  std::vector<int> cells = {
    XMU_POLYGON, 8, 0, 1, 2, 3, 4, 5, 6, 7,
    XMU_QUAD, 4, 3, 6, 5, 4
  };
  // clang-format on

  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  XmUGridTriangles2dImpl ugridTris;

  ugridTris.BuildTriangles(*ugrid, XmUGridTriangles2d::PO_CENTROIDS_ONLY);

  VecPt3d triPointsOut = ugridTris.GetPoints();
  VecPt3d triPointsExpected = points;
  triPointsExpected.push_back({7.5, 10, 0});
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

  VecInt trianglesOut = ugridTris.GetTriangles();
  VecInt trianglesExpected = {
    // clang-format off
    2, 3, 4,   5, 6, 7,   1, 2, 4,   0, 1, 4,   0, 4, 5,   0, 5, 7, // earcut
    3, 6, 8,   6, 5, 8,   5, 4, 8,   4, 3, 8 // centroid
    // clang-format off
  };
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);
} // XmUGridTriangles2dUnitTests::testBuildCentroidAndEarcutTriangles

#endif
