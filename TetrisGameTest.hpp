#ifndef TETRISGAMETEST_HPP
#define TETRISGAMETEST_HPP

#include <cppunit/extensions/HelperMacros.h>

class TetrisGameTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE ( TetrisGameTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testRunCallback );
  CPPUNIT_TEST( testExceptions );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testConstructor();
  void testRunCallback();
  void testExceptions();
};

#endif // TETRISGAMETEST_HPP
