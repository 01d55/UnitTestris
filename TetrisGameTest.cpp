#include "TetrisGameTest.hpp"
#include "TetrisGame.hpp"
// Registers the fixture
CPPUNIT_TEST_SUITE_REGISTRATION( TetrisGameTest );

#include "config.h"
#ifndef HAVE_STDCXX_0X
#define nullptr 0
#endif

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
  RenderFunc<renderfuncptr> dummyRenderFunctor(dummyRenderCallback);
  mptr = new TetrisGame();
  // Set callback outside of constructor
  CPPUNIT_ASSERT_NO_THROW(mptr->setRenderer(&dummyRenderFunctor));
  // Test normal run/stop

  CPPUNIT_ASSERT_NO_THROW(mptr->run());
  
  CPPUNIT_ASSERT_NO_THROW(mptr->pause());
  CPPUNIT_ASSERT(0 < dummyCount);
  // Test that the destructor stops the internal thread.

  CPPUNIT_ASSERT_NO_THROW(mptr->run());

  CPPUNIT_ASSERT_NO_THROW(delete mptr);

  mptr=0;
  // STUB
  CPPUNIT_FAIL("NYI");

}

void TetrisGameTest::testExceptions()
{
  RenderFunc<renderfuncptr> dummyRenderFunctor(dummyRenderCallback);
  TetrisGame game;

  CPPUNIT_ASSERT_THROW(game.queueInput(shift_right) , GameNotRunningError);
  CPPUNIT_ASSERT_THROW(game.pause() , GameNotRunningError);

  game.run();

  CPPUNIT_ASSERT_THROW(game.setRenderer(&dummyRenderFunctor) , GameRunningError);
  CPPUNIT_ASSERT_THROW(game.run() , GameRunningError);
  CPPUNIT_FAIL("NYI");
}

void TetrisGameTest::dummyRenderCallback(const Field &afield, const Piece & curr, 
					 const Piece * ghost)
{
#ifdef HAVE_ATOMIC
  ++dummyCount;
#else // HAVE_ATOMIC
#error Alternative to <atomic> NYI
#endif // HAVE_ATOMIC
}
#ifdef HAVE_ATOMIC
std::atomic_int TetrisGameTest::dummyCount = ATOMIC_VAR_INIT(0);
#else // HAVE_ATOMIC
int TetrisGameTest::dummyCount = 0;
#endif // HAVE_ATOMIC
