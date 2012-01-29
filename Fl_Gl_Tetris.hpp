#ifndef FL_GL_TETRIS_HPP
#define FL_GL_TETRIS_HPP

#include <mutex>

// Get full openGL API from FL/gl.h
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif

#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/Fl_Gl_Window.H>
#include "TetrisGame.hpp"
#include "Field.hpp"
#include "Piece.hpp"

struct DataBuffer
{
  Field field;
  Piece current,hold;
  bool holding;
  DataBuffer():current(I,0,nullptr),hold(I,0,nullptr),holding(false)
  {}
};
class DataDoubleBuffer
{
  DataBuffer buffers[2];
  bool swapped,dirty;
  std::mutex swap_guard;
public:
  DataDoubleBuffer():swapped(false),dirty(false),swap_guard()
  {}

  void write(const Field &, const Piece &, const Piece *);
  const DataBuffer& swap_and_read();
};

class Fl_Gl_Tetris: public Fl_Gl_Window
{
public:
  
  Fl_Gl_Tetris(int x,int y,int h ,int w, const char *l=0);
  virtual ~Fl_Gl_Tetris();
  
  // Callbacks named in tetris_fltkgui.fl
  void setGridMode(char mode);
  void setColorMode(char mode);
  void startTetris();
  void reset();
  // fltk event handler
  int handle(int);

protected:
  // The first call to draw will call initGL, which may throw.
  virtual void draw();

private:
  bool gmod,cmod,running;
  GLuint squareVBO,squareTexID,squareIBO,VAO,
    shaderProgram,vertexShader,fragShader;
  GLint projectionUniform,modelviewUniform,tintUniform;

  DataDoubleBuffer mBuff;
  RenderFunc<Fl_Gl_Tetris> mCB;
  TetrisGame mGame;

  // Throws std::runtime_error if shader loading fails.
  void initGL();

  void write_and_redraw(const Field &foo, const Piece &bar, const Piece *baz)
  {
    mBuff.write(foo,bar,baz);
    Fl::lock();
    redraw();
    Fl::unlock();
  }
};


#endif // FL_GL_TETRIS_HPP
