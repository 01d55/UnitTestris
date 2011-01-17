# Rules for the test code (use `make check` to execute)
TESTS = UnitTestris
check_PROGRAMS = $(TESTS)
UnitTestris_SOURCES = Field.hpp FieldTest.hpp FieldTest.cpp FieldTest.cpp\
 Block.hpp BlockTest.hpp BlockTest.cpp UnitTestris.cpp 
UnitTestris_CXXFLAGS = $(CPPUNIT_CFLAGS)
UnitTestris_LDFLAGS = $(CPPUNIT_LIBS)
UnitTestris_LDFLAGS = -ldl
