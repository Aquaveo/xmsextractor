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
  /// Type of triangles generated
  enum TriangleTypeEnum { NO_TRIANGLES, POINT_TRIANGLES, CELL_TRIANGLES };

  XmUGrid2dDataExtractorImpl(BSHP<XmUGrid> a_ugrid);
  XmUGrid2dDataExtractorImpl(BSHP<XmUGrid2dDataExtractorImpl> a_extractor);

  virtual void SetExtractLocations(const VecPt3d& a_locations) override;

  virtual void SetGridPointScalars(const VecFlt& a_pointScalars) override;
  //virtual void SetGridCellScalars(const VecFlt& a_cellScalars) override;
  //virtual void SetGridPointActivity(const DynBitset& a_activity) override;
  virtual void SetGridCellActivity(const DynBitset& a_activity) override;

  virtual void ExtractData(VecFlt& a_outData) override;

private:
  void PushPointDataToCentroids();
  //void PushCellDataToTriangles();

  BSHP<XmUGrid> m_ugrid;              ///< UGrid for dataset
  TriangleTypeEnum m_triangleType;    ///< keeps track of type of generated triangles
  BSHP<XmUGridTriangles> m_triangles; ///< manages triangles
  VecPt3d m_extractLocations;         ///< output locations for interpolated values
  VecFlt m_pointScalars;              ///< scalars to interpolate from
  DynBitset m_cellActivity;
};


////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractorImpl
/// \brief Implementation for XmUGrid2dDataExtractor which provides ability
///        to extract dataset values at points and along arcs for an
///        unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Construct from a UGrid.
//------------------------------------------------------------------------------
XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl(BSHP<XmUGrid> a_ugrid)
: m_ugrid(a_ugrid)
, m_triangleType(NO_TRIANGLES)
, m_triangles(new XmUGridTriangles())
, m_extractLocations()
, m_pointScalars()
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
{
} // XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl
//------------------------------------------------------------------------------
/// \brief Sets locations of points to extract interpolated scalar data from.
/// \param[in] a_locations The locations.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetExtractLocations(const VecPt3d& a_locations)
{
  m_extractLocations = a_locations;
} // XmUGrid2dDataExtractorImpl::SetExtractLocations
//------------------------------------------------------------------------------
/// \brief Setup point scalars to be used to extract interpolated data.
/// \param[in] a_pointScalars The point scalars.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetGridPointScalars(const VecFlt& a_pointScalars)
{
  if (m_triangleType != POINT_TRIANGLES)
  {
    m_triangles->BuildTriangles(*m_ugrid, false);
    m_triangleType = POINT_TRIANGLES;
  }
  m_pointScalars = a_pointScalars;
  PushPointDataToCentroids();
} // XmUGrid2dDataExtractorImpl::SetGridPointScalars
//------------------------------------------------------------------------------
/// \brief Set activity on cells
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetGridCellActivity(const DynBitset& a_cellActivity)
{
  m_cellActivity = a_cellActivity;
} // XmUGrid2dDataExtractorImpl::SetGridCellActivity
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
  for (const auto& pt: m_extractLocations)
  {
    int cellIdx = m_triangles->GetIntersectedCell(pt, interpIdxs, interpWeights);
    if (cellIdx >= 0 && (m_cellActivity.empty() || m_cellActivity[cellIdx]))
    {
      double interpValue = 0.0;
      for (size_t i = 0; i < interpIdxs.size(); ++i)
      {
        int ptIdx = interpIdxs[i];
        double weight = interpWeights[i];
        interpValue += m_pointScalars[ptIdx]*weight;
      }
      a_outData.push_back(static_cast<float>(interpValue));
    }
    else
    {
      a_outData.push_back(XM_NODATA);
    }
  }
} // XmUGrid2dDataExtractorImpl::ExtractData
//------------------------------------------------------------------------------
/// \brief Push point scalar data to cell centroids using average.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::PushPointDataToCentroids()
{
  m_pointScalars.resize(m_triangles->GetPoints().size());
  VecInt cellPoints;
  int numCells = m_ugrid->GetNumberOfCells();
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    int centroidIdx = m_triangles->GetCellCentroid(cellIdx);
    if (centroidIdx >= 0)
    {
      m_ugrid->GetPointsOfCell(cellIdx, cellPoints);
      double sum = 0.0;
      for (auto ptIdx: cellPoints)
        sum += m_pointScalars[ptIdx];
      double average = sum/cellPoints.size();
      m_pointScalars[centroidIdx] = static_cast<float>(average);
    }
  }
} // XmUGrid2dDataExtractorImpl::SetGridPointScalars

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
/// \brief Test extractor built by copying triangles.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testPointScalarsOnly()
{
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2, XMU_TRIANGLE, 3, 2, 3, 0};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  extractor->SetGridPointScalars({1, 2, 3, 2});
  extractor->SetExtractLocations({
    Pt3d(0.0, 0.0, 0.0),
    Pt3d(0.25, 0.75, 100.0),
    Pt3d(0.5, 0.5, 0.0),
    Pt3d(0.75, 0.25, -100.0),
    Pt3d(-1.0, -1.0, 0.0)});

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = { 1.0, 2.0, 2.0, 2.0, XM_NODATA };
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testScalarsOnly
//------------------------------------------------------------------------------
/// \brief Test extractor built by copying triangles.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testPointScalarCellActivity()
{
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2, XMU_TRIANGLE, 3, 2, 3, 0};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  extractor->SetGridPointScalars({1, 2, 3, 2});
  extractor->SetExtractLocations({
    Pt3d(0.25, 0.75, 100.0),
    Pt3d(0.75, 0.25, -100.0),
    Pt3d(-1.0, -1.0, 0.0)});
  DynBitset cellActivity;
  cellActivity.push_back(true);
  cellActivity.push_back(false);
  extractor->SetGridCellActivity(cellActivity);

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = { XM_NODATA, 2.0, XM_NODATA };
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testPointScalarCellActivity
//------------------------------------------------------------------------------
/// \brief Test extractor built by copying triangles.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testCopiedExtractor()
{
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 1, 2, 3};
  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  extractor->SetGridPointScalars({1, 2, 3, 4});
  extractor->SetExtractLocations({Pt3d(0.5, 0.5, 0.0)});
  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = { static_cast<float>((1.0 + 2.0 + 3.0 + 4.0)/4) };
  TS_ASSERT_EQUALS(expected, interpValues);

  BSHP<XmUGrid2dDataExtractor> extractor2 = XmUGrid2dDataExtractor::New(extractor);
  extractor2->SetGridPointScalars({1, 2, 3, 4});
  extractor2->SetExtractLocations({Pt3d(0.5, 0.5, 0.0)});
  extractor2->ExtractData(interpValues);
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testCopiedExtractor

#endif
