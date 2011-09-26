#Rules for compilation
bin_PROGRAMS = tetris_fltk

tetris_fltk_SOURCES = main_fltk.cpp tetris_fltkgui.cpp tetris_fltkgui.hpp\
Field.hpp Field.cpp Piece.hpp Piece.cpp Fl_Gl_Tetris.hpp Fl_Gl_Tetris.cpp
tetris_fltk_CXXFLAGS = $(CXXFLAGS) $(FLTK_CXXFLAGS)
tetris_fltk_LDFLAGS = $(LDFLAGS) $(FLTK_LDFLAGS)

# Rule for compiling fluid file
tetris_fltkgui.cpp tetris_fltkgui.hpp: tetris_fltkgui.fl
	fluid -c tetris_fltkgui.fl

# Rules for the test code (use `make check` to execute)
TESTS = FieldCheck PieceCheck
check_PROGRAMS = $(TESTS)

FieldCheck_SOURCES = Field.hpp Field.cpp FieldTest.hpp FieldTest.cpp\
FieldCheck.cpp 
FieldCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)
FieldCheck_LDFLAGS = $(CPPUNIT_LIBS)

PieceCheck_SOURCES = Field.hpp FieldDummy.hpp FieldDummy.cpp\
Piece.hpp Piece.cpp PieceTest.hpp PieceTest.cpp PieceCheck.cpp 
PieceCheck_CXXFLAGS = $(CPPUNIT_CFLAGS)
PieceCheck_LDFLAGS = $(CPPUNIT_LIBS)