//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018.
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsextractor/ugrid/XmUGridTriangles2d.h>

// 3. Standard library headers
#include <sstream>

// 4. External library headers

// 5. Shared code headers
#include <xmscore/misc/XmError.h>
#include <xmscore/misc/XmLog.h>
#include <xmsinterp/geometry/geoms.h>
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
//------------------------------------------------------------------------------
/// \brief Calculate the magnitude of a vector.
/// \param[in] a_vec: the vector
//------------------------------------------------------------------------------
double iMagnitude(const Pt3d& a_vec)
{
  double magnitude = sqrt(a_vec.x * a_vec.x + a_vec.y * a_vec.y + a_vec.z * a_vec.z);
  return magnitude;
} // iMagnitude
//------------------------------------------------------------------------------
/// \brief Calculate a quality ratio to use to determine which triangles to cut
///        using earcut triangulation. Better triangles have a lower ratio.
/// \param[in] a_points: the array the points come from
/// \param[in] a_idx1: first point of first edge
/// \param[in] a_idx2: second point of first edge, first point of second edge
/// \param[in] a_idx2: second point of second edge
/// \return The ratio, -1.0 for an inverted triangle, or -2.0 for a zero area
///         triangle
//------------------------------------------------------------------------------
double iGetEarcutTriangleRatio(const VecPt3d& a_points, int a_idx1, int a_idx2, int a_idx3)
{
  Pt3d pt1 = a_points[a_idx1];
  Pt3d pt2 = a_points[a_idx2];
  Pt3d pt3 = a_points[a_idx3];
  Pt3d v1 = pt1 - pt2;
  Pt3d v2 = pt3 - pt2;
  Pt3d v3 = pt3 - pt1;

  // get 2*area
  Pt3d cross;
  gmCross3D(v2, v1, &cross);
  double ratio;
  if (cross.z <= 0.0)
  {
    // inverted
    ratio = -1.0;
  }
  else
  {
    double area2 = iMagnitude(cross);
    if (area2 == 0.0)
    {
      // degenerate triangle
      ratio = -2.0;
    }
    else
    {
      double perimeter = iMagnitude(v1) + iMagnitude(v2) + iMagnitude(v3);
      ratio = perimeter * perimeter / area2;
    }
  }

  return ratio;
} // iGetEarcutTriangleRatio
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
bool iValidTriangle(const VecPt3d& a_points,
                    const VecInt a_polygon,
                    int a_idx1,
                    int a_idx2,
                    int a_idx3)
{
  const Pt3d& pt1 = a_points[a_idx1];
  const Pt3d& pt2 = a_points[a_idx2];
  const Pt3d& pt3 = a_points[a_idx3];
  for (size_t pointIdx = 0; pointIdx < a_polygon.size(); ++pointIdx)
  {
    int idx = a_polygon[pointIdx];
    if (idx != a_idx1 && idx != a_idx2 && idx != a_idx3)
    {
      const Pt3d& pt = a_points[idx];
      if (gmTurn(pt1, pt2, pt, 0.0) == TURN_LEFT && gmTurn(pt2, pt3, pt) == TURN_LEFT &&
          gmTurn(pt3, pt1, pt, 0.0) == TURN_LEFT)
      {
        return false;
      }
    }
  }

  return true;
} // iValidTriangle
//------------------------------------------------------------------------------
/// \brief Attempt to generate triangles for a cell by adding a point at the
///        centroid.
/// \param[in] a_ugridTris The triangles to add to.
/// \param[in] a_cellIdx The cell index.
/// \param[in] a_polygonIdxs The indices for the cell polygon.
/// \return true on success (can fail for concave cell)
//------------------------------------------------------------------------------
bool iGenerateCentroidTriangles(XmUGridTriangles& a_ugridTris,
                                int a_cellIdx,
                                const VecInt& a_polygonIdxs)
{
  const VecPt3d& points = a_ugridTris.GetPoints();
  size_t numPoints = a_polygonIdxs.size();

  VecPt3d polygon;
  for (size_t pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    polygon.push_back(points[a_polygonIdxs[pointIdx]]);

  Pt3d centroid = gmComputeCentroid(polygon);

  // make sure none of the triangles connected to the centroid are inverted
  for (size_t pointIdx = 0; pointIdx < polygon.size(); ++pointIdx)
  {
    const Pt3d& pt1 = polygon[pointIdx];
    const Pt3d& pt2 = polygon[(pointIdx + 1) % numPoints];

    // centroid should be to the left of each edge
    if (gmTurn(pt1, pt2, centroid, 0.0) != TURN_LEFT)
      return false;
  }

  // add centroid to list of points
  int centroidIdx = a_ugridTris.AddCellCentroid(a_cellIdx, centroid);

  // add triangles
  for (size_t pointIdx = 0; pointIdx < polygon.size(); ++pointIdx)
  {
    int idx1 = a_polygonIdxs[pointIdx];
    int idx2 = a_polygonIdxs[(pointIdx + 1) % numPoints];
    a_ugridTris.AddCellTriangle(a_cellIdx, idx1, idx2, centroidIdx);
  }

  return true;
} // iGenerateCentroidTriangles
//------------------------------------------------------------------------------
/// \brief Generate triangles using ear cut algorithm for plan view 2D cells.
/// \param[in] a_ugridTris The triangles to add to.
/// \param[in] a_cellIdx The cell index.
/// \param[in] a_polygonIdxs The indices for the cell polygon.
//------------------------------------------------------------------------------
void iBuildEarcutTriangles(XmUGridTriangles& a_ugridTris,
                           int a_cellIdx,
                           const VecInt& a_polygonIdxs)
{
  VecInt polygonIdxs = a_polygonIdxs;
  const VecPt3d& points = a_ugridTris.GetPoints();

  // continually find best triangle on adjacent edges and cut it off polygon
  while (polygonIdxs.size() >= 4)
  {
    int bestIdx = -1;
    int secondBestIdx = -1;

    int numPoints = (int)polygonIdxs.size();
    double bestRatio = std::numeric_limits<double>::max();
    double secondBestRatio = std::numeric_limits<double>::max();
    for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    {
      int idx1 = polygonIdxs[(pointIdx + numPoints - 1) % numPoints];
      int idx2 = polygonIdxs[pointIdx];
      int idx3 = polygonIdxs[(pointIdx + 1) % numPoints];

      // make sure triangle is valid (not inverted and doesn't have other points in it)
      double ratio = iGetEarcutTriangleRatio(points, idx1, idx2, idx3);
      if (ratio > 0.0)
      {
        if (iValidTriangle(points, polygonIdxs, idx1, idx2, idx3))
        {
          if (ratio < bestRatio)
          {
            secondBestRatio = bestRatio;
            bestRatio = ratio;
            secondBestIdx = bestIdx;
            bestIdx = pointIdx;
          }
          else if (ratio < secondBestRatio)
          {
            secondBestRatio = ratio;
            secondBestIdx = pointIdx;
          }
        }
      }
    }

    if (bestIdx >= 0)
    {
      if (numPoints == 4 && secondBestIdx >= 0)
        bestIdx = secondBestIdx;

      // cut off the ear triangle
      int polygonIdx1 = (bestIdx + numPoints - 1) % numPoints;
      int polygonIdx2 = bestIdx;
      int polygonIdx3 = (bestIdx + 1) % numPoints;
      int idx1 = polygonIdxs[polygonIdx1];
      int idx2 = polygonIdxs[polygonIdx2];
      int idx3 = polygonIdxs[polygonIdx3];
      a_ugridTris.AddCellTriangle(a_cellIdx, idx1, idx2, idx3);
      polygonIdxs.erase(polygonIdxs.begin() + polygonIdx2);
    }
    else
    {
      std::ostringstream ss;
      ss << "Unable to split cell number " << a_cellIdx + 1 << " into triangles.";
      XM_LOG(xmlog::error, ss.str());
      return;
    }
  }

  // push on remaining triangle
  a_ugridTris.AddCellTriangle(a_cellIdx, polygonIdxs[0], polygonIdxs[1], polygonIdxs[2]);
} // iBuildEarcutTriangles

} // namespace

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangles
/// \brief Class to store XmUGrid triangles. Tracks where midpoints and
///        triangles came from.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Constructor
//------------------------------------------------------------------------------
XmUGridTriangles::XmUGridTriangles()
: m_points(new VecPt3d)
, m_triangles(new VecInt)
, m_centroidIdxs()
, m_triangleToCellIdx()
{
} // XmUGridTriangles::XmUGridTriangles
//------------------------------------------------------------------------------
/// \brief Initialize triangulation for a UGrid.
/// \param[in] a_ugrid The UGrid for which triangles are generated.
//------------------------------------------------------------------------------
void XmUGridTriangles::Initialize(const XmUGrid& a_ugrid)
{
  *m_points = a_ugrid.GetPoints();
  m_triangles->clear();
  m_centroidIdxs.assign(a_ugrid.GetNumberOfCells(), -1);
} // XmUGridTriangles::Initialize
//------------------------------------------------------------------------------
/// \brief Generate triangles for the UGrid.
/// \param[in] a_ugrid The UGrid for which triangles are generated.
/// \param[in] a_addTriangleCenters Whether or not triangle cells get a centroid added.
//------------------------------------------------------------------------------
void XmUGridTriangles::BuildTriangles(const XmUGrid& a_ugrid, bool a_addTriangleCenters)
{
  Initialize(a_ugrid);

  int numCells = a_ugrid.GetNumberOfCells();
  VecInt cellPoints;
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    bool triangulate = a_ugrid.GetCellType(cellIdx) != XMU_TRIANGLE || a_addTriangleCenters;
    if (triangulate)
    {
      a_ugrid.GetPointsOfCell(cellIdx, cellPoints);
      if (!iGenerateCentroidTriangles(*this, cellIdx, cellPoints))
        iBuildEarcutTriangles(*this, cellIdx, cellPoints);
    }
  }
} // XmUGridTriangles::BuildTriangles
//------------------------------------------------------------------------------
/// \brief Generate triangles for the UGrid using earcut algorithm.
/// \param[in] a_ugrid The UGrid for which triangles are generated.
//------------------------------------------------------------------------------
void XmUGridTriangles::BuildEarcutTriangles(const XmUGrid& a_ugrid)
{
  Initialize(a_ugrid);

  int numCells = a_ugrid.GetNumberOfCells();
  VecInt cellPoints;
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    a_ugrid.GetPointsOfCell(cellIdx, cellPoints);
    iBuildEarcutTriangles(*this, cellIdx, cellPoints);
  }
} // XmUGridTriangles::BuildEarcutTriangles
//------------------------------------------------------------------------------
/// \brief Get the generated triangle points.
/// \return The triangle points
//------------------------------------------------------------------------------
const VecPt3d& XmUGridTriangles::GetPoints() const
{
  return *m_points;
} // XmUGridTriangles::GetPoints
//------------------------------------------------------------------------------
/// \brief Get the generated triangles.
/// \return a vector of indices for the triangles.
//------------------------------------------------------------------------------
const VecInt& XmUGridTriangles::GetTriangles() const
{
  return *m_triangles;
} // XmUGridTriangles::GetTriangles
//------------------------------------------------------------------------------
/// \brief Get the generated triangle points as a shared pointer.
/// \return The triangle points
//------------------------------------------------------------------------------
BSHP<VecPt3d> XmUGridTriangles::GetPointsPtr()
{
  return m_points;
} // XmUGridTriangles::GetPointsPtr
//------------------------------------------------------------------------------
/// \brief Get the generated triangles as a shared pointer.
/// \return a vector of indices for the triangles.
//------------------------------------------------------------------------------
BSHP<VecInt> XmUGridTriangles::GetTrianglesPtr()
{
  return m_triangles;
} // XmUGridTriangles::GetTrianglesPtr
//------------------------------------------------------------------------------
/// \brief Add a cell centroid point.
/// \param a_cellIdx The cell index for the centroid point
/// \param a_point The centroid point
/// \return The index of the added point
//------------------------------------------------------------------------------
int XmUGridTriangles::AddCellCentroid(int a_cellIdx, const Pt3d& a_point)
{
  int centroidIdx = (int)m_points->size();
  m_points->push_back(a_point);
  m_centroidIdxs[a_cellIdx] = centroidIdx;
  return centroidIdx;
} // XmUGridTriangles::AddCellCentroid
//------------------------------------------------------------------------------
/// \brief Add a triangle cell.
/// \param[in] a_cellIdx The cell index the triangle is from
/// \param[in] a_idx1 The first triangle point index (counter clockwise)
/// \param[in] a_idx2 The second triangle point index (counter clockwise)
/// \param[in] a_idx3 The third triangle point index (counter clockwise)
//------------------------------------------------------------------------------
void XmUGridTriangles::AddCellTriangle(int a_cellIdx, int a_idx1, int a_idx2, int a_idx3)
{
  m_triangles->push_back(a_idx1);
  m_triangles->push_back(a_idx2);
  m_triangles->push_back(a_idx3);
  m_triangleToCellIdx.push_back(a_cellIdx);
} // XmUGridTriangles::AddCellTriangle
//------------------------------------------------------------------------------
/// \brief Get the centroid of a cell.
/// \param[in] a_cellIdx The cell index.
/// \return The index of the cell point.
//------------------------------------------------------------------------------
int XmUGridTriangles::GetCellCentroid(int a_cellIdx) const
{
  int pointIdx = -1;
  if (a_cellIdx >= 0 || a_cellIdx < (int)m_centroidIdxs.size())
    pointIdx = m_centroidIdxs[a_cellIdx];
  return pointIdx;
} // XmUGridTriangles::GetCellCentroid
//------------------------------------------------------------------------------
/// \brief Get vector of cell centroid point indeces.
/// \return The vector of cell centroid point indeces.
//------------------------------------------------------------------------------
const VecInt& XmUGridTriangles::GetCellCentroids() const
{
  return m_centroidIdxs;
} // XmUGridTriangles::GetCellCentroids

} // namespace xms

#ifdef CXX_TEST
//------------------------------------------------------------------------------
// Unit Tests
//------------------------------------------------------------------------------
using namespace xms;
#include <xmsextractor/ugrid/XmUGridTriangles2d.t.h>
#include <xmsgrid/ugrid/XmUGrid.t.h>

#include <xmscore/testing/TestTools.h>

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangles2dUnitTests
/// \brief Class to to test XmUGridTriangles2d
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Test creating triangles using cell centroid.
//------------------------------------------------------------------------------
void XmUGridTriangles2dUnitTests::testBuildCentroidTriangles()
{
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  TS_REQUIRE_NOT_NULL(ugrid);
  XmUGridTriangles triangles;
  triangles.BuildTriangles(*ugrid, true);
  VecPt3d triPointsOut = triangles.GetPoints();
  VecPt3d triPointsExpected = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0.5, 0.5, 0}};
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

  VecInt trianglesOut = triangles.GetTriangles();
  VecInt trianglesExpected = {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4};
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);

  TS_ASSERT_EQUALS(4, triangles.GetCellCentroid(0));
  VecInt cellCentroids = triangles.GetCellCentroids();
  VecInt expectedCellCentroids = { 4 };
  TS_ASSERT_EQUALS(expectedCellCentroids, cellCentroids);
} // XmUGridTriangles2dUnitTests::testBuildCentroidTriangles
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

  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  TS_REQUIRE_NOT_NULL(ugrid);
  XmUGridTriangles triangles;
  triangles.BuildTriangles(*ugrid, true);

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
    // clang-format off
    VecPt3d points = {
      {0, 0, 0},   // 0
      {10, 0, 0},  // 1
      {1, 1, 0},   // 2
      {0, 10, 0},  // 3
    };

    // Cell type (5), number of points (3), point numbers, counterclockwise
    std::vector<int> cells = {(int)XMU_QUAD, 4, 0, 1, 2, 3};
    // clang-format on

    BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
    XmUGridTriangles ugridTris;

    ugridTris.BuildEarcutTriangles(*ugrid);

    VecPt3d triPointsOut = ugridTris.GetPoints();
    VecPt3d triPointsExpected = points;
    TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

    VecInt trianglesOut = ugridTris.GetTriangles();
    VecInt trianglesExpected = {2, 3, 0, 0, 1, 2};
    TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);
  }

  {
    // clang-format off
    VecPt3d points = {
      {0, 0, 0},   // 0
      {10, 0, 0},  // 1
      {10, 10, 0}, // 2
      {5, 11, 0},  // 3
      {0, 10, 0},  // 4
    };

    // Cell type (5), number of points (3), point numbers, counterclockwise
    std::vector<int> cells = {(int)XMU_POLYGON, 5, 0, 1, 2, 3, 4};
    // clang-format on

    BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
    XmUGridTriangles ugridTris;
    ugridTris.BuildEarcutTriangles(*ugrid);

    VecPt3d triPointsOut = ugridTris.GetPoints();
    VecPt3d triPointsExpected = points;
    TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

    VecInt trianglesOut = ugridTris.GetTriangles();
    VecInt trianglesExpected = {4, 0, 1, 1, 2, 3, 1, 3, 4};
    TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);
  }
} // XmUGridTriangles2dUnitTests::testBuildEarcutTriangles

#endif
