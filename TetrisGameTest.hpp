#ifndef TETRISGAMETEST_HPP
#define TETRISGAMETEST_HPP

#include <cppunit/extensions/HelperMacros.h>

class TetrisGame;
class Field;
class Piece;
class TetrisGameTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE ( TetrisGameTest );
  CPPUNIT_TEST( testNewDelete );
  CPPUNIT_TEST( testRunCallback );
  CPPUNIT_TEST( testExceptions );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testNewDelete();
  void testRunCallback();
  void testExceptions();

  static void dummyRenderCallback(const Field &, const Piece &, const Piece *);
protected:
private:
  TetrisGame * mptr;

  static int dummyCount;
};

#endif // TETRISGAMETEST_HPP
