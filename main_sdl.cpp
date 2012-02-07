#include <SDL.h>

#include <iostream>

int main()
{
  int retcode;
  retcode=SDL_Init(SDL_INIT_VIDEO);
  if(-1==retcode)
    {
      std::cerr << "sdlInitFailed: " << SDL_GetError() << std::endl;
      return retcode;
    }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

  SDL_Quit();

  return 0;
}
