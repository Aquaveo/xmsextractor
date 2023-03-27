#pragma once
//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

#ifdef CXX_TEST

// 3. Standard Library Headers

// 4. External Library Headers
#include <cxxtest/TestSuite.h>

// 5. Shared Headers
#include <xmscore/misc/base_macros.h>
#include <xmscore/misc/boost_defines.h>
#include <xmscore/points/pt.h>

// 6. Non-shared Headers

////////////////////////////////////////////////////////////////////////////////
class XmUGridTriangles2dUnitTests : public CxxTest::TestSuite
{
public:
  void testBuildCentroidTrianglesOnTriangle();
  void testBuildCentroidTrianglesOnQuad();
  void testBuildCentroidTriangles2dCellTypes();
  void testBuildEarcutTriangles();
  void testBuildCentroidAndEarcutTriangles();
  void testBuildCentroidAndEarcutTrianglesBottomFace();
}; // XmUGridTriangles2d

#endif
