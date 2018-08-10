#pragma once
//------------------------------------------------------------------------------
/// \file
/// \ingroup extractor
/// \copyright (C) Copyright Aquaveo 2018.
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
class XmUGrid2dPolylineDataExtractorUnitTests : public CxxTest::TestSuite
{
public:
  void testOneCellOneSegment();
  void testSegmentAllInCell();
  void testSegmentAlongEdge();
  void testSegmentAllOutside();
  void testSegmentOutToTouch();
  void testSegmentTouchToOut();
  void testSegmentCrossCellPoint();

  void testSegmentAcrossCellIntoSecond();
  void testSegmentAcrossSplitCells();

  void testTwoSegmentsAcrossOneCell();
  void testTwoSegmentsAllOutside();
  void testTwoSegmentsFirstExiting();
  void testTwoSegmentsJoinInCell();
  void testTwoSegmentsJoinOnBoundary();

  void testThreeSegmentsCrossOnBoundary();

  void testCellScalars();
  
  void testTransientTutorial();
}; // XmUGrid2dPolylineDataExtractorUnitTests

#endif
