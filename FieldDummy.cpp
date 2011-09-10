#include Field.hpp
#include FieldDummy.hpp

namespace FieldDummy
{
  // Control variables for dummy methods
  std::map<coord,bool> get_results;
  int dummy_score=0;
  // Spy variables
  std::deque<coord> get_args;
  std::deque<coord> set_args;
  int readScore_count, resetScore_count,resetBlocks_count;
}
// Dummy implementations of Field methods

// Constructors
Field::Field():score(0)
{
  // STUB
}
// Empty field
Field::Field(const Field& toCopy):score(0)
{
  // STUB
}
// Vector holding blocks in the lowest row
Field::Field(std::vector<bool> row) throw (FieldSizeError):score(0)
{
  // STUB
}
// 10x22 vector. Indices are x-y co-ordinates, e.g. blocks[x][y].
Field::Field(std::vector< std::vector<bool> > blocks) throw (FieldSizeError)
{
  // STUB
}

Field::~Field()
{
  // STUB
}

// Return true if there is a block at those co-ordinates
bool Field::get(int x, int y) const throw (FieldSizeError)
{
  if(0>x||0>y||x>=FIELD_WIDTH||y>=FIELD_HEIGHT)
    {
      throw FieldSizeError();
    }
  coord arg(x,y);
  map<coord,bool>::iterator it;

  FieldDummy::get_args.push_back(arg);
  it=FieldDummy::get_results.find(arg);
  return (it==FieldDummy::get_results.end() ? false : it->second);
}
// Find the current score
int Field::readScore() const
{
  ++FieldDummy::readScore_count;
  return 0;
}

// Insert a block at the given co-ordinate. Inserting a block on top of an 
// existing block is an error.
void Field::set(int x, int y) throw (FieldSizeError, DuplicateBlockError)
{
  if(0>x||0>y||x>=FIELD_WIDTH||y>=FIELD_HEIGHT)
    {
      throw FieldSizeError();
    }
  FieldDummy::set_args.push_back(coord(x,y));
}
// Set the score to 0
void Field::resetScore()
{
  ++FieldDummy::resetScore_count;
}
// Set all blocks to false.
void Field::resetBlocks()
{
  ++resetBlocks_count;
}

// Private method - nothing to spy for.
void Field::checkLine(int) {}

namespace FieldDummy
{
  bool compare_get_arg(std::set<coord> args)
  {
    int i;
    std::set<coord> sample;
    for (i=0;i<args.size();++i)
      {
	if(get_args.empty())
	  {return false;}
	sample.insert(get_args.front());
	get_args.pop_front();
      }
    return args==sample;
  }

  bool compare_set_arg(std::set<coord> args)
  {
    int i;
    std::set<coord> sample;
    for (i=0;i<args.size();++i)
      {
	if(set_args.empty())
	  {return false;}
	sample.insert(set_args.front());
	set_args.pop_front();
      }
    return args==sample;
  }

  int count_readScore()
  {
    return readScore_count;
  }
  int count_resetScore()
  {
    return resetScore_count;
  }
  int count_resetBlocks()
  {
    return resetBlocks_count;
  }

  void populate_get_results(coord arg, bool result)
  {
    get_results[arg]=result;
  }

  void reset_readScore()
  {
    readScore_count=0;
  }
  void reset_resetScore()
  {
    resetScore_count=0;
  }
  void reset_resetBlocks()
  {
    resetBlocks_count=0;
  }
}
