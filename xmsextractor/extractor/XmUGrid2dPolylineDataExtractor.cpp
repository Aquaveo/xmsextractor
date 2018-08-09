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
#include <xmsgrid/ugrid/XmUGrid.h>
#include <xmsinterp/geometry/GmMultiPolyIntersector.h>

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

  virtual void ExtractData(const VecPt3d& a_polyline,
                           VecFlt& a_extractedData,
                           VecPt3d& a_extractedLocations) override;

  virtual void SetUseIdwForPointData(bool a_useIdw) override;
  virtual void SetNoDataValue(float a_noDataValue) override;

private:
  void ComputeExtractLocations(const VecPt3d& a_polyline, VecPt3d& a_locations);

  BSHP<XmUGrid> m_ugrid;                    ///< The ugrid which holds the points and the grid.
  BSHP<XmUGrid2dDataExtractor> m_extractor; ///< The data extractor.
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
/// \brief Extracts a scalar profile for the UGrid across the given polyline.
/// \param[in] a_polyline The polyline to extract the data across.
/// \param[out] a_extractedData The extracted values interpolated from the
///             scalar values.
/// \param[out] a_extractedLocations The locations of the extracted data values.
///             The polyline will get split up into locations across the UGrid.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorImpl::ExtractData(const VecPt3d& a_polyline,
                                                     VecFlt& a_extractedData,
                                                     VecPt3d& a_extractedLocations)
{
  ComputeExtractLocations(a_polyline, a_extractedLocations);
  m_extractor->SetExtractLocations(a_extractedLocations);
  m_extractor->ExtractData(a_extractedData);
} // XmUGrid2dPolylineDataExtractorImpl::ExtractData
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

  BSHP<GmMultiPolyIntersector> intersector = m_extractor->GetMultiPolyIntersector();

  // get points crossing cell edges
  Pt3d lastPoint = a_polyline[0];
  a_locations.push_back(lastPoint);

  for (size_t polyIdx = 1; polyIdx < a_polyline.size(); ++polyIdx)
  {
    Pt3d pt1 = a_polyline[polyIdx - 1];
    Pt3d pt2 = a_polyline[polyIdx];
    VecInt intersectIdxs;
    VecPt3d intersectPts;

    intersector->TraverseLineSegment(pt1.x, pt1.y, pt2.x, pt2.y, intersectIdxs,
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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

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
  extractor->ExtractData(polyline, extractedData, extractedLocations);

  VecFlt expectedData = {1.5, 2.5, XM_NODATA, XM_NODATA, 2.5, 2.25, 2.0};
  TS_ASSERT_EQUALS(expectedData, extractedData);
  VecPt3d expectedLocations = {{0.5, 0.5, 0}, {1.0, 0.5, 0}, {1.5, 0.5, 0.0},
    {1.5, 0.0, 0.0}, {1.0, 0.5, 0}, {0.75, 0.75, 0.0}, {0.5, 1.0, 0.0}};
  TS_ASSERT_EQUALS(expectedLocations, extractedLocations);
} // XmUGrid2dPolylineDataExtractorUnitTests::testTwoSegmentsJoinOnBoundary

#endif
