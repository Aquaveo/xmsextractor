//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsextractor/ugrid/XmUGridTriangulatorBase.h>

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

class XmUGridTriangulatorBase::impl
{
public:
  impl();
  virtual ~impl();

private:
  XM_DISALLOW_COPY_AND_ASSIGN(XmUGridTriangulatorBase::impl);
};

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangulatorImpl
/// \brief Class to store XmUGrid triangles. Tracks where midpoints and
///        triangles came from.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Constructor
//------------------------------------------------------------------------------
XmUGridTriangulatorBase::impl::impl()
{
} // XmUGridTriangulatorBase::impl::impl
//------------------------------------------------------------------------------
/// \brief Destructor
//------------------------------------------------------------------------------
XmUGridTriangulatorBase::impl::~impl()
{
} // XmUGridTriangulatorBase::impl::~impl
////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangulatorBase
/// \brief Class to triangulate.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Default contructor.
//------------------------------------------------------------------------------
XmUGridTriangulatorBase::XmUGridTriangulatorBase()
: m_impl(new XmUGridTriangulatorBase::impl())
{
} // XmUGridTriangulatorBase::XmUGridTriangulatorBase
//------------------------------------------------------------------------------
/// \brief Destructor.
//------------------------------------------------------------------------------
XmUGridTriangulatorBase::~XmUGridTriangulatorBase()
{
} // XmUGridTriangulatorBase::XmUGridTriangulatorBase
//------------------------------------------------------------------------------
/// \brief Triangulates the points.
//------------------------------------------------------------------------------
void XmUGridTriangulatorBase::Triangulate(const VecInt& a_pts)
{
} // XmUGridTriangulatorBase::Triangulate

} // namespace xms

#ifdef CXX_TEST
//------------------------------------------------------------------------------
// Unit Tests
//------------------------------------------------------------------------------
using namespace xms;
#include <xmsextractor/ugrid/XmUGridTriangulatorBase.t.h>

#include <xmscore/testing/TestTools.h>

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTriangulatorBaseUnitTests
/// \brief Class to to test XmUGridTriangulatorBase
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Test.
//------------------------------------------------------------------------------
void XmUGridTriangulatorBaseUnitTests::test()
{
} // XmUGridTriangulatorBaseUnitTests::test

#endif
