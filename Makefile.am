# Required by AC_CONFIG_MACRO_DIR
ACLOCAL_AMFLAGS = -I m4

#Rules for compilation
bin_PROGRAMS = tetris_fltk

tetris_fltk_SOURCES = main_fltk.cpp Tetris_fltkgui.cpp Tetris_fltkgui.hpp\
Field.hpp Field.cpp Piece.hpp Piece.cpp Fl_Gl_Tetris.hpp Fl_Gl_Tetris.cpp\
RenderFunc.hpp TetrisGame.hpp TetrisGame.cpp\
libLilyEngineUtils/matrix.h
tetris_fltk_CXXFLAGS = $(FLTK_CXXFLAGS) $(CXX11FLAG)
tetris_fltk_LDADD = $(FLTK_LDADD) 

# Rule for compiling fluid file
BUILT_SOURCES = Tetris_fltkgui.cpp Tetris_fltkgui.hpp
Tetris_fltkgui.cpp Tetris_fltkgui.hpp: Tetris_fltkgui.fl
	fluid -c Tetris_fltkgui.fl

# Rules for the test code (use `make check` to execute)
TESTS = FieldCheck PieceCheck TetrisGameCheck IntegrationCheck
check_PROGRAMS = $(TESTS)

FieldCheck_SOURCES = Field.hpp Field.cpp FieldTest.hpp FieldTest.cpp\
FieldCheck.cpp 
FieldCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)$(CXX11FLAG)
FieldCheck_LDADD = $(CPPUNIT_LIBS)

PieceCheck_SOURCES = Field.hpp FieldDummy.hpp FieldDummy.cpp\
Piece.hpp Piece.cpp PieceTest.hpp PieceTest.cpp PieceCheck.cpp 
PieceCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)$(CXX11FLAG)
PieceCheck_LDADD = $(CPPUNIT_LIBS)

TetrisGameCheck_SOURCES = TetrisGame.hpp TetrisGame.cpp TetrisGameTest.hpp TetrisGameTest.cpp RenderFunc.hpp\
Field.hpp FieldDummy.hpp FieldDummy.cpp Piece.hpp PieceDummy.hpp PieceDummy.cpp\
TetrisGameCheck.cpp
TetrisGameCheck_CXXFLAGS = $(CPPUNIT_CFLAGS) $(CXX11FLAG) -pthread
TetrisGameCheck_LDADD = $(CPPUNIT_LIBS)

IntegrationCheck_SOURCES= TetrisGame.cpp Field.cpp Piece.cpp\
IntegrationTest.cpp IntegrationCheck.cpp
IntegrationCheck_CXXFLAGS = $(CPPUNIT_CFLAGS) $(CXX11FLAG) -pthread
IntegrationCheck_LDADD = $(CPPUNIT_LIBS)
