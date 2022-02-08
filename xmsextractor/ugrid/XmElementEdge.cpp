//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2022. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsextractor/ugrid/XmElementEdge.h>

// 3. Standard library headers

// 4. External library headers

// 5. Shared code headers

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
/// \class XmElementEdge
/// \brief Utility class to keep track of element edges.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Default contructor.
//------------------------------------------------------------------------------
XmElementEdge::XmElementEdge(long i, long j)
{
  if (i < j)
  {
    first = i;
    second = j;
  }
  else
  {
    first = j;
    second = i;
  }
} // XmElementEdge::XmElementEdge
//------------------------------------------------------------------------------
/// \brief Returns the element pair
//------------------------------------------------------------------------------
XmElementEdge::Pair XmElementEdge::GetPair() const
{
  return Pair(first, second);
} // XmElementEdge::GetPair
//------------------------------------------------------------------------------
/// \brief < operator for edge
//------------------------------------------------------------------------------
bool XmElementEdge::operator<(const XmElementEdge& rhs) const
{
  return GetPair() < rhs.GetPair();
} // XmElementEdge::operator<
} // namespace xms
