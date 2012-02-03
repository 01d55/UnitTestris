#include "Fl_Gl_Tetris.hpp"
#include "glutil.hpp"
#include "GLMatrix.h"

#include <stack>

#define WBUFF swapped ? 1:0;
#define RBUFF swapped ? 0:1;

static const float delay=1/60.0f;

void redraw_cb(void *vthis)
{
  ((Fl_Gl_Tetris*)vthis)->redraw();
  Fl::lock();
  Fl::repeat_timeout(delay,redraw_cb,vthis);
  Fl::unlock();
}

struct BGRA
{
  union
  {
    struct
    {
      GLfloat b,g,r,a;
    };
    GLfloat mBGRA[4];
  };
  BGRA():b(1.0f),g(1.0f),r(1.0f),a(1.0f)
  {
  }

};
// Ensure the compiler isn't padding BGRA
static_assert(sizeof(BGRA) == 4*sizeof(GLfloat),"Struct BGRA is the wrong size!");
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
  dirty=true;
}

const DataBuffer& DataDoubleBuffer::swap_and_read()
{
  swap_guard.lock();
  if(dirty)
    {
      swapped = !swapped;
      dirty=false;
    }
  swap_guard.unlock();
  auto idx = RBUFF;
  return buffers[idx];
}

// Fl_Gl_Tetris

Fl_Gl_Tetris::Fl_Gl_Tetris( int x,int y,int w,int h, const char *l):
  Fl_Gl_Window(x,y,w,h,l),
  gmod(false),cmod(true),running(false),
  squareVBO(0),squareTexID(0),squareIBO(0),VAO(0),
  shaderProgram(0),vertexShader(0),fragShader(0),
  projectionUniform(-1),modelviewUniform(-1),tintUniform(-1),
  mBuff(),mCB(this,&Fl_Gl_Tetris::write_and_redraw),mGame(&mCB)
{
}
Fl_Gl_Tetris::~Fl_Gl_Tetris()
{
  if(context_valid())
    {
      glBindVertexArray(0);
      glDeleteVertexArrays(1,&VAO);

      glBindBuffer(GL_ARRAY_BUFFER,0);
      glDeleteBuffers(1,&squareVBO);

      glBindTexture(GL_TEXTURE_2D,0);
      glDeleteTextures(1,&squareTexID);
      // Shader cleanup

      glUseProgram(0);

      glDetachShader(shaderProgram, vertexShader);
      glDetachShader(shaderProgram, fragShader);
      
      glDeleteShader(vertexShader);
      glDeleteShader(fragShader);
      
      glDeleteProgram(shaderProgram);
    }
  
}

// Callbacks named in tetris_fltkgui.fl
void Fl_Gl_Tetris::setGridMode(char mode)
{
  gmod=(bool)mode;
  redraw();
}
void Fl_Gl_Tetris::setColorMode(char mode)
{
  cmod=(bool)mode;
  redraw();
}
void Fl_Gl_Tetris::startTetris()
{
  if(running)
    {
      mGame.pause();
      Fl::lock();
      Fl::remove_timeout(redraw_cb,this);
      Fl::unlock();
    }
  else
    {
      mGame.run();
      Fl::lock();
      Fl::add_timeout(delay,redraw_cb,this);
      Fl::unlock();
    }
  running = !running;
  redraw();
}
void Fl_Gl_Tetris::reset()
{
  mGame.~TetrisGame();
  new(&mGame) TetrisGame(&mCB);
  
  mBuff.~DataDoubleBuffer();
  new(&mBuff) DataDoubleBuffer();

  running = false;

  Fl::lock();
  Fl::remove_timeout(redraw_cb,this);
  Fl::unlock();

  redraw();
}

// fltk event handler
int Fl_Gl_Tetris::handle(int e)
{
  constexpr int FL_SPACEBAR=32;
  int ret=0;


  // Handle keyboard events if running
  if(running && FL_SHORTCUT==e)
    {
      if(Fl::event_key('x'))
	{
	  mGame.queueInput(hard_drop);
	  ret=1;
	}
      if(Fl::event_key('q'))
	{
	  mGame.queueInput(rotate_ccw);
	  ret=1;
	}
      if(Fl::event_key('a'))
	{
	  mGame.queueInput(shift_left);
	  ret=1;
	}
      if(Fl::event_key('e'))
	{
	  mGame.queueInput(rotate_cw);
	  ret=1;
	}
      if(Fl::event_key('d'))
	{
	  mGame.queueInput(shift_right);
	  ret=1;
	}
    }
  
  redraw();

  return ret;
}

// Protected functions

void Fl_Gl_Tetris::draw()
{
  static GLFMatrix4x4 Projection,tScale;
  static int width,height;
  constexpr float near=-1.0f,far=1.0f;
  const glVec WHITE_COLOR(1.0f,1.0f,1.0f,1.0f);
  const glVec I_COLOR(1.0f,0.0f,0.0f,1.0f);
  const glVec J_COLOR(0.0f,1.0f,0.0f,1.0f);
  const glVec L_COLOR(0.0f,0.0f,1.0f,1.0f);
  const glVec O_COLOR(1.0f,1.0f,0.0f,1.0f);
  const glVec S_COLOR(1.0f,0.0f,1.0f,1.0f);
  const glVec T_COLOR(0.0f,1.0f,1.0f,1.0f);
  const glVec Z_COLOR(0.5f,1.0f,0.5f,1.0f);
  if(!context_valid())
    {
      initGL();
    }
  if(!valid())
    {
      width=w();
      height=h();
      // Set up viewport
      glViewport(0,0,width,height);
      // Set up projection (Ortho)
      Projection=makeOrtho(-width/2.0f,width/2.0f,
			   -height/2.0f,height/2.0f,
			   near,far);
      tScale=makeScaleMatrix( glVec(width/10,height/20,1.0f,0) );
    }
  //std::cout << "Beginning draw\n";
  //printGlError();
  const DataBuffer & gameState=mBuff.swap_and_read();

  std::stack<GLFMatrix4x4> ModelviewStack;
  GLFMatrix4x4 Modelview;

  Modelview=tScale;
  Modelview=Modelview*makeTranslationMatrix( glVec(-4.5f,-9.5f,0,0) );
  ModelviewStack.push(Modelview);

  
  // Begin GL operations
  glClear(GL_COLOR_BUFFER_BIT);
  glUniformMatrix4fv(projectionUniform,1,GL_FALSE,Projection.data);

  // Draw a grey square for each block in the field which is set.
  glUniform4fv(tintUniform, 1, WHITE_COLOR.data); // The texture makes this grey enough
  //std::cout << "Entering field loop\n";
  //printGlError();
  // Draw blocks set in field
  for(unsigned i=0;i<FIELD_WIDTH;++i)
    {
      for(unsigned j=0;j<FIELD_HEIGHT;++j)
	{
	  if(gameState.field.get(i,j))
	    {
	      Modelview=ModelviewStack.top();
	      Modelview=Modelview*makeTranslationMatrix( glVec(i,j,0.0f,0.0f) );
	      glUniformMatrix4fv(modelviewUniform,1,GL_FALSE,Modelview.data);
	      glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
	    }
	}
    }


  //std::cout << "Entering current piece loop\n";
  //printGlError();
  // Draw current piece
  arrayt blocks=gameState.current.getBlocks();

  if(cmod)
    {
      switch(gameState.current.getType())
	{
	case I:
	  glUniform4fv(tintUniform, 1, I_COLOR.data);
	  break;
	case J:
	  glUniform4fv(tintUniform, 1, J_COLOR.data);
	  break;
	case L:
	  glUniform4fv(tintUniform, 1, L_COLOR.data);
	  break;
	case O:
	  glUniform4fv(tintUniform, 1, O_COLOR.data);
	  break;
	case S:
	  glUniform4fv(tintUniform, 1, S_COLOR.data);
	  break;
	case T:
	  glUniform4fv(tintUniform, 1, T_COLOR.data);
	  break;
	case Z:
	  glUniform4fv(tintUniform, 1, Z_COLOR.data);
	  break;
	default:
	  std::cerr << "Unhandled case in enumerated switch statement!\n";
	  break;
	}
    }

  for(auto block : blocks)
    {
      Modelview=ModelviewStack.top();
      Modelview=Modelview*makeTranslationMatrix( glVec(block.x,block.y,0,0) );
      glUniformMatrix4fv(modelviewUniform,1,GL_FALSE,Modelview.data);
      glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
    }
  //std::cout << "Drawing test square\n";
  //printGlError();
  /* Test triangle
  Modelview=ModelviewStack.top();
  Modelview=Modelview*makeTranslationMatrix( glVec(1.0f,19.0f,0.0f,0.0f) );

  Modelview.debugDump();
  Projection.debugDump();

  glUniform4f(tintUniform, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniformMatrix4fv(modelviewUniform,1,GL_FALSE,Modelview.data);
  glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_SHORT,0);
  /**/
  //std::cout << "draw() finished\n";
  printGlError(); // Keep this to tell us if we need to go fishing.
}

// Private functions

void Fl_Gl_Tetris::initGL()
{
  GLint retcode=-1;

  glGetIntegerv(GL_MAJOR_VERSION,&retcode);  
  std::cout << "GL version: " << retcode << '.';
  glGetIntegerv(GL_MINOR_VERSION,&retcode);
  std::cout << retcode << '\n';

  // Set up some global state
  glDisable(GL_CULL_FACE);
  //glDisable(GL_DEPTH_TEST);
  //glDisable(GL_SCISSOR_TEST);


  // Set up VAO
  glGenVertexArrays(1,&VAO);
  glBindVertexArray(VAO);

  // Set up Buffer Objects
  glGenBuffers(1,&squareVBO);
  glBindBuffer(GL_ARRAY_BUFFER,squareVBO);

  constexpr GLsizei STRIDE=sizeof(GLfloat)*(3+2);
  constexpr GLsizeiptr sqSize = STRIDE*4;
  constexpr GLfloat square[4*(3+2)]=
    {// Space+texture co-ordinates, Specified in clockwise order
      0.5f, 0.5f,0.0f,
      1.0f, 1.0f,
      0.5f,-0.5f,0.0f,
      1.0f,-0.0f,
      -0.5f,-0.5f,0.0f,
      -0.0f,-0.0f,
      -0.5f, 0.5f,0.0f,
      -0.0f, 1.0f
    };
  constexpr GLvoid* TEX0_OFFSET=BUFFER_OFFSET(sizeof(float)*3);
  glBufferData(GL_ARRAY_BUFFER,sqSize,square,GL_STATIC_DRAW);

  glGenBuffers(1,&squareIBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,squareIBO);

  constexpr GLushort squareIDX[3*2]=
    { // Triangle indices
      0,3,1,
      2,1,3
    };
  constexpr GLsizeiptr squareIDXsize=sizeof(squareIDX);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,squareIDXsize,squareIDX,GL_STATIC_DRAW);


  // Set up texture
  glGenTextures(1,&squareTexID);
  glBindTexture(GL_TEXTURE_2D,squareTexID);
  // To avoid dealing with files, generate a simple texture.
  constexpr unsigned TEXDIMENSION=256,TEXBUFFSIZE=TEXDIMENSION*TEXDIMENSION;
  BGRA *texdata=new BGRA[TEXBUFFSIZE];
  for(unsigned i=0;i<TEXDIMENSION;++i)
    {
      for(unsigned j=0;j<TEXDIMENSION;++j)
	{
	  unsigned idist=std::min(TEXDIMENSION-i,i);
	  unsigned jdist=std::min(TEXDIMENSION-j,j);
	  unsigned dist=std::min(idist,jdist);
	  GLfloat value=((GLfloat)dist)/TEXDIMENSION;
	  texdata[i*TEXDIMENSION+j].r=
	    texdata[i*TEXDIMENSION+j].g=
	    texdata[i*TEXDIMENSION+j].b=value;
	}
    }
  // Copy the generated texure into GL controlled memory
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
	       TEXDIMENSION,TEXDIMENSION,0,
	       GL_BGRA,GL_FLOAT,
	       texdata);
  delete[] texdata;
  // Default filtering requires mipmaps
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  // Set up shader program
  char constexpr VERT_SHADER_PATH[]="shaders/default.vert";
  char constexpr FRAG_SHADER_PATH[]="shaders/default.frag";
  retcode=LoadShader(VERT_SHADER_PATH,FRAG_SHADER_PATH,
		     true,false,false,
		     shaderProgram,vertexShader,fragShader);
  if(-1==retcode)
    {
      throw std::runtime_error("Shader loading failed.");
    }
  glUseProgram(shaderProgram);

  GLchar constexpr PROJECTION_UNIFORM_NAME[]="Projection";
  GLchar constexpr MODELVIEW_UNIFORM_NAME[]="Modelview";
  GLchar constexpr TINT_UNIFORM_NAME[]="tint";
  GLchar constexpr SAMPLER_UNIFORM_NAME[]="msampler";
  projectionUniform=glGetUniformLocation(shaderProgram,PROJECTION_UNIFORM_NAME);
  modelviewUniform=glGetUniformLocation(shaderProgram,MODELVIEW_UNIFORM_NAME);
  tintUniform=glGetUniformLocation(shaderProgram,TINT_UNIFORM_NAME);
  // We don't need to keep the samplerUniform after init - for now.
  GLint samplerUniform=glGetUniformLocation(shaderProgram,SAMPLER_UNIFORM_NAME);


  if(-1==projectionUniform || -1 == modelviewUniform ||
     -1 == tintUniform || -1 == samplerUniform)
    {
      std::cerr << projectionUniform << modelviewUniform << tintUniform << samplerUniform << '\n';
      throw std::runtime_error("Uniform name loading failed.");
    }
  glUniform1i(samplerUniform,0);
  // Rebind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,squareTexID);


  glVertexAttribPointer(INVERTEX_ATTRIB_LOC,
			3, // size
			GL_FLOAT,
			GL_FALSE,
			STRIDE,
			BUFFER_OFFSET(0)); // offset
  glEnableVertexAttribArray(INVERTEX_ATTRIB_LOC);

  glVertexAttribPointer(INTEXCOORD0_ATTRIB_LOC,
			2, // size
			GL_FLOAT,
			GL_FALSE,
			STRIDE,
			TEX0_OFFSET); // offset
  glEnableVertexAttribArray(INTEXCOORD0_ATTRIB_LOC);

  std::cerr << glGetError() << '\n';
}
