# Rules for the test code (use `make check` to execute)
TESTS = UnitTestris
check_PROGRAMS = $(TESTS)
UnitTestris_SOURCES = Field.hpp Field.cpp FieldTest.hpp FieldTest.cpp\
Piece.hpp Piece.cpp PieceTest.hpp PieceTest.cpp UnitTestris.cpp 
UnitTestris_CXXFLAGS = $(CPPUNIT_CFLAGS)
UnitTestris_LDFLAGS = $(CPPUNIT_LIBS)
#UnitTestris_LDFLAGS = -ldl
