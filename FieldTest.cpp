#include "FieldTest.hpp"
#include "Field.hpp"
#include <limits>
// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FieldTest );

void FieldTest::setUp()
{
}

void FieldTest::tearDown()
{
}


void FieldTest::testConstructor0()
{
  int i,j;
  Field *test_field;
  // Only one possible case: an empty field.
  test_field = new Field();
  for(j=0;j< FIELD_HEIGHT ;++j)
    {
      for (i=0;i<FIELD_WIDTH;++i)
	{
	  // The field should be empty, i.e. all gets should return false.
	  CPPUNIT_ASSERT( (!test_field->get(i,j) ) );
	}
    }
  delete test_field;
}

void FieldTest::testConstructor1()
{
  int i,j;
  Field *test_field;
  std::vector<bool> row;

  // Emtpy field
  row.resize(FIELD_WIDTH,false);
  test_field=new Field(row);
  row.clear();
  for(j=0;j<FIELD_HEIGHT;++j)
    {
      for (i=0;i<FIELD_WIDTH;++i)
	{
	  CPPUNIT_ASSERT( !(test_field->get(i,j) ) );
	}
    }
  delete test_field;

  // The bottom row should be missing only the far right block
  // All other rows should be empty.
  row.resize(FIELD_WIDTH,true);
  row[FIELD_WIDTH-1]=false;
  test_field = new Field(row);
  row.clear();
  for(j=0;j<FIELD_HEIGHT;++j)
    {
      for (i=0;i<FIELD_WIDTH;++i)
	{
	  if(j==0 && i < FIELD_WIDTH-1) {CPPUNIT_ASSERT( (test_field->get(i,j) ) );}
	  else {CPPUNIT_ASSERT( (!test_field->get(i,j) ) );}
	}
    }
  delete test_field;

  // The bottom row should have only the far right block.
  // All other rows should be empty.
  row.resize(FIELD_WIDTH,false);
  row[FIELD_WIDTH-1]=true;
  test_field=new Field(row);
  row.clear();
  for(j=0;j<FIELD_HEIGHT;++j)
    {
      for (i=0;i<FIELD_WIDTH;++i)
	{
	  if(j==0 && i == 0) {CPPUNIT_ASSERT( test_field->get(i,j) );}
	  else {CPPUNIT_ASSERT( !(test_field->get(i,j) ) );}
	}
    }
  delete test_field;

  // A single block in the middle.
  row.resize(FIELD_WIDTH,false);
  row[5]=true;
  test_field=new Field(row);
  row.clear();
  for(j=0;j<FIELD_HEIGHT;++j)
    {
      for (i=0;i<FIELD_WIDTH;++i)
	{
	  if(i==5 && j==0) { CPPUNIT_ASSERT( test_field->get(i,j) ); }
	  else { CPPUNIT_ASSERT( !(test_field->get(i,j) ) ); }
	}
    }
  delete test_field;

  // The full row should automatically clear itself, therefore:
  // The field should be empty, i.e. all gets should return false.
  row.resize(FIELD_WIDTH,true);
  test_field = new Field(row);
  row.clear();
  for(j=0;j<FIELD_HEIGHT;++j)
    {
      for (i=0;i<FIELD_WIDTH;++i)
	{
	  CPPUNIT_ASSERT( (!test_field->get(i,j) ) );
	}
    }
  delete test_field;

  // Test errors
  // row is clear
  CPPUNIT_ASSERT_THROW((test_field = new Field(row) ),FieldSizeError);

  row.resize(FIELD_WIDTH-1,true);
  CPPUNIT_ASSERT_THROW((test_field = new Field(row) ),FieldSizeError);

  row.resize(FIELD_WIDTH+1);
  CPPUNIT_ASSERT_THROW((test_field = new Field(row) ),FieldSizeError);
}

void FieldTest::testConstructor2()
{
  int i,j;
  Field *test_field, *copy_field;
  std::vector< std::vector<bool> > blocks;
 
  // Empty field
  blocks.resize(FIELD_WIDTH);
  for(i=0;i<FIELD_WIDTH;++i) {blocks[i].resize(FIELD_HEIGHT,false);}
  test_field= new Field(blocks);
  blocks.clear();
  for(i=0;i<FIELD_WIDTH;++i)
    {
      for (j=0;j<FIELD_HEIGHT;++j)
	{
	  CPPUNIT_ASSERT( !(test_field->get(i,j) ) );
	}
    }
  delete test_field;

  // Check the extreme corners
  blocks.resize(FIELD_WIDTH);
  for(i=0;i<FIELD_WIDTH;++i) {blocks[i].resize(FIELD_HEIGHT,false);}
  blocks[0][0]=true;
  blocks[FIELD_WIDTH-1][FIELD_HEIGHT-1]=true;
  test_field= new Field(blocks);
  blocks.clear();
  for(i=0;i<FIELD_WIDTH;++i)
    {
      for (j=0;j<FIELD_HEIGHT;++j)
	{
	  if ( (i==0 && j == 0) || (i==FIELD_WIDTH-1 && j==FIELD_HEIGHT-1) )
	    {
	      CPPUNIT_ASSERT( test_field->get(i,j) );
	    }
	  else { CPPUNIT_ASSERT( !(test_field->get(i,j) ) ); }
	}
    }
  delete test_field;

  // A non-full row placed over a full row. The non-full row should drop into
  // the space opened when the full row is cleared.
  blocks.resize(FIELD_WIDTH);
  for(i=0;i<FIELD_WIDTH;++i) {blocks[i].resize(FIELD_HEIGHT,false);}
  for(i=0;i<FIELD_WIDTH;++i)
    {
      blocks[i][0]=true;
      blocks[i][1]= ((i%2)?true:false);
    }
  test_field= new Field(blocks);
  blocks.clear();
  for(i=0;i<FIELD_WIDTH;++i)
    {
      for (j=0;j<FIELD_HEIGHT;++j)
	{
	  if ( j==0  )
	    {
	      CPPUNIT_ASSERT( (i%2)?test_field->get(i,j):(!test_field->get(i,j) ) );
	    }
	  else { CPPUNIT_ASSERT( !(test_field->get(i,j) ) ); }
	}
    }
  delete test_field;

  // Clear multiple rows.
  // Three full rows, two contiguous, one non contiguous,
  // and a row with alternating empty and full blocks above.
  // The row with alternating blocks should shift down 3 as the full
  // rows beneath it are removed.
  blocks.resize(FIELD_WIDTH);
  for(i=0;i<FIELD_WIDTH;++i) {blocks[i].resize(FIELD_HEIGHT,false);}
  for(i=0;i<FIELD_WIDTH;++i)
    {
      blocks[i][1]=true;
      blocks[i][2]=true;
      blocks[i][5]=true;
      blocks[i][6]=i%2?true:false;
    }
  test_field= new Field(blocks);
  blocks.clear();
  for(i=0;i<FIELD_WIDTH;++i)
    {
      for (j=0;j<FIELD_HEIGHT;++j)
	{
	  if( j==3 )
	    {
	      CPPUNIT_ASSERT( (i%2)?test_field->get(i,j):(!test_field->get(i,j) ) );
	    }
	  else { CPPUNIT_ASSERT( !(test_field->get(i,j) ) ); }
	}
    }

  // Full column.
  blocks.resize(FIELD_WIDTH);
  for(i=0;i<FIELD_WIDTH;++i) {blocks[i].resize(FIELD_HEIGHT,false);}
  for(j=0;j<FIELD_HEIGHT;++j) {blocks[0][j]=true;}
  test_field = new Field(blocks);
  blocks.clear();
  for(i=0;i<FIELD_WIDTH;++i)
    {
      for (j=0;j<FIELD_HEIGHT;++j)
	{
	  CPPUNIT_ASSERT( ( i==0 ? test_field->get(i,j) : !test_field->get(i,j) ) );
	}
    }

  // All rows full. The full rows should auto-clear.
  blocks.resize(FIELD_WIDTH);
  for(i=0;i<FIELD_WIDTH;++i) {blocks[i].resize(FIELD_HEIGHT,true);}
  test_field= new Field(blocks);
  blocks.clear();
  for(i=0;i<FIELD_WIDTH;++i)
    {
      for (j=0;j<FIELD_HEIGHT;++j)
	{
	  CPPUNIT_ASSERT( !(test_field->get(i,j) ) );
	}
    }
  delete test_field;
  /* Copy constructor
   */
  blocks.resize(FIELD_WIDTH);
  for(i=0;i<FIELD_WIDTH;++i) {blocks[i].resize(FIELD_HEIGHT,false);}
  for(j=0;j<FIELD_HEIGHT;++j) {blocks[0][j]=true;}
  test_field = new Field(blocks);
  blocks.clear();
  copy_field = new Field(*test_field);
  delete test_field;
  for(i=0;i<FIELD_WIDTH;++i)
    {
      for (j=0;j<FIELD_HEIGHT;++j)
	{
	  CPPUNIT_ASSERT( ( i==0 ? copy_field->get(i,j) : !copy_field->get(i,j) ) );
	}
    }
  delete copy_field;
  
  // Test for exception throws
  // blocks is clear
  CPPUNIT_ASSERT_THROW((test_field = new Field(blocks) ),FieldSizeError);

  // Throw if not 10 columns
  blocks.resize(FIELD_WIDTH-1);
  for(i=0;i<FIELD_WIDTH-1;++i) {blocks[i].resize(FIELD_HEIGHT,false);}
  CPPUNIT_ASSERT_THROW((test_field = new Field(blocks) ),FieldSizeError);

  blocks.resize(FIELD_WIDTH+1);
  blocks[FIELD_WIDTH-1].resize(FIELD_HEIGHT,false);
  CPPUNIT_ASSERT_THROW((test_field = new Field(blocks) ),FieldSizeError);

  blocks[FIELD_WIDTH].resize(FIELD_HEIGHT,false);
  CPPUNIT_ASSERT_THROW((test_field = new Field(blocks) ),FieldSizeError);

  // Throw if not every column is size FIELD_HEIGHT.
  blocks.resize(FIELD_WIDTH);
  blocks[5].clear();
  CPPUNIT_ASSERT_THROW((test_field = new Field(blocks) ),FieldSizeError);

  blocks[5].resize(FIELD_HEIGHT-1, false);
  CPPUNIT_ASSERT_THROW((test_field = new Field(blocks) ),FieldSizeError);
  
  blocks[5].resize(FIELD_HEIGHT+1, false);
  CPPUNIT_ASSERT_THROW((test_field = new Field(blocks) ),FieldSizeError);

  blocks[5].resize(FIELD_HEIGHT);
  blocks[0].clear();
  CPPUNIT_ASSERT_THROW((test_field = new Field(blocks) ),FieldSizeError);

  blocks[0].resize(FIELD_HEIGHT);
  blocks[FIELD_WIDTH-1].clear();
  CPPUNIT_ASSERT_THROW((test_field = new Field(blocks) ),FieldSizeError);

}

void FieldTest::testSet()
{
  Field test_field;
  int i,j;

  // Check that the corners set correctly
  test_field.set(0,0);
  CPPUNIT_ASSERT(test_field.get(0,0) );
  test_field.set(0,FIELD_HEIGHT-1);
  CPPUNIT_ASSERT(test_field.get(0,FIELD_HEIGHT-1) );
  test_field.set(FIELD_WIDTH-1,0);
  CPPUNIT_ASSERT(test_field.get(FIELD_WIDTH-1,0) );
  test_field.set(FIELD_WIDTH-1,FIELD_HEIGHT-1);
  CPPUNIT_ASSERT(test_field.get(FIELD_WIDTH-1,FIELD_HEIGHT-1) );

  // Check line removal; check moving lines above downward.
  for(i=1;i<FIELD_HEIGHT-1;++i)
    {
      test_field.set(i,0);
    }
  for(i=0;i<FIELD_HEIGHT;++i)
    {
      CPPUNIT_ASSERT( !(test_field.get(i,0) ) );
    }
  CPPUNIT_ASSERT( !(test_field.get(0,FIELD_HEIGHT-1) ) );
  CPPUNIT_ASSERT( !(test_field.get(FIELD_WIDTH-1,FIELD_HEIGHT-1) ) );
  CPPUNIT_ASSERT( test_field.get(0,20) );
  CPPUNIT_ASSERT( test_field.get(FIELD_WIDTH-1,20) );

  // Check that line removal does not disturb lower lines.
  test_field.set(5,2);
  for(i=0;i<FIELD_HEIGHT;++i)
    {
      test_field.set(i,3);
    }
  // The blocks that started in the upper corners have moved down again...
  CPPUNIT_ASSERT( !(test_field.get(0,20) ) );
  CPPUNIT_ASSERT( !(test_field.get(FIELD_WIDTH-1,20) ) );
  CPPUNIT_ASSERT( test_field.get(0,19) );
  CPPUNIT_ASSERT( test_field.get(FIELD_WIDTH-1,19) );
  // But the block beneath the cleared line is undisturbed, right?
  CPPUNIT_ASSERT( test_field.get(5,2) );
  CPPUNIT_ASSERT( !(test_field.get(5,1) ) );

  // Test for exception throws
  CPPUNIT_ASSERT_THROW( (test_field.set(-1,0) ),FieldSizeError );
  CPPUNIT_ASSERT_THROW( (test_field.set(0,-1) ),FieldSizeError );
  CPPUNIT_ASSERT_THROW( (test_field.set(FIELD_WIDTH,0) ),FieldSizeError );
  CPPUNIT_ASSERT_THROW( (test_field.set(0,FIELD_HEIGHT) ),FieldSizeError );
  CPPUNIT_ASSERT_THROW( (test_field.set(std::numeric_limits<int>::min(),0) ) , FieldSizeError );
  CPPUNIT_ASSERT_THROW( (test_field.set(std::numeric_limits<int>::max(),0) ) , FieldSizeError );
  if(std::numeric_limits<int>::has_infinity)
    {
      CPPUNIT_ASSERT_THROW( (test_field.set(std::numeric_limits<int>::infinity(),0) ) , FieldSizeError );
    }

  CPPUNIT_ASSERT_THROW( (test_field.set(5,2) ), DuplicateBlockError );
  // Test reset
  test_field.resetBlocks();
  for(j=0;j< FIELD_HEIGHT ;++j)
    {
      for (i=0;i<FIELD_WIDTH;++i)
	{
	  // The field should be empty, i.e. all gets should return false.
	  CPPUNIT_ASSERT( (!test_field.get(i,j) ) );
	}
    }
}

void FieldTest::testFieldScore()
{
  Field test_field;
  int i;
  // Score should begin at zero.
  CPPUNIT_ASSERT( (0==test_field.readScore() ) );
  // For now assume the simplest scoring system - 1 line clear is 1 point.
  for(i=0;i<FIELD_HEIGHT;++i)
    {
      test_field.set(i,0);
    }
  CPPUNIT_ASSERT( (1==test_field.readScore() ) );
  for(i=0;i<FIELD_HEIGHT;++i)
    {
      test_field.set(i,6);
    }
  CPPUNIT_ASSERT( (2==test_field.readScore() ) );

  // Test reset, and that increasing score after a reset works correctly.
  test_field.resetScore();
  CPPUNIT_ASSERT( (0==test_field.readScore() ) );
  for(i=0;i<FIELD_HEIGHT;++i)
    {
      test_field.set(i,19);
    }
  CPPUNIT_ASSERT( (1==test_field.readScore() ) );

}
