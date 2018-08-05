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
#include <xmscore/misc/XmError.h>
#include <xmscore/misc/XmLog.h>
#include <xmscore/misc/xmstype.h>
#include <xmsextractor/ugrid/XmUGridTriangles2d.h>
#include <xmsinterp/geometry/geoms.h>
#include <xmsinterp/geometry/GmTriSearch.h>
#include <xmsinterp/interpolate/InterpUtil.h>
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
  XmUGrid2dDataExtractorImpl(BSHP<XmUGrid2dDataExtractorImpl> a_extractor);

  virtual void SetGridPointScalars(const VecFlt& a_pointScalars,
                                   const DynBitset& a_activity,
                                   DataLocationEnum a_activityType) override;
  virtual void SetGridCellScalars(const VecFlt& a_cellScalars,
                                  const DynBitset& a_activity,
                                  DataLocationEnum a_activityType) override;

  virtual void SetExtractLocations(const VecPt3d& a_locations) override;
  virtual void ExtractData(VecFlt& a_outData) override;

  virtual void SetUseIdwForPointData(bool a_) override;
  virtual void SetNoDataValue(float a_value) override;

private:
  void BuildTriangles(DataLocationEnum a_location);
  void ApplyActivity(const DynBitset& a_activity,
                     DataLocationEnum a_location,
                     DynBitset& a_cellActivity);
  void SetGridPointActivity(const DynBitset& a_pointActivity, DynBitset& a_cellActivity);
  void SetGridCellActivity(const DynBitset& a_cellActivity);
  void PushPointDataToCentroids(const DynBitset& a_cellActivity);
  void PushCellDataToTrianglePoints(const VecFlt& a_cellScalars, const DynBitset& a_cellActivity);
  float CalculatePointByAverage(const VecInt& a_cellIdxs,
                                const VecFlt& a_cellScalars,
                                const DynBitset& a_cellActivity);
  float CalculatePointByIdw(int a_pointIdx,
                            const VecInt& a_cellIdxs,
                            const VecFlt& a_cellScalars,
                            const DynBitset& a_cellActivity);

  BSHP<XmUGrid> m_ugrid;              ///< UGrid for dataset
  DataLocationEnum m_triangleType;    ///< if triangles been generated for points or cells
  BSHP<XmUGridTriangles> m_triangles; ///< manages triangles
  VecPt3d m_extractLocations;         ///< output locations for interpolated values
  VecFlt m_pointScalars;              ///< scalars to interpolate from
  bool m_useIdwForPointData;          ///< use IDW to calculate point data from cell data
  float m_noDataValue;                ///< value to use for inactive result
};

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractorImpl
/// \brief Implementation for XmUGrid2dDataExtractor which provides ability
///        to extract dataset values at points and along arcs for an
///        unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Construct from a UGrid.
/// \param[in] a_ugrid The UGrid to construct an extractor for.
//------------------------------------------------------------------------------
XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl(BSHP<XmUGrid> a_ugrid)
: m_ugrid(a_ugrid)
, m_triangleType(LOC_UNKNOWN)
, m_triangles(XmUGridTriangles::New())
, m_extractLocations()
, m_pointScalars()
, m_useIdwForPointData(false)
, m_noDataValue(XM_NODATA)
{
} // XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl
//------------------------------------------------------------------------------
/// \brief Create a new XmUGrid2dDataExtractorImpl using shallow copy from
///        existing extractor.
/// \param[in] a_extractor The extractor to shallow copy
/// \return the new XmUGrid2dDataExtractorImpl.
//------------------------------------------------------------------------------
XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl(BSHP<XmUGrid2dDataExtractorImpl> a_extractor)
: m_ugrid(a_extractor->m_ugrid)
, m_triangleType(a_extractor->m_triangleType)
, m_triangles(a_extractor->m_triangles)
, m_extractLocations()
, m_pointScalars()
, m_useIdwForPointData(a_extractor->m_useIdwForPointData)
, m_noDataValue(a_extractor->m_noDataValue)
{
} // XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl
//------------------------------------------------------------------------------
/// \brief Setup point scalars to be used to extract interpolated data.
/// \param[in] a_pointScalars The point scalars.
/// \param[in] a_activity The activity of the cells.
/// \param[in] a_activityType The location at which the data is currently stored.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetGridPointScalars(const VecFlt& a_pointScalars,
                                                     const DynBitset& a_activity,
                                                     DataLocationEnum a_activityType)
{
  if (a_pointScalars.size() != m_ugrid->GetNumberOfPoints())
  {
    XM_LOG(xmlog::debug, "Invalid scalar size in 2D data extractor.");
  }

  BuildTriangles(LOC_POINTS);

  DynBitset cellActivity;
  ApplyActivity(a_activity, a_activityType, cellActivity);

  m_pointScalars = a_pointScalars;
  PushPointDataToCentroids(cellActivity);
} // XmUGrid2dDataExtractorImpl::SetGridPointScalars
//------------------------------------------------------------------------------
/// \brief Setup cell scalars to be used to extract interpolated data.
/// \param[in] a_cellScalars The point scalars.
/// \param[in] a_activity The activity of the cells.
/// \param[in] a_activityType The location at which the data is currently stored.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetGridCellScalars(const VecFlt& a_cellScalars,
                                                    const DynBitset& a_activity,
                                                    DataLocationEnum a_activityType)
{
  if ((int)a_cellScalars.size() != m_ugrid->GetNumberOfCells())
  {
    XM_LOG(xmlog::debug, "Invalid scalar size in 2D data extractor.");
  }

  BuildTriangles(LOC_CELLS);

  DynBitset cellActivity;
  ApplyActivity(a_activity, a_activityType, cellActivity);

  PushCellDataToTrianglePoints(a_cellScalars, cellActivity);
} // XmUGrid2dDataExtractorImpl::SetGridCellScalars
//------------------------------------------------------------------------------
/// \brief Sets locations of points to extract interpolated scalar data from.
/// \param[in] a_locations The locations.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetExtractLocations(const VecPt3d& a_locations)
{
  m_extractLocations = a_locations;
} // XmUGrid2dDataExtractorImpl::SetExtractLocations
//------------------------------------------------------------------------------
/// \brief Extract interpolated data for the previously set locations.
/// \param[in] a_outData The interpolated scalars.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::ExtractData(VecFlt& a_outData)
{
  a_outData.clear();

  a_outData.reserve(m_extractLocations.size());
  VecInt interpIdxs;
  VecDbl interpWeights;
  for (const auto& pt : m_extractLocations)
  {
    int cellIdx = m_triangles->GetIntersectedCell(pt, interpIdxs, interpWeights);
    if (cellIdx >= 0)
    {
      double interpValue = 0.0;
      for (size_t i = 0; i < interpIdxs.size(); ++i)
      {
        int ptIdx = interpIdxs[i];
        double weight = interpWeights[i];
        interpValue += m_pointScalars[ptIdx] * weight;
      }
      a_outData.push_back(static_cast<float>(interpValue));
    }
    else
    {
      a_outData.push_back(m_noDataValue);
    }
  }
} // XmUGrid2dDataExtractorImpl::ExtractData
//------------------------------------------------------------------------------
/// \brief Set to use IDW to calculate point scalar values from cell scalars.
/// \param a_ Whether to turn IDW on or off.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetUseIdwForPointData(bool a_)
{
  m_useIdwForPointData = a_;
} // XmUGrid2dDataExtractorImpl::SetUseIdwForPointData
//------------------------------------------------------------------------------
/// \brief Set value to use when extracted value is in inactive cell or doen't
///        intersect with the grid.
/// \param[in] a_value The no data value
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetNoDataValue(float a_value)
{
  m_noDataValue = a_value;
} // XmUGrid2dDataExtractorImpl::SetNoDataValue
//------------------------------------------------------------------------------
/// \brief Build triangles for UGrid for either point or cell scalars.
/// \param[in] a_location Location to build on (points or cells).
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::BuildTriangles(DataLocationEnum a_location)
{
  if (m_triangleType != a_location)
  {
    bool triangleCentroids = a_location == LOC_CELLS;
    m_triangles->BuildTriangles(*m_ugrid, triangleCentroids);
    m_triangleType = a_location;
  }
} // XmUGrid2dDataExtractorImpl::BuildTriangles
//------------------------------------------------------------------------------
/// \brief Apply point or cell activity to triangles.
/// \param[in] a_activity The activity of the scalar values.
/// \param[in] a_location The location of the activity (cells or points).
/// \param[out] a_cellActivity The cell activity of the scalar values.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::ApplyActivity(const DynBitset& a_activity,
                                               DataLocationEnum a_location,
                                               DynBitset& a_cellActivity)
{
  if (!a_activity.empty())
  {
    if (a_location == LOC_POINTS)
    {
      SetGridPointActivity(a_activity, a_cellActivity);
    }
    else if (a_location == LOC_CELLS)
    {
      SetGridCellActivity(a_activity);
      a_cellActivity = a_activity;
    }
  }
} // XmUGrid2dDataExtractorImpl::ApplyActivity
//------------------------------------------------------------------------------
/// \brief Set point activity. Turns off each cell attached to an inactive
///        point.
/// \param[in] a_pointActivity The activity for the UGrid points.
/// \param[out] a_cellActivity The resulting activity transfered to the cells.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetGridPointActivity(const DynBitset& a_pointActivity,
                                                      DynBitset& a_cellActivity)
{
  if (a_pointActivity.size() != m_ugrid->GetNumberOfPoints() && !a_pointActivity.empty())
  {
    XM_LOG(xmlog::debug, "Invalid point activity size in 2D data extractor.");
  }
  a_cellActivity.reset();
  a_cellActivity.resize(m_ugrid->GetNumberOfCells(), true);
  VecInt attachedCells;
  int numPoints = m_ugrid->GetNumberOfPoints();
  for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
  {
    if (!a_pointActivity[pointIdx])
    {
      m_ugrid->GetPointCells(pointIdx, attachedCells);
      for (auto cellIdx : attachedCells)
      {
        a_cellActivity[cellIdx] = false;
      }
    }
  }
  m_triangles->SetCellActivity(a_cellActivity);
} // XmUGrid2dDataExtractorImpl::SetGridPointActivity
//------------------------------------------------------------------------------
/// \brief Set activity on cells
/// \param[in] a_cellActivity The cell activity of the scalar values.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetGridCellActivity(const DynBitset& a_cellActivity)
{
  if (a_cellActivity.size() != m_ugrid->GetNumberOfCells() && !a_cellActivity.empty())
  {
    XM_LOG(xmlog::debug, "Invalid cell activity size in 2D data extractor.");
  }
  m_triangles->SetCellActivity(a_cellActivity);
} // XmUGrid2dDataExtractorImpl::SetGridCellActivity
//------------------------------------------------------------------------------
/// \brief Push point scalar data to cell centroids using average.
/// \param[in] a_cellActivity The cell activity of the scalar values.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::PushPointDataToCentroids(const DynBitset& a_cellActivity)
{
  m_pointScalars.resize(m_triangles->GetPoints().size());
  VecInt cellPoints;
  int numCells = m_ugrid->GetNumberOfCells();
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    if (a_cellActivity.empty() || a_cellActivity[cellIdx])
    {
      int centroidIdx = m_triangles->GetCellCentroid(cellIdx);
      if (centroidIdx >= 0)
      {
        m_ugrid->GetPointsOfCell(cellIdx, cellPoints);
        double sum = 0.0;
        for (auto ptIdx : cellPoints)
          sum += m_pointScalars[ptIdx];
        double average = sum / cellPoints.size();
        m_pointScalars[centroidIdx] = static_cast<float>(average);
      }
    }
  }
} // XmUGrid2dDataExtractorImpl::PushPointDataToCentroids
//------------------------------------------------------------------------------
/// \brief Push cell scalar data to triangle points using cells connected to
///        a point with average or IDW.
/// \param[in] a_cellScalars the cell scalar values.
/// \param[in] a_cellActivity the cell activity vector.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::PushCellDataToTrianglePoints(const VecFlt& a_cellScalars,
                                                              const DynBitset& a_cellActivity)
{
  m_pointScalars.resize(m_triangles->GetPoints().size());
  VecInt cellIdxs;
  int numPoints = m_ugrid->GetNumberOfPoints();
  for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
  {
    m_ugrid->GetPointCells(pointIdx, cellIdxs);
    if (m_useIdwForPointData)
    {
      m_pointScalars[pointIdx] =
        CalculatePointByIdw(pointIdx, cellIdxs, a_cellScalars, a_cellActivity);
    }
    else
    {
      m_pointScalars[pointIdx] = CalculatePointByAverage(cellIdxs, a_cellScalars, a_cellActivity);
    }
  }

  int numCells = m_ugrid->GetNumberOfCells();
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    int pointIdx = m_triangles->GetCellCentroid(cellIdx);
    if (pointIdx >= 0)
      m_pointScalars[pointIdx] = a_cellScalars[cellIdx];
  }
} // XmUGrid2dDataExtractorImpl::PushCellDataToTrianglePoints
//------------------------------------------------------------------------------
/// \brief Calculate the average of the cell values connected to a point.
/// \param[in] a_cellIdxs the cells surrounding the point.
/// \param[in] a_cellScalars the cell scalar values.
/// \param[in] a_cellActivity the cell activity vector.
/// \return Average of active surrounding cell scalars.
//------------------------------------------------------------------------------
float XmUGrid2dDataExtractorImpl::CalculatePointByAverage(const VecInt& a_cellIdxs,
                                                          const VecFlt& a_cellScalars,
                                                          const DynBitset& a_cellActivity)
{
  double sum = 0.0;
  int sumCount = 0;
  for (auto cellIdx : a_cellIdxs)
  {
    if (a_cellActivity.empty() || a_cellActivity[cellIdx])
    {
      sum += a_cellScalars[cellIdx];
      ++sumCount;
    }
  }
  double average;
  if (sumCount)
    average = sum / sumCount;
  else
    average = m_noDataValue;
  return static_cast<float>(average);
} // XmUGrid2dDataExtractorImpl::CalculatePointByAverage
//------------------------------------------------------------------------------
/// \brief Calculate the point value by IDW method from surrounding cells.
/// \param[in] a_pointIdx the point to calculate the value for.
/// \param[in] a_cellIdxs the cells surrounding the point.
/// \param[in] a_cellScalars the cell scalar values.
/// \param[in] a_cellActivity the cell activity vector.
/// \return IDW interpolated value of active surrounding cell scalars.
//------------------------------------------------------------------------------
float XmUGrid2dDataExtractorImpl::CalculatePointByIdw(int a_pointIdx,
                                                      const VecInt& a_cellIdxs,
                                                      const VecFlt& a_cellScalars,
                                                      const DynBitset& a_cellActivity)
{
  Pt3d pt = m_ugrid->GetPoint(a_pointIdx);
  VecInt cellCentroids;
  for (auto cellIdx : a_cellIdxs)
  {
    int centroidIdx = m_triangles->GetCellCentroid(cellIdx);
    if (0 <= centroidIdx && a_cellActivity[cellIdx])
    {
      cellCentroids.push_back(centroidIdx);
    }
  }
  if (!cellCentroids.empty())
  {
    VecDbl d2;
    VecDbl weights;
    inDistanceSquared(pt, cellCentroids, m_triangles->GetPoints(), true, d2);
    inIdwWeights(d2, 2, false, weights);

    double interpValue = 0.0;
    for (size_t cellIdx = 0; cellIdx < cellCentroids.size(); ++cellIdx)
    {
      int ptIdx = cellCentroids[cellIdx];
      double weight = weights[cellIdx];
      interpValue += a_cellScalars[cellIdx] * weight;
    }
    return static_cast<float>(interpValue);
  }
  else
  {
    return CalculatePointByAverage(a_cellIdxs, a_cellScalars, a_cellActivity);
  }
} // XmUGrid2dDataExtractorImpl::CalculatePointByIdw
////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractor
/// \brief Provides ability to extract dataset values at points and along arcs
///        for an unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Create a new XmUGrid2dDataExtractor.
/// \param[in] a_ugrid The UGrid geometry to use to extract values from
/// \return the new XmUGrid2dDataExtractor
//------------------------------------------------------------------------------
BSHP<XmUGrid2dDataExtractor> XmUGrid2dDataExtractor::New(BSHP<XmUGrid> a_ugrid)
{
  BSHP<XmUGrid2dDataExtractor> extractor(new XmUGrid2dDataExtractorImpl(a_ugrid));
  return extractor;
} // XmUGrid2dDataExtractor::New
//------------------------------------------------------------------------------
/// \brief Create a new XmUGrid2dDataExtractor using shallow copy from existing
///        extractor.
/// \param[in] a_extractor The extractor to shallow copy
/// \return the new XmUGrid2dDataExtractor.
//------------------------------------------------------------------------------
BSHP<XmUGrid2dDataExtractor> XmUGrid2dDataExtractor::New(BSHP<XmUGrid2dDataExtractor> a_extractor)
{
  BSHP<XmUGrid2dDataExtractorImpl> copied = BDPC<XmUGrid2dDataExtractorImpl>(a_extractor);
  if (copied)
  {
    BSHP<XmUGrid2dDataExtractor> extractor(new XmUGrid2dDataExtractorImpl(copied));
    return extractor;
  }

  XM_ASSERT(0);
  return nullptr;
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

#include <xmscore/testing/TestTools.h>

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractorUnitTests
/// \brief Class to to test XmUGrid2dDataExtractor
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Test extractor with point scalars only.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testPointScalarsOnly()
{
  //  3----2
  //  | 1 /|
  //  |  / |
  //  | /  |
  //  |/ 0 |
  //  0----1
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2, XMU_TRIANGLE, 3, 2, 3, 0};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);
  extractor->SetNoDataValue(-999.0);

  VecFlt pointScalars = {1, 2, 3, 2};
  extractor->SetGridPointScalars(pointScalars);
  extractor->SetExtractLocations({Pt3d(0.0, 0.0, 0.0), Pt3d(0.25, 0.75, 100.0), Pt3d(0.5, 0.5, 0.0),
                                  Pt3d(0.75, 0.25, -100.0), Pt3d(-1.0, -1.0, 0.0)});

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {1.0, 2.0, 2.0, 2.0, -999.0};
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testScalarsOnly
//------------------------------------------------------------------------------
/// \brief Test extractor when using point scalars and cell activity.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testPointScalarCellActivity()
{
  //  3----2
  //  | 1 /|
  //  |  / |
  //  | /  |
  //  |/ 0 |
  //  0----1
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2, XMU_TRIANGLE, 3, 2, 3, 0};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  VecFlt pointScalars = {1, 2, 3, 2};
  DynBitset cellActivity;
  cellActivity.push_back(true);
  cellActivity.push_back(false);
  extractor->SetGridPointScalars(pointScalars, cellActivity, XmUGrid2dDataExtractor::LOC_CELLS);
  extractor->SetExtractLocations(
    {Pt3d(0.25, 0.75, 100.0), Pt3d(0.75, 0.25, -100.0), Pt3d(-1.0, -1.0, 0.0)});

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {XM_NODATA, 2.0, XM_NODATA};
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testPointScalarCellActivity
//------------------------------------------------------------------------------
/// \brief Test extractor when using point scalars and point activity.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testPointScalarPointActivity()
{
  // clang-format off
  ///  6----7---------8 point row 3
  ///  |   / \       /|
  ///  |  /   \     / |
  ///  | /     \   /  |
  ///  |/       \ /   |
  ///  3---------4----5 point row 2
  ///  |\       / \   |
  ///  | \     /   \  |
  ///  |  \   /     \ |
  ///  |   \ /       \|
  ///  0----1---------2 point row 1
  ///
  VecPt3d points = {
    {0, 0, 0}, {1, 0, 0}, {3, 0, 0},  // row 1 of points
    {0, 1, 0}, {2, 1, 0}, {3, 1, 0},  // row 2 of points
    {0, 2, 0}, {1, 2, 0}, {3, 2, 0}}; // row 3 of points
  VecInt cells = {
    XMU_TRIANGLE, 3, 0, 1, 3, // row 1 of triangles
    XMU_TRIANGLE, 3, 1, 4, 3,
    XMU_TRIANGLE, 3, 1, 2, 4,
    XMU_TRIANGLE, 3, 2, 5, 4,

    XMU_TRIANGLE, 3, 3, 7, 6, // row 2 of triangles
    XMU_TRIANGLE, 3, 3, 4, 7,
    XMU_TRIANGLE, 3, 4, 8, 7,
    XMU_TRIANGLE, 3, 4, 5, 8};

  VecFlt pointScalars = {
    0, 0, 0,  // row 1
    1, 1, 1,  // row 2
    2, 2, 2}; // row 3

  // extract value for each cell
  VecPt3d extractLocations = {
    {0.25, 0.25, 0}, // cell 0
    {1.00, 0.25, 0}, // cell 1
    {2.00, 0.50, 0}, // cell 2
    {2.75, 0.75, 0}, // cell 3
    {0.25, 1.75, 0}, // cell 4
    {1.00, 1.25, 0}, // cell 5
    {1.50, 1.75, 0}, // cell 6
    {2.75, 1.25, 0}  // cell 7
  };

  // expected results with point 4 inactive
  VecFlt expectedPerCell = {
    0.25, XM_NODATA, XM_NODATA, XM_NODATA,
    1.75, XM_NODATA, XM_NODATA, XM_NODATA };
  // clang-format on

  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  // set point 4 inactive
  // should cause all cells connected to point 4 to return XM_NODATA
  DynBitset pointActivity;
  pointActivity.resize(9, true);
  pointActivity[4] = false;
  extractor->SetGridPointScalars(pointScalars, pointActivity, XmUGrid2dDataExtractor::LOC_POINTS);

  // extract interpolated scalar for each cell
  extractor->SetExtractLocations(extractLocations);

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  TS_ASSERT_EQUALS(expectedPerCell, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testPointScalarPointActivity
//------------------------------------------------------------------------------
/// \brief Test extractor with cell scalars only.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testCellScalarsOnly()
{
  //  3----2
  //  | 1 /|
  //  |  / |
  //  | /  |
  //  |/ 0 |
  //  0----1
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2, XMU_TRIANGLE, 3, 2, 3, 0};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  VecFlt cellScalars = {1, 2};
  extractor->SetGridCellScalars(cellScalars);
  extractor->SetExtractLocations({Pt3d(0.0, 0.0, 0.0), Pt3d(0.25, 0.75, 100.0), Pt3d(0.5, 0.5, 0.0),
                                  Pt3d(0.75, 0.25, -100.0), Pt3d(-1.0, -1.0, 0.0)});

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {1.5, 2.0, 1.5, 1.0, XM_NODATA};
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testCellScalarsOnly
//------------------------------------------------------------------------------
/// \brief Test extractor when using cell scalars and cell activity.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testCellScalarCellActivity()
{
  // clang-format off
  //  6----7---------8 point row 3
  //  | 4 / \       /|
  //  |  /   \  6  / |
  //  | /  5  \   /  |
  //  |/       \ / 7 |
  //  3---------4----5 point row 2
  //  |\       / \ 3 |
  //  | \  1  /   \  |
  //  |  \   /     \ |
  //  | 0 \ /   2   \|
  //  0----1---------2 point row 1
  //
  VecPt3d points = {
    {0, 0, 0}, {1, 0, 0}, {3, 0, 0},  // row 1 of points
    {0, 1, 0}, {2, 1, 0}, {3, 1, 0},  // row 2 of points
    {0, 2, 0}, {1, 2, 0}, {3, 2, 0}}; // row 3 of points
  VecInt cells = {
    XMU_TRIANGLE, 3, 0, 1, 3, // row 1 of triangles
    XMU_TRIANGLE, 3, 1, 4, 3,
    XMU_TRIANGLE, 3, 1, 2, 4,
    XMU_TRIANGLE, 3, 2, 5, 4,

    XMU_TRIANGLE, 3, 3, 7, 6, // row 2 of triangles
    XMU_TRIANGLE, 3, 3, 4, 7,
    XMU_TRIANGLE, 3, 4, 8, 7,
    XMU_TRIANGLE, 3, 4, 5, 8};

  VecFlt cellScalars = {
    2, 4, 6, 8,  // row 1
    4, 6, 8, 10  // row 2
  };

  // extract value for each cell
  VecPt3d extractLocations = {
    {0.25, 0.25, 0}, // cell 0
    {1.00, 0.25, 0}, // cell 1
    {2.00, 0.50, 0}, // cell 2
    {2.75, 0.75, 0}, // cell 3
    {0.25, 1.75, 0}, // cell 4
    {1.00, 1.25, 0}, // cell 5
    {1.50, 1.75, 0}, // cell 6
    {2.75, 1.25, 0}  // cell 7
  };

  // expected results with point 4 inactive
  VecFlt expectedPerCell = {
    XM_NODATA, 4.0000, XM_NODATA, 8.2500, // row 1 cells
    XM_NODATA, 6.0000, XM_NODATA, 9.750   // row 2 cells
  };
  // clang-format on

  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  // set point 4 inactive
  // should cause all cells connected to point 4 to return XM_NODATA
  DynBitset cellActivity;
  cellActivity.resize(8, true);
  cellActivity[0] = false;
  cellActivity[2] = false;
  cellActivity[4] = false;
  cellActivity[6] = false;
  extractor->SetGridCellScalars(cellScalars, cellActivity, XmUGrid2dDataExtractor::LOC_CELLS);

  // extract interpolated scalar for each cell
  extractor->SetExtractLocations(extractLocations);

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  TS_ASSERT_EQUALS(expectedPerCell, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testCellScalarCellActivity
//------------------------------------------------------------------------------
/// \brief Test extractor when using cell scalars and cell activity.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testCellScalarCellActivityIdw()
{
  // clang-format off
  //  6----7---------8 point row 3
  //  | 4 / \       /|
  //  |  /   \  6  / |
  //  | /  5  \   /  |
  //  |/       \ / 7 |
  //  3---------4----5 point row 2
  //  |\       / \ 3 |
  //  | \  1  /   \  |
  //  |  \   /     \ |
  //  | 0 \ /   2   \|
  //  0----1---------2 point row 1
  //
  VecPt3d points = {
    {0, 0, 0}, {1, 0, 0}, {3, 0, 0},  // row 1 of points
    {0, 1, 0}, {2, 1, 0}, {3, 1, 0},  // row 2 of points
    {0, 2, 0}, {1, 2, 0}, {3, 2, 0}}; // row 3 of points
  VecInt cells = {
    XMU_TRIANGLE, 3, 0, 1, 3, // row 1 of triangles
    XMU_TRIANGLE, 3, 1, 4, 3,
    XMU_TRIANGLE, 3, 1, 2, 4,
    XMU_TRIANGLE, 3, 2, 5, 4,

    XMU_TRIANGLE, 3, 3, 7, 6, // row 2 of triangles
    XMU_TRIANGLE, 3, 3, 4, 7,
    XMU_TRIANGLE, 3, 4, 8, 7,
    XMU_TRIANGLE, 3, 4, 5, 8};

  VecFlt cellScalars = {
    2, 4, 6, 8,  // row 1
    4, 6, 8, 10  // row 2
  };

  // extract value for each cell
  VecPt3d extractLocations = {
    {0.25, 0.25, 0}, // cell 0
    {1.00, 0.25, 0}, // cell 1
    {2.00, 0.50, 0}, // cell 2
    {2.75, 0.75, 0}, // cell 3
    {0.25, 1.75, 0}, // cell 4
    {1.00, 1.25, 0}, // cell 5
    {1.50, 1.75, 0}, // cell 6
    {2.75, 1.25, 0}  // cell 7
  };

  // expected results with point 4 inactive
  VecFlt expectedPerCell = {
    2.0f, 3.4444f, XM_NODATA, 6.75f, // row 1 cells
    3.5f, 5.7303f, 5.4652f, 8.25f    // row 2 cells
  };
  // clang-format on

  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);
  extractor->SetUseIdwForPointData(true);

  // set point 4 inactive
  // should cause all cells connected to point 4 to return XM_NODATA
  DynBitset cellActivity;
  cellActivity.resize(8, true);
  cellActivity[2] = false;
  extractor->SetGridCellScalars(cellScalars, cellActivity, XmUGrid2dDataExtractor::LOC_CELLS);

  // extract interpolated scalar for each cell
  extractor->SetExtractLocations(extractLocations);

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  TS_ASSERT_DELTA_VEC(expectedPerCell, interpValues, 0.001);
} // XmUGrid2dDataExtractorUnitTests::testCellScalarCellActivityIdw
//------------------------------------------------------------------------------
/// \brief Test extractor when using point scalars and point activity.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testCellScalarPointActivity()
{
  //  3----2
  //  | 1 /|
  //  |  / |
  //  | /  |
  //  |/ 0 |
  //  0----1
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2, XMU_TRIANGLE, 3, 2, 3, 0};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  DynBitset pointActivity;
  pointActivity.resize(4, true);
  pointActivity[1] = false;
  VecFlt cellScalars = {1, 2};
  extractor->SetGridCellScalars(cellScalars, pointActivity, XmUGrid2dDataExtractor::LOC_POINTS);
  extractor->SetExtractLocations({Pt3d(0.0, 0.0, 0.0), Pt3d(0.25, 0.75, 100.0), Pt3d(0.5, 0.5, 0.0),
                                  Pt3d(0.75, 0.25, -100.0), Pt3d(-1.0, -1.0, 0.0)});

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {2.0, 2.0, 2.0, XM_NODATA, XM_NODATA};
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testCellScalarPointActivity
//------------------------------------------------------------------------------
/// \brief Test extractor built by copying triangles.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testCopiedExtractor()
{
  //  3----2
  //  | 1 /|
  //  |  / |
  //  | /  |
  //  |/ 0 |
  //  0----1
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  VecFlt pointScalars = {1, 2, 3, 4};
  extractor->SetGridPointScalars(pointScalars);
  extractor->SetExtractLocations({Pt3d(0.5, 0.5, 0.0)});
  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {static_cast<float>((1.0 + 2.0 + 3.0 + 4.0) / 4)};
  TS_ASSERT_EQUALS(expected, interpValues);

  BSHP<XmUGrid2dDataExtractor> extractor2 = XmUGrid2dDataExtractor::New(extractor);
  extractor2->SetGridPointScalars(pointScalars);
  extractor2->SetExtractLocations({Pt3d(0.5, 0.5, 0.0)});
  extractor2->ExtractData(interpValues);
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testCopiedExtractor

#endif
