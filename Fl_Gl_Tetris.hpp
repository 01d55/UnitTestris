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
  bool swapped;
  std::mutex swap_guard;
public:
  DataDoubleBuffer():swapped(false),swap_guard()
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

  //
  void draw();
protected:
private:
  bool gmod,cmod,mGLready;
  GLuint squareVBO,squareTexID,
    shaderProgram,vertexShader,fragShader;

  DataDoubleBuffer mBuff;
  RenderFunc<DataDoubleBuffer> mCB;
  TetrisGame mGame;

  void initGL();

};


#endif // FL_GL_TETRIS_HPP
