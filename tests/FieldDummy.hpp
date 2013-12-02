#ifndef FIELDDUMMY_HPP
#define FIELDDUMMY_HPP
#include <set>
#include <map>

namespace FieldDummy
{
  bool compare_get_arg(const std::set<coord> &args);
  bool compare_set_arg(const std::set<coord> &args);
  int count_readScore();
  int count_resetScore();
  int count_resetBlocks();

  void populate_get_results(const coord &arg, bool result=true);

  void reset_readScore();
  void reset_resetScore();
  void reset_resetBlocks();
  void reset_set_args();
  void reset_get_results();
  void reset_all();
}

#endif // FIELDDUMMY_HPP
