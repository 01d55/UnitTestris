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