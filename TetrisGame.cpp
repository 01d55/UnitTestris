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
  const TetrisGame& parent;
  // Tetris members
  static constexpr unsigned int gravity=1,lockdelay=30;
  Field mField;
  Piece current, ghost;

  // Threading members
  std::atomic_bool is_paused,is_continuing;
  std::mutex pause_mutex,cb_mutex;
  std::unique_lock<std::mutex> pause_lock;
  std::condition_variable pause_condition;
  std::thread runner;

  TetrisGame_impl(const TetrisGame &parent_):parent(parent_),
					     mField(),current(I,lockdelay,&mField),
					     ghost(I,lockdelay,&mField),
					     is_paused(true),is_continuing(false),
					     pause_mutex(),cb_mutex(),
					     pause_lock(pause_mutex),
					     pause_condition(),
					     runner()
  {
    pause_lock.unlock();
  }

  ~TetrisGame_impl()
  {
    if(runner.joinable())
      {
	is_continuing=false;
	is_paused=false;
	pause_condition.notify_all();
	runner.join();
      }
  }


  void threadFunc()
  {
    
    pause_lock.lock();
    while(is_continuing)
      {
	while (is_paused)
	  {
	    pause_condition.wait(pause_lock);
	  }
	auto t0=g_clock::now();
	cb_mutex.lock();
	if(nullptr!=parent.cb)
	  {
	    (*parent.cb)(mField,current,nullptr);
	  }
	cb_mutex.unlock();

	while(std::chrono::duration_cast<sleep_time>(g_clock::now()-t0).count() < frame_err)
	  {
	    std::this_thread::sleep_for(sleep_time(1));
	  }
      }
    
  }

  void run()
  {
    is_paused=false;
    if(!runner.joinable()) // Thread not yet initialized
      {
	is_continuing=true;
	runner = std::thread( &TetrisGame_impl::threadFunc, this);
      }
    
    pause_condition.notify_one();
  }
    
};

TetrisGame::TetrisGame(): cb(nullptr),me(new TetrisGame_impl(*this))
{
}
TetrisGame::TetrisGame(IRenderFunc *callback):cb(callback),me(new TetrisGame_impl(*this))
{
}

TetrisGame::~TetrisGame()
{
  me->is_paused=true;
  me->cb_mutex.lock();
  cb=nullptr;
  me->cb_mutex.unlock();
}

// Start the thread.
void TetrisGame::run()
{
  if(!me->is_paused)
    {
      throw GameRunningError();
    }
  me->run();
}
// Stop the thread, preserving current state.
void TetrisGame::pause()
{
  if(me->is_paused)
    {
      throw GameNotRunningError();
    }
  me->is_paused=true;
}
// Render callback. Calling during run is an error.
void TetrisGame::setRenderer(IRenderFunc *callback)
{
  if(!me->is_paused)
    {
      throw GameRunningError();
    }

  me->cb_mutex.lock();
  cb=callback;
  me->cb_mutex.unlock();
}
// Control functions. May be called asyncronously.
void TetrisGame::queueInput(PieceInput)
{
  if(me->is_paused)
    {
      throw GameNotRunningError();
    }

  
}
