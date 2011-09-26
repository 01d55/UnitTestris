#include "Fl_Gl_Tetris.hpp"
 

Fl_Gl_Tetris::Fl_Gl_Tetris( int x,int y,int w,int h, const char *l):
  Fl_Gl_Window(x,y,w,h,l)
{
}
Fl_Gl_Tetris::~Fl_Gl_Tetris() throw()
{
}

// Callbacks named in tetris_fltkgui.fl
void Fl_Gl_Tetris::setGridMode(char mode)
{
  // STUB
}
void Fl_Gl_Tetris::setColorMode(char mode)
{
  // STUB
}
void Fl_Gl_Tetris::startTetris()
{
  // STUB
}
void Fl_Gl_Tetris::reset()
{
  // STUB
}
