#include "TetrisGameTest.hpp"
#include "TetrisGame.hpp"
// Registers the fixture
CPPUNIT_TEST_SUITE_REGISTRATION( TetrisGameTest );

#include "config.h"
#ifndef HAVE_STDCXX_0X
#define nullptr 0
#endif // HAVE_STDCXX_0X

#ifdef HAVE_STDCXX_SYNCH
#include <chrono>
#include <thread>
typedef std::chrono::system_clock mclock;
typedef std::chrono::duration<mclock::rep,std::ratio<1,60>> duration_frames;
#endif //HAVE_STDCXX_SYNCH

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

#ifdef HAVE_STDCXX_SYNCH
  auto t0 = mclock::now();
#endif //HAVE_STDCXX_SYNCH

  CPPUNIT_ASSERT_NO_THROW(mptr->run());

#ifdef HAVE_STDCXX_SYNCH
  std::this_thread::sleep_for(std::chrono::seconds(1));
#endif //HAVE_STDCXX_SYNCH

  CPPUNIT_ASSERT_NO_THROW(mptr->pause());

#ifdef HAVE_STDCXX_SYNCH
  auto t1 = mclock::now();
  int frames = std::chrono::duration_cast<duration_frames>(t1-t0).count();
  CPPUNIT_ASSERT(0 == frames-dummyCount);
#else // HAVE_STDCXX_SYNCH
  CPPUNIT_ASSERT(0 < dummyCount);
#endif // HAVE_STDCXX_SYNCH

  // Test that the destructor stops the internal thread.

  CPPUNIT_ASSERT_NO_THROW(mptr->run());

  CPPUNIT_ASSERT_NO_THROW(delete mptr);

  mptr=0;

  dummyCount=0;

#ifdef HAVE_STDCXX_SYNCH
  std::this_thread::sleep_for(std::chrono::seconds(1));
#endif // HAVE_STDCXX_SYNCH

  CPPUNIT_ASSERT( 0 == dummyCount );


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
  ++dummyCount;
}

#ifdef HAVE_STDCXX_SYNCH
std::atomic_int TetrisGameTest::dummyCount = ATOMIC_VAR_INIT(0);
#else // HAVE_STDCXX_SYNCH
int TetrisGameTest::dummyCount = 0;
#endif // HAVE_STDCXX_SYNCH
