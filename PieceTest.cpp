#include "PieceTest.hpp"
#include "Piece.hpp"
#include <list>
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PieceTest );


static const coord originCoord(4,20);
static const coord originI(5,20);
static const coord originO(5,21);

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

inline bool testSameCoords(boost::array<coord,4>presorted,boost::array<coord,4>unsorted)
{
  std::list<coord> sorted(unsorted.begin(),unsorted.end());
  sorted.sort(coordLess);
  std::list<coord>::iterator itor=sorted.begin();
  boost::array<coord,4>::iterator jtor=presorted.begin();
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
  boost::array<coord,4> expectedBlocks;


  Piece *test_piece=NULL;
  
  // I block
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,20);
  expectedBlocks[2](5,20);
  expectedBlocks[3](6,20);

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
  boost::array<coord,4> expectedBlocks;
  std::vector<Piece>::iterator itor;

  testPieces.reserve(7); 
  testPieces.push_back(Piece(J,testDelay,&testField));
  testPieces.push_back(Piece(L,testDelay,&testField));
  testPieces.push_back(Piece(S,testDelay,&testField));
  testPieces.push_back(Piece(Z,testDelay,&testField));
  testPieces.push_back(Piece(T,testDelay,&testField));

  
  expectedBlocks[0](3,0);
  expectedBlocks[1](4,0);
  expectedBlocks[2](5,0);
  expectedBlocks[3](5,1);

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
      // Test that above function did not inappropriately change center.
      CPPUNIT_ASSERT( testCoord==expectedCoord );
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
  CPPUNIT_ASSERT( testCoord==expectedCoord );

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
  CPPUNIT_ASSERT( testCoord==expectedCoord );
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

  testPieces.clear();
  // Test that locking with a larger-than-necessary timeStep does NOT throw,
  // that timeStepping after such a lock DOES throw,
  // and finally, that throwing a PieceLockError on timeStep does not inappropriately
  // change the result of getBlocks.
  testField.resetBlocks();
  testPieces.clear();
  testPieces.push_back(Piece(O,testDelay,&testField));
  testPieces.push_back(Piece(I,testDelay,&testField));
  CPPUNIT_ASSERT_NO_THROW( testPieces[0].timeStep(30) );
  CPPUNIT_ASSERT_THROW( testPieces[0].timeStep(2),PieceLockError );
  CPPUNIT_ASSERT_NO_THROW( testPieces[1].timeStep(30) );
  CPPUNIT_ASSERT_THROW( testPieces[1].timeStep(2),PieceLockError );
  /*__________*
   *          *04
   *          *03
   *   IIiI   *02
   *    Oo    *01 
   *    OO    *00
   *0123456789*
   */
  // Is O correct?
  expectedBlocks[0](4,0);
  expectedBlocks[1](4,1);
  expectedBlocks[2](5,0);
  expectedBlocks[3](5,1);
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, testPieces[0].getBlocks()) );
  // Is I correct?
  expectedBlocks[0](3,2);
  expectedBlocks[1](4,2);
  expectedBlocks[2](5,2);
  expectedBlocks[3](6,2);
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks, testPieces[1].getBlocks()) );
}

void PieceTest::testShift()
{
  int testDelay = 0, i=0;
  coord testCoord(-1,-1), expectedCoord(-1,-1);
  Field testField;
  std::vector<Piece> testPieces;
  boost::array<coord,4> expectedBlocks;

  // I block

  testPieces.push_back(Piece(I,testDelay,&testField));

  // Shift right
  testPieces[0].handleInput(shift_right);

  expectedCoord=originI;
  ++expectedCoord.x;

  
  expectedBlocks[0](4,20);
  expectedBlocks[1](5,20);
  expectedBlocks[2](6,20);
  expectedBlocks[3](7,20);
  
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
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Press against blocks
  testField.set(3,21);
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

  // T block represents all non-special case blocks.
  testPieces.push_back(Piece(T,testDelay,&testField));
  
  expectedCoord=originCoord;
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,20);
  expectedBlocks[2](4,21);
  expectedBlocks[3](5,20);

  // Shift right
  testPieces[0].handleInput(shift_right);
  testCoord=testPieces[0].getCenter();
  ++expectedCoord.x;
  for(i=0;i<3;++i) ++expectedBlocks[i].x;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Press against right wall
  for(i=0;i<5;++i) testPieces[0].handleInput(shift_right);
  testCoord=testPieces[0].getCenter();
  expectedCoord.x+=3;
  for(i=0;i<3;++i) expectedBlocks[i].x+=3;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Shift left
  testPieces[0].handleInput(shift_left);
  testCoord=testPieces[0].getCenter();
  ++expectedCoord.x;
  for(i=0;i<3;++i) ++expectedBlocks[i].x;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Press against left wall

  for(i=0;i<10;++i) testPieces[0].handleInput(shift_left);
  testCoord=testPieces[0].getCenter();
  expectedCoord.x-=6;
  for(i=0;i<3;++i) expectedBlocks[i].x-=6;
  
  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Press against blocks
  testField.set(4,20);
  testField.set(3,21);
  
  for(i=0;i<2;++i) testPieces[0].handleInput(shift_right);
  testCoord=testPieces[0].getCenter();
  ++expectedCoord.x;
  for(i=0;i<3;++i) ++expectedBlocks[i].x;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
 
  testField.set(1,21);
  testPieces[0].handleInput(shift_left);
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
   
  // Test lock exception
  testPieces[0].timeStep(20);
  testCoord=testPieces[0].getCenter();
  expectedCoord.y-=20;
  for(i=0;i<3;++i) expectedBlocks[i].y-=20;
  CPPUNIT_ASSERT_THROW( testPieces[0].handleInput(shift_right),PieceLockError );
  // Test that no change was made
  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

}

void PieceTest::testRotate()
{
  int testDelay = 0, i=0;
  coord testCoord(-1,-1), expectedCoord(-1,-1);
  Field testField;
  std::vector<Piece> testPieces;
  boost::array<coord,4> expectedBlocks;

  
  // I block
  testPieces.push_back(Piece(I,testDelay,&testField));

  expectedCoord=originI;

  // Turn CCW
  /*
   *__________*
   *          *21
   *   IIcI   *20
   *          *19 
   *          *18
   *0123456789*
   1 rotation
   *__________*
   *    I     *21
   *    Ic    *20
   *    I     *19 
   *    I     *18
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *     c    *20
   *   IIII   *19 
   *          *18
   *0123456789*
   3 rotations
   *__________*
   *     I    *21
   *     c    *20
   *     I    *19 
   *     I    *18
   *0123456789*
   */

  // 1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,18);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](4,21);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,19);
  expectedBlocks[2](5,19);
  expectedBlocks[3](6,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](5,21);
  expectedBlocks[1](5,20);
  expectedBlocks[2](5,19);
  expectedBlocks[3](5,18);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Does rotation persist through timestep?
  testPieces[0].timeStep(1);
  testCoord=testPieces[0].getCenter();
  --expectedCoord.y;
  for(i=0;i<3;++i) --expectedBlocks[i].y;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
 
  // Return to original orientation (but not original position!)
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,19);
  expectedBlocks[2](5,19);
  expectedBlocks[3](6,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // CW
  /*
   *__________*
   *          *21
   *          *20
   *   IIcI   *19
   *          *18 
   *          *17
   *0123456789*
   1 rotation  
   *__________*
   *          *21
   *     I    *20
   *     c    *19
   *     I    *18 
   *     I    *17
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *          *20
   *     c    *19
   *   IIII   *18 
   *          *17
   *0123456789*
   3 rotations
   *__________*
   *          *21
   *    I     *20
   *    Ic    *19
   *    I     *18 
   *    I     *17
   *0123456789*
   */
  //1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](5,20);
  expectedBlocks[1](5,19);
  expectedBlocks[2](5,18);
  expectedBlocks[3](5,17);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,18);
  expectedBlocks[1](4,18);
  expectedBlocks[2](5,18);
  expectedBlocks[3](6,18);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,20);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,18);
  expectedBlocks[3](4,17);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Blocked rotations
  /* Prevent CW rotation, but not CCW
   *__________*
   *          *21
   *    I     *20
   *   bIc    *19
   *    I     *18 
   *    I     *17
   *0123456789*
   *__________*
   *          *21
   *          *20
   *   bIcI   *19
   *          *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *          *20
   *   b c    *19
   *   IIII   *18 
   *          *17
   *0123456789*
   */

  testField.set(3,19);
  testPieces[0].handleInput(rotate_cw);//blocked
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,18);
  expectedBlocks[1](4,18);
  expectedBlocks[2](5,18);
  expectedBlocks[3](6,18);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces.clear();
  testField.resetBlocks();
  // L block
  testPieces.push_back(Piece(L,testDelay,&testField));

  expectedCoord=originCoord;

  // Turn CCW
  /*
   *__________*
   *     L    *21
   *   LcL    *20
   *          *19 
   *          *18
   *0123456789*
   1 rotation
   *__________*
   *   LL     *21
   *    c     *20
   *    L     *19 
   *          *18
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *   LcL    *20
   *   L      *19 
   *          *18
   *0123456789*
   3 rotations
   *__________*
   *    L     *21
   *    c     *20
   *    LL    *19 
   *          *18
   *0123456789*
   */

  // 1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,21);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](4,21);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](3,20);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,19);
  expectedBlocks[1](4,20);
  expectedBlocks[2](4,21);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Does rotation persist through timestep?
  testPieces[0].timeStep(1);
  testCoord=testPieces[0].getCenter();
  --expectedCoord.y;
  for(i=0;i<3;++i) --expectedBlocks[i].y;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
 
  // Return to original orientation (but not original position!)
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,19);
  expectedBlocks[2](5,19);
  expectedBlocks[3](5,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // CW
  /*
   *__________*
   *          *21
   *     L    *20
   *   LcL    *19
   *          *18 
   *          *17
   *0123456789*
   1 rotation  
   *__________*
   *          *21
   *    L     *20
   *    c     *19
   *    LL    *18 
   *          *17
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *          *20
   *   LcL    *19
   *   L      *18 
   *          *17
   *0123456789*
   3 rotations
   *__________*
   *          *21
   *   LL     *20
   *    c     *19
   *    L     *18 
   *          *17
   *0123456789*
   */
  //1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,20);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,18);
  expectedBlocks[3](5,18);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,18);
  expectedBlocks[1](3,19);
  expectedBlocks[2](4,19);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,18);
  expectedBlocks[2](4,19);
  expectedBlocks[3](4,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Blocked rotations
  /* Prevent CW rotation, but not CCW
   *__________*
   *          *21
   *   LLb    *20
   *    c     *19
   *    L     *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *     b    *20
   *   LcL    *19
   *          *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *     b    *20
   *   LcL    *19
   *   L      *18 
   *          *17
   *0123456789*
   */

  testField.set(5,20);
  testPieces[0].handleInput(rotate_cw);//blocked
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,18);
  expectedBlocks[1](3,19);
  expectedBlocks[2](4,19);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces.clear();
  testField.resetBlocks();
  // J block
  testPieces.push_back(Piece(J,testDelay,&testField));

  expectedCoord=originCoord;

  // Turn CCW
  /*
   *__________*
   *   J      *21
   *   JcJ    *20
   *          *19 
   *          *18
   *0123456789*
   1 rotation
   *__________*
   *    J     *21
   *    c     *20
   *   JJ     *19 
   *          *18
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *   JcJ    *20
   *     J    *19 
   *          *18
   *0123456789*
   3 rotations
   *__________*
   *    JJ    *21
   *    c     *20
   *    J     *19 
   *          *18
   *0123456789*
   */

  // 1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](4,21);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,20);
  expectedBlocks[2](5,20);
  expectedBlocks[3](5,21);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,19);
  expectedBlocks[1](4,20);
  expectedBlocks[2](4,21);
  expectedBlocks[3](5,21);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Does rotation persist through timestep?
  testPieces[0].timeStep(1);
  testCoord=testPieces[0].getCenter();
  --expectedCoord.y;
  for(i=0;i<3;++i) --expectedBlocks[i].y;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
 
  // Return to original orientation (but not original position!)
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](3,20);
  expectedBlocks[2](4,19);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // CW
  /*
   *__________*
   *          *21
   *   J      *20
   *   JcJ    *19
   *          *18 
   *          *17
   *0123456789*
   1 rotation  
   *__________*
   *          *21
   *    JJ    *20
   *    c     *19
   *    J     *18 
   *          *17
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *          *20
   *   JcJ    *19
   *     J    *18 
   *          *17
   *0123456789*
   3 rotations
   *__________*
   *          *21
   *    J     *20
   *    c     *19
   *   JJ     *18 
   *          *17
   *0123456789*
   */
  //1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,18);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,19);
  expectedBlocks[2](5,18);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,18);
  expectedBlocks[1](4,18);
  expectedBlocks[2](4,19);
  expectedBlocks[3](4,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Blocked rotations
  /* Prevent CW rotation, but not CCW
   *__________*
   *          *21
   *   bJ     *20
   *    c     *19
   *   JJ     *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *   b      *20
   *   JcJ    *19
   *          *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *   b      *20
   *   JcJ    *19
   *     J    *18 
   *          *17
   *0123456789*
   */

  testField.set(0,00);
  testPieces[0].handleInput(rotate_cw);//blocked
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,19);
  expectedBlocks[2](5,18);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces.clear();
  testField.resetBlocks();
  // S block
  testPieces.push_back(Piece(S,testDelay,&testField));

  expectedCoord=originCoord;

  // Turn CCW
  /*
   *__________*
   *    SS    *21
   *   Sc     *20
   *          *19 
   *          *18
   *0123456789*
   1 rotation
   *__________*
   *   S      *21
   *   Sc     *20
   *    S     *19 
   *          *18
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *    cS    *20
   *   SS     *19 
   *          *18
   *0123456789*
   3 rotations
   *__________*
   *    S     *21
   *    cS    *20
   *     S    *19 
   *          *18
   *0123456789*
   */

  // 1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,20);
  expectedBlocks[1](3,21);
  expectedBlocks[2](4,19);
  expectedBlocks[3](4,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,20);
  expectedBlocks[1](4,21);
  expectedBlocks[2](5,19);
  expectedBlocks[3](5,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Does rotation persist through timestep?
  testPieces[0].timeStep(1);
  testCoord=testPieces[0].getCenter();
  --expectedCoord.y;
  for(i=0;i<3;++i) --expectedBlocks[i].y;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
 
  // Return to original orientation (but not original position!)
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // CW
  /*
   *__________*
   *          *21
   *    SS    *20
   *   Sc     *19
   *          *18 
   *          *17
   *0123456789*
   1 rotation  
   *__________*
   *          *21
   *    S     *20
   *    cS    *19
   *     S    *18 
   *          *17
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *          *20
   *    cS    *19
   *   SS     *18 
   *          *17
   *0123456789*
   3 rotations
   *__________*
   *          *21
   *   S      *20
   *   Sc     *19
   *    S     *18 
   *          *17
   *0123456789*
   */
  //1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,19);
  expectedBlocks[1](4,20);
  expectedBlocks[2](5,18);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,18);
  expectedBlocks[1](4,18);
  expectedBlocks[2](4,19);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](3,20);
  expectedBlocks[2](4,18);
  expectedBlocks[3](4,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Blocked rotations
  /* Prevent CW rotation, but not CCW
   *__________*
   *          *21
   *   Sb     *20
   *   Sc     *19
   *    S     *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *    bS    *20
   *   Sc     *19
   *          *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *    b     *20
   *    cS    *19
   *   SS     *18 
   *          *17
   *0123456789*
   */

  testField.set(4,20);
  testPieces[0].handleInput(rotate_cw);//blocked
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,18);
  expectedBlocks[1](4,18);
  expectedBlocks[2](4,19);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces.clear();
  testField.resetBlocks();
  // Z block
  testPieces.push_back(Piece(Z,testDelay,&testField));

  expectedCoord=originCoord;

  // Turn CCW
  /*
   *__________*
   *   ZZ     *21
   *    cZ    *20
   *          *19 
   *          *18
   *0123456789*
   1 rotation
   *__________*
   *    Z     *21
   *   Zc     *20
   *   Z      *19 
   *          *18
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *   Zc     *20
   *    ZZ    *19 
   *          *18
   *0123456789*
   3 rotations
   *__________*
   *     Z    *21
   *    cZ    *20
   *    Z     *19 
   *          *18
   *0123456789*
   */

  // 1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](3,20);
  expectedBlocks[2](4,20);
  expectedBlocks[3](4,21);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,19);
  expectedBlocks[1](4,20);
  expectedBlocks[2](5,20);
  expectedBlocks[3](5,21);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Does rotation persist through timestep?
  testPieces[0].timeStep(1);
  testCoord=testPieces[0].getCenter();
  --expectedCoord.y;
  for(i=0;i<3;++i) --expectedBlocks[i].y;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
 
  // Return to original orientation (but not original position!)
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // CW
  /*
   *__________*
   *          *21
   *   ZZ     *20
   *    cZ    *19
   *          *18 
   *          *17
   *0123456789*
   1 rotation  
   *__________*
   *          *21
   *     Z    *20
   *    cZ    *19
   *    Z     *18 
   *          *17
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *          *20
   *   Zc     *19
   *    ZZ    *18 
   *          *17
   *0123456789*
   3 rotations
   *__________*
   *          *21
   *    Z     *20
   *   Zc     *19
   *   Z      *18 
   *          *17
   *0123456789*
   */
  //1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,18);
  expectedBlocks[1](4,19);
  expectedBlocks[2](5,19);
  expectedBlocks[3](5,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,18);
  expectedBlocks[2](4,19);
  expectedBlocks[3](5,18);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,18);
  expectedBlocks[1](3,19);
  expectedBlocks[2](4,19);
  expectedBlocks[3](4,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Blocked rotations
  /* Prevent CCW rotation, but not CW
   *__________*
   *          *21
   *    Z     *20
   *   Zc     *19
   *   Zb     *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *          *20
   *   Zc     *19
   *    bZ    *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *   ZZ     *20
   *    cZ    *19
   *    b     *18 
   *          *17
   *0123456789*
   */

  testField.set(4,18);
  testPieces[0].handleInput(rotate_ccw);//blocked
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces[0].handleInput(rotate_cw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces.clear();
  testField.resetBlocks();
  // T block
  testPieces.push_back(Piece(T,testDelay,&testField));

  expectedCoord=originCoord;

  // Turn CCW
  /*
   *__________*
   *    T     *21
   *   TcT    *20
   *          *19 
   *          *18
   *0123456789*
   1 rotation
   *__________*
   *    T     *21
   *   Tc     *20
   *    T     *19 
   *          *18
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *   TcT    *20
   *    T     *19 
   *          *18
   *0123456789*
   3 rotations
   *__________*
   *    T     *21
   *    cT    *20
   *    T     *19 
   *          *18
   *0123456789*
   */

  // 1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](4,21);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,20);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  // 3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,19);
  expectedBlocks[1](4,20);
  expectedBlocks[2](4,21);
  expectedBlocks[3](5,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Does rotation persist through timestep?
  testPieces[0].timeStep(1);
  testCoord=testPieces[0].getCenter();
  --expectedCoord.y;
  for(i=0;i<3;++i) --expectedBlocks[i].y;

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
 
  // Return to original orientation (but not original position!)
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](0,00);
  expectedBlocks[1](0,00);
  expectedBlocks[2](0,00);
  expectedBlocks[3](0,00);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // CW
  /*
   *__________*
   *          *21
   *    T     *20
   *   TcT    *19
   *          *18 
   *          *17
   *0123456789*
   1 rotation  
   *__________*
   *          *21
   *    T     *20
   *    cT    *19
   *    T     *18 
   *          *17
   *0123456789*
   2 rotations
   *__________*
   *          *21
   *          *20
   *   TcT    *19
   *    T     *18 
   *          *17
   *0123456789*
   3 rotations
   *__________*
   *          *21
   *    T     *20
   *   Tc     *19
   *    T     *18 
   *          *17
   *0123456789*
   */
  //1
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,18);
  expectedBlocks[1](4,19);
  expectedBlocks[2](4,20);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //2
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,18);
  expectedBlocks[2](4,19);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  //3
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,18);
  expectedBlocks[2](4,19);
  expectedBlocks[3](4,20);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  // Blocked rotations
  /* Prevent CW rotation, but not CCW
   *__________*
   *          *21
   *    T     *20
   *   Tc     *19
   *    T     *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *    b     *20
   *   TcT    *19
   *          *18 
   *          *17
   *0123456789*
   *__________*
   *          *21
   *    b     *20
   *   TcT    *19
   *    T     *18 
   *          *17
   *0123456789*
   */

  testField.set(4,20);
  testPieces[0].handleInput(rotate_cw);//blocked
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](3,19);
  expectedBlocks[1](4,18);
  expectedBlocks[2](4,19);
  expectedBlocks[3](5,19);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces.clear();
  testField.resetBlocks();
  // O rly? yarly.
  testPieces.push_back(Piece(O,testDelay,&testField));
  /* O block should not change when rotated.
   *__________*
   *    Oc    *21
   *    OO    *20
   *          *19
   *          *18 
   *          *17
   *0123456789*
   */
  expectedCoord=originO;
  testPieces[0].handleInput(rotate_cw);

  testCoord=testPieces[0].getCenter();
  expectedBlocks[0](4,20);
  expectedBlocks[1](4,21);
  expectedBlocks[2](5,20);
  expectedBlocks[3](5,21);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces[0].handleInput(rotate_ccw);
  testPieces[0].handleInput(rotate_ccw);
  testCoord=testPieces[0].getCenter();

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  testPieces.clear();
  // Check exception (but not with an O block)
  testPieces.push_back(Piece(L,testDelay,&testField));
  testPieces[0].handleInput(rotate_cw);
  expectedCoord=originCoord;
  expectedBlocks[0](4,19);
  expectedBlocks[1](4,20);
  expectedBlocks[2](4,21);
  expectedBlocks[3](5,19);
  
  testPieces[0].timeStep(20);
  testCoord=testPieces[0].getCenter();
  expectedCoord.y-=19;
  for(i=0;i<3;++i) expectedBlocks[i].y-=19;

  CPPUNIT_ASSERT_THROW( testPieces[0].handleInput(rotate_cw),PieceLockError );
  // Test that no change was made
  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

}

void PieceTest::testDrop()
{
  int testDelay = 0, i=0;
  coord testCoord(-1,-1), expectedCoord(-1,-1);
  Field testField;
  std::vector<Piece> testPieces;
  boost::array<coord,4> expectedBlocks;
  std::vector<Piece>::iterator itor;

  // Hard drop
  testPieces.reserve(5); 
  testPieces.push_back(Piece(J,testDelay,&testField));
  testPieces.push_back(Piece(L,testDelay,&testField));
  testPieces.push_back(Piece(S,testDelay,&testField));
  testPieces.push_back(Piece(Z,testDelay,&testField));
  testPieces.push_back(Piece(T,testDelay,&testField));
  
  for(itor=testPieces.begin();itor!=testPieces.end();++itor)
    {
      itor->handleInput(hard_drop);
      CPPUNIT_ASSERT_THROW( itor->handleInput(hard_drop),PieceLockError );
      CPPUNIT_ASSERT( testField.get(4,0) );
      CPPUNIT_ASSERT( coord(4,0) == itor->getCenter() );
      testField.resetBlocks();
    }
  
  testPieces.clear();
  
  testPieces.push_back(Piece(I,testDelay,&testField));
  testPieces.push_back(Piece(O,testDelay,&testField));
  testPieces[0].handleInput(hard_drop);
  testPieces[1].handleInput(hard_drop);
  /*
   *__________*
   *          *04
   *          *03
   *    Oo    *02
   *    OO    *01 
   *   IIiI   *00
   *0123456789*
   */

  expectedCoord=originI;
  expectedCoord.y-=20;
  
  for(i=0;i<3;++i)
    {
      expectedBlocks[i](i+3,0);
    }

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  CPPUNIT_ASSERT_THROW( testPieces[0].handleInput(hard_drop),PieceLockError );
  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

  expectedCoord=originO;
  expectedCoord.y-=18;
  expectedBlocks[0](4,1);
  expectedBlocks[1](4,2);
  expectedBlocks[2](5,1);
  expectedBlocks[3](5,2);

  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );
  CPPUNIT_ASSERT_THROW( testPieces[0].handleInput(hard_drop),PieceLockError );
  CPPUNIT_ASSERT( testCoord==expectedCoord );
  CPPUNIT_ASSERT( testSameCoords(expectedBlocks,testPieces[0].getBlocks()) );

}
