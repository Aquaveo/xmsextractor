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
#include <xmsinterp/geometry/GmTriSearch.h>
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

class XmUGridTrianglesImpl : public XmUGridTriangles
{
public:
  XmUGridTrianglesImpl();

  virtual void BuildTriangles(const XmUGrid& a_ugrid, bool a_addTriangleCenters) override;
  virtual void BuildEarcutTriangles(const XmUGrid& a_ugrid) override;
  virtual void SetCellActivity(const DynBitset& a_cellActivity) override;

  virtual const VecPt3d& GetPoints() const override;
  virtual const VecInt& GetTriangles() const override;
  virtual BSHP<VecPt3d> GetPointsPtr() override;
  virtual BSHP<VecInt> GetTrianglesPtr() override;

  int AddCellCentroid(int a_cellIdx, const Pt3d& a_point);
  void AddCellTriangle(int a_cellIdx, int a_idx1, int a_idx2, int a_idx3);

  virtual int GetCellCentroid(int a_cellIdx) const override;

  virtual int GetIntersectedCell(const Pt3d& a_point, VecInt& a_idxs, VecDbl& a_weights) override;

private:
  void Initialize(const XmUGrid& a_ugrid);
  BSHP<GmTriSearch> GetTriSearch();

  BSHP<VecPt3d> m_points;      ///< Triangle points for the UGrid
  BSHP<VecInt> m_triangles;    ///< Triangles for the UGrid
  VecInt m_centroidIdxs;       ///< Index of each cell centroid or -1 if none
  VecInt m_triangleToCellIdx;  ///< The cell index for each triangle
  mutable BSHP<GmTriSearch> m_triSearch; ///< Triangle searcher for triangles
};

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
bool iGenerateCentroidTriangles(XmUGridTrianglesImpl& a_ugridTris,
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
void iBuildEarcutTriangles(XmUGridTrianglesImpl& a_ugridTris,
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
/// \class XmUGridTrianglesImpl
/// \brief Class to store XmUGrid triangles. Tracks where midpoints and
///        triangles came from.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Constructor
//------------------------------------------------------------------------------
XmUGridTrianglesImpl::XmUGridTrianglesImpl()
: m_points(new VecPt3d)
, m_triangles(new VecInt)
, m_centroidIdxs()
, m_triangleToCellIdx()
, m_triSearch()
{
} // XmUGridTrianglesImpl::XmUGridTrianglesImpl
//------------------------------------------------------------------------------
/// \brief Generate triangles for the UGrid.
/// \param[in] a_ugrid The UGrid for which triangles are generated.
/// \param[in] a_addTriangleCenters Whether or not triangle cells get a centroid added.
//------------------------------------------------------------------------------
void XmUGridTrianglesImpl::BuildTriangles(const XmUGrid& a_ugrid, bool a_addTriangleCenters)
{
  Initialize(a_ugrid);

  int numCells = a_ugrid.GetNumberOfCells();
  VecInt cellPoints;
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    a_ugrid.GetPointsOfCell(cellIdx, cellPoints);
    if (a_ugrid.GetCellType(cellIdx) == XMU_TRIANGLE && !a_addTriangleCenters)
    {
      AddCellTriangle(cellIdx, cellPoints[0], cellPoints[1], cellPoints[2]);
    }
    else
    {
      if (!iGenerateCentroidTriangles(*this, cellIdx, cellPoints))
        iBuildEarcutTriangles(*this, cellIdx, cellPoints);
    }
  }
} // XmUGridTrianglesImpl::BuildTriangles
//------------------------------------------------------------------------------
/// \brief Generate triangles for the UGrid using earcut algorithm.
/// \param[in] a_ugrid The UGrid for which triangles are generated.
//------------------------------------------------------------------------------
void XmUGridTrianglesImpl::BuildEarcutTriangles(const XmUGrid& a_ugrid)
{
  Initialize(a_ugrid);

  int numCells = a_ugrid.GetNumberOfCells();
  VecInt cellPoints;
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    a_ugrid.GetPointsOfCell(cellIdx, cellPoints);
    iBuildEarcutTriangles(*this, cellIdx, cellPoints);
  }
} // XmUGridTrianglesImpl::BuildEarcutTriangles
//------------------------------------------------------------------------------
/// \brief Set triangle activity based on each triangles cell.
//------------------------------------------------------------------------------
void XmUGridTrianglesImpl::SetCellActivity(const DynBitset& a_cellActivity)
{
  DynBitset triangleActivity;
  int numTriangles = (int)m_triangleToCellIdx.size();
  triangleActivity.resize(numTriangles);
  for (size_t triangleIdx = 0; triangleIdx < numTriangles; ++triangleIdx)
  {
    int cellIdx = m_triangleToCellIdx[triangleIdx];
    triangleActivity[triangleIdx] = a_cellActivity[cellIdx];
  }
  GetTriSearch()->SetTriActivity(triangleActivity);
} // XmUGridTrianglesImpl::SetCellActivity
//------------------------------------------------------------------------------
/// \brief Get the generated triangle points.
/// \return The triangle points
//------------------------------------------------------------------------------
const VecPt3d& XmUGridTrianglesImpl::GetPoints() const
{
  return *m_points;
} // XmUGridTrianglesImpl::GetPoints
//------------------------------------------------------------------------------
/// \brief Get the generated triangles.
/// \return a vector of indices for the triangles.
//------------------------------------------------------------------------------
const VecInt& XmUGridTrianglesImpl::GetTriangles() const
{
  return *m_triangles;
} // XmUGridTrianglesImpl::GetTriangles
//------------------------------------------------------------------------------
/// \brief Get the generated triangle points as a shared pointer.
/// \return The triangle points
//------------------------------------------------------------------------------
BSHP<VecPt3d> XmUGridTrianglesImpl::GetPointsPtr()
{
  return m_points;
} // XmUGridTrianglesImpl::GetPointsPtr
//------------------------------------------------------------------------------
/// \brief Get the generated triangles as a shared pointer.
/// \return a vector of indices for the triangles.
//------------------------------------------------------------------------------
BSHP<VecInt> XmUGridTrianglesImpl::GetTrianglesPtr()
{
  return m_triangles;
} // XmUGridTrianglesImpl::GetTrianglesPtr
//------------------------------------------------------------------------------
/// \brief Add a cell centroid point.
/// \param a_cellIdx The cell index for the centroid point
/// \param a_point The centroid point
/// \return The index of the added point
//------------------------------------------------------------------------------
int XmUGridTrianglesImpl::AddCellCentroid(int a_cellIdx, const Pt3d& a_point)
{
  int centroidIdx = (int)m_points->size();
  m_points->push_back(a_point);
  m_centroidIdxs[a_cellIdx] = centroidIdx;
  return centroidIdx;
} // XmUGridTrianglesImpl::AddCellCentroid
//------------------------------------------------------------------------------
/// \brief Add a triangle cell.
/// \param[in] a_cellIdx The cell index the triangle is from
/// \param[in] a_idx1 The first triangle point index (counter clockwise)
/// \param[in] a_idx2 The second triangle point index (counter clockwise)
/// \param[in] a_idx3 The third triangle point index (counter clockwise)
//------------------------------------------------------------------------------
void XmUGridTrianglesImpl::AddCellTriangle(int a_cellIdx, int a_idx1, int a_idx2, int a_idx3)
{
  m_triangles->push_back(a_idx1);
  m_triangles->push_back(a_idx2);
  m_triangles->push_back(a_idx3);
  m_triangleToCellIdx.push_back(a_cellIdx);
} // XmUGridTrianglesImpl::AddCellTriangle
//------------------------------------------------------------------------------
/// \brief Get the centroid of a cell.
/// \param[in] a_cellIdx The cell index.
/// \return The index of the cell point.
//------------------------------------------------------------------------------
int XmUGridTrianglesImpl::GetCellCentroid(int a_cellIdx) const
{
  int pointIdx = -1;
  if (a_cellIdx >= 0 || a_cellIdx < (int)m_centroidIdxs.size())
    pointIdx = m_centroidIdxs[a_cellIdx];
  return pointIdx;
} // XmUGridTrianglesImpl::GetCellCentroid
//------------------------------------------------------------------------------
/// \brief 
//------------------------------------------------------------------------------
int XmUGridTrianglesImpl::GetIntersectedCell(const Pt3d& a_point, VecInt& a_idxs, VecDbl& a_weights)
{
  if (!m_triSearch)
    GetTriSearch();
  int cellIdx = -1;
  int triangleLocation;
  if (m_triSearch->InterpWeightsTriangleIdx(a_point, triangleLocation, a_idxs, a_weights))
  {
    int triangleIdx = triangleLocation / 3;
    cellIdx = m_triangleToCellIdx[triangleIdx];
  }
  return cellIdx;
} // XmUGridTrianglesImpl::GetIntersectedCell
//------------------------------------------------------------------------------
/// \brief Initialize triangulation for a UGrid.
/// \param[in] a_ugrid The UGrid for which triangles are generated.
//------------------------------------------------------------------------------
void XmUGridTrianglesImpl::Initialize(const XmUGrid& a_ugrid)
{
  *m_points = a_ugrid.GetPoints();
  m_triangles->clear();
  m_centroidIdxs.assign(a_ugrid.GetNumberOfCells(), -1);
  m_triangleToCellIdx.clear();
  m_triSearch.reset();
} // XmUGridTrianglesImpl::Initialize
//------------------------------------------------------------------------------
/// \brief Get triangle search object.
//------------------------------------------------------------------------------
BSHP<GmTriSearch> XmUGridTrianglesImpl::GetTriSearch()
{
  if (!m_triSearch)
  {
    m_triSearch = GmTriSearch::New();
    m_triSearch->TrisToSearch(m_points, m_triangles);
  }
  return m_triSearch;
} // XmUGridTrianglesImpl::GetTriSearch
////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangles
/// \brief Class to store XmUGrid triangles. Tracks where midpoints and
///        triangles came from.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Build an instance of XmUGridTriangles
//------------------------------------------------------------------------------
BSHP<XmUGridTriangles> XmUGridTriangles::New()
{
  BSHP<XmUGridTriangles> triangles(new XmUGridTrianglesImpl);
  return triangles;
} // XmUGridTriangles::New
//------------------------------------------------------------------------------
/// \brief Default contructor.
//------------------------------------------------------------------------------
XmUGridTriangles::XmUGridTriangles()
{
} // XmUGridTriangles::XmUGridTriangles
//------------------------------------------------------------------------------
/// \brief Destructor.
//------------------------------------------------------------------------------
XmUGridTriangles::~XmUGridTriangles()
{
} // XmUGridTriangles::XmUGridTriangles

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
void XmUGridTriangles2dUnitTests::testBuildCentroidTrianglesOnTriangle()
{
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {0.5, 1, 0}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  TS_REQUIRE_NOT_NULL(ugrid);
  XmUGridTrianglesImpl triangles;

  // test building cetroid in triangle cell
  triangles.BuildTriangles(*ugrid, true);
  
  VecPt3d triPointsOut = triangles.GetPoints();
  VecPt3d triPointsExpected = {{0, 0, 0}, {1, 0, 0}, {0.5, 1, 0}, {0.5, 1/3.0, 0}};
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

  VecInt trianglesOut = triangles.GetTriangles();
  VecInt trianglesExpected = {0, 1, 3, 1, 2, 3, 2, 0, 3};
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);

  TS_ASSERT_EQUALS(3, triangles.GetCellCentroid(0));

  VecInt idxs;
  VecDbl weights;
  // point intersecting last triangle
  int cellIdx = triangles.GetIntersectedCell(Pt3d(0.25, 0.25, 0), idxs, weights);
  TS_ASSERT_EQUALS(0, cellIdx);
  VecInt idxsExpected = {2, 0, 3};
  TS_ASSERT_EQUALS(idxsExpected, idxs);
  VecDbl weightsExpected = {0.125, 0.5, 0.375};
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
  triangles.BuildTriangles(*ugrid, false);

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
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  TS_REQUIRE_NOT_NULL(ugrid);
  XmUGridTrianglesImpl triangles;
  triangles.BuildTriangles(*ugrid, true);
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

  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  TS_REQUIRE_NOT_NULL(ugrid);
  XmUGridTrianglesImpl triangles;
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

    BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
    XmUGridTrianglesImpl ugridTris;

    ugridTris.BuildTriangles(*ugrid, true);

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

    BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
    XmUGridTrianglesImpl ugridTris;
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

  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  XmUGridTrianglesImpl ugridTris;

  ugridTris.BuildTriangles(*ugrid, true);

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
