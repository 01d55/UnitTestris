#include "TetrisGameTest.hpp"
#include "TetrisGame.hpp"
#include "PieceDummy.hpp"
#include "Field.hpp"
#include "Piece.hpp"
// Registers the fixture
CPPUNIT_TEST_SUITE_REGISTRATION( TetrisGameTest );

#include "compat.h"
#ifdef HAVE_STDCXX_0X
#include <algorithm>
#else // HAVE_STDCXX_0X
#define nullptr 0
#define constexpr const
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
  PieceDummy::reset_all();
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
  const int frames = std::chrono::duration_cast<duration_frames>(t1-t0).count();
  //std::cerr << dummyCount << " " << frames;
  const int difference = frames-dummyCount;
  CPPUNIT_ASSERT(2 > difference && -2 < difference);
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
#else // HAVE_STDCXX_SYNCH
#error NYI
#endif // HAVE_STDCXX_SYNCH

  CPPUNIT_ASSERT( 0 == dummyCount );

}

void TetrisGameTest::whitebox_testRunCallback()
{
  TetrisGame game;
  game.run();
#ifdef HAVE_STDCXX_SYNCH
  std::this_thread::sleep_for(std::chrono::seconds(1));
#else // HAVE_STDCXX_SYNCH
#error NYI
#endif // HAVE_STDCXX_SYNCH
  game.pause();

  CPPUNIT_ASSERT(0 < PieceDummy::count_timeStep());
}

void TetrisGameTest::whitebox_testInput()
{
  TetrisGame game;
  constexpr PieceInput cinputs[5]={shift_right,shift_left,rotate_cw,rotate_ccw,hard_drop};
  std::vector<PieceInput> inputs(cinputs,cinputs+5);

  game.run();

  std::for_each(inputs.begin(),inputs.end(),
		[&game](PieceInput i)
		{
		  game.queueInput(i);
		});
#ifdef HAVE_STDCXX_SYNCH
  std::this_thread::sleep_for(duration_frames(2));
#else // HAVE_STDCXX_SYNCH
#error NYI
#endif // HAVE_STD_CXX_SYNCH
  game.pause();

  CPPUNIT_ASSERT( PieceDummy::compare_handleInput_arg(inputs) );
  
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
}

void TetrisGameTest::dummyRenderCallback(const Field &afield, const Piece & curr, 
					 const Piece * ghost)
{
  dummyCount=dummyCount+1;
}

#ifdef HAVE_STDCXX_SYNCH
std::atomic_int TetrisGameTest::dummyCount = ATOMIC_VAR_INIT(0);
#else // HAVE_STDCXX_SYNCH
int TetrisGameTest::dummyCount = 0;
#endif // HAVE_STDCXX_SYNCH
