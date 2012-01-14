#include "TetrisGame.hpp"
#include "Piece.hpp"
#include "config.h"

#ifdef HAVE_STDCXX_SYNCH
#include <atomic>
#include <chrono>
#include <thread>
#include <condition_variable>


typedef std::chrono::system_clock g_clock;
constexpr unsigned frame_err=32;
constexpr unsigned FPS=60;
typedef std::chrono::duration<g_clock::rep,std::ratio<1,FPS*frame_err>> sleep_time;
#else // HAVE_STDCXX_SYNCH
#error NYI
#endif // HAVE_STDCXX_SYNCH

#ifndef HAVE_STDCXX_0x
#define nullptr 0
#define constexpr const
#endif// HAVE_STDCXX_0X




struct TetrisGame_impl
{
  IRenderFunc *cb;
  // Tetris members
  static constexpr unsigned int gravity=1,lockdelay=30,minBuffer=8;
  Field mField;
  Piece current, ghost;
  std::vector<PieceInput> inputBuffer;

  // Threading members
  std::atomic_bool isPaused,isContinuing;
  std::mutex pauseMutex,cbMutex,inputMutex;
  std::unique_lock<std::mutex> pauseLock;
  std::condition_variable pauseCondition;
  std::thread runner;

  TetrisGame_impl(IRenderFunc *cb_):cb(cb_),
				    mField(),current(I,lockdelay,&mField),
				    ghost(I,lockdelay,&mField),inputBuffer(),
				    isPaused(true),isContinuing(false),
				    pauseMutex(),cbMutex(),
				    pauseLock(pauseMutex),
				    pauseCondition(),
				    runner()
  {
    pauseLock.unlock();
    inputBuffer.reserve(minBuffer);
  }

  ~TetrisGame_impl()
  {
    if(runner.joinable())
      {
	cbMutex.lock();
	cb=nullptr;
	cbMutex.unlock();
	isContinuing=false;
	isPaused=false;
	pauseCondition.notify_all();
	runner.join();
      }
  }


  void threadFunc()
  {
    
    pauseLock.lock();
    while(isContinuing)
      {
	while (isPaused)
	  {
	    pauseCondition.wait(pauseLock);
	  }
	auto t0=g_clock::now();
	cbMutex.lock();
	if(nullptr!=cb)
	  {
	    (*cb)(mField,current,nullptr);
	  }
	cbMutex.unlock();

	consumeInput();


	while(std::chrono::duration_cast<sleep_time>(g_clock::now()-t0).count() < frame_err)
	  {
	    std::this_thread::sleep_for(sleep_time(1));
	  }
      }
    
  }

  void consumeInput()
  {
    inputMutex.lock();
    std::vector<PieceInput> input(std::move(inputBuffer));
    inputBuffer.clear();
    inputBuffer.reserve(minBuffer);
    inputMutex.unlock();
    std::for_each(input.begin(),input.end(),
		  [&current](PieceInput i)
		  {
		    current.handleInput(i);
		  });
  }

  void run()
  {
    isPaused=false;
    if(!runner.joinable()) // Thread not yet initialized
      {
	isContinuing=true;
	runner = std::thread( &TetrisGame_impl::threadFunc, this);
      }
    
    pauseCondition.notify_one();
  }
    
};

TetrisGame::TetrisGame(): me(new TetrisGame_impl(nullptr))
{
}
TetrisGame::TetrisGame(IRenderFunc *callback):me(new TetrisGame_impl(callback))
{
}

TetrisGame::~TetrisGame()
{

}

// Start the thread.
void TetrisGame::run()
{
  if(!me->isPaused)
    {
      throw GameRunningError();
    }
  me->run();
}
// Stop the thread, preserving current state.
void TetrisGame::pause()
{
  if(me->isPaused)
    {
      throw GameNotRunningError();
    }
  me->isPaused=true;
}
// Render callback. Calling during run is an error.
void TetrisGame::setRenderer(IRenderFunc *callback)
{
  if(!me->isPaused)
    {
      throw GameRunningError();
    }

  me->cbMutex.lock();
  me->cb=callback;
  me->cbMutex.unlock();
}
// Control functions. May be called asyncronously.
void TetrisGame::queueInput(PieceInput in)
{
  if(me->isPaused)
    {
      throw GameNotRunningError();
    }
  me->inputMutex.lock();
  me->inputBuffer.push_back(in);
  me->inputMutex.unlock();
}
