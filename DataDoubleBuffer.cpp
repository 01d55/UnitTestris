#include "DataDoubleBuffer.hpp"

#define WBUFF swapped ? 1:0;
#define RBUFF swapped ? 0:1;

void DataDoubleBuffer::write(const Field &field_, const Piece &cur, const Piece *holdp)
{
  std::lock_guard<std::mutex> lg(swap_guard);
  auto idx = WBUFF;

  buffers[idx].field=field_;
  buffers[idx].current=cur;
  if(nullptr!=holdp)
    {
      buffers[idx].hold=*holdp;
      buffers[idx].holding=true;
    }
  else
    {
      buffers[idx].holding=false;
    }
  dirty=true;
}

const DataBuffer& DataDoubleBuffer::swap_and_read()
{
  swap_guard.lock();
  if(dirty)
    {
      swapped = !swapped;
      dirty=false;
    }
  swap_guard.unlock();
  auto idx = RBUFF;
  return buffers[idx];
}
