#Rules for compilation
bin_PROGRAMS = tetris_fltk

tetris_fltk_SOURCES = main_fltk.cpp Tetris_fltkgui.cpp Tetris_fltkgui.hpp\
Field.hpp Field.cpp Piece.hpp Piece.cpp Fl_Gl_Tetris.hpp Fl_Gl_Tetris.cpp
tetris_fltk_CXXFLAGS = $(CXXFLAGS) $(FLTK_CXXFLAGS)
tetris_fltk_LDFLAGS = $(LDFLAGS) $(FLTK_LDFLAGS)

# Rule for compiling fluid file
BUILT_SOURCES = Tetris_fltkgui.cpp Tetris_fltkgui.hpp
Tetris_fltkgui.cpp Tetris_fltkgui.hpp: Tetris_fltkgui.fl
	fluid -c Tetris_fltkgui.fl

# Rules for the test code (use `make check` to execute)
TESTS = FieldCheck PieceCheck TetrisGameCheck
check_PROGRAMS = $(TESTS)

FieldCheck_SOURCES = Field.hpp Field.cpp FieldTest.hpp FieldTest.cpp\
FieldCheck.cpp 
FieldCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)
FieldCheck_LDFLAGS = $(CPPUNIT_LIBS)

PieceCheck_SOURCES = Field.hpp FieldDummy.hpp FieldDummy.cpp\
Piece.hpp Piece.cpp PieceTest.hpp PieceTest.cpp PieceCheck.cpp 
PieceCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)
PieceCheck_LDFLAGS = $(CPPUNIT_LIBS)

TetrisGameCheck_SOURCES = TetrisGame.hpp TetrisGame.cpp TetrisGameTest.hpp TetrisGameTest.cpp\
Field.hpp FieldDummy.hpp FieldDummy.cpp Piece.hpp PieceDummy.hpp PieceDummy.cpp\
TetrisGameCheck.cpp
TetrisGameCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)
TetrisGameCheck_LDFLAGS = $(CPPUNIT_LIBS)
