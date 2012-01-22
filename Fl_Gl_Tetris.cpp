#include "Fl_Gl_Tetris.hpp"


#define WBUFF swapped ? 1:0;
#define RBUFF swapped ? 0:1;

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
  squareVBO(0),squareTexID(0),gmod(0),cmod(0),
  mBuff(),mCB(&mBuff,&DataDoubleBuffer::write),mGame(&mCB)
{
  glGenBuffers(1,&squareVBO);
  glGenTextures(1,&squareTexID);
  // Set up Buffer Object
  glBindBuffer(GL_ARRAY_BUFFER,squareVBO);
  constexpr GLsizeiptr sqSize = sizeof(GLfloat)*4*(3+2);
  constexpr GLfloat square[4*(3+2)]=
    {// Space+texture co-ordinates, Specified in clockwise order
       0.5f, 0.5f,0.0f,
       1.0f, 1.0f,
       0.5f,-0.5f,0.0f,
       1.0f,-1.0f,
      -0.5f,-0.5f,0.0f,
      -1.0f,-1.0f,
      -0.5f, 0.5f,0.0f,
      -1.0f, 1.0f
    };
  glBufferData(GL_ARRAY_BUFFER,sqSize,square,GL_STATIC_DRAW);
  // Set up texture
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
  glBindTexture(GL_TEXTURE_2D,squareTexID);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
	      TEXDIMENSION,TEXDIMENSION,0,
	      GL_BGRA,GL_FLOAT,
	      texdata);
  delete[] texdata;
}
Fl_Gl_Tetris::~Fl_Gl_Tetris()
{
  glDeleteBuffers(1,&squareVBO);
  glDeleteTextures(1,&squareTexID);
}

// Callbacks named in tetris_fltkgui.fl
void Fl_Gl_Tetris::setGridMode(char mode)
{
  gmod=(bool)mode;

}
void Fl_Gl_Tetris::setColorMode(char mode)
{
  cmod=(bool)mode;

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
