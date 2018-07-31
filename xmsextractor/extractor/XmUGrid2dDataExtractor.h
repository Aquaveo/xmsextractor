#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief Contains the XmUGrid Class and supporting data types.
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

  /// \cond
  virtual void SetExtractLocations(const VecPt3d& a_locations) = 0;

  virtual void SetGridPointScalars(const VecFlt& a_pointScalars) = 0;
  //virtual void SetGridCellScalars(const VecFlt& a_cellScalars) = 0;
  //virtual void SetGridPointActivity(const DynBitset& a_activity) = 0;
  //virtual void SetGridCellActivity(const DynBitset& a_activity) = 0;

  virtual void ExtractData(VecFlt& outData) = 0;

  //virtual void ExtractFromCellData(const VecFlt& a_cellScalars, const DynBitset& a_activity,
  //                                 VecFlt& outData);
  //virtual void ExtractFromPointData(const VecFlt& a_pointScalars, const DynBitset& a_activity,
  //                                  VecFlt& outData);

private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGrid2dDataExtractor)
  /// \endcond

protected:
  XmUGrid2dDataExtractor();
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
