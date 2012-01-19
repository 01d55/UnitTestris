#include "Fl_Gl_Tetris.hpp"

#define WBUFF swapped ? 1:0;
#define RBUFF swapped ? 0:1;

// DataDoubleBuffer

void DataDoubleBuffer::write(const Field &field_, const Piece &cur, const Piece *holdp)
{
  std::lock_guard<std::mutex> lg(swap_guard);
  auto idx = WBUFF;

  buffers[idx].field=field_;
  buffers[idx].current=cur;
  if(nullptr!=holdp)
    {
      buffers[idx].hold=*holdp;
      buffers[idx].holding=true;
    }
  else
    {
      buffers[idx].holding=false;
    }
}

const DataBuffer& DataDoubleBuffer::swap_and_read()
{
  swap_guard.lock();
  swapped = !swapped;
  swap_guard.unlock();
  auto idx = RBUFF;
  return buffers[idx];
}

// Fl_Gl_Tetris

Fl_Gl_Tetris::Fl_Gl_Tetris( int x,int y,int w,int h, const char *l):
  Fl_Gl_Window(x,y,w,h,l),
  gmod(0),cmod(0),
  mBuff(),mCB(&mBuff,&DataDoubleBuffer::write),mGame(&mCB)
{
}
Fl_Gl_Tetris::~Fl_Gl_Tetris()
{
}

// Callbacks named in tetris_fltkgui.fl
void Fl_Gl_Tetris::setGridMode(char mode)
{
  gmod=mode;
}
void Fl_Gl_Tetris::setColorMode(char mode)
{
  cmod=mode;
}
void Fl_Gl_Tetris::startTetris()
{
  // STUB
}
void Fl_Gl_Tetris::reset()
{
  // STUB
}

//
void Fl_Gl_Tetris::draw()
{
  // STUB
}
