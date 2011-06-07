#include "Field.hpp"
// Constructors
Field::Field():score(0) // Empty field
{
  int i;
  for(i=0;i<FIELD_SIZE;++i)
    iblocks[i]=false;
}
Field::Field(const Field& toCopy):score(toCopy.score)
{
  int i,j;
  for(j=0;j<FIELD_HEIGHT;++j)
    for (i=0;i<FIELD_WIDTH;++i)
      iblocks[i+FIELD_WIDTH*j]=toCopy.iblocks[i+FIELD_WIDTH*j];
}
// Vector holding blocks in the lowest row
Field::Field(std::vector<bool> row) throw (FieldSizeError):score(0)
{
  int i;
  if(row.size()!=FIELD_WIDTH) throw FieldSizeError();

  for(i=FIELD_WIDTH;i<FIELD_SIZE;++i)
    iblocks[i]=false;
  for(i=0;i<FIELD_WIDTH;++i)
      iblocks[i]=row[i];
  checkLine(0);
}
// 10x22 vector. Indices are x-y co-ordinates, e.g. blocks[x][y].
Field::Field(std::vector< std::vector<bool> > blocks) throw (FieldSizeError):score(0)
{
  int i,j;
  // Check size
  if(blocks.size()!=FIELD_WIDTH) throw FieldSizeError();
  for(i=0;i<FIELD_WIDTH;++i)
    if(blocks[i].size()!=FIELD_HEIGHT) throw FieldSizeError();

  for(j=0;j<FIELD_HEIGHT;++j)
    for (i=0;i<FIELD_WIDTH;++i)
      iblocks[i+FIELD_WIDTH*j]=blocks[i][j];
  checkLines();
}

Field::~Field()
{
  //STUB
}

// Return true if there is a block at those co-ordinates
bool Field::get(int x, int y) const throw (FieldSizeError)
{
  // Check range
  if(0>x||0>y||x>=FIELD_WIDTH||y>=FIELD_HEIGHT) throw FieldSizeError();

  return iblocks[x+y*FIELD_WIDTH];
}
// Insert a block at the given co-ordinate. Inserting a block on top of an 
// existing block is an error.
void Field::set(int x, int y) throw (FieldSizeError, DuplicateBlockError)
{
  // Check range
  if(0>x||0>y||x>=FIELD_WIDTH||y>=FIELD_HEIGHT)
    {
      throw FieldSizeError();
    }
  // Check duplicate
  if(iblocks[x+y*FIELD_WIDTH])
    {
      throw DuplicateBlockError();
    }
  iblocks[x+y*FIELD_WIDTH]=true;
  checkLine(y);
}

// Find the current score
int Field::readScore() const
{
  return score;
}
// Set the score to 0
void Field::resetScore()
{
  score=0;
}
// Set all blocks to false.
void Field::resetBlocks()
{
  int i;
  for(i=0;i<FIELD_SIZE;++i)
    iblocks[i]=false;
}

void Field::checkLine(int y)
{
  int i;

  for(i=0;i<FIELD_WIDTH;++i)
    {
      if(!iblocks[i+FIELD_WIDTH*y])
	return;
    }
  // Line is full
  ++score;
  for(i=FIELD_WIDTH*y;i<FIELD_WIDTH*(FIELD_HEIGHT-1);++i)
    {
      iblocks[i]=iblocks[i+FIELD_WIDTH];
    }
  for(/*where we left off*/;i<FIELD_SIZE;++i)
    {
      iblocks[i]=false;
    }
}
