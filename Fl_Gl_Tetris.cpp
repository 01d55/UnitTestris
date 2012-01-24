#include "Fl_Gl_Tetris.hpp"
#include "glutil.hpp"

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
  gmod(false),cmod(false),
  squareVBO(0),squareTexID(0),squareIBO(0),VAO(0),
  shaderProgram(0),vertexShader(0),fragShader(0),
  projectionUniform(-1),modelviewUniform(-1),tintUniform(-1),
  mBuff(),mCB(&mBuff,&DataDoubleBuffer::write),mGame(&mCB)
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
  mGame.run();
  redraw();
}
void Fl_Gl_Tetris::reset()
{
  mGame.~TetrisGame();
  new(&mGame) TetrisGame(&mCB);
  
  mBuff.~DataDoubleBuffer();
  new(&mBuff) DataDoubleBuffer();

  redraw();
}

// Protected functions

void Fl_Gl_Tetris::draw()
{
  static GLfloat Projection[16];
  static int width,height;

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
      // Set up projection
      memset(Projection,0,sizeof(Projection));
      Projection[0]=1.0f;// /width;
      Projection[5]=1.0f;// /height;
      Projection[10]=1.0f;
      Projection[15]=1.0f;
    }
  //std::cout << "Beginning draw\n";
  //printGlError();
  const DataBuffer & gameState=mBuff.swap_and_read();
  /* TODO: Use a proper matrix class & have a stack of said class with which to do proper
     transform matrix composition.
     (LilyUtils' matrix looks good. TODO: Figure out a sensible way to merge that repo 
     into our repo)
  */
  GLfloat Modelview[16];
  memset(Modelview,0,sizeof(Modelview));
  Modelview[0]=1.0f;
  Modelview[5]=1.0f;
  Modelview[10]=1.0f;
  Modelview[15]=1.0f;

  // Begin GL operations
  glClear(GL_COLOR_BUFFER_BIT);
  glUniformMatrix4fv(projectionUniform,1,GL_FALSE,Projection);
  // TODO: Use a vector struct to hold color values, preferably as named constants
  // Draw a grey square for each block in the field which is set.
  glUniform4f(tintUniform, 0.8f, 0.8f, 0.8f,1.0f);// TODO: GREY_COLOR
  //std::cout << "Entering field loop\n";
  //printGlError();
  // Draw blocks set in field
  for(unsigned i=0;i<FIELD_WIDTH;++i)
    {
      for(unsigned j=0;j<FIELD_HEIGHT;++j)
	{
	  if(gameState.field.get(i,j))
	    {
	      Modelview[3]=i*Modelview[0];
	      Modelview[7]=j*Modelview[5];
	      glUniformMatrix4fv(modelviewUniform,1,GL_FALSE,Modelview);
	      glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
	    }
	}
    }
  Modelview[3]=0;
  Modelview[7]=0;

  //std::cout << "Entering current piece loop\n";
  //printGlError();
  // Draw current piece
  arrayt blocks=gameState.current.getBlocks();
  for(auto block : blocks)
    {
      Modelview[3]=block.x*Modelview[0];
      Modelview[7]=block.y*Modelview[5];
      glUniformMatrix4fv(modelviewUniform,1,GL_FALSE,Modelview);
      glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
    }
  //std::cout << "Drawing test square\n";
  //printGlError();
  // Test square
  Modelview[3]=0.5f;
  Modelview[7]=0.5f;
  glUniform4f(tintUniform, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniformMatrix4fv(modelviewUniform,1,GL_FALSE,Modelview);
  glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
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
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_SCISSOR_TEST);


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
  // To avoid dealing with files, generate a simple texture.
  glGenTextures(1,&squareTexID);
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
  projectionUniform=glGetUniformLocation(shaderProgram,PROJECTION_UNIFORM_NAME);
  modelviewUniform=glGetUniformLocation(shaderProgram,MODELVIEW_UNIFORM_NAME);
  tintUniform=glGetUniformLocation(shaderProgram,TINT_UNIFORM_NAME);

  if(-1==projectionUniform || -1 == modelviewUniform || -1 == tintUniform)
    {
      throw std::runtime_error("Uniform name loading failed.");
    }


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
