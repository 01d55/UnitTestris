#include "PieceTest.hpp"
#include "Piece.hpp"
#include <list>
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PieceTest );


const coord originCoord(4,20);
const coord originI(5,20);
const coord originO(5,21);

/*
 *__________*
 *     L    *21
 *   LcL    *20
 *          *19 
 *          *18
 *0123456789*

 *__________*
 *    Oc    *21
 *    OO    *20
 *          *19 
 *          *18
 *0123456789*

 *__________*
 *          *21
 *   LLcL   *20
 *          *19 
 *          *18
 *0123456789*
 */
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
  unsigned int testDelay = 1;
  Field testField;
  coord testCoord(-1,-1);
  std::vector<coord> expectedBlocks;


  Piece *test_piece=NULL;
  expectedBlocks.reserve(4);
  // I block
  expectedBlocks.push_back(coord(3,20));
  expectedBlocks.push_back(coord(4,20));
  expectedBlocks.push_back(coord(5,20));
  expectedBlocks.push_back(coord(6,20));

  test_piece=new Piece(I,testDelay,&testField);

  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originI );
  // Don't insist that the blocks be sorted in a particular way.
  // Do insist that there be exactly 4 blocks, and that they be in the correct places.
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );
   
  delete test_piece;

  // J block
  expectedBlocks[0](3,20);
  expectedBlocks[1](3,21);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,20);

  test_piece=new Piece(J,testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // L block
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,20);
  expectedBlocks[2](5,20);
  expectedBlocks[3](5,21);

  test_piece=new Piece(L,testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // O block
  expectedBlocks[0](4,20);
  expectedBlocks[1](4,21);
  expectedBlocks[2](5,20);
  expectedBlocks[3](5,21);

  test_piece=new Piece(O,testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originO );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // S block
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,20);
  expectedBlocks[2](4,21);
  expectedBlocks[3](5,21);

  test_piece=new Piece(S,testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );
  
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // T block
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,20);
  expectedBlocks[2](4,21);
  expectedBlocks[3](5,20);

  test_piece=new Piece(T,testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );

  delete test_piece;

  // Z Block
  expectedBlocks[0](3,21);
  expectedBlocks[1](4,20);
  expectedBlocks[2](4,21);
  expectedBlocks[3](5,20);

  test_piece=new Piece(Z,testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, test_piece->getBlocks()) );
  delete test_piece;

}

void PieceTest::testStep()
{
  int testDelay = 0, failSafe=44, i=0;
  coord testCoord(-1,-1), expectedCoord(-1,-1);
  Field testField;
  std::vector<Piece> testPieces;
  std::vector<coord> expectedBlocks;
  std::vector<Piece>::iterator itor;

  testPieces.reserve(7); 
  testPieces.push_back(Piece(J,testDelay,&testField));
  testPieces.push_back(Piece(L,testDelay,&testField));
  testPieces.push_back(Piece(S,testDelay,&testField));
  testPieces.push_back(Piece(Z,testDelay,&testField));
  testPieces.push_back(Piece(T,testDelay,&testField));

  expectedBlocks.reserve(4);
  expectedBlocks.push_back(coord(3,0));
  expectedBlocks.push_back(coord(4,0));
  expectedBlocks.push_back(coord(5,0));
  expectedBlocks.push_back(coord(5,1));

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
  // Special case tests: I and O

  testPieces.push_back(Piece(I,testDelay,&testField));
  itor=testPieces.end();
  --itor;
  expectedCoord=originI;
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
  testPieces.push_back(Piece(O,testDelay,&testField));
  itor=testPieces.end();
  --itor;
  expectedCoord=originO;
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

  // Test that locked pieces placed blocks into Field properly. Errors in Field may cause this test to fail.
  /*
   *          *12
   *    OO    *11
   *    OO    *10
   *   IIII   *9
   *    T     *8
   *   TTT    *7
   *   ZZ     *6
   *    ZZ    *5
   *    SS    *4
   *   SSL    *3
   *   LLL    *2
   *   J      *1
   *   JJJ    *0
   *0123456789*
   */
  for(i=0;i<FIELD_WIDTH;++i)
    {
      CPPUNIT_ASSERT( (2<i<7) ? (testField.get(i,0)) : (!testField.get(i,9)) );

      CPPUNIT_ASSERT( (2<i<6) ? (testField.get(i,1)) : (!testField.get(i,0)) );
      CPPUNIT_ASSERT( (2<i<6) ? (testField.get(i,3)) : (!testField.get(i,2)) );
      CPPUNIT_ASSERT( (2<i<6) ? (testField.get(i,4)) : (!testField.get(i,3)) );
      CPPUNIT_ASSERT( (2<i<6) ? (testField.get(i,8)) : (!testField.get(i,7)) );

      CPPUNIT_ASSERT( (i==3) ? (testField.get(i,2)) : (!testField.get(i,1)) );

      CPPUNIT_ASSERT( (3<i<6) ? (testField.get(i,5)) : (!testField.get(i,4)) );
      CPPUNIT_ASSERT( (3<i<6) ? (testField.get(i,6)) : (!testField.get(i,5)) );
      CPPUNIT_ASSERT( (3<i<6) ? (testField.get(i,6)) : (!testField.get(i,10)) );
      CPPUNIT_ASSERT( (3<i<6) ? (testField.get(i,6)) : (!testField.get(i,11)) );

      CPPUNIT_ASSERT( (2<i<5) ? (testField.get(i,7)) : (!testField.get(i,6)) );

      CPPUNIT_ASSERT( (i==4) ? (testField.get(i,9)) : (!testField.get(i,8)) );
    }

  // Test that Pieces accurately report block positions both before and after locking.
  // After lock:

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, testPieces[0].getBlocks()) );
  
  // Before lock:
  testPieces.clear();
  testPieces.push_back(Piece(I,testDelay,&testField));
  testPieces[0].timeStep(3);
  expectedBlocks[0](3,20-3);
  expectedBlocks[1](4,20-3);
  expectedBlocks[2](5,20-3);
  expectedBlocks[3](6,20-3);

  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, testPieces[0].getBlocks()) );

  // Test lock delay.
  /*
   *          *13
   *   IIII   *12
   *    OO    *11
   *    OO    *10
   *   IIII   *9
   *    T     *8
   *   TTT    *7
   *   ZZ     *6
   *    ZZ    *5
   *    SS    *4
   *   SSL    *3
   *   LLL    *2
   *   J      *1
   *   JJJ    *0
   *0123456789*
   */
  testPieces[0].timeStep(4);
  CPPUNIT_ASSERT_NO_THROW( testPieces[0].timeStep(1) );
  CPPUNIT_ASSERT_THROW( testPieces[0].timeStep(1),PieceLockError );
  
  /*
   *          *13
   *   IIII   *12
   *    OO    *11
   *    OO    *10 
   *   IIII   *9
   *0123456789*
   */

  testDelay=3;
  testPieces.clear();
  testPieces.push_back(Piece(I,testDelay,&testField));
  testPieces[0].timeStep(9);
  CPPUNIT_ASSERT_NO_THROW( testPieces[0].timeStep(1) );
  CPPUNIT_ASSERT_THROW( testPieces[0].timeStep(1),PieceLockError );

}

void PieceTest::testShift()
{
  int testDelay = 0, failSafe=44, i=0;
  coord testCoord(-1,-1), expectedCoord(-1,-1);
  Field testField;
  std::vector<Piece> testPieces;
  std::vector<coord> expectedBlocks;
  std::vector<Piece>::iterator itor;

  // I block

  testPieces.push_back(Piece(I,testDelay,&testField));

  // Shift right
  testPieces[0].handleInput(shift_right);

  expectedCoord=originI;
  ++expectedCoord.x;

  expectedBlocks.reserve(4);
  expectedBlocks.push_back(coord(4,20));
  expectedBlocks.push_back(coord(5,20));
  expectedBlocks.push_back(coord(6,20));
  expectedBlocks.push_back(coord(7,20));
  
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Press against right wall.
  for(i=0;i<5;++i) testPieces[0].handleInput(shift_right);

  expectedCoord.x+=2;

  expectedBlocks[0](6,20);
  expectedBlocks[1](7,20);
  expectedBlocks[2](8,20);
  expectedBlocks[3](9,20);

  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Shift left
  testPieces[0].handleInput(shift_left);
  --expectedCoord.x;

  for(i=0;i<3;++i) --expectedBlocks[i].x;

  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
 
  // Press against left wall.
  for(i=0;i<10;++i) testPieces[0].handleInput(shift_left);

  expectedCoord.x-=5;

  expectedBlocks[0].x=0;
  expectedBlocks[1].x=1;
  expectedBlocks[2].x=2;
  expectedBlocks[3].x=3;

  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Press against a block in the field
  testField.set(5,20);
  for(i=0;i<2;++i) testPieces[0].handleInput(shift_right);

  ++expectedCoord.x;

  for(i=0;i<3;++i) ++expectedBlocks[i].x;

  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) ); 

  testField.set(0,20);

  testPieces[0].handleInput(shift_left);
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testField.resetBlocks();
  testPieces.clear();

  // O block
  testPieces.push_back(Piece(O,testDelay,&testField));

  expectedCoord=originO;

  expectedBlocks[0](4,20);
  expectedBlocks[1](4,21);
  expectedBlocks[2](5,20);
  expectedBlocks[3](5,21);

  // Shift right
  testPieces[0].handleInput(shift_right);
  ++expectedCoord.x;

  for(i=0;i<3;++i) ++expectedBlocks[i].x;
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Press against right wall
  for(i=0;i<5;++i) testPieces[0].handleInput(shift_right);

  expectedCoord.x+=3;

  for(i=0;i<3;++i) expectedBlocks[i].x+=3;
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Shift left
  testPieces[0].handleInput(shift_left);
  --expectedCoord.x;
  for(i=0;i<3;++i) --expectedBlocks[i].x;
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Press against left wall

  for(i=0;i<10;++i) testPieces[0].handleInput(shift_left);
  expectedCoord.x-=7;
  for(i=0;i<3;++i) expectedBlocks[i].x-=7;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Press against blocks
  testField.set(3,21);
  for(i=0;i<2;++i) testPieces[0].handleInput(shift_right);
  ++expectedCoord.x;
  for(i=0;i<3;++i) ++expectedBlocks[i].x;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  
  testField.set(0,20);
  testPieces[0].handleInput(shift_left);
  
  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  
  testField.resetBlocks();
  testPieces.clear();

  // T block represents all non-special case blocks.
  testPieces.push_back(Piece(T,testDelay,&testField));
  
  expectedCoord=originCoord;
  expectedBlocks[0](3,20);
  expectedBlocks[0](4,20);
  expectedBlocks[0](4,21);
  expectedBlocks[0](5,20);

  // Shift right
  testPieces[0].handleInput(shift_right);
  ++expectedCoord.x;
  for(i=0;i<3;++i) ++expectedBlocks[i].x;
  CPPUNIT_FAIL( "Test not complete." );
}

void PieceTest::testRotate()
{
  CPPUNIT_FAIL( "not implemented" );
}

void PieceTest::testDrop()
{
  CPPUNIT_FAIL( "not implemented" );
}
