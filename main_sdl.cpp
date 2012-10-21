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
#include "music.hpp"

constexpr unsigned WIDTH=400,HEIGHT=800, FIELD_VIEW_HEIGHT = 20;

struct glstate
{
  GLuint squareVBO,squareTexID,squareIBO,VAO,
    shaderProgram,vertexShader,fragShader;
  GLint offsetUniform,tintUniform;

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

  void checkGameOver()
  {
    if(game.isGameOver())
      {
	reset();
	reset_music();
      }
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

  init_music();

  while(true)
    {
      process_events();
      render_gl();
    }
  terminate_music();
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
	    texdata[i*TEXDIMENSION+j].z=value+0.5;
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
  char constexpr VERT_SHADER_PATH[]="shaders/simple.vert";
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

  GLchar constexpr OFFSET_UNIFORM_NAME[]="offset";
  GLchar constexpr TINT_UNIFORM_NAME[]="tint";
  GLchar constexpr SAMPLER_UNIFORM_NAME[]="msampler";
  GL_STATE.offsetUniform=glGetUniformLocation(GL_STATE.shaderProgram,OFFSET_UNIFORM_NAME);
  GL_STATE.tintUniform=glGetUniformLocation(GL_STATE.shaderProgram,TINT_UNIFORM_NAME);
  // We don't need to keep the samplerUniform after init - for now.
  GLint samplerUniform=glGetUniformLocation(GL_STATE.shaderProgram,SAMPLER_UNIFORM_NAME);


  if(-1==GL_STATE.offsetUniform ||
     -1 == GL_STATE.tintUniform || -1 == samplerUniform)
    {
      std::cerr << GL_STATE.offsetUniform << GL_STATE.tintUniform << samplerUniform << '\n';
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

  glViewport(0,0,WIDTH,HEIGHT);

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
      toggle_play_music();
      break;
    case SDLK_RETURN:
      GAME_STATE.reset();
      reset_music();
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
  GAME_STATE.checkGameOver();
}

void render_gl()
{
  const glVec WHITE_COLOR(1.0f,1.0f,1.0f,1.0f);
  const glVec GREY_COLOR(0.6f,0.6f,0.6f,0.6f);
  const glVec I_COLOR(1.0f,0.0f,0.0f,1.0f);
  const glVec J_COLOR(0.0f,1.0f,0.0f,1.0f);
  const glVec L_COLOR(0.0f,0.0f,1.0f,1.0f);
  const glVec O_COLOR(1.0f,1.0f,0.0f,1.0f);
  const glVec S_COLOR(1.0f,0.0f,1.0f,1.0f);
  const glVec T_COLOR(0.0f,1.0f,1.0f,1.0f);
  const glVec Z_COLOR(0.5f,1.0f,0.5f,1.0f);

  const DataBuffer &gameState=GAME_STATE.buffer.swap_and_read();

  SDL_GL_SwapBuffers();
  printGlError();

  
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw blocks set in field
  glUniform4fv(GL_STATE.tintUniform, 1, GREY_COLOR.data);
  for(unsigned i=0;i<FIELD_WIDTH;++i)
    {
      for(unsigned j=0;j<FIELD_VIEW_HEIGHT;++j)
	{
	  if(gameState.field.get(i,j))
	    {
	      glUniform2f(GL_STATE.offsetUniform,(GLfloat)i,(GLfloat)j);
	      glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
	    }
	}
    }
  
  // Draw current piece
  arrayt blocks=gameState.current.getBlocks();
  switch(gameState.current.getType())
    {
    case I:
      glUniform4fv(GL_STATE.tintUniform, 1, I_COLOR.data);
      break;
    case J:
      glUniform4fv(GL_STATE.tintUniform, 1, J_COLOR.data);
      break;
    case L:
      glUniform4fv(GL_STATE.tintUniform, 1, L_COLOR.data);
      break;
    case O:
      glUniform4fv(GL_STATE.tintUniform, 1, O_COLOR.data);
      break;
    case S:
      glUniform4fv(GL_STATE.tintUniform, 1, S_COLOR.data);
      break;
    case T:
      glUniform4fv(GL_STATE.tintUniform, 1, T_COLOR.data);
      break;
    case Z:
      glUniform4fv(GL_STATE.tintUniform, 1, Z_COLOR.data);
      break;
    default:
      std::cerr << "Unhandled case in enumerated switch statement!\n";
      break;
    }

  for(auto block : blocks)
    {
      glUniform2f(GL_STATE.offsetUniform,(GLfloat)block.x,(GLfloat)block.y);
      glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
    }

}

void terminate_program(int ec)
{
  terminate_music();
  terminate_gl();
  SDL_Quit();
  exit(ec);
}
