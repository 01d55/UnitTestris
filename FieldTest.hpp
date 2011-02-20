#ifndef FIELDTEST_HPP
#define FIELDTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

// Forward declaration
class FieldSizeError;

class FieldTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( FieldTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSet );
  CPPUNIT_TEST( testFieldScore );
  CPPUNIT_TEST_EXCEPTION( testFieldSizeException, FieldSizeError );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testConstructor();
  void testSet();
  void testFieldScore();
  void testFieldSizeException();
};

#endif  // FIELDTEST_HPP
