#ifndef FL_GL_TETRIS_HPP
#define FL_GL_TETRIS_HPP

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

class Fl_Gl_Tetris: public Fl_Gl_Window
{
public:
  
  Fl_Gl_Tetris(int x,int y,int h ,int w, const char *l=0);
  virtual ~Fl_Gl_Tetris() throw();
  
  // Callbacks named in tetris_fltkgui.fl
  void setGridMode(char mode);
  void setColorMode(char mode);
  void startTetris();
  void reset();
protected:
private:
  
};


#endif // FL_GL_TETRIS_HPP
