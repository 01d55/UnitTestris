#ifndef PIECETEST_HPP
#define PIECETEST_HPP

#include <cppunit/extensions/HelperMacros.h>

// Forward declaration
class PieceLockError;

class PieceTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( PieceTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testStep );
  CPPUNIT_TEST( testShift );
  CPPUNIT_TEST( testRotate );
  CPPUNIT_TEST( testDrop );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testConstructor();
  void testStep();
  void testShift();
  void testRotate();
  void testDrop();
};

#endif  // PIECETEST_HPP
