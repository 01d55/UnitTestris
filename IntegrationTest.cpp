#include "IntegrationTest.hpp"
#include "Field.hpp"
#include "Piece.hpp"
#include "TetrisGame.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( IntegrationTest );


void IntegrationTest::setUp()
{
}

void IntegrationTest::tearDown()
{
}

void IntegrationTest::testError1()
{
  /*
    This:
    *__________*
    *          *03
    *          *02
    *    OO    *01 
    *XXXXOOXXXX*00
    *0123456789*
    Should become this
    *__________*
    *          *03
    *          *02
    *          *01 
    *    XX    *00
    *0123456789*
    Observed error, becomes this:
    *__________*
    *          *03
    *          *02
    *     X    *01 
    *    X     *00
    *0123456789*
    */
  Field testField;
  Piece testPiece(O,0,&testField);

  for(int i=0;i<4;++i)
    {
      testField.set(i,0);
    }
  for(int i=6;i<10;++i)
    {
      testField.set(i,0);
    }
  testPiece.handleInput(hard_drop);

  CPPUNIT_ASSERT( testField.get(4,0) );
  CPPUNIT_ASSERT( testField.get(5,0) );
  CPPUNIT_ASSERT(!testField.get(4,1) );
  CPPUNIT_ASSERT(!testField.get(5,1) );
}
