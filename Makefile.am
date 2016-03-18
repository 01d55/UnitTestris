# Required by AC_CONFIG_MACRO_DIR
ACLOCAL_AMFLAGS = -I m4

#Rules for compilation
bin_PROGRAMS = tetris_fltk tetris_sdl

tetris_fltk_SOURCES = src/main_fltk.cpp src/Tetris_fltkgui.cpp src/Field.cpp\
src/Piece.cpp src/Fl_Gl_Tetris.cpp src/TetrisGame.cpp src/DataDoubleBuffer.cpp
tetris_fltk_CXXFLAGS = $(FLTK_CXXFLAGS) $(CXX11FLAG)
tetris_fltk_LDADD = $(FLTK_LDADD) $(GL_LIBS) -lpthread

tetris_sdl_SOURCES = src/main_sdl.cpp src/Field.cpp src/Piece.cpp src/TetrisGame.cpp\
src/DataDoubleBuffer.cpp src/music.cpp
tetris_sdl_CXXFLAGS = $(CXX11FLAG) $(SDL_CFLAGS)
tetris_sdl_LDADD = $(SDL_LIBS) -lpthread

# Rule for compiling fluid file
BUILT_SOURCES = src/Tetris_fltkgui.cpp src/Tetris_fltkgui.hpp
src/Tetris_fltkgui.cpp src/Tetris_fltkgui.hpp: src/Tetris_fltkgui.fl
	fluid -c -o src/Tetris_fltkgui.cpp -h src/Tetris_fltkgui.hpp src/Tetris_fltkgui.fl

# Rules for the test code (use `make check` to execute)
TESTS = tests/FieldCheck tests/PieceCheck tests/TetrisGameCheck tests/IntegrationCheck
check_PROGRAMS = $(TESTS)

tests_FieldCheck_SOURCES = src/Field.cpp tests/FieldTest.cpp\
tests/FieldCheck.cpp
tests_FieldCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)$(CXX11FLAG) -I./src
tests_FieldCheck_LDADD = $(CPPUNIT_LIBS)

tests_PieceCheck_SOURCES = tests/FieldDummy.cpp\
src/Piece.cpp tests/PieceTest.cpp tests/PieceCheck.cpp 
tests_PieceCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)$(CXX11FLAG) -I./src
tests_PieceCheck_LDADD = $(CPPUNIT_LIBS)

tests_TetrisGameCheck_SOURCES = src/TetrisGame.cpp tests/TetrisGameTest.cpp \
tests/FieldDummy.cpp tests/PieceDummy.cpp\
tests/TetrisGameCheck.cpp
tests_TetrisGameCheck_CXXFLAGS = $(CPPUNIT_CFLAGS) $(CXX11FLAG) -pthread -I./src
tests_TetrisGameCheck_LDADD = $(CPPUNIT_LIBS)

tests_IntegrationCheck_SOURCES= src/TetrisGame.cpp src/Field.cpp src/Piece.cpp\
tests/IntegrationTest.cpp tests/IntegrationCheck.cpp
tests_IntegrationCheck_CXXFLAGS = $(CPPUNIT_CFLAGS) $(CXX11FLAG) -pthread -I./src
tests_IntegrationCheck_LDADD = $(CPPUNIT_LIBS)
