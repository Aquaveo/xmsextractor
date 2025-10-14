//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsextractor/ugrid/XmUGridTriangulatorBase.h>

// 3. Standard library headers
#include <sstream>

// 4. External library headers
#include <boost/container/flat_map.hpp>

// 5. Shared code headers
#include <xmscore/misc/XmLog.h>   // XM_LOG
#include <xmscore/misc/XmError.h> // XM_ASSERT

#include <xmsgrid/geometry/geoms.h> // gmTurn, gmComputeCentroid

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
typedef std::tuple<int, int, int> Triangle; ///< three consecutive polygon points for triangle
typedef boost::container::flat_map<Triangle, double>
  TriDoubleCache; ///< ratio cache to speed up earcut calculation
typedef boost::container::flat_map<Triangle, bool>
  TriBoolCache; ///< valid cache to speed up earcut calculation

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
/// \brief Find the area of two vectors times 2.
/// \param v1 Vector 1.
/// \param v2 Vector 2.
/// \param area2 The returned area.
/// \param crossZ The returned cross product Z value.
//------------------------------------------------------------------------------
void iFindArea2(const Pt3d& v1,
                const Pt3d& v2,
                double& area2,
                double& crossZ)
{
  Pt3d cross;
  gmCross3D(v2, v1, &cross);
  crossZ = cross.z;
  area2 = iMagnitude(cross);
} // iFindArea2
//------------------------------------------------------------------------------
/// \brief Calculate a quality ratio to use to determine which triangles to cut
///        using earcut triangulation. Better triangles have a lower ratio.
/// \param[in] a_points: the array the points come from
/// \param[in] a_idx1: first point of first edge
/// \param[in] a_idx2: second point of first edge, first point of second edge
/// \param[in] a_idx2: second point of second edge
/// \param[in] a_topFace: is this a top face or a bottom face
/// \param[out] a_areaCache: The ratio cache
/// \param[out] a_crossZCache: The ratio cache
/// \param[out] a_ratioCache: The ratio cache
/// \return The ratio, -1.0 for an inverted triangle, or -2.0 for a zero area
///         triangle
//------------------------------------------------------------------------------
double iGetEarcutTriangleRatio(const VecPt3d& a_points,
                               int a_idx1,
                               int a_idx2,
                               int a_idx3,
                               bool a_topFace,
                               TriDoubleCache& a_areaCache,
                               TriDoubleCache& a_crossZCache,
                               TriDoubleCache& a_ratioCache)
{
  Triangle triangle(a_idx1, a_idx2, a_idx3);
  auto foundRatio = a_ratioCache.find(triangle);
  if (foundRatio != a_ratioCache.end())
    return foundRatio->second;

  const Pt3d& pt1 = a_points[a_idx1];
  const Pt3d& pt2 = a_points[a_idx2];
  const Pt3d& pt3 = a_points[a_idx3];
  Pt3d v1 = pt1 - pt2;
  Pt3d v2 = pt3 - pt2;
  Pt3d v3 = pt3 - pt1;
  double area2;
  double crossZ;

  auto foundArea = a_areaCache.find(triangle);
  auto foundCrossZ = a_crossZCache.find(triangle);
  if (foundArea == a_areaCache.end() || foundCrossZ == a_crossZCache.end())
  {
    iFindArea2(v1, v2, area2, crossZ);
    a_areaCache[triangle] = area2;
    a_crossZCache[triangle] = crossZ;
  }
  else
  {
    area2 = foundArea->second;
    crossZ = foundCrossZ->second;
  }

  double ratio;
  if ((a_topFace && crossZ <= 0.0) || (!a_topFace && crossZ >= 0.0))
  {
    // inverted
    ratio = -1.0;
  }
  else
  {
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

  a_ratioCache[triangle] = ratio;
  return ratio;
} // iGetEarcutTriangleRatio
//------------------------------------------------------------------------------
/// \brief Is this a valid triangle?
/// \param a_points The points
/// \param a_polygon The triangle polygon
/// \param a_idx1 First triangle point
/// \param a_idx2 Second triangle point
/// \param a_idx3 Third triangle point
/// \param[in] a_topFace Is this a top face or a bottom face
/// \param a_validCache Cache of valid triangles
/// \return
//------------------------------------------------------------------------------
bool iValidTriangle(const VecPt3d& a_points,
                    const VecInt& a_polygon,
                    int a_idx1,
                    int a_idx2,
                    int a_idx3,
                    bool a_topFace,
                    TriBoolCache& a_validCache)
{
  Triangle triangle(a_idx1, a_idx2, a_idx3);
  auto validIter = a_validCache.find(triangle);
  if (validIter != a_validCache.end())
    return validIter->second;

  const Pt3d& pt1 = a_points[a_idx1];
  const Pt3d& pt2 = a_points[a_idx2];
  const Pt3d& pt3 = a_points[a_idx3];
  for (int pointIndex : a_polygon)
  {
    if (pointIndex != a_idx1 && pointIndex != a_idx2 && pointIndex != a_idx3)
    {
      const Pt3d& pt = a_points[pointIndex];
      if (a_topFace)
      {
        if (gmTurn(pt1, pt2, pt, 0.0) == TURN_LEFT &&
            gmTurn(pt2, pt3, pt) == TURN_LEFT &&
            gmTurn(pt3, pt1, pt, 0.0) == TURN_LEFT)
        {
          a_validCache[triangle] = false;
          return false;
        }
      }
      else if (gmTurn(pt1, pt2, pt, 0.0) == TURN_RIGHT &&
               gmTurn(pt2, pt3, pt) == TURN_RIGHT &&
               gmTurn(pt3, pt1, pt, 0.0) == TURN_RIGHT)
      {
        a_validCache[triangle] = false;
        return false;
      }
    }
  }

  a_validCache[triangle] = true;
  return true;
} // iValidTriangle
//------------------------------------------------------------------------------
/// \brief Determine if cell polygon is for a top or bottom face.
/// \param polygonIdxs The cell point indices
/// \param points The points
/// \param areaCache The area cache
/// \param crossZCache The cross Z cache
/// \return True if the cell points are counter-clock-wise.
//------------------------------------------------------------------------------
bool iIsTopFace(const VecInt& polygonIdxs,
                const VecPt3d& points,
                TriDoubleCache& areaCache,
                TriDoubleCache& crossZCache)
{
  double area2Sum = 0.0;
  int numPoints = (int)polygonIdxs.size();
  for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
  {
    int idx1 = polygonIdxs[(pointIdx + numPoints - 1) % numPoints];
    int idx2 = polygonIdxs[pointIdx];
    int idx3 = polygonIdxs[(pointIdx + 1) % numPoints];
    const Pt3d& pt1 = points[idx1];
    const Pt3d& pt2 = points[idx2];
    const Pt3d& pt3 = points[idx3];
    Pt3d v1 = pt1 - pt2;
    Pt3d v2 = pt3 - pt2;
    double area2, crossZ;
    iFindArea2(v1, v2, area2, crossZ);
    if (crossZ > 0.0)
      area2Sum += area2;
    else
      area2Sum -= area2;
    Triangle triangle(idx1, idx2, idx3);
    areaCache[triangle] = area2;
    crossZCache[triangle] = crossZ;
  }
  bool topFace = area2Sum > 0.0;
  return topFace;
} // iIsTopFace
} // namespace

class XmUGridTriangulatorBase::impl
{
public:
  impl();
  virtual ~impl() = default;
  BSHP<FlatMapEdgeMidpointInfo> m_midPoints;
};

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangulatorImpl
/// \brief Class to store XmUGrid triangles. Tracks where midpoints and
///        triangles came from.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Constructor
//------------------------------------------------------------------------------
XmUGridTriangulatorBase::impl::impl()
: m_midPoints(nullptr)
{
} // XmUGridTriangulatorBase::impl::impl
////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangulatorBase
/// \brief Class to triangulate.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Default contructor.
//------------------------------------------------------------------------------
XmUGridTriangulatorBase::XmUGridTriangulatorBase()
: m_impl(new XmUGridTriangulatorBase::impl())
{
} // XmUGridTriangulatorBase::XmUGridTriangulatorBase
//------------------------------------------------------------------------------
/// \brief Destructor.
//------------------------------------------------------------------------------
XmUGridTriangulatorBase::~XmUGridTriangulatorBase()
{
} // XmUGridTriangulatorBase::XmUGridTriangulatorBase
//------------------------------------------------------------------------------
/// \brief Sets the midpoints.
//------------------------------------------------------------------------------
void XmUGridTriangulatorBase::SetMidpoints(BSHP<FlatMapEdgeMidpointInfo> a_midPoints)
{
  m_impl->m_midPoints = a_midPoints;
} // XmUGridTriangulatorBase::SetMidpoints
//------------------------------------------------------------------------------
/// \brief Returns the midpoint info from the element edge.
//------------------------------------------------------------------------------
XmElementMidpointInfo& XmUGridTriangulatorBase::FindMidPoint(const XmElementEdge& a_edge)
{
  auto it = m_impl->m_midPoints->find(a_edge);
  if (it == m_impl->m_midPoints->end())
  {
    XM_ASSERT(0);
    XmElementMidpointInfo info = {-1, -1};
    it = m_impl->m_midPoints->insert(std::pair<XmElementEdge, XmElementMidpointInfo>(a_edge, info))
           .first;
  }
  return it->second;
} // XmUGridTriangulatorBase::FindMidPoint
//------------------------------------------------------------------------------
/// \brief Generate triangles using ear cut algorithm for plan view 2D cells.
/// \param[in] a_ugridTris The triangles to add to.
/// \param[in] a_cellIdx The cell index.
/// \param[in] a_cellPointIdxs The indices for the cell polygon.
//------------------------------------------------------------------------------
void XmUGridTriangulatorBase::BuildEarcutTriangles(int a_cellIdx, const VecInt& a_cellPointIdxs)
{
  VecInt polygonIdxs = a_cellPointIdxs;
  const VecPt3d& points = GetPoints();

  TriDoubleCache areaCache;
  TriDoubleCache crossZCache;
  bool topFace = iIsTopFace(polygonIdxs, points, areaCache, crossZCache);

  // continually find best triangle on adjacent edges and cut it off polygon
  TriDoubleCache ratioCache;
  TriBoolCache validCache;
  while (polygonIdxs.size() >= 4)
  {
    int bestIdx = -1;
    int secondBestIdx = -1;
    double bestRatio = std::numeric_limits<double>::max();
    double secondBestRatio = std::numeric_limits<double>::max();
    int numPoints = (int)polygonIdxs.size();
    for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    {
      int idx1 = polygonIdxs[(pointIdx + numPoints - 1) % numPoints];
      int idx2 = polygonIdxs[pointIdx];
      int idx3 = polygonIdxs[(pointIdx + 1) % numPoints];

      // make sure triangle is valid (not inverted and doesn't have other points in it)
      double ratio = iGetEarcutTriangleRatio(points, idx1, idx2, idx3, topFace,
                                             areaCache, crossZCache, ratioCache);
      if (ratio > 0.0)
      {
        if (iValidTriangle(points, polygonIdxs, idx1, idx2, idx3, topFace, validCache))
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
      AddTriangle(a_cellIdx, idx1, idx2, idx3);
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
  AddTriangle(a_cellIdx, polygonIdxs[0], polygonIdxs[1], polygonIdxs[2]);
} // XmUGridTriangulatorBase::BuildEarcutTriangles
//------------------------------------------------------------------------------
/// \brief Attempt to generate triangles for a cell by adding a point at the
///        centroid.
/// \param[in] a_ugridTris The triangles to add to.
/// \param[in] a_cellIdx The cell index.
/// \param[in] a_cellPointIdxs The indices for the cell polygon.
/// \return true on success (can fail for concave cell)
//------------------------------------------------------------------------------
bool XmUGridTriangulatorBase::GenerateCentroidTriangles(int a_cellIdx,
                                                        const VecInt& a_cellPointIdxs)
{
  const VecPt3d& points = GetPoints();
  size_t numPoints = a_cellPointIdxs.size();

  VecPt3d polygon;
  for (size_t pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    polygon.push_back(points[a_cellPointIdxs[pointIdx]]);

  Pt3d centroid = gmComputePolygonCentroid(polygon);

  // make sure the centroid is located inside the cell
  if (gmPointInPolygon2D(polygon, centroid) != 1)
  {
    return false;
  }

  // Compute the centroid Z value from the polygon points
  double z = 0.0;
  for (auto &&pt : polygon)
    z += pt.z;
  z /= static_cast<int>(polygon.size());
  centroid.z = z;

  // add centroid to list of points
  int centroidIdx = AddCentroidPoint(a_cellIdx, centroid);

  // add triangles
  for (size_t pointIdx = 0; pointIdx < numPoints; ++pointIdx)
  {
    int idx1 = a_cellPointIdxs[pointIdx];
    int idx2 = a_cellPointIdxs[(pointIdx + 1) % numPoints];
    AddTriangle(a_cellIdx, idx1, idx2, centroidIdx);
  }

  return true;
} // XmUGridTriangulatorBase::GenerateCentroidTriangles
} // namespace xms
