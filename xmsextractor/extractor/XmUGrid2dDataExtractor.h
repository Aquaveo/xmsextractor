#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief Contains the XmUGrid2dDataExtractor Class and supporting data types.
/// \ingroup ugrid
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 3. Standard library headers

// 4. External library headers

// 5. Shared code headers
#include <xmscore/misc/base_macros.h>
#include <xmscore/misc/boost_defines.h>
#include <xmscore/misc/DynBitset.h>
#include <xmscore/stl/vector.h>

//----- Forward declarations ---------------------------------------------------

//----- Namespace declaration --------------------------------------------------

/// XMS Namespace
namespace xms
{
//----- Forward declarations ---------------------------------------------------
class GmMultiPolyIntersector;
class XmUGrid;
class XmUGridTriangles2d;
class XmUGrid2dPolylineDataExtractor;

//----- Constants / Enumerations -----------------------------------------------

/// The location at which the data will be stored.
enum DataLocationEnum { LOC_POINTS, LOC_CELLS, LOC_UNKNOWN };

//----- Structs / Classes ------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
class XmUGrid2dDataExtractor
{
public:
  static BSHP<XmUGrid2dDataExtractor> New(std::shared_ptr<XmUGrid> a_ugrid);
  static BSHP<XmUGrid2dDataExtractor> New(BSHP<XmUGrid2dDataExtractor> a_extractor);
  virtual ~XmUGrid2dDataExtractor();

  /// \brief Setup point scalars to be used to extract interpolated data.
  /// \param[in] a_pointScalars The point scalars.
  /// \param[in] a_activity The activity of the cells.
  /// \param[in] a_activityType The location at which the data is currently stored.
  virtual void SetGridPointScalars(const VecFlt& a_pointScalars,
                                   const DynBitset& a_activity,
                                   DataLocationEnum a_activityType) = 0;
  /// \brief Setup cell scalars to be used to extract interpolated data.
  /// \param[in] a_cellScalars The point scalars.
  /// \param[in] a_activity The activity of the cells.
  /// \param[in] a_activityType The location at which the data is currently stored.
  virtual void SetGridCellScalars(const VecFlt& a_cellScalars,
                                  const DynBitset& a_activity,
                                  DataLocationEnum a_activityType) = 0;

  /// \brief Sets locations of points to extract interpolated scalar data from.
  /// \param[in] a_locations The locations.
  virtual void SetExtractLocations(const VecPt3d& a_locations) = 0;
  /// \brief Extract interpolated data for the previously set locations.
  /// \param[out] a_outData The interpolated scalars.
  virtual void ExtractData(VecFlt& a_outData) = 0;
  /// \brief Extract interpolated data for the previously set locations.
  /// \param[in] a_location The location to get the interpolated scalar.
  /// \return The interpolated value.
  virtual float ExtractAtLocation(const Pt3d& a_location) = 0;

  /// \brief Set to use IDW to calculate point scalar values from cell scalars.
  /// \param a_useIdw Whether to turn IDW on or off.
  virtual void SetUseIdwForPointData(bool a_useIdw) = 0;
  /// \brief Set value to use when extracted value is in inactive cell or doesn't
  ///        intersect with the grid.
  /// \param[in] a_noDataValue The no data value
  virtual void SetNoDataValue(float a_noDataValue) = 0;
  /// \brief Set to force triangles to be split at cell center with cell data
  /// \param[in] a_splitTrisWithCellData
  virtual void SetSplitTrisWithCellData(bool a_splitTrisWithCellData) = 0;

  /// \brief Build triangles for UGrid for either point or cell scalars.
  /// \param[in] a_location Location to build on (points or cells).
  virtual void BuildTriangles(DataLocationEnum a_location) = 0;
  /// \brief Get the UGrid triangles.
  /// \return Shared pointer to triangles.
  virtual const BSHP<XmUGridTriangles2d> GetUGridTriangles() const = 0;

  /// \brief Gets the scalars
  /// \return The scalars.
  virtual const VecFlt& GetScalars() const = 0;
  /// \brief Gets the location of the scalars (points or cells)
  /// \return The location of the scalars.
  virtual DataLocationEnum GetScalarLocation() const = 0;
  /// \brief Gets locations of points to extract interpolated scalar data from.
  /// \return The locations.
  virtual const VecPt3d& GetExtractLocations() const = 0;
  /// \brief Gets cell indexes associated with the extract location points.
  /// \return The cell indexes.
  virtual const VecInt& GetCellIndexes() const = 0;
  /// \brief Gets the option for using IDW for point data
  /// \return The option.
  virtual bool GetUseIdwForPointData() const = 0;
  /// \brief Gets the no data value
  /// \return The no data value.
  virtual float GetNoDataValue() const = 0;
  /// \brief Gets the option for splitting triangles with cell data
  /// \return The option.
  virtual bool GetSplitTrisWithCellData() const = 0;

private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGrid2dDataExtractor)

protected:
  XmUGrid2dDataExtractor();
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
