# Required by AC_CONFIG_MACRO_DIR
ACLOCAL_AMFLAGS = -I m4

#Rules for compilation
bin_PROGRAMS = tetris_fltk tetris_sdl

tetris_fltk_SOURCES = main_fltk.cpp Tetris_fltkgui.cpp Field.cpp Piece.cpp\
Fl_Gl_Tetris.cpp TetrisGame.cpp DataDoubleBuffer.cpp
tetris_fltk_CXXFLAGS = $(FLTK_CXXFLAGS) $(CXX11FLAG)
tetris_fltk_LDADD = $(FLTK_LDADD) 

tetris_sdl_SOURCES = main_sdl.cpp Field.cpp Piece.cpp TetrisGame.cpp\
DataDoubleBuffer.cpp

tetris_sdl_CXXFLAGS = $(CXX11FLAG) $(SDL_CFLAGS)
tetris_sdl_LDADD = $(SDL_LIBS)

# Rule for compiling fluid file
BUILT_SOURCES = Tetris_fltkgui.cpp Tetris_fltkgui.hpp
Tetris_fltkgui.cpp Tetris_fltkgui.hpp: Tetris_fltkgui.fl
	fluid -c Tetris_fltkgui.fl

# Rules for the test code (use `make check` to execute)
TESTS = FieldCheck PieceCheck TetrisGameCheck IntegrationCheck
check_PROGRAMS = $(TESTS)

FieldCheck_SOURCES = Field.cpp FieldTest.cpp\
FieldCheck.cpp 
FieldCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)$(CXX11FLAG)
FieldCheck_LDADD = $(CPPUNIT_LIBS)

PieceCheck_SOURCES = FieldDummy.cpp\
Piece.cpp PieceTest.cpp PieceCheck.cpp 
PieceCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)$(CXX11FLAG)
PieceCheck_LDADD = $(CPPUNIT_LIBS)

TetrisGameCheck_SOURCES = TetrisGame.cpp TetrisGameTest.cpp \
FieldDummy.cpp PieceDummy.cpp\
TetrisGameCheck.cpp
TetrisGameCheck_CXXFLAGS = $(CPPUNIT_CFLAGS) $(CXX11FLAG) -pthread
TetrisGameCheck_LDADD = $(CPPUNIT_LIBS)

IntegrationCheck_SOURCES= TetrisGame.cpp Field.cpp Piece.cpp\
IntegrationTest.cpp IntegrationCheck.cpp
IntegrationCheck_CXXFLAGS = $(CPPUNIT_CFLAGS) $(CXX11FLAG) -pthread
IntegrationCheck_LDADD = $(CPPUNIT_LIBS)
