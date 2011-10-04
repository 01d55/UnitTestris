#include "TetrisGameTest.hpp"
#include "TetrisGame.hpp"
#include "config.h"
#ifndef HAVE_STDCXX_11_NULLPTR
#define nullptr 0
#endif
// Registers the fixture
CPPUNIT_TEST_SUITE_REGISTRATION( TetrisGameTest );

void TetrisGameTest::setUp()
{
  TetrisGameTest::dummyCount = 0;
}
void TetrisGameTest::tearDown()
{
  if(mptr)
    {
      delete mptr;
      mptr=nullptr;
    }
}

void TetrisGameTest::testNewDelete()
{
  // Check default constructor doesn't explode.
  mptr = new TetrisGame();
  delete mptr;
  // Check callback constructor doesn't explode.
  mptr = new TetrisGame(dummyRenderCallback);
  delete mptr;
  mptr=nullptr;
  // Check stack allocation & destruction
  {
    TetrisGame stackTetrisGame();
  }
  {
    TetrisGame stackTetrisGame(dummyRenderCallback);
  }
}
void TetrisGameTest::testRunCallback()
{
  // STUB
}
void TetrisGameTest::testExceptions()
{
  // STUB
}

void TetrisGameTest::dummyRenderCallback(const Field &afield, const Piece & curr, 
					 const Piece * ghost)
{
  ++dummyCount;
}

int TetrisGameTest::dummyCount = 0;
