#include <FL/Fl.H>
#include "Tetris_fltkgui.hpp"

int main(int argc, char **argv){
  Fl::gl_visual(FL_DEPTH | FL_DOUBLE);
  Tetris_fltkgui *g = new Tetris_fltkgui();
  g->show();
  
  return Fl::run();
}
