#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief Contains the XmUGrid2dPolylineDataExtractor Class and supporting data
///         types.
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
class XmUGrid2dPolylineDataExtractor
{
public:
  static BSHP<XmUGrid2dPolylineDataExtractor> New(BSHP<XmUGrid> a_ugrid);
  virtual ~XmUGrid2dPolylineDataExtractor();

private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGrid2dPolylineDataExtractor)

protected:
  XmUGrid2dPolylineDataExtractor();
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
