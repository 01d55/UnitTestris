#include "Field.hpp"
  // Constructors
Field::Field() // Empty field
  {
  }
Field::Field(const Field& toCopy)
  {
  }
  // Vector holding blocks in the lowest row
Field::Field(std::vector<bool> row) throw (FieldSizeError)
  {
  }
  // Vector holding initial blocks in the bottom N rows, where N is the size of 
  // the outer vector.
Field::Field(std::vector< std::vector<bool> > rows) throw (FieldSizeError)
  {
  }

Field::~Field()
  {
  }

  // Return true if there is a block at those co-ordinates
bool Field::get(int, int)
  {
    //STUB
    return false;
  }
  // Insert a block at the given co-ordinate. Inserting a block on top of an 
  // existing block is an error.
void Field::set(int, int) throw (DuplicateBlockError)
  {
    //STUB
  }
