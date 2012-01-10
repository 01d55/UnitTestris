#ifndef TETRISGAMETEST_HPP
#define TETRISGAMETEST_HPP

#include <cppunit/extensions/HelperMacros.h>

#include "config.h"
#ifdef HAVE_ATOMIC
#include <atomic>
#endif // HAVE_ATOMIC

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

#ifdef HAVE_ATOMIC
  static std::atomic_int dummyCount;
#else // HAVE_ATOMIC
  static int dummyCount;
#endif // HAVE_ATOMIC
};

#endif // TETRISGAMETEST_HPP
