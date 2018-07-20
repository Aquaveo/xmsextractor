//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018.
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsextractor/extractor/XmUGrid2dDataExtractor.h>

// 3. Standard library headers
#include <sstream>

// 4. External library headers

// 5. Shared code headers
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
////////////////////////////////////////////////////////////////////////////////
/// Implementation for XmUGrid2dDataExtractor
class XmUGrid2dDataExtractorImpl : public XmUGrid2dDataExtractor
{
public:
  XmUGrid2dDataExtractorImpl(BSHP<XmUGrid> a_ugrid);

  virtual const VecPt3d& GetTrianglePoints() override;
  virtual const VecInt& GetTriangles() override;

private:
  void GenerateTriangles();
  bool GenerateCentroidTriangles(const int a_cellIdx, const VecInt& a_cellPolygon);
  void GenerateEarcutTriangles(const int a_cellIdx, const VecInt& a_cellPolygon);

  BSHP<XmUGrid> m_ugrid;          ///< UGrid for dataset
  BSHP<VecPt3d> m_trianglePoints; ///< Triangle points for the UGrid
  BSHP<VecInt> m_triangles;       ///< Triangles for the UGrid
};

namespace {

//------------------------------------------------------------------------------
/// \brief Calculate the length of a cell edge.
/// \param[in] a_ugrid: the ugrid
/// \param[in] a_idx1: the first edge point index
/// \param[in] a_idx2: the second edge point index
/// \return the length of the edge
//------------------------------------------------------------------------------
double iGetEdgeLength(XmUGrid& a_ugrid, int a_idx1, int a_idx2)
{
  const Pt3d& pt1 = a_ugrid.GetPoint(a_idx1);
  const Pt3d& pt2 = a_ugrid.GetPoint(a_idx2);
  double length = gmXyDistance(pt1, pt2);
  return length;
} // iGetEdgeLength
//------------------------------------------------------------------------------
/// \brief Calculate the magnitude of a vector.
/// \param[in] a_vec: the vector
//------------------------------------------------------------------------------
double iMagnitude(const Pt3d& a_vec)
{
  double magnitude = sqrt(a_vec.x*a_vec.x + a_vec.y*a_vec.y + a_vec.z*a_vec.z);
  return magnitude;
} // iMagnitude
//------------------------------------------------------------------------------
/// \brief Calculate a quality ratio to use to determine which triangles to cut
///        using earcut triangulation. Better triangles have a lower ratio.
/// \param[in] a_ugrid: the ugrid
/// \param[in] a_idx1: first point of first edge
/// \param[in] a_idx2: second point of first edge, first point of second edge
/// \param[in] a_idx2: second point of second edge
/// \return The ratio, -1.0 for an inverted triangle, or -2.0 for a zero area
///         triangle
//------------------------------------------------------------------------------
double iGetEarcutTriangleRatio(XmUGrid& a_ugrid, int a_idx1, int a_idx2, int a_idx3)
{
  Pt3d pt1 = a_ugrid.GetPoint(a_idx1);
  Pt3d pt2 = a_ugrid.GetPoint(a_idx2);
  Pt3d pt3 = a_ugrid.GetPoint(a_idx3);
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
bool iValidTriangle(XmUGrid& a_ugrid, const VecInt a_polygon, int a_idx1, int a_idx2, int a_idx3)
{
  const Pt3d& pt1 = a_ugrid.GetPoint(a_idx1);
  const Pt3d& pt2 = a_ugrid.GetPoint(a_idx2);
  const Pt3d& pt3 = a_ugrid.GetPoint(a_idx3);
  for (size_t pointIdx = 0; pointIdx < a_polygon.size(); ++pointIdx)
  {
    int idx = a_polygon[pointIdx];
    if (idx != a_idx1 && idx != a_idx2 && idx != a_idx3)
    {
      const Pt3d& pt = a_ugrid.GetPoint(idx);
      if (gmTurn(pt1, pt2, pt, 0.0) == TURN_LEFT && gmTurn(pt2, pt3, pt) == TURN_LEFT &&
          gmTurn(pt3, pt1, pt, 0.0) == TURN_LEFT)
      {
        return false;
      }
    }
  }

  return true;
} // iValidTriangle

} // namespace {

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractor
/// \brief Implementation for XmUGrid2dDataExtractor which provides ability
///        to extract dataset values at points and along arcs for an
///        unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl(BSHP<XmUGrid> a_ugrid)
: m_ugrid(a_ugrid)
, m_trianglePoints(new VecPt3d)
, m_triangles(new VecInt)
{
  *m_trianglePoints = m_ugrid->GetPoints();
  GenerateTriangles();
} // XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl
//------------------------------------------------------------------------------
/// \brief Get generated triangle points for testing.
//------------------------------------------------------------------------------
const VecPt3d& XmUGrid2dDataExtractorImpl::GetTrianglePoints()
{
  return *m_trianglePoints;
} // XmUGrid2dDataExtractorImpl::GetTrianglePoints
//------------------------------------------------------------------------------
/// \brief Get generated triangles for testing.
//------------------------------------------------------------------------------
const VecInt& XmUGrid2dDataExtractorImpl::GetTriangles()
{
  return *m_triangles;
} // XmUGrid2dDataExtractorImpl::GetTriangles
//------------------------------------------------------------------------------
/// \brief Generate triangles for the UGrid.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::GenerateTriangles()
{
  VecInt cellPoints;

  int numCells = m_ugrid->GetNumberOfCells();
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    m_ugrid->GetPointsOfCell(cellIdx, cellPoints);
    if (!GenerateCentroidTriangles(cellIdx, cellPoints))
      GenerateEarcutTriangles(cellIdx, cellPoints);
  }
} // XmUGrid2dDataExtractorImpl::GenerateTriangles
//------------------------------------------------------------------------------
/// \brief Attempt to generate triangles for a cell by adding a point at the
///        centroid.
//------------------------------------------------------------------------------
bool XmUGrid2dDataExtractorImpl::GenerateCentroidTriangles(const int a_cellIdx,
                                                           const VecInt& a_cellPoints)
{
  VecPt3d polygon = m_ugrid->GetPointsFromPointIdxs(a_cellPoints);
  Pt3d centroid = gmComputeCentroid(polygon);
  size_t numPoints = polygon.size();

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
  VecPt3d& points = *m_trianglePoints;
  int centroidIdx = (int)points.size();
  points.push_back(centroid);

  // add triangles
  VecInt& triangles = *m_triangles;
  for (size_t pointIdx = 0; pointIdx < polygon.size(); ++pointIdx)
  {
    int idx1 = a_cellPoints[pointIdx];
    int idx2 = a_cellPoints[(pointIdx + 1) % numPoints];
    triangles.push_back(idx1);
    triangles.push_back(idx2);
    triangles.push_back(centroidIdx);
  }

  return true;
} // XmUGrid2dDataExtractorImpl::GenerateCentroidTriangles
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::GenerateEarcutTriangles(const int a_cellIdx,
                                                         const VecInt& a_cellPoints)
{
  VecInt polygon = a_cellPoints;
  VecInt& triangles = *m_triangles;

  // continually find best triangle on adjacent edges and cut it off polygon
  while (polygon.size() >= 4)
  {
    int numPoints = (int)polygon.size();
    double bestRatio = std::numeric_limits<double>::max();
    int bestIdx = -1;
    bool foundInverted = false;
    for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    {
      int idx1 = polygon[(pointIdx + numPoints - 1) % numPoints];
      int idx2 = polygon[pointIdx];
      int idx3 = polygon[(pointIdx + 1) % numPoints];

      // make sure triangle is valid (not inverted and doesn't have other points in it)
      double ratio = iGetEarcutTriangleRatio(*m_ugrid, idx1, idx2, idx3);
      if (ratio > 0.0 && iValidTriangle(*m_ugrid, polygon, idx1, idx2, idx3) && ratio < bestRatio)
      {
        bestRatio = ratio;
        bestIdx = pointIdx;
      }
      else if (ratio < 0.0)
      {
        foundInverted = true;
      }
    }

    if (numPoints == 4 && !foundInverted)
    {
      // when four points take triangles that are
      bestIdx = (bestIdx + 1) % numPoints;
    }

    if (bestIdx >= 0)
    {
      // cut off the ear triangle
      int idx1 = polygon[(bestIdx + numPoints - 1) % numPoints];
      int idx2 = polygon[bestIdx];
      int idx3 = polygon[(bestIdx + 1) % numPoints];
      triangles.push_back(idx1);
      triangles.push_back(idx2);
      triangles.push_back(idx3);
      polygon.erase(polygon.begin() + idx2);
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
  triangles.push_back(polygon[0]);
  triangles.push_back(polygon[1]);
  triangles.push_back(polygon[2]);
} // XmUGrid2dDataExtractorImpl::GenerateEarcutTriangles

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractor
/// \brief Provides ability to extract dataset values at points and along arcs
///        for an unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Create a new XmUGrid2dDataExtractor.
/// \return the new XmUGrid2dDataExtractor.
//------------------------------------------------------------------------------
BSHP<XmUGrid2dDataExtractor> XmUGrid2dDataExtractor::New(BSHP<XmUGrid> a_ugrid)
{
  BSHP<XmUGrid2dDataExtractor> extractor(new XmUGrid2dDataExtractorImpl(a_ugrid));
  return extractor;
} // XmUGrid2dDataExtractor::New
//------------------------------------------------------------------------------
/// \brief Constructor
//------------------------------------------------------------------------------
XmUGrid2dDataExtractor::XmUGrid2dDataExtractor()
{
} // XmUGrid2dDataExtractor::XmUGrid2dDataExtractor
//------------------------------------------------------------------------------
/// \brief Destructor
//------------------------------------------------------------------------------
XmUGrid2dDataExtractor::~XmUGrid2dDataExtractor()
{
} // XmUGrid2dDataExtractor::~XmUGrid2dDataExtractor

} // namespace xms

#ifdef CXX_TEST
//------------------------------------------------------------------------------
// Unit Tests
//------------------------------------------------------------------------------
using namespace xms;
#include <xmsextractor/extractor/XmUGrid2dDataExtractor.t.h>
#include <xmsgrid/ugrid/XmUGrid.t.h>

//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testGenerateCentroidTriangles()
{
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);
  VecPt3d triPointsOut = extractor->GetTrianglePoints();
  VecPt3d triPointsExpected = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0.5, 0.5, 0}};
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);
  
  VecInt trianglesOut = extractor->GetTriangles();
  VecInt trianglesExpected = {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4};
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);
} // XmUGrid2dDataExtractorUnitTests::testGenerateCentroidTriangles
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testGenerateCentroidTriangles2dCellTypes()
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
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);
  VecPt3d triPointsOut = extractor->GetTrianglePoints();
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
    {5.0, 5.0, 0.0},
    {15.0, 5.0, 0.0},
    {23.333333333333332, 3.3333333333333335, 0.0},
    {33.333333333333336, 10.0, 0.0}
  };
  // clang-format on
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

  VecInt trianglesOut = extractor->GetTriangles();
  // clang-format off
  VecInt trianglesExpected = {
    0, 1, 14, 1, 6, 14, 6, 5, 14, 5, 0, 14, // quad
    1, 2, 15, 2, 7, 15, 7, 6, 15, 6, 1, 15, // pixel
    2, 3, 16, 3, 7, 16, 7, 2, 16,           // triangle
    3, 4, 17, 4, 8, 17, 8, 13, 17, 13, 12, 17, 12, 7, 17, 7, 3, 17}; // polygon
  // clang-format on
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);
} // XmUGrid2dDataExtractorUnitTests::testGenerateCentroidTriangles2dCellTypes
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testGenerateEarcutTriangles()
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
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);
  VecPt3d triPointsOut = extractor->GetTrianglePoints();
  VecPt3d triPointsExpected = points;
  TS_ASSERT_EQUALS(triPointsExpected, triPointsOut);

  VecInt trianglesOut = extractor->GetTriangles();
  // clang-format off
  VecInt trianglesExpected = {0, 1, 2, 0, 2, 3};
  // clang-format on
  TS_ASSERT_EQUALS(trianglesExpected, trianglesOut);
} // XmUGrid2dDataExtractorUnitTests::testGenerateEarcutTriangles

#endif
