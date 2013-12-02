#ifndef DATADOUBLEBUFFER_HPP
#define DATADOUBLEBUFFER_HPP

#include <mutex>

#include "TetrisGame.hpp"
#include "Field.hpp"
#include "Piece.hpp"

struct DataBuffer
{
  Field field;
  Piece current,hold;
  bool holding;
  DataBuffer():current(I,0,nullptr),hold(I,0,nullptr),holding(false)
  {}
};
class DataDoubleBuffer
{
  DataBuffer buffers[2];
  bool swapped,dirty;
  std::mutex swap_guard;
public:
  DataDoubleBuffer():swapped(false),dirty(false),swap_guard()
  {}

  void write(const Field &, const Piece &, const Piece *);
  const DataBuffer& swap_and_read();
};

#endif // DATADOUBLEBUFFER_HPP
