#include <SDL.h>
#include <GL/gl.h>

#include <iostream>


constexpr unsigned WIDTH=200,HEIGHT=400;


bool init_gl();
bool init_sdl_context();

int main()
{
  int retcode;
  retcode=SDL_Init(SDL_INIT_VIDEO);
  if(-1==retcode)
    {
      std::cerr << "sdlInitFailed: " << SDL_GetError() << std::endl;
      return retcode;
    }

  if(!init_sdl_context)
    {
      SDL_Quit();
      return -1;
    }

  if(!init_gl())
    {
      SDL_Quit();
      return -1;
    }


  

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

  glGetIntegerv(GL_MAJOR_VERSION,&retcode);  
  std::cout << "GL version: " << retcode << '.';
  glGetIntegerv(GL_MINOR_VERSION,&retcode);
  std::cout << retcode << '\n';

  return true;
}
