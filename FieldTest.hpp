#ifndef FIELDTEST_HPP
#define FIELDTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

// Forward declaration
class FieldSizeError: public std::std_runtime_error;

class FieldTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( FieldTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testSet );
  CPPUNIT_TEST_EXCEPTION( testFieldSize, FieldSizeError );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testConstructor();
  void testSet();
  void testFieldSize();
};

#endif  // FIELDTEST_HPP
