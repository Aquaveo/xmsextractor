//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018.
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h>

// 3. Standard library headers
#include <sstream>

// 4. External library headers

// 5. Shared code headers
#include <xmscore/misc/XmLog.h>
#include <xmsextractor/extractor/XmUGrid2dDataExtractor.h>
#include <xmsextractor/ugrid/XmUGridTriangles2d.h>
#include <xmsgrid/ugrid/XmUGrid.h>
#include <xmsinterp/geometry/GmMultiPolyIntersector.h>
#include <xmsinterp/geometry/GmMultiPolyIntersectionSorterTerse.h>

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
/// Implementation for XmUGrid2dPolylineDataExtractor
class XmUGrid2dPolylineDataExtractorImpl : public XmUGrid2dPolylineDataExtractor
{
public:
  XmUGrid2dPolylineDataExtractorImpl(BSHP<XmUGrid> a_ugrid);

  virtual void SetGridPointScalars(const VecFlt& a_pointScalars,
                                   const DynBitset& a_activity,
                                   DataLocationEnum a_activityType) override;
  virtual void SetGridCellScalars(const VecFlt& a_cellScalars,
                                  const DynBitset& a_activity,
                                  DataLocationEnum a_activityType) override;

  virtual void SetPolyline(const VecPt3d& a_polyline) override;
  virtual const VecPt3d& GetExtractLocations() const override;
  virtual void ExtractData(VecFlt& a_extractedData) override;

  virtual void ComputeLocationsAndExtractData(const VecPt3d& a_polyline,
                                              VecFlt& a_extractedData,
                                              VecPt3d& a_extractedLocations) override;

  virtual void SetUseIdwForPointData(bool a_useIdw) override;
  virtual void SetNoDataValue(float a_noDataValue) override;

private:
  void ComputeExtractLocations(const VecPt3d& a_polyline, VecPt3d& a_locations);

  BSHP<XmUGrid> m_ugrid;                    ///< The ugrid which holds the points and the grid.
  BSHP<XmUGrid2dDataExtractor> m_extractor; ///< The data extractor.
  mutable BSHP<GmMultiPolyIntersector> m_multiPolyIntersector; ///< The intersection tool used from
                                                       ///   xmsinterp to find the intersections
                                                       ///   with the polyline.
};

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dPolylineDataExtractorImpl
/// \brief Implementation for XmUGrid2dDataExtractor which provides ability
///        to extract dataset values along arcs for an unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Construct from a UGrid.
/// \param[in] a_ugrid The UGrid to construct an extractor for.
//------------------------------------------------------------------------------
XmUGrid2dPolylineDataExtractorImpl::XmUGrid2dPolylineDataExtractorImpl(BSHP<XmUGrid> a_ugrid)
: m_ugrid(a_ugrid)
, m_extractor(XmUGrid2dDataExtractor::New(a_ugrid))
{
} // XmUGrid2dPolylineDataExtractorImpl::XmUGrid2dPolylineDataExtractorImpl
//------------------------------------------------------------------------------
/// \brief Setup point scalars to be used to extract interpolated data.
/// \param[in] a_pointScalars The point scalars.
/// \param[in] a_activity The activity of the cells.
/// \param[in] a_activityType The location at which the data is currently stored.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorImpl::SetGridPointScalars(const VecFlt& a_pointScalars,
                                                             const DynBitset& a_activity,
                                                             DataLocationEnum a_activityType)
{
  m_extractor->SetGridPointScalars(a_pointScalars, a_activity, a_activityType);
} // XmUGrid2dPolylineDataExtractorImpl::SetGridPointScalars
//------------------------------------------------------------------------------
/// \brief Set the polyline along which to extract the scalar data. Locations
///        crossing cell boundaries are computed along the polyline.
/// \param[in] a_polyline The polyline.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorImpl::SetPolyline(const VecPt3d& a_polyline)
{
  VecPt3d locations;
  ComputeExtractLocations(a_polyline, locations);
  m_extractor->SetExtractLocations(locations);
} // XmUGrid2dPolylineDataExtractorImpl::SetPolyline
//------------------------------------------------------------------------------
/// \brief Gets computed locations along polyline to extract interpolated scalar
///        data from.
/// \return The locations.
//------------------------------------------------------------------------------
const VecPt3d& XmUGrid2dPolylineDataExtractorImpl::GetExtractLocations() const
{
  return m_extractor->GetExtractLocations();
} // XmUGrid2dPolylineDataExtractorImpl::GetExtractLocations
//------------------------------------------------------------------------------
/// \brief \brief Extract data at previously computed locations returned by
///               GetExtractLocations.
/// \param[out] a_extractedData The extracted values interpolated from the
///             scalar values.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorImpl::ExtractData(VecFlt& a_extractedData)
{
  m_extractor->ExtractData(a_extractedData);
} // XmUGrid2dPolylineDataExtractorImpl::ExtractData
//------------------------------------------------------------------------------
/// \brief Extracts a scalar profile for the UGrid across the given polyline.
/// \param[in] a_polyline The polyline to extract the data across.
/// \param[out] a_extractedData The extracted values interpolated from the
///             scalar values.
/// \param[out] a_extractedLocations The locations of the extracted data values.
///             The polyline will get split up into locations across the UGrid.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorImpl::ComputeLocationsAndExtractData(const VecPt3d& a_polyline,
                                                                     VecFlt& a_extractedData,
                                                                     VecPt3d& a_extractedLocations)
{
  SetPolyline(a_polyline);
  ExtractData(a_extractedData);
  a_extractedLocations = GetExtractLocations();
} // XmUGrid2dPolylineDataExtractorImpl::ComputeLocationsAndExtractData
//------------------------------------------------------------------------------
/// \brief Setup cell scalars to be used to extract interpolated data.
/// \param[in] a_cellScalars The point scalars.
/// \param[in] a_activity The activity of the cells.
/// \param[in] a_activityType The location at which the data is currently stored.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorImpl::SetGridCellScalars(const VecFlt& a_cellScalars,
                                                            const DynBitset& a_activity,
                                                            DataLocationEnum a_activityType)
{
  m_extractor->SetGridCellScalars(a_cellScalars, a_activity, a_activityType);
} // XmUGrid2dPolylineDataExtractorImpl::SetGridCellScalars
//------------------------------------------------------------------------------
/// \brief Set to use IDW to calculate point scalar values from cell scalars.
/// \param a_useIdw Whether to turn IDW on or off.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorImpl::SetUseIdwForPointData(bool a_useIdw)
{
  m_extractor->SetUseIdwForPointData(a_useIdw);
} // XmUGrid2dPolylineDataExtractorImpl::SetUseIdwForPointData
//------------------------------------------------------------------------------
/// \brief Set value to use when extracted value is in inactive cell or doesn't
///        intersect with the grid.
/// \param[in] a_value The no data value
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorImpl::SetNoDataValue(float a_value)
{
  m_extractor->SetNoDataValue(a_value);
} // XmUGrid2dPolylineDataExtractorImpl::SetNoDataValue
//------------------------------------------------------------------------------
/// \brief Compute locations to extract scalar values from across a polyline.
/// \param[in] a_polyline The line used to calculate the extraction points.
/// \param[out] a_locations The points at which will the data will be extracted.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorImpl::ComputeExtractLocations(const VecPt3d& a_polyline,
                                                                 VecPt3d& a_locations)
{
  a_locations.clear();
  if (a_polyline.empty())
  {
    XM_LOG(xmlog::error, "Attempting to extract polyline profile with empty polyline.");
    return;
  }

  if (!m_extractor->GetUGridTriangles())
  {
    XM_LOG(xmlog::error, "Attempting to extract polyline profile without setting scalars.");
    return;
  }

  if (!m_multiPolyIntersector)
  {
    const VecPt3d& points = m_extractor->GetUGridTriangles()->GetPoints();
    const VecInt& triangles = m_extractor->GetUGridTriangles()->GetTriangles();
    VecInt2d polygons;
    VecInt triangle;
    for (size_t triangleIdx = 0; triangleIdx <triangles.size(); triangleIdx += 3)
    {
      triangle = { triangles[triangleIdx], triangles[triangleIdx+1], triangles[triangleIdx+2] };
      polygons.push_back(triangle);
    }

    BSHP<GmMultiPolyIntersectionSorter> sorter(new GmMultiPolyIntersectionSorterTerse());
    m_multiPolyIntersector = GmMultiPolyIntersector::New(points, polygons, sorter, 0);
  }

  // get points crossing cell edges
  Pt3d lastPoint = a_polyline[0];
  a_locations.push_back(lastPoint);

  for (size_t polyIdx = 1; polyIdx < a_polyline.size(); ++polyIdx)
  {
    Pt3d pt1 = a_polyline[polyIdx - 1];
    Pt3d pt2 = a_polyline[polyIdx];
    VecInt intersectIdxs;
    VecPt3d intersectPts;

    m_multiPolyIntersector->TraverseLineSegment(pt1.x, pt1.y, pt2.x, pt2.y, intersectIdxs,
                                                intersectPts);

    if (intersectIdxs.size() != intersectPts.size())
    {
      XM_LOG(xmlog::error, "Internal error when extracting polyline profile.");
      return;
    }

    for (size_t i = 0; i < intersectIdxs.size(); ++i)
    {
      const Pt3d& currPoint = intersectPts[i];
      if (lastPoint != currPoint)
      {
        if (i != 0 && intersectIdxs[i - 1] == -1)
        {
          a_locations.push_back((currPoint + lastPoint)/2.0);
        }
        a_locations.push_back(currPoint);
        lastPoint = currPoint;
      }
    }

    if (pt2 != a_locations.back())
    {
      a_locations.push_back(pt2);
      lastPoint = pt2;
    }
  }
} // XmUGrid2dPolylineDataExtractorImpl::ComputeExtractLocations

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractor
/// \brief Provides ability to interpolate and extract the scalar values  points and along arcs
///        for an unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Create a new XmUGrid2dPolylineDataExtractor.
/// \param[in] a_ugrid The UGrid geometry to use to extract values from
/// \return the new XmUGrid2dPolylineDataExtractor
//------------------------------------------------------------------------------
BSHP<XmUGrid2dPolylineDataExtractor> XmUGrid2dPolylineDataExtractor::New(BSHP<XmUGrid> a_ugrid)
{
  BSHP<XmUGrid2dPolylineDataExtractor> extractor(new XmUGrid2dPolylineDataExtractorImpl(a_ugrid));
  return extractor;
} // XmUGrid2dPolylineDataExtractor::New
//------------------------------------------------------------------------------
/// \brief Constructor
//------------------------------------------------------------------------------
XmUGrid2dPolylineDataExtractor::XmUGrid2dPolylineDataExtractor()
{
} // XmUGrid2dPolylineDataExtractor::XmUGrid2dPolylineDataExtractor
//------------------------------------------------------------------------------
/// \brief Destructor
//------------------------------------------------------------------------------
XmUGrid2dPolylineDataExtractor::~XmUGrid2dPolylineDataExtractor()
{
} // XmUGrid2dPolylineDataExtractor::~XmUGrid2dPolylineDataExtractor

} // namespace xms

#ifdef CXX_TEST
//------------------------------------------------------------------------------
// Unit Tests
//------------------------------------------------------------------------------
using namespace xms;
#include <xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.t.h>
#include <xmsgrid/ugrid/XmUGrid.t.h>

#include <xmscore/misc/xmstype.h>
#include <xmscore/testing/TestTools.h>

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dPolylineDataExtractorUnitTests
/// \brief Class to to test XmUGrid2dPolylineDataExtractor
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Test extractor with point scalars only.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testOneCellOneSegment()
{
  // clang-format off
  //     3-------2
  //     |       |
  // 0===============1
  //     |       |
  //     0-------1
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{-1, 0.5, 0}, {2, 0.5, 0}};
  extractor->SetPolyline(polyline);
  extractedLocations = extractor->GetExtractLocations();
  extractor->ExtractData(extractedData);

  VecFlt expectedData = {XM_NODATA, 0.5, 1.5, 2.5, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {{-1, 0.5, 0}, {0.0, 0.5, 0.0}, {0.5, 0.5, 0.0}, {1.0, 0.5, 0.0}, {2, 0.5, 0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testOneCellOneSegment
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAllInCell()
{
  // clang-format off
  // (1)  3--------2  (3)
  //      |        |
  //      | 0====1 |
  //      |        |
  // (0)  0--------1  (2)
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{0.25, 0.50, 0.0}, {0.75, 0.50, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {1.0, 1.5, 2.0};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {{0.25, 0.5, 0.0}, {0.5, 0.5, 0.0}, {0.75, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAllInCell
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAlongEdge()
{
  // clang-format off
  //  0===3========2===1
  //      |        |
  //      |        |
  //      |        |
  //      0--------1
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{-0.5, 1.0, 0.0}, {1.55, 1.0, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {XM_NODATA, 1.0, 3.0, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {{-0.5, 1.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.55, 1.0, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAlongEdge
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAllOutside()
{
  // clang-format off
  //      3========2
  //      |        |
  // 0==1 |        |
  //      |        |
  //      0--------1
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{-0.5, 0.5, 0.0}, {-0.25, 0.5, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {XM_NODATA, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations =  {{-0.5, 0.5, 0.0}, {-0.25, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAllOutside
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testSegmentOutToTouch()
{
  // clang-format off
  //      3========2
  //      |        |
  // 0====1        |
  //      |        |
  //      0--------1
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{-0.5, 0.5, 0.0}, {0.0, 0.5, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {XM_NODATA, 0.5};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations =  {{-0.5, 0.5, 0.0}, {0.0, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testSegmentOutToTouch
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testSegmentTouchToOut()
{
  // clang-format off
  //      3========2
  //      |        |
  //      |        0===1
  //      |        |
  //      0--------1
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{1.0, 0.5, 0.0}, {1.5, 0.5, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {2.5, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations =  {{1.0, 0.5, 0.0}, {1.5, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testSegmentTouchToOut
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testSegmentCrossCellPoint()
{
  // clang-format off
  //        1
  //       /
  //      3========2
  //    / |        |
  //   0  |        |
  //      |        |
  //      0--------1
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{-0.5, 0.5, 0.0}, {0.0, 1.0, 0.0}, {0.5, 1.5, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {XM_NODATA, 1.0, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations =  {{-0.5, 0.5, 0.0}, {0.0, 1.0, 0.0}, {0.5, 1.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testSegmentCrossCellPoint
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAcrossCellIntoSecond()
{
  // clang-format off
  //      3========2========5
  //      |        |        |
  //   0===============1    |
  //      |        |        |
  //      0--------1--------4
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {2, 0, 0}, {2, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3, XMU_QUAD, 4, 1, 4, 5, 2};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1, 4, 5};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{-0.5, 0.5, 0.0}, {1.5, 0.5, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {XM_NODATA, 0.5, 1.5, 2.5, 3.5};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations =  {{-0.5, 0.5, 0.0}, {0.0, 0.5, 0.0}, {0.5, 0.5, 0.0},
    {1.0, 0.5, 0.0}, {1.5, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAcrossCellIntoSecond
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAcrossSplitCells()
{
  // clang-format off
  //       3-------2       7-------6
  //       |       |       |       |
  //  0========================1   |
  //       |       |       |       |
  //       0-------1       4-------5
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
                    {2, 0, 0}, {3, 0, 0}, {3, 1, 0}, {2, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3, XMU_QUAD, 4, 4, 5, 6, 7};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1, 4, 6, 7, 5};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{-1, 0.5, 0}, {2.5, 0.5, 0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {XM_NODATA, 0.5, 1.5, 2.5, XM_NODATA, 4.5, 5.5};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {{-1.0, 0.5, 0.0}, {0.0, 0.5, 0.0}, {0.5, 0.5, 0.0},
                               {1.0, 0.5, 0.0}, {1.5, 0.5, 0.0}, {2.0, 0.5, 0.0},
                               {2.5, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testSegmentAcrossSplitCells
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsAcrossOneCell()
{
  // clang-format off
  //      3--------2
  //      |        |
  // 0=========1=========2
  //      |        |
  //      0--------1
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{-1, 0.5, 0}, {0.5, 0.5, 0.0}, {2, 0.5, 0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {XM_NODATA, 0.5, 1.5, 2.5, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {
    {-1.0, 0.5, 0.0}, {0.0, 0.5, 0.0}, {0.5, 0.5, 0.0}, {1.0, 0.5, 0.0}, {2.0, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsAcrossOneCell
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsAllOutside()
{
  // clang-format off
  //      3--------2
  //      |        |
  //      |        | 0====1====2
  //      |        |
  //      0--------1
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{2.0, 0.5, 0}, {3.0, 0.5, 0.0}, {4.0, 0.5, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {XM_NODATA, XM_NODATA, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {{2.0, 0.5, 0}, {3.0, 0.5, 0.0}, {4.0, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsAllOutside
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsFirstExiting()
{
  // clang-format off
  //      3--------2
  //      |        |
  //      |   0========1=======2
  //      |        |
  //      0--------1
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{0.5, 0.5, 0}, {3.0, 0.5, 0.0}, {4.0, 0.5, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {1.5, 2.5, XM_NODATA, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {{0.5, 0.5, 0}, {1.0, 0.5, 0}, {3.0, 0.5, 0.0}, {4.0, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsFirstExiting
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsJoinInCell()
{
  // clang-format off
  //      3========2========5
  //      |        |        |
  //      |   0========1=========2
  //      |        |        |
  //      0--------1--------4
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {2, 0, 0}, {2, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3, XMU_QUAD, 4, 1, 4, 5, 2};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1, 4, 5};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{0.5, 0.5, 0.0}, {1.5, 0.5, 0.0}, {2.5, 0.5, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {1.5, 2.5, 3.5, 4.5, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations =  {{0.5, 0.5, 0.0}, {1.0, 0.5, 0.0}, {1.5, 0.5, 0.0},
    {2.0, 0.5, 0.0}, {2.5, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsJoinInCell
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsJoinOnBoundary()
{
  // clang-format off
  //      3========2========5
  //      |        |        |
  //      |   0====1=============2
  //      |        |        |
  //      0--------1--------4
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {2, 0, 0}, {2, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3, XMU_QUAD, 4, 1, 4, 5, 2};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1, 4, 5};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{0.5, 0.5, 0.0}, {1.0, 0.5, 0.0}, {2.5, 0.5, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {1.5, 2.5, 3.5, 4.5, XM_NODATA};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations =  {{0.5, 0.5, 0.0}, {1.0, 0.5, 0.0}, {1.5, 0.5, 0.0},
    {2.0, 0.5, 0.0}, {2.5, 0.5, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsJoinOnBoundary
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::testThreeSegmentsCrossOnBoundary()
{
  // clang-format off
  //      3----3---2
  //      |      \ |
  //      |   0========1
  //      |        | \ |
  //      0--------1   2
  // clang-format on

  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  VecFlt pointScalars = {0, 2, 3, 1};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {{0.5, 0.5, 0}, {1.5, 0.5, 0.0}, {1.5, 0.0, 0.0}, {0.5, 1.0, 0.0}};
  extractor->ComputeLocationsAndExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {1.5, 2.5, XM_NODATA, XM_NODATA, 2.5, 2.25, 2.0};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {{0.5, 0.5, 0}, {1.0, 0.5, 0}, {1.5, 0.5, 0.0},
    {1.5, 0.0, 0.0}, {1.0, 0.5, 0}, {0.75, 0.75, 0.0}, {0.5, 1.0, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsJoinOnBoundary

#include <fstream>
void iWriteOutputVtkFile(const VecFlt& a_scalars, const VecPt3d& a_points)
{
  std::ofstream out("output.vtk");
  out << "# vtk DataFile Version 4.0\n";
  out << "vtk output\n";
  out << "ASCII\n";
  out << "DATASET UNSTRUCTURED_GRID\n";
  out << "POINTS " << a_points.size() << "double\n";
  for (size_t i = 0; i < a_points.size(); ++i)
  {
    const Pt3d& pt = a_points[i];
    out << pt.x << " " << pt.y << " " << 0.0 << "\n";
  }

  size_t numCells = a_points.size() - 1;
  out << "CELLS " << numCells << " " << numCells*3 << "\n";
  for (size_t i = 1; i < a_points.size(); ++i)
  {
    out << "2 " << i-1 << " " << i << "\n";
  }
  out << "CELL_TYPES " << numCells << "\n";
  for (size_t i = 1; i < a_points.size(); ++i)
  {
    out << "3\n";
  }
  //out << "CELL_DATA " << numCells << "\n";
  //out << "SCALARS expectedValue float 1\n";
  //out << "LOOKUP_TABLE default\n";
  //for (size_t i = 0; i < a_points.size(); ++i)
  //{
  //  out << a_scalars[i]*2 << "\n";
  //}
  out << "\n";
}
void iWriteOutputVtkFile2(const VecFlt& a_scalars, const VecPt3d& a_points)
{
  std::ofstream out("output.vtk");
  out << "# vtk DataFile Version 4.0\n";
  out << "vtk output\n";
  out << "ASCII\n";
  out << "DATASET UNSTRUCTURED_GRID\n";
  out << "POINTS " << a_points.size()*2 << "double\n";
  for (size_t i = 0; i < a_scalars.size(); ++i)
  {
    const Pt3d& pt = a_points[i];
    out << pt.x << " " << pt.y << " " << 0.0 << "\n";
    out << pt.x << " " << pt.y << " " << a_scalars[i]*2 << "\n";
  }

  size_t numCells = a_points.size();
  out << "CELLS " << numCells << " " << numCells*3 << "\n";
  for (size_t i = 0; i < a_points.size()*2; i += 2)
  {
    out << "2 " << i << " " << i+1 << "\n";
  }
  out << "CELL_TYPES " << numCells << "\n";
  for (size_t i = 0; i < a_points.size()*2; i += 2)
  {
    out << "3\n";
  }
  //out << "CELL_DATA " << numCells << "\n";
  //out << "SCALARS expectedValue float 1\n";
  //out << "LOOKUP_TABLE default\n";
  //for (size_t i = 0; i < a_points.size(); ++i)
  //{
  //  out << a_scalars[i]*2 << "\n";
  //}
  out << "\n";
}
//------------------------------------------------------------------------------
/// \brief
//------------------------------------------------------------------------------
#include <xmsgrid\ugrid\XmUGridUtils.h>
void XmUGrid2dPolylineDataExtractorUnitTests::testTutorial()
{
  //{
  //// build 3x4 grid
  //VecPt3d points = {
  //  {383272, 4.28547e+006, 0}, {383272, 4.28460e+006, 0}, {384309, 4.28460e+006, 0}, 
  //  {384309, 4.28547e+006, 0}, {383272, 4.28373e+006, 0}, {384309, 4.28373e+006, 0}, 
  //  {383272, 4.28285e+006, 0}, {384309, 4.28285e+006, 0}, {385347, 4.28460e+006, 0}, 
  //  {385347, 4.28547e+006, 0}, {385347, 4.28373e+006, 0}, {385347, 4.28285e+006, 0}, 
  //  {386384, 4.28460e+006, 0}, {386384, 4.28547e+006, 0}, {386384, 4.28373e+006, 0}, 
  //  {386384, 4.28285e+006, 0}, {387421, 4.28460e+006, 0}, {387421, 4.28547e+006, 0}, 
  //  {387421, 4.28373e+006, 0}, {387421, 4.28285e+006, 0}
  //};
  //VecInt cells = {
  //  XMU_QUAD, 4, 0, 1, 2, 3,
  //  XMU_QUAD, 4, 1, 4, 5, 2,
  //  XMU_QUAD, 4, 4, 6, 7, 5,
  //  XMU_QUAD, 4, 3, 2, 8, 9,
  //  XMU_QUAD, 4, 2, 5, 10, 8,
  //  XMU_QUAD, 4, 5, 7, 11, 10,
  //  XMU_QUAD, 4, 9, 8, 12, 13,
  //  XMU_QUAD, 4, 8, 10, 14, 12,
  //  XMU_QUAD, 4, 10, 11, 15, 14,
  //  XMU_QUAD, 4, 13, 12, 16, 17,
  //  XMU_QUAD, 4, 12, 14, 18, 16,
  //  XMU_QUAD, 4, 14, 15, 19, 18
  //};
  //BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  //XmWriteUGridToAsciiFile(ugrid, "tutorial.xmugrid");
  //BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  //extractor->SetNoDataValue(-999.0);
  //VecFlt cellScalars = {2081.0264f, 2158.6672f, 2268.552f, 2169.3428f, 2249.8545f, 2301.4797f, 2265.8857f, 2304.7388f, 2459.8547f, 2242.324f, 2339.6143f, 2456.0151f};
  //extractor->SetGridCellScalars(cellScalars, DynBitset(), LOC_CELLS);

  //VecFlt extractedData;
  //VecPt3d extractedLocations;
  //VecPt3d polyline = {
  //  {383083.0, 4283410.0, 0.0}, {385668.0, 4284561.0, 0.0}, {386307.0, 4285629.0, 0.0},
  //  {388161.0, 4285075.0, 0.0}};
  //extractor->ExtractData(polyline, extractedData, extractedLocations);
  //iWriteOutputVtkFile(extractedData, extractedLocations);

  //VecFlt expectedData = {
  //  -999.0f, 2228.33447f, 2232.92480f, 2229.45850f, 2215.47339f, 2223.88867f, 2247.04517f,
  //  2249.59595f, 2264.50391f, 2263.10645f, 2263.36133f, 2260.96509f, 2268.34619f, 2261.95776f,
  //  2248.04785f, -999.0f, 2248.93457f, 2242.32397f, 2252.04614f, -999.0f};
  //TS_ASSERT_EQUALS(expectedData, extractedData);
  //VecPt3d expectedLocations = {
  //  {383083.00000000000, 4283410.0000000000, 0.0},
  //  {383272.00000000000, 4283494.1543520307, 0.0},
  //  {383454.28025217488, 4283575.3166616065, 0.0},
  //  {383801.67940920941, 4283730.0000000000, 0.0},
  //  {384133.10319424310, 4283877.5701263342, 0.0},
  //  {384309.00000000000, 4283955.8901353963, 0.0},
  //  {384810.87325681071, 4284179.3547847541, 0.0},
  //  {384883.94620436878, 4284211.8913273606, 0.0},
  //  {385347.00000000000, 4284418.0711798836, 0.0},
  //  {385488.66487738548, 4284481.1490421165, 0.0},
  //  {385668.00000000000, 4284561.0000000000, 0.0},
  //  {385691.33426966291, 4284600.0000000000, 0.0},
  //  {385922.82579957508, 4284986.9060312146, 0.0},
  //  {386038.38033416367, 4285180.0394317480, 0.0},
  //  {386211.86797752808, 4285470.0000000000, 0.0},
  //  {386307.00000000000, 4285629.0000000000, 0.0},
  //  {386839.10469314078, 4285470.0000000000, 0.0},
  //  {387268.17666189099, 4285341.7875562636, 0.0},
  //  {387421.00000000000, 4285296.1218985980, 0.0},
  //  {388161.00000000000, 4285075.0000000000, 0.0}};
  //TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
  //}

  // build 2x3 grid
  VecPt3d points = {
    {288050, 3.90777e+006, 0.0}, {294050, 3.90777e+006, 0.0}, {300050, 3.90777e+006, 0.0},
    {306050, 3.90777e+006, 0.0}, {288050, 3.90177e+006, 0.0}, {294050, 3.90177e+006, 0.0},
    {300050, 3.90177e+006, 0.0}, {306050, 3.90177e+006, 0.0}, {288050, 3.89577e+006, 0.0},
    {294050, 3.89577e+006, 0.0}, {300050, 3.89577e+006, 0.0}, {306050, 3.89577e+006, 0.0}
  };
  VecInt cells = {
    XMU_QUAD, 4, 0, 4, 5, 1,
    XMU_QUAD, 4, 1, 5, 6, 2,
    XMU_QUAD, 4, 2, 6, 7, 3,
    XMU_QUAD, 4, 4, 8, 9, 5,
    XMU_QUAD, 4, 5, 9, 10, 6,
    XMU_QUAD, 4, 6, 10, 11, 7
  };
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  XmWriteUGridToAsciiFile(ugrid, "tutorial.xmugrid");
  BSHP<XmUGrid2dPolylineDataExtractor> extractor = XmUGrid2dPolylineDataExtractor::New(ugrid);

  extractor->SetNoDataValue(-999.0);
  VecFlt pointScalars = {730.787f, 1214.54f, 1057.145f, 629.2069f, 351.1153f, 631.6649f, 1244.366f, 449.9133f, 64.04247f, 240.9716f, 680.0491f, 294.9547f};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_CELLS);

  VecFlt extractedData;
  VecPt3d extractedLocations;
  VecPt3d polyline = {
    {290764.0, 3895106.0, 0.0}, {291122.0, 3909108.0, 0.0},
    {302772.0, 3909130.0, 0.0}, {302794.0, 3895775.0, 0.0}
  };
  extractor->SetPolyline(polyline);
  extractor->ExtractData(extractedData);
  extractedLocations = extractor->GetExtractLocations();
  iWriteOutputVtkFile(extractedData, extractedLocations);

  VecFlt expectedData = {-999.000000f, 144.574036f, 299.485901f, 485.984100f, 681.852783f,
                         975.710388f, -999.000000f, -999.000000f, 862.843994f, 780.982544f,
                         882.343933f, 811.017395f, 504.402863f};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {
    {290764.00000000000, 3895106.0000000000, 0.0},
    {290780.97700328525, 3895770.0000000000, 0.0},
    {290862.47493036214, 3898957.5250696377, 0.0},
    {290934.38365947723, 3901770.0000000000, 0.0},
    {291012.05710306409, 3904807.9428969361, 0.0},
    {291087.79031566920, 3907770.0000000000, 0.0},
    {291122.00000000000, 3909108.0000000000, 0.0},
    {302772.00000000000, 3909130.0000000000, 0.0},
    {302774.24035941594, 3907770.0000000000, 0.0},
    {302778.73546838673, 3905041.2645316133, 0.0},
    {302784.12429801573, 3901770.0000000000, 0.0},
    {302788.63571589289, 3899031.3642841070, 0.0},
    {302794.00000000000, 3895775.0000000000, 0.0},
  };
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testTutorial

#endif
