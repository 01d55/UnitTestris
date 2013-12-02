#ifndef FIELD_HPP
#define FIELD_HPP

#include <vector>
#include <stdexcept>

#include "common.hpp"
/** Field
    The Field enforces the following rules of Tetris: There can be no full 
    rows. All empty rows are above all non-empty rows. The width of the field is
    10. The height of the field is at least 22.

    The field will allow unlimited read-access and insertion of blocks, but
    does not permit deletion except by enforcement of the rules.
 */

class Field
{
public:
  // Constructors
  Field(); // Empty field
  Field(const Field& toCopy);
  // Vector holding blocks in the lowest row
  Field(std::vector<bool> row); //throw (FieldSizeError);
  // 10x22 vector. Indices are x-y co-ordinates, e.g. blocks[x][y].
  Field(std::vector< std::vector<bool> > blocks); //throw (FieldSizeError);

  ~Field();

  // Return true if there is a block at those co-ordinates
  bool get(int x, int y) const; //throw (FieldSizeError);
  inline bool get(const coord&c) const //throw(FieldSizeError)
  {return get(c.x,c.y);}
  // Find the current score
  int readScore() const;

  // Insert a block at the given co-ordinate. Inserting a block on top of an 
  // existing block is an error.
  void set(int x, int y); //throw (FieldSizeError, DuplicateBlockError);
  inline void set(const coord&c) //throw(FieldSizeError,DuplicateBlockError)
  {set(c.x,c.y);}
  // Set the score to 0
  void resetScore();
  // Set all blocks to false.
  void resetBlocks();

private:
  int score;
  bool iblocks[10*22];

  inline void checkLines()
  {
    for(int j=FIELD_HEIGHT-1;j>=0;--j)
      checkLine(j);
  }
  void checkLine(int);
};


#endif // FIELD_HPP
