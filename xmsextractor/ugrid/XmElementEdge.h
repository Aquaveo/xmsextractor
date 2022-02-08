#pragma once
//------------------------------------------------------------------------------
/// \file
/// \brief Contains the XmUGrid Class and supporting data types.
/// \ingroup ugrid
/// \copyright (C) Copyright Aquaveo 2022. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 3. Standard library headers
#include <utility> // std::pair

// 4. External library headers

// 5. Shared code headers

//----- Forward declarations ---------------------------------------------------

//----- Namespace declaration --------------------------------------------------

/// XMS Namespace
namespace xms
{
//----- Forward declarations ---------------------------------------------------

//----- Constants / Enumerations -----------------------------------------------

//----- Structs / Classes ------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
struct XmElementEdge
{
  typedef std::pair<long, long> Pair;
  XmElementEdge(long i, long j);
  Pair GetPair() const;
  bool operator<(const XmElementEdge& rhs) const;
  long first, second;

private:
  XmElementEdge();
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms
