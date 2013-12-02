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
#include "DataDoubleBuffer.hpp"



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
  RenderFunc<DataDoubleBuffer> mCB;
  TetrisGame mGame;

  // Throws std::runtime_error if shader loading fails.
  void initGL();

};


#endif // FL_GL_TETRIS_HPP
