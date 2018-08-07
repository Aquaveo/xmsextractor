//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018.
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.h>

// 3. Standard library headers
#include <sstream>

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
/// Implementation for XmUGrid2dPolylineDataExtractor
class XmUGrid2dPolylineDataExtractorImpl : public XmUGrid2dPolylineDataExtractor
{
public:
  XmUGrid2dPolylineDataExtractorImpl(BSHP<XmUGrid> a_ugrid);

private:

};

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dPolylineDataExtractorImpl
/// \brief Implementation for XmUGrid2dDataExtractor which provides ability
///        to extract dataset values along arcs for an unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Construct from a UGrid.
/// \param[in] a_ugrid The UGrid to construct an extractor for.
//------------------------------------------------------------------------------
XmUGrid2dPolylineDataExtractorImpl::XmUGrid2dPolylineDataExtractorImpl(BSHP<XmUGrid> a_ugrid)
{
} // XmUGrid2dPolylineDataExtractorImpl::XmUGrid2dPolylineDataExtractorImpl
////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dDataExtractor
/// \brief Provides ability to interpolate and extract the scalar values  points and along arcs
///        for an unstructured grid.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Create a new XmUGrid2dPolylineDataExtractor.
/// \param[in] a_ugrid The UGrid geometry to use to extract values from
/// \return the new XmUGrid2dPolylineDataExtractor
//------------------------------------------------------------------------------
BSHP<XmUGrid2dPolylineDataExtractor> XmUGrid2dPolylineDataExtractor::New(BSHP<XmUGrid> a_ugrid)
{
  BSHP<XmUGrid2dPolylineDataExtractor> extractor(new XmUGrid2dPolylineDataExtractorImpl(a_ugrid));
  return extractor;
} // XmUGrid2dPolylineDataExtractor::New
//------------------------------------------------------------------------------
/// \brief Constructor
//------------------------------------------------------------------------------
XmUGrid2dPolylineDataExtractor::XmUGrid2dPolylineDataExtractor()
{
} // XmUGrid2dPolylineDataExtractor::XmUGrid2dPolylineDataExtractor
//------------------------------------------------------------------------------
/// \brief Destructor
//------------------------------------------------------------------------------
XmUGrid2dPolylineDataExtractor::~XmUGrid2dPolylineDataExtractor()
{
} // XmUGrid2dPolylineDataExtractor::~XmUGrid2dPolylineDataExtractor

} // namespace xms

#ifdef CXX_TEST
//------------------------------------------------------------------------------
// Unit Tests
//------------------------------------------------------------------------------
using namespace xms;
#include <xmsextractor/extractor/XmUGrid2dPolylineDataExtractor.t.h>
#include <xmsgrid/ugrid/XmUGrid.t.h>

#include <xmscore/testing/TestTools.h>

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGrid2dPolylineDataExtractorUnitTests
/// \brief Class to to test XmUGrid2dPolylineDataExtractor
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Test extractor with point scalars only.
//------------------------------------------------------------------------------
void XmUGrid2dPolylineDataExtractorUnitTests::test()
{
} // XmUGrid2dPolylineDataExtractorUnitTests::test

#endif
