#include "PieceTest.hpp"
#include "Piece.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PieceTest );

void PieceTest::setUp()
{
}

void PieceTest::tearDown()
{
}

void PieceTest::testConstructor()
{
  int testDelay = 1;
  Field testField;
  // Piece(PieceType t, const int *d, Field *f);
  Piece *test_piece=NULL;

  test_piece=new Piece(I,&testDelay,&testField);
  delete test_piece;

  test_piece=new Piece(J,&testDelay,&testField);
  delete test_piece;

  test_piece=new Piece(L,&testDelay,&testField);
  delete test_piece;

  test_piece=new Piece(O,&testDelay,&testField);
  delete test_piece;

  test_piece=new Piece(S,&testDelay,&testField);
  delete test_piece;

  test_piece=new Piece(T,&testDelay,&testField);
  delete test_piece;

  test_piece=new Piece(Z,&testDelay,&testField);
  delete test_piece;
  CPPUNIT_FAIL( "not implemented" );
}

void PieceTest::testStep()
{
  CPPUNIT_FAIL( "not implemented" );
}

void PieceTest::testShift()
{
  CPPUNIT_FAIL( "not implemented" );
}

void PieceTest::testRotate()
{
  CPPUNIT_FAIL( "not implemented" );
}

void PieceTest::testDrop()
{
  CPPUNIT_FAIL( "not implemented" );
}
