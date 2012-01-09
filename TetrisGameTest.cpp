#include "TetrisGameTest.hpp"
#include "TetrisGame.hpp"
#include "config.h"
#ifndef HAVE_STDCXX_0X
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
  RenderFunc<renderfuncptr> dummyRenderFunctor(dummyRenderCallback);

  // Check default constructor doesn't explode.
  mptr = new TetrisGame();
  delete mptr;
  // Check callback constructor doesn't explode.
  mptr = new TetrisGame(&dummyRenderFunctor);
  delete mptr;
  mptr=nullptr;
  // Check automatic storage
  {
    TetrisGame stackTetrisGame();
  }
  {
    TetrisGame stackTetrisGame(&dummyRenderFunctor);
  }
}
void TetrisGameTest::testRunCallback()
{
  // STUB
  CPPUNIT_FAIL("NYI");
}
void TetrisGameTest::testExceptions()
{
  // STUB
  CPPUNIT_FAIL("NYI");
}

void TetrisGameTest::dummyRenderCallback(const Field &afield, const Piece & curr, 
					 const Piece * ghost)
{
  ++dummyCount;
}

int TetrisGameTest::dummyCount = 0;
