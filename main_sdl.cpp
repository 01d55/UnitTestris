#include <SDL.h>
// Get full OpenGL API
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/gl.h>

#include <iostream>

#include "DataDoubleBuffer.hpp"
#include "glutil.hpp"
#include "GLMatrix.h"

constexpr unsigned WIDTH=200,HEIGHT=400;

struct glstate
{
  GLuint squareVBO,squareTexID,squareIBO,VAO,
    shaderProgram,vertexShader,fragShader;
  GLint projectionUniform,modelviewUniform,tintUniform;

} GL_STATE;

struct tetrisstate
{
  DataDoubleBuffer buffer;
  RenderFunc<DataDoubleBuffer> rfunc;
  TetrisGame game;

  bool running;

  tetrisstate():buffer(),rfunc(&buffer,&DataDoubleBuffer::write),game(&rfunc),
		running(false)
  {}

  void toggle_pause()
  {
    if(running)
      {
	game.pause();
      }
    else
      {
	game.run();
      }
    running = !running;
  }

  void reset()
  {
    game.~TetrisGame();
    new(&game) TetrisGame(&rfunc);

    buffer.~DataDoubleBuffer();
    new(&buffer) DataDoubleBuffer();

    running = false;
  }
} GAME_STATE;

bool init_gl();
bool init_sdl_context();

void keyboard_event(const SDL_KeyboardEvent &ev);

void process_events();

void render_gl();

void terminate_gl();
void terminate_program(int ec);

int main()
{
  int retcode;
  retcode=SDL_Init(SDL_INIT_VIDEO);
  if(-1==retcode)
    {
      std::cerr << "sdlInitFailed: " << SDL_GetError() << std::endl;
      return retcode;
    }

  if(!init_sdl_context() )
    {
      SDL_Quit();
      return -1;
    }

  if(!init_gl())
    {
      SDL_Quit();
      return -1;
    }


  while(true)
    {
      process_events();
      render_gl();
    }
  terminate_gl();
  SDL_Quit();

  return 0;
}

bool init_sdl_context()
{

  const SDL_VideoInfo *infoptr;

  infoptr = SDL_GetVideoInfo();

  if(nullptr == infoptr)
    {
      std::cerr << "GetVideoInfo failed: " << SDL_GetError() << std::endl;
      return false;
    }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

  if(nullptr == 
     SDL_SetVideoMode(WIDTH, HEIGHT, infoptr->vfmt->BitsPerPixel, SDL_OPENGL) )
    {
      std::cerr << "SetVideoMode failed: " << SDL_GetError() << std::endl;
      return false;
    }

  return true;
}

bool init_gl()
{
  GLint retcode=-1;
  GLenum errcode=GL_NO_ERROR;

  glGetIntegerv(GL_MAJOR_VERSION,&retcode);  
  std::cout << "GL version: " << retcode << '.';
  glGetIntegerv(GL_MINOR_VERSION,&retcode);
  std::cout << retcode << '\n';
  // Set up VAO
  glGenVertexArrays(1,&GL_STATE.VAO);
  glBindVertexArray(GL_STATE.VAO);

  // Set up Buffer Objects
  glGenBuffers(1,&GL_STATE.squareVBO);
  glBindBuffer(GL_ARRAY_BUFFER,GL_STATE.squareVBO);

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

  glGenBuffers(1,&GL_STATE.squareIBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_STATE.squareIBO);

  constexpr GLushort squareIDX[3*2]=
    { // Triangle indices
      0,3,1,
      2,1,3
    };
  constexpr GLsizeiptr squareIDXsize=sizeof(squareIDX);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,squareIDXsize,squareIDX,GL_STATIC_DRAW);


  // Set up texture
  glGenTextures(1,&GL_STATE.squareTexID);
  glBindTexture(GL_TEXTURE_2D,GL_STATE.squareTexID);
  // To avoid dealing with files, generate a simple texture.
  constexpr unsigned TEXDIMENSION=256,TEXBUFFSIZE=TEXDIMENSION*TEXDIMENSION;
  glVec *texdata=new glVec[TEXBUFFSIZE];
  for(unsigned i=0;i<TEXDIMENSION;++i)
    {
      for(unsigned j=0;j<TEXDIMENSION;++j)
	{
	  unsigned idist=std::min(TEXDIMENSION-i,i);
	  unsigned jdist=std::min(TEXDIMENSION-j,j);
	  unsigned dist=std::min(idist,jdist);
	  GLfloat value=((GLfloat)dist)/TEXDIMENSION;
	  texdata[i*TEXDIMENSION+j].x=
	    texdata[i*TEXDIMENSION+j].y=
	    texdata[i*TEXDIMENSION+j].z=value;
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
		     GL_STATE.shaderProgram,GL_STATE.vertexShader,GL_STATE.fragShader);
  if(-1==retcode)
    {
      std::cerr <<"Shader loading failed. \n";
      return false;
    }
  glUseProgram(GL_STATE.shaderProgram);

  GLchar constexpr PROJECTION_UNIFORM_NAME[]="Projection";
  GLchar constexpr MODELVIEW_UNIFORM_NAME[]="Modelview";
  GLchar constexpr TINT_UNIFORM_NAME[]="tint";
  GLchar constexpr SAMPLER_UNIFORM_NAME[]="msampler";
  GL_STATE.projectionUniform=glGetUniformLocation(GL_STATE.shaderProgram,PROJECTION_UNIFORM_NAME);
  GL_STATE.modelviewUniform=glGetUniformLocation(GL_STATE.shaderProgram,MODELVIEW_UNIFORM_NAME);
  GL_STATE.tintUniform=glGetUniformLocation(GL_STATE.shaderProgram,TINT_UNIFORM_NAME);
  // We don't need to keep the samplerUniform after init - for now.
  GLint samplerUniform=glGetUniformLocation(GL_STATE.shaderProgram,SAMPLER_UNIFORM_NAME);


  if(-1==GL_STATE.projectionUniform || -1 == GL_STATE.modelviewUniform ||
     -1 == GL_STATE.tintUniform || -1 == samplerUniform)
    {
      std::cerr << GL_STATE.projectionUniform << GL_STATE.modelviewUniform << GL_STATE.tintUniform << samplerUniform << '\n';
      std::cerr <<"Uniform name loading failed. \n";
      return false;
    }
  glUniform1i(samplerUniform,0);
  // Rebind texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,GL_STATE.squareTexID);


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

  
  errcode = glGetError();
  if(GL_NO_ERROR!=errcode)
    {
      std::cerr << "GL error during init_gl:"<< errcode <<"\n";
      return false;
    }

  return true;
}

void terminate_gl()
{
  glBindVertexArray(0);
  glDeleteVertexArrays(1,&GL_STATE.VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glDeleteBuffers(1,&GL_STATE.squareVBO);
  
  glBindTexture(GL_TEXTURE_2D,0);
  glDeleteTextures(1,&GL_STATE.squareTexID);
  // Shader cleanup
  
  glUseProgram(0);
  
  glDetachShader(GL_STATE.shaderProgram, GL_STATE.vertexShader);
  glDetachShader(GL_STATE.shaderProgram, GL_STATE.fragShader);
  
  glDeleteShader(GL_STATE.vertexShader);
  glDeleteShader(GL_STATE.fragShader);
  
  glDeleteProgram(GL_STATE.shaderProgram);
  if(GL_NO_ERROR!=glGetError())
    {
      std::cerr << "WARNING: terminate_gl detected a GL error. \n";
    }
}

void process_events()
{
  SDL_Event ev;

  while( SDL_PollEvent( &ev ) )
    {
      switch(ev.type)
	{
	case SDL_QUIT:
	  terminate_program(0);// never returns
	case SDL_KEYDOWN:
	  keyboard_event(ev.key);
	  break;
	default:
	  break;
	}
    }
}

void keyboard_event(const SDL_KeyboardEvent &ev)
{

  switch(ev.keysym.sym)
    {
    case SDLK_ESCAPE:
      terminate_program(0);// never returns
    case SDLK_PAUSE:
    case SDLK_p:
      GAME_STATE.toggle_pause();
      break;
      // Game control
    case SDLK_q:
    case SDLK_UP:
      if(GAME_STATE.running)
	{
	  GAME_STATE.game.queueInput(rotate_ccw);
	}
      break;
    case SDLK_a:
    case SDLK_LEFT:
      if(GAME_STATE.running)
	{
	  GAME_STATE.game.queueInput(shift_left);
	}
      break;
    case SDLK_e:
      if(GAME_STATE.running)
	{
	  GAME_STATE.game.queueInput(rotate_cw);
	}
      break;
    case SDLK_d:
    case SDLK_RIGHT:
      if(GAME_STATE.running)
	{
	  GAME_STATE.game.queueInput(shift_right);
	}
      break;
    case SDLK_x:
    case SDLK_DOWN:
      if(GAME_STATE.running)
	{
	  GAME_STATE.game.queueInput(hard_drop);
	}
      break;
    default:
      break;
    }
}

void render_gl()
{
  // STUB
  SDL_GL_SwapBuffers();
}

void terminate_program(int ec)
{
	  terminate_gl();
	  SDL_Quit();
	  exit(ec);
}
