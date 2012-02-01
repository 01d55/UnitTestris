#ifndef INTEGRATIONTEST_HPP
#define INTEGRATIONTEST_HPP

#include <cppunit/extensions/HelperMacros.h>
// This suite is for reproduction cases of errors observed during tetris game execution.

class IntegrationTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( IntegrationTest );
  CPPUNIT_TEST( testError1 );
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp();
  void tearDown();
  /* Error1:
     When a piece causes rows to clear, some blocks from the piece are not properly 
     shifted down.
  */
  void testError1();
  
};

#endif // INTEGRATIONTEST_HPP
