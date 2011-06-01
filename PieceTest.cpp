#include "PieceTest.hpp"
#include "Piece.hpp"
#include <list>
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PieceTest );

// For sorting coords, somewhat arbitrarily.
inline bool coordLess(coord a, coord b)
{
  if(a.x<b.x) return true;
  else if (a.x>b.x) return false;
  // a.x==b.x
  return a.y<b.y;
}

inline bool testSameCoords(std::vector<coord>presorted,std::vector<coord>unsorted)
{
  if ( presorted.size()!=unsorted.size() ) return false;
  std::list<coord> sorted(unsorted.begin(),unsorted.end());
  sorted.sort(coordLess);
  std::list<coord>::iterator itor=sorted.begin();
  std::vector<coord>::iterator jtor=presorted.begin();
  while(jtor!=presorted.end())
    {
      if( *jtor!=*itor ) return false;
      jtor++;
      itor++;
    }
  return true;
}

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
  coord testCoord(-1,-1), originCoord(4,20);
  std::vector<coord> expectedBlocks;


  Piece *test_piece=NULL;
  expectedBlocks.reserve(4);
  // I block
  expectedBlocks.push_back(coord(3,21));
  expectedBlocks.push_back(coord(4,21));
  expectedBlocks.push_back(coord(5,21));
  expectedBlocks.push_back(coord(6,21));

  test_piece=new Piece(I,&testDelay,&testField);

  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );
  // Don't insist that the blocks be sorted in a particular way.
  // Do insist that there be exactly 4 blocks, and that they be in the correct places.
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );
   
  delete test_piece;

  // J block
  expectedBlocks[0]=coord(3,20);
  expectedBlocks[1]=coord(3,21);
  expectedBlocks[2]=coord(4,20);
  expectedBlocks[3]=coord(5,20);

  test_piece=new Piece(J,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // L block
  expectedBlocks[0]=coord(3,20);
  expectedBlocks[1]=coord(4,20);
  expectedBlocks[2]=coord(5,20);
  expectedBlocks[3]=coord(5,21);

  test_piece=new Piece(L,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // O block
  expectedBlocks[0]=coord(4,20);
  expectedBlocks[1]=coord(4,21);
  expectedBlocks[2]=coord(5,20);
  expectedBlocks[3]=coord(5,21);

  test_piece=new Piece(O,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // S block
  expectedBlocks[0]=coord(3,20);
  expectedBlocks[1]=coord(4,20);
  expectedBlocks[2]=coord(4,21);
  expectedBlocks[3]=coord(5,21);

  test_piece=new Piece(S,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );
  
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // T block
  expectedBlocks[0]=coord(3,20);
  expectedBlocks[1]=coord(4,20);
  expectedBlocks[2]=coord(4,21);
  expectedBlocks[3]=coord(5,20);

  test_piece=new Piece(T,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // Z Block
  expectedBlocks[0]=coord(3,21);
  expectedBlocks[1]=coord(4,20);
  expectedBlocks[2]=coord(4,21);
  expectedBlocks[3]=coord(5,20);

  test_piece=new Piece(Z,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );
  delete test_piece;

}

void PieceTest::testStep()
{
  int testDelay = 0, failSafe=44, i=0;
  coord testCoord(-1,-1), expectedCoord(-1,-1), originCoord(4,20);
  Field testField;
  std::vector<Piece> testPieces;
  std::vector<coord> expectedBlocks;
  std::vector<Piece>::iterator itor;

  testPieces.reserve(6); 
  testPieces.push_back(Piece(I,&testDelay,&testField));
  testPieces.push_back(Piece(J,&testDelay,&testField));
  testPieces.push_back(Piece(L,&testDelay,&testField));
  testPieces.push_back(Piece(S,&testDelay,&testField));
  testPieces.push_back(Piece(Z,&testDelay,&testField));
  testPieces.push_back(Piece(T,&testDelay,&testField));

  expectedBlocks.reserve(4);
  expectedBlocks.push_back(coord(3,0));
  expectedBlocks.push_back(coord(4,0));
  expectedBlocks.push_back(coord(5,0));
  expectedBlocks.push_back(coord(6,0));

  // Test proper center movement & locking
  for(itor=testPieces.begin();itor!=testPieces.end();++itor)
    {
      expectedCoord=originCoord;
      i=0;
      while(itor->timeStep(1))
	{
	  // If time step never returns false (i.e. the block falls forever)
	  // we need to detect and fail.
	  ++i;
	  CPPUNIT_ASSERT(i<failSafe);
	  // Test delay is 0, so the piece locks as soon as it touches bottom
	  expectedCoord.y--;
	  testCoord=itor->getCenter();
	  CPPUNIT_ASSERT( testCoord==expectedCoord );
	}
      // Test throw when timestepping a locked Piece
      CPPUNIT_ASSERT_THROW( itor->timeStep(1),PieceLockError );
    }
  // Test that locked pieces placed blocks into Field properly. Errors in Field may cause this test to fail.
  /*    T     *9
   *   TTT    *8
   *   ZZ     *7
   *    ZZ    *6
   *    SS    *5
   *   SSL    *4
   *   LLL    *3
   *   J      *2
   *   JJJ    *1
   *   IIII   *0
   *0123456789*
   */
  for(i=0;i<FIELD_WIDTH;++i)
    {
      CPPUNIT_ASSERT( (2<i<7) ? (testField.get(i,0)) : (!testField.get(i,0)) );

      CPPUNIT_ASSERT( (2<i<6) ? (testField.get(i,1)) : (!testField.get(i,1)) );
      CPPUNIT_ASSERT( (2<i<6) ? (testField.get(i,3)) : (!testField.get(i,3)) );
      CPPUNIT_ASSERT( (2<i<6) ? (testField.get(i,4)) : (!testField.get(i,4)) );
      CPPUNIT_ASSERT( (2<i<6) ? (testField.get(i,8)) : (!testField.get(i,8)) );

      CPPUNIT_ASSERT( (i==3) ? (testField.get(i,2)) : (!testField.get(i,2)) );

      CPPUNIT_ASSERT( (3<i<6) ? (testField.get(i,5)) : (!testField.get(i,5)) );
      CPPUNIT_ASSERT( (3<i<6) ? (testField.get(i,6)) : (!testField.get(i,6)) );

      CPPUNIT_ASSERT( (2<i<5) ? (testField.get(i,7)) : (!testField.get(i,7)) );

      CPPUNIT_ASSERT( (i==4) ? (testField.get(i,9)) : (!testField.get(i,9)) );
    }

  // Test that Pieces accurately report block positions both before and after locking.
  // After lock:

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, testPieces[0].getBlocks()) );
  
  // Before lock:
  testPieces.clear();
  testPieces.push_back(Piece(I,&testDelay,&testField));
  testPieces[0].timeStep(3);
  expectedBlocks[0]=coord(3,21-3);
  expectedBlocks[1]=coord(4,21-3);
  expectedBlocks[2]=coord(5,21-3);
  expectedBlocks[3]=coord(6,21-3);

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, testPieces[0].getBlocks()) );
}

void PieceTest::testShift()
{
  int testDelay = 0, failSafe=44, i=0;
  coord testCoord(-1,-1), expectedCoord(-1,-1), originCoord(4,20);
  Field testField;
  std::vector<Piece> testPieces;
  std::vector<coord> expectedBlocks;
  std::vector<Piece>::iterator itor;

  testPieces.push_back(Piece(I,&testDelay,&testField));
  testPieces[0].handleInput(shift_right);

  expectedBlocks.reserve(4);
  expectedBlocks.push_back(coord(4,21));
  expectedBlocks.push_back(coord(5,21));
  expectedBlocks.push_back(coord(6,21));
  expectedBlocks.push_back(coord(7,21));

  expectedCoord=originCoord;
  ++expectedCoord.x;

  testCoord=testPieces[0].getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  
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
