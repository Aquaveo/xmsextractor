#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief Contains the XmUGrid2dDataExtractor Class and supporting data types.
/// \ingroup ugrid
/// \copyright (C) Copyright Aquaveo 2018. Distributed under the xmsng
///  Software License, Version 1.0. (See accompanying file
///  LICENSE_1_0.txt or copy at http://www.aquaveo.com/xmsng/LICENSE_1_0.txt)
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
class XmUGrid;

//----- Constants / Enumerations -----------------------------------------------

//----- Structs / Classes ------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
class XmUGrid2dDataExtractor
{
public:
  static BSHP<XmUGrid2dDataExtractor> New(BSHP<XmUGrid> a_ugrid);
  static BSHP<XmUGrid2dDataExtractor> New(BSHP<XmUGrid2dDataExtractor> a_extractor);
  virtual ~XmUGrid2dDataExtractor();

  /// The location at which the data will be stored.
  enum DataLocationEnum { LOC_UNKNOWN, LOC_POINTS, LOC_CELLS };

  /// \brief Setup point scalars to be used to extract interpolated data.
  /// \param[in] a_pointScalars The point scalars.
  /// \param[in] a_activity The activity of the cells.
  /// \param[in] a_activityType The location at which the data is currently stored.
  virtual void SetGridPointScalars(const VecFlt& a_pointScalars,
                                   const DynBitset& a_activity = DynBitset(),
                                   DataLocationEnum a_activityType = LOC_POINTS) = 0;
  /// \brief Setup cell scalars to be used to extract interpolated data.
  /// \param[in] a_cellScalars The point scalars.
  /// \param[in] a_activity The activity of the cells.
  /// \param[in] a_activityType The location at which the data is currently stored.
  virtual void SetGridCellScalars(const VecFlt& a_cellScalars,
                                  const DynBitset& a_activity = DynBitset(),
                                  DataLocationEnum a_activityType = LOC_CELLS) = 0;

  /// \brief Sets locations of points to extract interpolated scalar data from.
  /// \param[in] a_locations The locations.
  virtual void SetExtractLocations(const VecPt3d& a_locations) = 0;
  /// \brief Extract interpolated data for the previously set locations.
  /// \param[out] a_outData The interpolated scalars.
  virtual void ExtractData(VecFlt& a_outData) = 0;

  /// \brief Set to use IDW to calculate point scalar values from cell scalars.
  /// \param a_useIdw Whether to turn IDW on or off.
  virtual void SetUseIdwForPointData(bool a_useIdw) = 0;
  /// \brief Set value to use when extracted value is in inactive cell or doen't
  ///        intersect with the grid.
  /// \param[in] a_noDataValue The no data value
  virtual void SetNoDataValue(float a_noDataValue) = 0;

private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGrid2dDataExtractor)

protected:
  XmUGrid2dDataExtractor();
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
