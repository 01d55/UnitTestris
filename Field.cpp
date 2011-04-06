#include "Field.hpp"
// Constructors
Field::Field() // Empty field
{
  //STUB
}
Field::Field(const Field& toCopy)
{
  //STUB
}
// Vector holding blocks in the lowest row
Field::Field(std::vector<bool> row) throw (FieldSizeError)
{
  //STUB
}
// 22x10 vector. Indices are x-y co-ordinates, e.g. blocks[x][y].
Field::Field(std::vector< std::vector<bool> > blocks) throw (FieldSizeError)
{
  //STUB
}

Field::~Field()
{
  //STUB
}

// Return true if there is a block at those co-ordinates
bool Field::get(int, int) throw (FieldSizeError)
{
  //STUB
  return false;
}
// Insert a block at the given co-ordinate. Inserting a block on top of an 
// existing block is an error.
void Field::set(int, int) throw (FieldSizeError, DuplicateBlockError)
{
  //STUB
}

// Find the current score
int Field::readScore()
{
  //STUB
  return -1;
}
// Set the score to 0
void Field::resetScore()
{
  //STUB
}
