#include "PieceTest.hpp"
#include "Piece.hpp"
#include <list>
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( PieceTest );

// For sorting coords, somewhat arbitrarily. Used in testConstructor.
bool coordLess(coord a, coord b)
{
  if(a.x<b.x) return true;
  else if (a.x>b.x) return false;
  // a.x==b.x
  return a.y<b.y;
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
  std::vector<coord> expectedBlocks, temp;


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
  temp=test_piece->getBlocks();
  CPPUNIT_ASSERT( temp.size()==4 );
  {
    // Sort the not-necessarily sorted blocks. Expected blocks are hand-sorted.
    std::list<coord> testBlocks(temp.begin(),temp.end());
    testBlocks.sort(coordLess);
    std::list<coord>::iterator itor=testBlocks.begin();
    std::vector<coord>::iterator jtor=expectedBlocks.begin();
    while(jtor!=expectedBlocks.end())
      {
	CPPUNIT_ASSERT( *jtor==*itor );
	jtor++;
	itor++;
      }
  }
  
  delete test_piece;

  // J block
  expectedBlocks[0]=coord(3,20);
  expectedBlocks[1]=coord(3,21);
  expectedBlocks[2]=coord(4,20);
  expectedBlocks[3]=coord(5,20);

  test_piece=new Piece(J,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );
  temp=test_piece->getBlocks();
  CPPUNIT_ASSERT( temp.size()==4 );
  {
    // Sort the not-necessarily sorted blocks. Expected blocks are hand-sorted.
    std::list<coord> testBlocks(temp.begin(),temp.end());
    testBlocks.sort(coordLess);
    std::list<coord>::iterator itor=testBlocks.begin();
    std::vector<coord>::iterator jtor=expectedBlocks.begin();
    while(jtor!=expectedBlocks.end())
      {
	CPPUNIT_ASSERT( *jtor==*itor );
	jtor++;
	itor++;
      }
  }

  delete test_piece;

  // L block
  expectedBlocks[0]=coord(3,20);
  expectedBlocks[1]=coord(4,20);
  expectedBlocks[2]=coord(5,20);
  expectedBlocks[3]=coord(5,21);

  test_piece=new Piece(L,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  temp=test_piece->getBlocks();
  CPPUNIT_ASSERT( temp.size()==4 );
  {
    // Sort the not-necessarily sorted blocks. Expected blocks are hand-sorted.
    std::list<coord> testBlocks(temp.begin(),temp.end());
    testBlocks.sort(coordLess);
    std::list<coord>::iterator itor=testBlocks.begin();
    std::vector<coord>::iterator jtor=expectedBlocks.begin();
    while(jtor!=expectedBlocks.end())
      {
	CPPUNIT_ASSERT( *jtor==*itor );
	jtor++;
	itor++;
      }
  }

  delete test_piece;

  // O block
  expectedBlocks[0]=coord(4,20);
  expectedBlocks[1]=coord(4,21);
  expectedBlocks[2]=coord(5,20);
  expectedBlocks[3]=coord(5,21);

  test_piece=new Piece(O,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  temp=test_piece->getBlocks();
  CPPUNIT_ASSERT( temp.size()==4 );
  {
    // Sort the not-necessarily sorted blocks. Expected blocks are hand-sorted.
    std::list<coord> testBlocks(temp.begin(),temp.end());
    testBlocks.sort(coordLess);
    std::list<coord>::iterator itor=testBlocks.begin();
    std::vector<coord>::iterator jtor=expectedBlocks.begin();
    while(jtor!=expectedBlocks.end())
      {
	CPPUNIT_ASSERT( *jtor==*itor );
	jtor++;
	itor++;
      }
  }

  delete test_piece;

  // S block
  expectedBlocks[0]=coord(3,20);
  expectedBlocks[1]=coord(4,20);
  expectedBlocks[2]=coord(4,21);
  expectedBlocks[3]=coord(5,21);

  test_piece=new Piece(S,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );
  
  temp=test_piece->getBlocks();
  CPPUNIT_ASSERT( temp.size()==4 );
  {
    // Sort the not-necessarily sorted blocks. Expected blocks are hand-sorted.
    std::list<coord> testBlocks(temp.begin(),temp.end());
    testBlocks.sort(coordLess);
    std::list<coord>::iterator itor=testBlocks.begin();
    std::vector<coord>::iterator jtor=expectedBlocks.begin();
    while(jtor!=expectedBlocks.end())
      {
	CPPUNIT_ASSERT( *jtor==*itor );
	jtor++;
	itor++;
      }
  }

  delete test_piece;

  // T block
  expectedBlocks[0]=coord(3,20);
  expectedBlocks[1]=coord(4,20);
  expectedBlocks[2]=coord(4,21);
  expectedBlocks[3]=coord(5,20);

  test_piece=new Piece(T,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  temp=test_piece->getBlocks();
  CPPUNIT_ASSERT( temp.size()==4 );
  {
    // Sort the not-necessarily sorted blocks. Expected blocks are hand-sorted.
    std::list<coord> testBlocks(temp.begin(),temp.end());
    testBlocks.sort(coordLess);
    std::list<coord>::iterator itor=testBlocks.begin();
    std::vector<coord>::iterator jtor=expectedBlocks.begin();
    while(jtor!=expectedBlocks.end())
      {
	CPPUNIT_ASSERT( *jtor==*itor );
	jtor++;
	itor++;
      }
  }

  delete test_piece;

  // Z Block
  expectedBlocks[0]=coord(3,21);
  expectedBlocks[1]=coord(4,20);
  expectedBlocks[2]=coord(4,21);
  expectedBlocks[3]=coord(5,20);

  test_piece=new Piece(Z,&testDelay,&testField);
  testCoord=test_piece->getCenter();
  CPPUNIT_ASSERT( testCoord==originCoord );

  temp=test_piece->getBlocks();
  CPPUNIT_ASSERT( temp.size()==4 );
  {
    // Sort the not-necessarily sorted blocks. Expected blocks are hand-sorted.
    std::list<coord> testBlocks(temp.begin(),temp.end());
    testBlocks.sort(coordLess);
    std::list<coord>::iterator itor=testBlocks.begin();
    std::vector<coord>::iterator jtor=expectedBlocks.begin();
    while(jtor!=expectedBlocks.end())
      {
	CPPUNIT_ASSERT( *jtor==*itor );
	jtor++;
	itor++;
      }
  }

  delete test_piece;

}

void PieceTest::testStep()
{
  int testDelay = 0, failSafe=44, i=0;
  coord testCoord(-1,-1), expectedCoord(-1,-1), originCoord(4,20);
  Field testField;
  std::vector<Piece> testPieces;
  std::vector<Piece>::iterator itor;
  testPieces.reserve(6); 
  testPieces.push_back(Piece(I,&testDelay,&testField));
  testPieces.push_back(Piece(J,&testDelay,&testField));
  testPieces.push_back(Piece(L,&testDelay,&testField));
  testPieces.push_back(Piece(S,&testDelay,&testField));
  testPieces.push_back(Piece(Z,&testDelay,&testField));
  testPieces.push_back(Piece(T,&testDelay,&testField));

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

  CPPUNIT_FAIL( "Test not complete." );
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
