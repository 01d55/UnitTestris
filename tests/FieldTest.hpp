#ifndef FIELDTEST_HPP
#define FIELDTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

// Forward declaration
class FieldSizeError;

class FieldTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( FieldTest );
  CPPUNIT_TEST( testConstructor0 );
  CPPUNIT_TEST( testConstructor1 );
  CPPUNIT_TEST( testConstructor2 );
  CPPUNIT_TEST( testSet );
  CPPUNIT_TEST( testFieldScore );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  // Field(); 
  void testConstructor0();
  // Field(std::vector<bool> row) throw (FieldSizeError);
  void testConstructor1();
  // Field(std::vector< std::vector<bool> > blocks) throw (FieldSizeError); Field(const Field& toCopy);
  void testConstructor2();

  void testSet();
  void testFieldScore();
};

#endif  // FIELDTEST_HPP
