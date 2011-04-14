#ifndef FIELD_HPP
#define FIELD_HPP

#include <vector>
#include <stdexcept>
/** Field
    The Field enforces the following rules of Tetris: There can be no full 
    rows. All empty rows are above all non-empty rows. The width of the field is
    10. The height of the field is at least 22.

    The field will allow unlimited read-access and insertion of blocks, but
    does not permit deletion except by enforcement of the rules.
 */

class FieldSizeError : public std::runtime_error
{
public:
  FieldSizeError() : runtime_error( "Argument does not match Tetris field dimensions." )
  {
  }
};

class DuplicateBlockError: public std::runtime_error
{
public:
  DuplicateBlockError() : runtime_error( "Block already exists." )
  {
  }
};

class Field
{
public:
  // Constructors
  Field(); // Empty field
  Field(const Field& toCopy);
  // Vector holding blocks in the lowest row
  Field(std::vector<bool> row) throw (FieldSizeError);
  // 10x22 vector. Indices are x-y co-ordinates, e.g. blocks[x][y].
  Field(std::vector< std::vector<bool> > blocks) throw (FieldSizeError);

  ~Field();

  // Return true if there is a block at those co-ordinates
  bool get(int x, int y) throw (FieldSizeError);
  // Insert a block at the given co-ordinate. Inserting a block on top of an 
  // existing block is an error.
  void set(int x, int y) throw (FieldSizeError, DuplicateBlockError);
  // Find the current score
  int readScore();
  // Set the score to 0
  void resetScore();

private:
};


#endif // FIELD_HPP
