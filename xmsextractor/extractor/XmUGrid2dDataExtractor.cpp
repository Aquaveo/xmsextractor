//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
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
#include <xmsgrid/ugrid/XmEdge.h>
#include <xmsgrid/ugrid/XmUGrid.h>
#include <xmsgrid/geometry/geoms.h>
#include <xmsgrid/geometry/GmTriSearch.h>
#include <xmsinterp/interpolate/InterpUtil.h>

// 6. Non-shared code headers
#include <xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h>

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

class ScalarToPt
{
public:
  ScalarToPt()
  : m_triWt(3, 0.0)
  {
  }

  VecDbl m_triWt;       ///< weight from linear interpolation
  VecInt m_cellIdxs[3]; ///< cell indexes to interpolate to an extract location
  VecInt m_ptIdxs[3];   ///< pt indexes to interpolate to an extract location
  VecDbl m_idwWts[3];   ///< idw weights if the option is on
}; 
////////////////////////////////////////////////////////////////////////////////
/// Implementation for XmUGrid2dDataExtractor
class XmUGrid2dDataExtractorImpl : public XmUGrid2dDataExtractor
{
public:
  XmUGrid2dDataExtractorImpl(std::shared_ptr<XmUGrid> a_ugrid);
  XmUGrid2dDataExtractorImpl(BSHP<XmUGrid2dDataExtractorImpl> a_extractor);

  virtual void SetGridPointScalars(const VecFlt& a_pointScalars,
                                   const DynBitset& a_activity,
                                   DataLocationEnum a_activityLocation) override;
  virtual void SetGridCellScalars(const VecFlt& a_cellScalars,
                                  const DynBitset& a_activity,
                                  DataLocationEnum a_activityLocation) override;

  virtual void SetExtractLocations(const VecPt3d& a_locations) override;
  virtual void ExtractData(VecFlt& a_outData) override;
  virtual float ExtractAtLocation(const Pt3d& a_location) override;

  virtual void SetUseIdwForPointData(bool a_) override;
  virtual void SetNoDataValue(float a_value) override;
  virtual void SetSplitTrisWithCellData(bool a_splitTrisWithCellData) override;

  virtual void BuildTriangles(DataLocationEnum a_location) override;
  virtual const BSHP<XmUGridTriangles2d> GetUGridTriangles() const override;

  /// \brief Gets the scalars
  /// \return The scalars.
  virtual const VecFlt& GetScalars() const override { return m_pointScalars; }
  /// \brief Gets the location of the scalars (points or cells)
  /// \return The location of the scalars.
  virtual DataLocationEnum GetScalarLocation() const override { return m_scalarLocation; }
  /// \brief Gets locations of points to extract interpolated scalar data from.
  /// \return The locations.
  virtual const VecPt3d& GetExtractLocations() const override { return m_extractLocations; }
  /// \brief Gets cell indexes associated with the extract location points.
  /// \return The cell indexes.
  virtual const VecInt& GetCellIndexes() const { return m_cellIdxs; }
  /// \brief Gets the option for using IDW for point data
  /// \return The option.
  virtual bool GetUseIdwForPointData() const override { return m_useIdwForPointData; }
  /// \brief Gets the no data value
  /// \return The no data value.
  virtual float GetNoDataValue() const override { return m_noDataValue; }
  /// \brief Gets the option for splitting triangles with cell data
  /// \return The option.
  virtual bool GetSplitTrisWithCellData() const override { return m_splitTrisWithCellData; }


private:
  void ApplyActivity(const DynBitset& a_activity,
                     DataLocationEnum a_location,
                     DynBitset& a_cellActivity);
  void SetGridPointActivity(const DynBitset& a_pointActivity, DynBitset& a_cellActivity);
  void SetGridCellActivity(DynBitset& a_cellActivity);
  void PushPointDataToCentroids(const DynBitset& a_cellActivity);
  void PushCellDataToTrianglePoints(const VecFlt& a_cellScalars, const DynBitset& a_cellActivity);
  float CalculatePointByAverage(const VecInt& a_cellIdxs,
                                const VecFlt& a_cellScalars,
                                const DynBitset& a_cellActivity);
  float CalculatePointByIdw(int a_pointIdx,
                            const VecInt& a_cellIdxs,
                            const VecFlt& a_cellScalars,
                            const DynBitset& a_cellActivity);
  void CalcExtractLocationInterp();
  bool CurrExtractLocationInActiveCell(int a_idx);
  float ExtractValueFromCellScalars(int a_idx);
  float ExtractValueFromPointScalars(int a_idx);

  std::shared_ptr<XmUGrid> m_ugrid;  ///< UGrid for dataset
  DataLocationEnum m_scalarLocation; ///< if triangles been generated for points or cells
  BSHP<XmUGridTriangles2d>
    m_triangles;                ///< triangles generated from UGrid to use for data extraction
  VecPt3d m_extractLocations;   ///< output locations for interpolated values
  VecFlt m_pointScalars;        ///< scalars to interpolate from
  VecFlt m_ptScalars;           ///< ugrid point scalars
  VecFlt m_cellScalars;         ///< ugrid cell scalars
  VecInt m_cellIdxs;            ///< ugrid cell indexes for extract locations
  VecInt m_ptIdxs;              ///< ugrid point indexes for extract locations
                                ///< if an extract location is at a ugrid point
  VecInt m_edgeIdx0;            ///< ugrid point index for extract location on edge
  VecInt m_edgeIdx1;            ///< ugrid point index for extract location on edge
  bool m_useIdwForPointData;    ///< use IDW to calculate point data from cell data
  float m_noDataValue;          ///< value to use for inactive result
  bool m_splitTrisWithCellData; ///< split triangles with cell data
  DynBitset m_cellActivity;     ///< activity of the cells
  std::vector<ScalarToPt>
    m_extractInterp; ///< information for interpolating from scalars to extraction locations
};

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractorImpl
/// \brief Implementation for XmUGrid2dDataExtractor which provides ability
///        to extract dataset values at points for an unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Construct from a UGrid.
/// \param[in] a_ugrid The UGrid to construct an extractor for.
//------------------------------------------------------------------------------
XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl(std::shared_ptr<XmUGrid> a_ugrid)
: m_ugrid(a_ugrid)
, m_scalarLocation(LOC_UNKNOWN)
, m_triangles(XmUGridTriangles2d::New())
, m_extractLocations()
, m_pointScalars()
, m_cellIdxs()
, m_useIdwForPointData(false)
, m_noDataValue(XM_NODATA)
, m_splitTrisWithCellData(false)
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
, m_scalarLocation(a_extractor->m_scalarLocation)
, m_triangles(a_extractor->m_triangles)
, m_extractLocations()
, m_pointScalars()
, m_cellIdxs()
, m_useIdwForPointData(a_extractor->m_useIdwForPointData)
, m_noDataValue(a_extractor->m_noDataValue)
, m_splitTrisWithCellData(a_extractor->m_splitTrisWithCellData)
{
} // XmUGrid2dDataExtractorImpl::XmUGrid2dDataExtractorImpl
//------------------------------------------------------------------------------
/// \brief Setup point scalars to be used to extract interpolated data.
/// \param[in] a_pointScalars The point scalars.
/// \param[in] a_activity The activity of the cells.
/// \param[in] a_activityLocation The location at which the data is currently
///            stored.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetGridPointScalars(const VecFlt& a_pointScalars,
                                                     const DynBitset& a_activity,
                                                     DataLocationEnum a_activityLocation)
{
  if (a_pointScalars.size() != m_ugrid->GetPointCount())
  {
    XM_LOG(xmlog::debug, "Invalid point scalar size in 2D data extractor.");
    m_ptScalars = VecFlt();
  }
  else
  {
    m_ptScalars = a_pointScalars;
  }

  BuildTriangles(LOC_POINTS);

  ApplyActivity(a_activity, a_activityLocation, m_cellActivity);

  m_pointScalars = a_pointScalars;
  //PushPointDataToCentroids(m_cellActivity);
} // XmUGrid2dDataExtractorImpl::SetGridPointScalars
//------------------------------------------------------------------------------
/// \brief Setup cell scalars to be used to extract interpolated data.
/// \param[in] a_cellScalars The point scalars.
/// \param[in] a_activity The activity of the cells.
/// \param[in] a_activityLocation The location at which the data is currently
///            stored.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetGridCellScalars(const VecFlt& a_cellScalars,
                                                    const DynBitset& a_activity,
                                                    DataLocationEnum a_activityLocation)
{
  if ((int)a_cellScalars.size() != m_ugrid->GetCellCount())
  {
    XM_LOG(xmlog::debug, "Invalid cell scalar size in 2D data extractor.");
    m_cellScalars = VecFlt();
  }
  else
  {
    m_cellScalars = a_cellScalars;
  }

  BuildTriangles(LOC_CELLS);

  ApplyActivity(a_activity, a_activityLocation, m_cellActivity);

  //PushCellDataToTrianglePoints(a_cellScalars, m_cellActivity);
} // XmUGrid2dDataExtractorImpl::SetGridCellScalars
//------------------------------------------------------------------------------
/// \brief Sets locations of points to extract interpolated scalar data from.
/// \param[in] a_locations The locations.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetExtractLocations(const VecPt3d& a_locations)
{
  m_extractLocations = a_locations;
  m_cellIdxs.clear();
} // XmUGrid2dDataExtractorImpl::SetExtractLocations
//------------------------------------------------------------------------------
/// \brief Compute the indexes and interp wts from the scalars to extraction
/// locations.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::CalcExtractLocationInterp()
{
  m_cellIdxs.assign(m_extractLocations.size(), -1);
  m_ptIdxs.assign(m_extractLocations.size(), -1);
  m_edgeIdx0.assign(m_extractLocations.size(), -1);
  m_edgeIdx1.assign(m_extractLocations.size(), -1);
  m_extractInterp.assign(m_extractLocations.size(), ScalarToPt());
  VecInt interpIdxs;
  VecDbl interpWeights;
  for (size_t i = 0; i < m_extractLocations.size(); ++i)
  {
    bool onEdge(false);
    Pt3d& pt(m_extractLocations[i]);
    m_cellIdxs[i] = m_triangles->GetIntersectedCell(pt, interpIdxs, interpWeights);

    // see if the location is on an edge (one of the weights will be zero)
    if (interpWeights.size() == 3)
    {
      if (interpWeights[0] == 0.0 && interpWeights[1] != 0.0 && interpWeights[2] != 0.0)
      {
        m_edgeIdx0[i] = interpIdxs[1];
        m_edgeIdx1[i] = interpIdxs[2];
      }
      else if (interpWeights[0] != 0.0 && interpWeights[1] == 0.0 && interpWeights[2] != 0.0)
      {
        m_edgeIdx0[i] = interpIdxs[0];
        m_edgeIdx1[i] = interpIdxs[2];
      }
      else if (interpWeights[0] != 0.0 && interpWeights[1] != 0.0 && interpWeights[2] == 0.0)
      {
        m_edgeIdx0[i] = interpIdxs[0];
        m_edgeIdx1[i] = interpIdxs[1];
      }
    }

    if (m_cellIdxs[i] > -1)
    {
      int ptIdx = m_triangles->GetCellCentroid(m_cellIdxs[i]);
      for (size_t j = 0; j < interpIdxs.size(); ++j)
      {
        ScalarToPt& s2p(m_extractInterp[i]);
        s2p.m_triWt[j] = interpWeights[j];
        if (interpWeights[j] == 1.0)
          m_ptIdxs[i] = interpIdxs[j];
        if (ptIdx == interpIdxs[j])
        { // this is a cell center point
          s2p.m_cellIdxs[j].push_back(m_cellIdxs[i]);
          s2p.m_ptIdxs[j] = m_ugrid->GetCellPoints(m_cellIdxs[i]);
        }
        else
        { // this is a ugrid point
          s2p.m_ptIdxs[j].push_back(interpIdxs[j]);
          s2p.m_cellIdxs[j] = m_ugrid->GetPointAdjacentCells(interpIdxs[j]);
          if (m_useIdwForPointData) {
            VecPt3d centers;
            VecInt idxs;
            for (size_t k = 0; k < s2p.m_cellIdxs[j].size(); ++k) {
              Pt3d p;
              m_ugrid->GetCellCentroid(s2p.m_cellIdxs[j][k], p);
              centers.push_back(p);
              idxs.push_back((int)k);
            }
            const VecPt3d& ugPts(m_ugrid->GetLocations());
            Pt3d pt1 = ugPts[interpIdxs[j]];
            VecDbl d2;
            VecDbl weights;
            inDistanceSquared(pt1, idxs, centers, true, d2);
            inIdwWeights(d2, 2, false, weights);
            s2p.m_idwWts[j] = weights;
          }
        }
      }
    }
  }
} // XmUGrid2dDataExtractorImpl::CalcExtractLocationInterp
//------------------------------------------------------------------------------
/// \brief Determines if the current extract location is in an active cell
/// \param a_idx The index to the current extraction location
/// \return (true/false) true if the point is in an active cell
//------------------------------------------------------------------------------
bool XmUGrid2dDataExtractorImpl::CurrExtractLocationInActiveCell(int a_idx)
{
  if (m_cellIdxs[a_idx] < 0)
  {
    return false;
  }

  if (!m_cellActivity.empty() && !m_cellActivity[m_cellIdxs[a_idx]])
  {
    bool activeCell(false);
    VecInt adjCells;
    if (m_ptIdxs[a_idx] != -1)
    { // extract location is at a point so see if the point is attached to any active cell
      adjCells = m_ugrid->GetPointAdjacentCells(m_ptIdxs[a_idx]);
    }
    else if (m_edgeIdx0[a_idx] != -1)
    { // extract location is on an edge so see if the edge is attached to an active cell
      XmEdge edge(m_edgeIdx0[a_idx], m_edgeIdx1[a_idx]);
      adjCells = m_ugrid->GetEdgeAdjacentCells(edge);
    }
    for (size_t j = 0; !activeCell && j < adjCells.size(); ++j)
    {
      if (m_cellActivity[adjCells[j]])
        activeCell = true;
    }

    if (!activeCell)
      return false;
  }
  return true;
} // XmUGrid2dDataExtractorImpl::CurrExtractLocationInActiveCell
//------------------------------------------------------------------------------
/// \brief Calculate the value at the extract location from cell scalars
/// \return The value at the extract location
//------------------------------------------------------------------------------
float XmUGrid2dDataExtractorImpl::ExtractValueFromCellScalars(int a_idx)
{
  VecInt activeIdxs(10);
  double interpVal = 0.0;
  VecDbl idwWts;
  ScalarToPt& s2p(m_extractInterp[a_idx]);
  for (int j = 0; j < 3; ++j)
  {
    activeIdxs.resize(0);
    double& w(s2p.m_triWt[j]);
    if (w == 0.0)
      continue;
    if (s2p.m_cellIdxs[j].size() != 1)
    {
      double sumIdwWts = 0.0;
      for (size_t k = 0; k < s2p.m_cellIdxs[j].size(); ++k)
      {
        if (m_cellActivity.empty() || m_cellActivity[s2p.m_cellIdxs[j][k]]) {
          activeIdxs.push_back(s2p.m_cellIdxs[j][k]);
          if (m_useIdwForPointData) {
            idwWts.push_back(s2p.m_idwWts[j][k]);
            sumIdwWts += idwWts.back();
          }
        }
      }
      if (!m_useIdwForPointData)
      {
        double factor = w * (1.0 / activeIdxs.size());
        for (auto& idx : activeIdxs)
          interpVal += factor * m_cellScalars[idx];
      }
      else
      {
        for (size_t k = 0; k < activeIdxs.size(); ++k) {
          interpVal += w * (idwWts[k] / sumIdwWts) * m_cellScalars[activeIdxs[k]];
        }
      }
    }
    else
    {
      interpVal += w * m_cellScalars[s2p.m_cellIdxs[j][0]];
    }
  }
  return (float)interpVal;
} // XmUGrid2dDataExtractorImpl::ExtractValueFromCellScalars
//------------------------------------------------------------------------------
/// \brief Calculate the value at the extract location from cell scalars
/// \return The value at the extract location
//------------------------------------------------------------------------------
float XmUGrid2dDataExtractorImpl::ExtractValueFromPointScalars(int a_idx)
{
  double interpVal = 0.0;
  ScalarToPt& s2p(m_extractInterp[a_idx]);
  for (int j = 0; j < 3; ++j)
  {
    double& w(s2p.m_triWt[j]);
    if (s2p.m_ptIdxs[j].size() != 1)
    {
      double factor = w * (1.0 / s2p.m_ptIdxs[j].size());
      for (size_t k = 0; k < s2p.m_ptIdxs[j].size(); ++k)
      {
        interpVal += factor * m_ptScalars[s2p.m_ptIdxs[j][k]];
      }
    }
    else
    {
      interpVal += w * m_ptScalars[s2p.m_ptIdxs[j][0]];
    }
  }
  return (float)interpVal;
} // XmUGrid2dDataExtractorImpl::ExtractValueFromPointScalars
//------------------------------------------------------------------------------
/// \brief Extract interpolated data for the previously set locations.
/// \param[out] a_outData The interpolated scalars.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::ExtractData(VecFlt& a_outData)
{
  a_outData.clear();
  a_outData.assign(m_extractLocations.size(), m_noDataValue);

  if (m_cellScalars.empty() && m_ptScalars.empty())
  {
    const VecPt3d& locations = m_ugrid->GetLocations();
    VecFlt pointScalars;
    pointScalars.reserve(locations.size());
    for (const auto& location : locations) {
      pointScalars.push_back(static_cast<float>(location.z));
    }

    SetGridPointScalars(pointScalars, DynBitset(), DataLocationEnum::LOC_POINTS);
  }

  if (m_cellIdxs.empty())
    CalcExtractLocationInterp();

  if (m_scalarLocation == LOC_CELLS && m_cellScalars.empty()) {
    std::string msg =
      "Attempting to extract data from cell scalars but no cell scalars are defined.";
    XM_LOG(xmlog::warning, msg);
    return;
  }
  else if (m_scalarLocation != LOC_CELLS && m_ptScalars.empty())
  {
    std::string msg =
      "Attempting to extract data from point scalars but no point scalars are defined.";
    XM_LOG(xmlog::warning, msg);
    return;
  }

  for (size_t i = 0; i < m_extractLocations.size(); ++i)
  {
    if (!CurrExtractLocationInActiveCell((int)i))
      continue;
    if (m_scalarLocation == LOC_CELLS)
      a_outData[i] = ExtractValueFromCellScalars((int)i);
    else
      a_outData[i] = ExtractValueFromPointScalars((int)i);
  }
} // XmUGrid2dDataExtractorImpl::ExtractData
//------------------------------------------------------------------------------
/// \brief Extract interpolated data for the previously set locations.
/// \param[in] a_location The location to get the interpolated scalar.
/// \return The interpolated value.
//------------------------------------------------------------------------------
float XmUGrid2dDataExtractorImpl::ExtractAtLocation(const Pt3d& a_location)
{
  VecPt3d locations(1, a_location);
  SetExtractLocations(locations);
  VecFlt values;
  ExtractData(values);
  return values[0];
} // XmUGrid2dDataExtractorImpl::ExtractAtLocation
//------------------------------------------------------------------------------
/// \brief Set to use IDW to calculate point scalar values from cell scalars.
/// \param a_useIdw Whether to turn IDW on or off.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetUseIdwForPointData(bool a_useIdw)
{
  m_useIdwForPointData = a_useIdw;
} // XmUGrid2dDataExtractorImpl::SetUseIdwForPointData
//------------------------------------------------------------------------------
/// \brief Set value to use when extracted value is in inactive cell or doesn't
///        intersect with the grid.
/// \param[in] a_value The no data value
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetNoDataValue(float a_value)
{
  m_noDataValue = a_value;
} // XmUGrid2dDataExtractorImpl::SetNoDataValue
//------------------------------------------------------------------------------
/// \brief Set to use IDW to calculate point scalar values from cell scalars.
/// \param a_useIdw Whether to turn IDW on or off.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetSplitTrisWithCellData(bool a_splitTrisWithCellData)
{
  m_splitTrisWithCellData = a_splitTrisWithCellData;
} // XmUGrid2dDataExtractorImpl::SetSplitTrisWithCellData
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
  if (a_activity.empty())
  {
    // when empty, everything gets enabled on the cells
    a_cellActivity = a_activity;
    SetGridCellActivity(a_cellActivity);
  }
  else
  {
    if (a_location == LOC_POINTS)
    {
      SetGridPointActivity(a_activity, a_cellActivity);
    }
    else if (a_location == LOC_CELLS)
    {
      a_cellActivity = a_activity;
      SetGridCellActivity(a_cellActivity);
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
  if (a_pointActivity.size() != m_ugrid->GetPointCount() && !a_pointActivity.empty())
  {
    XM_LOG(xmlog::debug, "Invalid point activity size in 2D data extractor.");
  }

  if (a_pointActivity.empty())
  {
    a_cellActivity = a_pointActivity;
    //m_triangles->SetCellActivity(a_cellActivity);
    return;
  }

  a_cellActivity.reset();
  a_cellActivity.resize(m_ugrid->GetCellCount(), true);
  a_cellActivity.set();
  VecInt attachedCells;
  int numPoints = m_ugrid->GetPointCount();
  for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
  {
    if (pointIdx < a_pointActivity.size() && !a_pointActivity[pointIdx])
    {
      m_ugrid->GetPointAdjacentCells(pointIdx, attachedCells);
      for (auto cellIdx : attachedCells)
      {
        a_cellActivity[cellIdx] = false;
      }
    }
  }
  //m_triangles->SetCellActivity(a_cellActivity);
} // XmUGrid2dDataExtractorImpl::SetGridPointActivity
//------------------------------------------------------------------------------
/// \brief Set activity on cells
/// \param[in] a_cellActivity The cell activity of the scalar values.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::SetGridCellActivity(DynBitset& a_cellActivity)
{
  if (a_cellActivity.size() != m_ugrid->GetCellCount() && !a_cellActivity.empty())
  {
    XM_LOG(xmlog::debug, "Invalid cell activity size in 2D data extractor.");
    a_cellActivity = DynBitset();
  }
  //m_triangles->SetCellActivity(a_cellActivity);
} // XmUGrid2dDataExtractorImpl::SetGridCellActivity
//------------------------------------------------------------------------------
/// \brief Push point scalar data to cell centroids using average.
/// \param[in] a_cellActivity The cell activity of the scalar values.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::PushPointDataToCentroids(const DynBitset& a_cellActivity)
{
  // default any missing scalar values to zero
  m_pointScalars.resize(m_triangles->GetPoints().size(), 0.0);

  VecInt cellPoints;
  int numCells = m_ugrid->GetCellCount();
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    if (a_cellActivity.empty() || a_cellActivity[cellIdx])
    {
      int centroidIdx = m_triangles->GetCellCentroid(cellIdx);
      if (centroidIdx >= 0)
      {
        m_ugrid->GetCellPoints(cellIdx, cellPoints);
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
  int numPoints = m_ugrid->GetPointCount();
  for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
  {
    m_ugrid->GetPointAdjacentCells(pointIdx, cellIdxs);
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

  int numCells = m_ugrid->GetCellCount();
  for (int cellIdx = 0; cellIdx < numCells; ++cellIdx)
  {
    int pointIdx = m_triangles->GetCellCentroid(cellIdx);
    if (pointIdx >= 0)
      m_pointScalars[pointIdx] = cellIdx >= a_cellScalars.size() ? 0.0f : a_cellScalars[cellIdx];
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
    if (cellIdx >= a_cellActivity.size() || a_cellActivity[cellIdx])
    {
      sum += cellIdx >= a_cellScalars.size() ? 0.0 : a_cellScalars[cellIdx];
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
  Pt3d pt = m_ugrid->GetPointLocation(a_pointIdx);
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
//------------------------------------------------------------------------------
/// \brief Build triangles for UGrid for either point or cell scalars.
/// \param[in] a_location Location to build on (points or cells).
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorImpl::BuildTriangles(DataLocationEnum a_location)
{
  if (m_scalarLocation != a_location)
  {
    XmUGridTriangles2d::PointOptionEnum option = a_location == LOC_CELLS
                                                   ? XmUGridTriangles2d::PO_CENTROIDS_ONLY
                                                   : XmUGridTriangles2d::PO_NO_POINTS;
    //XmUGridTriangles2d::PointOptionEnum option = XmUGridTriangles2d::PO_CENTROIDS_ONLY;
    m_triangles->SetSplitTrisWithCellData(m_splitTrisWithCellData);
    m_triangles->BuildTriangles(*m_ugrid, option);
    m_scalarLocation = a_location;
  }
} // XmUGrid2dDataExtractorImpl::BuildTriangles
//------------------------------------------------------------------------------
/// \brief Get the UGrid triangles.
/// \return The UGrid triangles.
//------------------------------------------------------------------------------
const BSHP<XmUGridTriangles2d> XmUGrid2dDataExtractorImpl::GetUGridTriangles() const
{
  return m_triangles;
} // XmUGrid2dDataExtractorImpl::GetUGridTriangles

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractor
/// \brief Provides ability to interpolate and extract the scalar values  points and along arcs
///        for an unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Create a new XmUGrid2dDataExtractor.
/// \param[in] a_ugrid The UGrid geometry to use to extract values from
/// \return the new XmUGrid2dDataExtractor
//------------------------------------------------------------------------------
BSHP<XmUGrid2dDataExtractor> XmUGrid2dDataExtractor::New(std::shared_ptr<XmUGrid> a_ugrid)
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
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);
  extractor->SetNoDataValue(-999.0);

  VecFlt pointScalars = {1, 2, 3, 2};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);
  VecPt3d extractLocations = {
    {0.0, 0.0, 0.0}, {0.25, 0.75, 100.0}, {0.5, 0.5, 0.0}, {0.75, 0.25, -100.0}, {-1.0, -1.0, 0.0}};
  extractor->SetExtractLocations(extractLocations);

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
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  VecFlt pointScalars = {1, 2, 3, 2};
  DynBitset cellActivity;
  cellActivity.push_back(true);
  cellActivity.push_back(false);
  extractor->SetGridPointScalars(pointScalars, cellActivity, LOC_CELLS);
  extractor->SetExtractLocations({{0.25, 0.75, 100.0}, {0.75, 0.25, -100.0}, {-1.0, -1.0, 0.0}});

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

  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  // set point 4 inactive
  // should cause all cells connected to point 4 to return XM_NODATA
  DynBitset pointActivity;
  pointActivity.resize(9, true);
  pointActivity[4] = false;
  extractor->SetGridPointScalars(pointScalars, pointActivity, LOC_POINTS);

  // extract interpolated scalar for each cell
  extractor->SetExtractLocations(extractLocations);

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  TS_ASSERT_EQUALS(expectedPerCell, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testPointScalarPointActivity
//------------------------------------------------------------------------------
/// \brief Test when scalar and activity arrays are sized incorrectly.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testInvalidPointScalarsAndActivitySize()
{
  //  3----2
  //  | 1 /|
  //  |  / |
  //  | /  |
  //  |/ 0 |
  //  0----1
  VecPt3d points = {{0, 0, 1}, {1, 0, 2}, {1, 1, 3}, {0, 1, 4}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2, XMU_TRIANGLE, 3, 2, 3, 0};
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  VecFlt pointScalars = {1, 2, 3};
  DynBitset activity;
  activity.push_back(true);
  activity.push_back(false);
  extractor->SetGridPointScalars(pointScalars, activity, LOC_POINTS);

  VecPt3d extractLocations = {{0.25, 0.75, 100.0}, {0.75, 0.25, 0.0}};
  extractor->SetExtractLocations(extractLocations);

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {3.0, 2.0};
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testInvalidPointScalarsAndActivitySize
//------------------------------------------------------------------------------
/// \brief Test extractor with cell scalars only.
/// \verbatim
///  3----2
///  | 1 /|
///  |  / |
///  | /  |
///  |/ 0 |
///  0----1
/// \endverbatim
//------------------------------------------------------------------------------
//! [snip_test_Example_SimpleLocationExtractor]
void XmUGrid2dDataExtractorUnitTests::testCellScalarsOnly()
{
  VecPt3d points = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2, XMU_TRIANGLE, 3, 2, 3, 0};
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);

  // Step 1. Create an extractor for an existing XmUGrid (call xms::XmUGrid2dDataExtractor).
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  // Step 2. Set scalar and activity values (call xms::XmUGrid2dDataExtractor::SetGridCellScalars or
  // XmUGrid2dDataExtractor::SetPointCellScalars).
  VecFlt cellScalars = {1, 2};
  extractor->SetSplitTrisWithCellData(true);
  extractor->SetGridCellScalars(cellScalars, DynBitset(), LOC_CELLS);

  // Step 3. Set extract locations (call XmUGrid2dDataExtractor::SetExtractLocations).
  extractor->SetExtractLocations({{0.0, 0.0, 0.0},
                                  {0.25, 0.75, 100.0},
                                  {0.5, 0.5, 0.0},
                                  {0.75, 0.25, -100.0},
                                  {-0.1, -0.1, 0.0}});

  // Step 4. Extract the data (call xms::XmUGrid2dDataExtractor::ExtractData).
  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {1.5, 2.0, 1.5, 1.0, XM_NODATA};
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testCellScalarsOnly
//! [snip_test_Example_SimpleLocationExtractor]
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

  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
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
  extractor->SetSplitTrisWithCellData(true);
  extractor->SetGridCellScalars(cellScalars, cellActivity, LOC_CELLS);

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
    2.0f, 3.4444f, XM_NODATA, 8.25f, // row 1 cells
    4.0, 6.0759f, 7.229f, 9.75f      // row 2 cells
  };
  // clang-format on

  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);
  extractor->SetUseIdwForPointData(true);

  // set point 4 inactive
  // should cause all cells connected to point 4 to return XM_NODATA
  DynBitset cellActivity;
  cellActivity.resize(8, true);
  cellActivity[2] = false;
  extractor->SetSplitTrisWithCellData(true);
  extractor->SetGridCellScalars(cellScalars, cellActivity, LOC_CELLS);

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
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  DynBitset pointActivity;
  pointActivity.resize(4, true);
  pointActivity[1] = false;
  VecFlt cellScalars = {1, 2};
  extractor->SetGridCellScalars(cellScalars, pointActivity, LOC_POINTS);
  extractor->SetExtractLocations({{0.0, 0.0, 0.0},
                                  {0.25, 0.75, 100.0},
                                  {0.5, 0.5, 0.0},
                                  {0.75, 0.25, -100.0},
                                  {-1.0, -1.0, 0.0}});

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {2.0, 2.0, 2.0, XM_NODATA, XM_NODATA};
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testCellScalarPointActivity
//------------------------------------------------------------------------------
/// \brief Test extractor with cell scalars only.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testInvalidCellScalarsAndActivitySize()
{
  //  3----2
  //  | 1 /|
  //  |  / |
  //  | /  |
  //  |/ 0 |
  //  0----1
  VecPt3d points = {{0, 0, 1}, {1, 0, 2}, {1, 1, 3}, {0, 1, 4}};
  VecInt cells = {XMU_TRIANGLE, 3, 0, 1, 2, XMU_TRIANGLE, 3, 2, 3, 0};
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  VecFlt cellScalars = {1};
  DynBitset activity;
  activity.push_back(false);
  extractor->SetGridCellScalars(cellScalars, activity, LOC_CELLS);
  VecPt3d extractLocations = {{0.25, 0.75, 100.0}, {0.75, 0.25, 0.0}};
  extractor->SetExtractLocations(extractLocations);

  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {3.0, 2.0};
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testInvalidCellScalarsAndActivitySize
//------------------------------------------------------------------------------
/// \brief Test extractor going through time steps with cell and point scalars.
//------------------------------------------------------------------------------
void XmUGrid2dDataExtractorUnitTests::testChangingScalarsAndActivity()
{
  // build a grid with 3 cells in a row
  VecPt3d points = {{0, 1, 0}, {1, 1, 0}, {2, 1, 0}, {3, 1, 0},
                    {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0}};
  VecInt cells = {
    XMU_QUAD, 4, 0, 4, 5, 1, // cell 0
    XMU_QUAD, 4, 1, 5, 6, 2, // cell 1
    XMU_QUAD, 4, 2, 6, 7, 3  // cell 2
  };

  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  VecFlt scalars;
  DynBitset activity;
  VecPt3d extractLocations = {
    {0.75, 0.25, 0.0}, // cell 0
    {1.5, 0.5, 0.0},   // cell 1
    {2.25, 0.75, 0.0}  // cell 3
  };

  // timestep 1
  scalars = {1, 2, 3};
  // empty activity means all are enabled
  extractor->SetGridCellScalars(scalars, activity, LOC_CELLS);
  extractor->SetExtractLocations(extractLocations);

  VecFlt extractedValues;
  extractor->ExtractData(extractedValues);
  VecFlt expectedValues = {1.25, 2.0, 2.75};
  TS_ASSERT_EQUALS(expectedValues, extractedValues);

  // timestep 2
  scalars = {2, 3, 4};
  activity.resize(3, true);
  activity[1] = false;
  extractor->SetGridCellScalars(scalars, activity, LOC_CELLS);
  extractor->SetExtractLocations(extractLocations);

  extractor->ExtractData(extractedValues);
  expectedValues = {2, XM_NODATA, 4};
  TS_ASSERT_EQUALS(expectedValues, extractedValues);

  // timestep 3
  scalars = {3, 4, 5};
  activity.clear();
  extractor->SetGridCellScalars(scalars, activity, LOC_CELLS);
  extractor->SetExtractLocations(extractLocations);

  extractor->ExtractData(extractedValues);
  expectedValues = {3.25, 4.0, 4.75};
  TS_ASSERT_EQUALS(expectedValues, extractedValues);

  // change to point data
  // timestep 1
  scalars = {1, 2, 3, 4, 2, 3, 4, 5};
  // empty activity means all are enabled
  extractor->SetGridPointScalars(scalars, activity, LOC_POINTS);
  extractor->SetExtractLocations(extractLocations);

  extractor->ExtractData(extractedValues);
  expectedValues = {2.5, 3.0, 3.5};
  TS_ASSERT_EQUALS(expectedValues, extractedValues);

  // timestep 2
  scalars = {2, 3, 4, 5, 3, 4, 5, 6};
  activity.resize(8, true);
  activity[0] = false;
  extractor->SetGridPointScalars(scalars, activity, LOC_POINTS);
  extractor->SetExtractLocations(extractLocations);

  extractor->ExtractData(extractedValues);
  expectedValues = {XM_NODATA, 4.0, 4.5};
  TS_ASSERT_EQUALS(expectedValues, extractedValues);

  // timestep 3
  scalars = {3, 4, 5, 6, 4, 5, 6, 7};
  activity.set();
  activity[1] = false;
  extractor->SetGridPointScalars(scalars, activity, LOC_POINTS);
  extractor->SetExtractLocations(extractLocations);

  extractor->ExtractData(extractedValues);
  expectedValues = {XM_NODATA, XM_NODATA, 5.5};
  TS_ASSERT_EQUALS(expectedValues, extractedValues);

  // timestep 4
  activity.clear();
  extractor->SetGridPointScalars(scalars, activity, LOC_POINTS);
  extractor->SetExtractLocations(extractLocations);

  extractor->ExtractData(extractedValues);
  expectedValues = {4.5, 5, 5.5};
  TS_ASSERT_EQUALS(expectedValues, extractedValues);
} // XmUGrid2dDataExtractorUnitTests::testChangingScalarsAndActivity
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
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);
  TS_ASSERT(extractor);

  VecFlt pointScalars = {1, 2, 3, 4};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);
  extractor->SetExtractLocations({{0.5, 0.5, 0.0}});
  VecFlt interpValues;
  extractor->ExtractData(interpValues);
  VecFlt expected = {2.0};
  TS_ASSERT_EQUALS(expected, interpValues);

  BSHP<XmUGrid2dDataExtractor> extractor2 = XmUGrid2dDataExtractor::New(extractor);
  extractor2->SetGridPointScalars(pointScalars, DynBitset(), LOC_POINTS);
  extractor2->SetExtractLocations({{0.5, 0.5, 0.0}});
  extractor2->ExtractData(interpValues);
  TS_ASSERT_EQUALS(expected, interpValues);
} // XmUGrid2dDataExtractorUnitTests::testCopiedExtractor
//------------------------------------------------------------------------------
/// \brief Test XmUGrid2dDataExtractor for tutorial.
//------------------------------------------------------------------------------
//! [snip_test_Example_TransientLocationExtractor]
void XmUGrid2dDataExtractorUnitTests::testTutorial()
{
  // build 2x3 grid
  VecPt3d points = {{288050, 3907770, 0}, {294050, 3907770, 0}, {300050, 3907770, 0},
                    {306050, 3907770, 0}, {288050, 3901770, 0}, {294050, 3901770, 0},
                    {300050, 3901770, 0}, {306050, 3901770, 0}, {288050, 3895770, 0},
                    {294050, 3895770, 0}, {300050, 3895770, 0}, {306050, 3895770, 0}};
  VecInt cells = {XMU_QUAD, 4, 0, 4, 5, 1, XMU_QUAD, 4, 1, 5, 6,  2, XMU_QUAD, 4, 2, 6,  7,  3,
                  XMU_QUAD, 4, 4, 8, 9, 5, XMU_QUAD, 4, 5, 9, 10, 6, XMU_QUAD, 4, 6, 10, 11, 7};
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);
  // Step 1. Create an extractor for an XmUGrid (call XmUGrid2dDataExtractor::New).
  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);

  // Step 2. Set extract locations (call XmUGrid2dDataExtractor::SetExtractLocations).
  VecPt3d extractLocations = {{289780, 3906220, 0}, {293780, 3899460, 0}, {298900, 3900780, 0},
                              {301170, 3904960, 0}, {296330, 3906180, 0}, {307395, 3901463, 0}};
  extractor->SetExtractLocations(extractLocations);
  VecFlt extractedData;
  VecPt3d retrievedLocations = extractor->GetExtractLocations();
  TS_ASSERT_EQUALS(extractLocations, retrievedLocations);

  // Step 3. Optionally set the "no data" value for output interpolated values
  //         (XmUGrid2dDataExtractor::SetNoDataValue).
  extractor->SetNoDataValue(-999.0);

  // time step 1
  // Step 4. Set the point scalars for the first time step
  // (XmUGrid2dDataExtractor::SetGridPointScalars).
  VecFlt pointScalars = {730.787f,  1214.54f,  1057.145f, 629.2069f, 351.1153f, 631.6649f,
                         1244.366f, 449.9133f, 64.04247f, 240.9716f, 680.0491f, 294.9547f};
  extractor->SetGridPointScalars(pointScalars, DynBitset(), LOC_CELLS);
  // Step 5. Extract the data (call xms::XmUGrid2dDataExtractor::ExtractData).
  extractor->ExtractData(extractedData);

  VecFlt expectedData = {719.6f, 468.6f, 1033.8f, 996.5f, 1204.3f, -999.0f};
  TS_ASSERT_DELTA_VEC(expectedData, extractedData, 0.2);

  // time step 2
  // Step 6. Continue using steps 4 and 5 for remaining time steps.
  pointScalars = {-999.0f, 1220.5f, 1057.1f, 613.2f, 380.1f, 625.6f,
                  722.2f,  449.9f,  51.0f,   240.9f, 609.0f, 294.9f};
  DynBitset cellActivity;
  cellActivity.resize(ugrid->GetCellCount(), true);
  cellActivity[0] = false;
  extractor->SetGridPointScalars(pointScalars, cellActivity, LOC_CELLS);
  // Step 7. Extract the data (call xms::XmUGrid2dDataExtractor::ExtractData).
  extractor->ExtractData(extractedData);

  expectedData = {-999.0f, 466.4f, 685.0f, 849.4f, 1069.6f, -999.0f};
  TS_ASSERT_DELTA_VEC(expectedData, extractedData, 0.2);
} // XmUGrid2dDataExtractorUnitTests::testTutorial
//! [snip_test_Example_TransientLocationExtractor]

void XmUGrid2dDataExtractorUnitTests::testUninitializedScalars()
{
  // build 2x3 grid
  VecPt3d points = {{-1.964, -0.555, 1.0}, {-1.370921465479, 8.1502514884205, 2.0},
                    {0.489, 12.409, 3.0},  {12.428, 11.87, 4.0},
                    {12.536, -1.552, 5.0}, {5.933, -3.493, 6.0}};
  VecInt cells = {XMU_TRIANGLE, 3, 1, 3, 2, XMU_TRIANGLE, 3, 5, 1, 0,
                  XMU_TRIANGLE, 3, 5, 4, 3, XMU_TRIANGLE, 3, 5, 3, 1};
  std::shared_ptr<XmUGrid> ugrid = XmUGrid::New(points, cells);

  BSHP<XmUGrid2dDataExtractor> extractor = XmUGrid2dDataExtractor::New(ugrid);

  // Step 2. Set extract locations (call XmUGrid2dDataExtractor::SetExtractLocations).
  VecPt3d extractLocations = {{0.5, 9.5, 0.0}, {1.5, 9.5, 0.0}, {2.5, 9.5, 0.0}, {3.5, 9.5, 0.0},
                              {4.5, 9.5, 0.0}, {5.5, 9.5, 0.0}, {6.5, 9.5, 0.0}, {7.5, 9.5, 0.0},
                              {8.5, 9.5, 0.0}, {9.5, 9.5, 0.0}};
  extractor->SetExtractLocations(extractLocations);
  VecFlt extractedData;
  
  // time step 1
  // Step 4. Set the point scalars for the first time step
  // (XmUGrid2dDataExtractor::SetGridPointScalars).
  // Step 5. Extract the data (call xms::XmUGrid2dDataExtractor::ExtractData).
  extractor->ExtractData(extractedData);
  VecFlt base = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };
 // TS_ASSERT_DELTA_VEC(base, extractedData, 1e-2);

  const VecInt& indexes = extractor->GetCellIndexes();
  VecInt baseIndexes = {0, 0,0, 0, 3, 3, 3, 3, 3, 3};
  TS_ASSERT_EQUALS(baseIndexes, indexes);
}

#endif
