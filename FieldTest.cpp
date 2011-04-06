#include "FieldTest.hpp"
#include "Field.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FieldTest );

void FieldTest::setUp()
{
}

void FieldTest::tearDown()
{
}


void FieldTest::testConstructor()
{
  int i,j;
  Field * test_field;
  std::vector<bool> row;
  std::vector< std::vector<bool> > blocks;
  /* Default constructor
   */

  // Only one possible case: an empty field.
  test_field = new Field();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  // The field should be empty, i.e. all gets should return false.
	  CPPUNIT_ASSERT( (!test_field->get(i,j)) );
	}
    }
  delete test_field;

  /* Constructor with a single bottom row
   */

  // Emtpy field
  row.resize(10,false);
  test_field=new Field(row);
  row.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  CPPUNIT_ASSERT( !(test_field->get(i,j)) );
	}
    }
  delete test_field;

  // The bottom row should be missing only the far right block
  // All other rows should be empty.
  row.resize(10,true);
  row[9]=false;
  test_field = new Field(row);
  row.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  if(j==0 && i < 9) {CPPUNIT_ASSERT( (test_field->get(i,j)) );}
	  else {CPPUNIT_ASSERT( (!test_field->get(i,j)) );}
	}
    }
  delete test_field;

  // The bottom row should have only the far right block.
  // All other rows should be empty.
  row.resize(10,false);
  row[9]=true;
  test_field=new Field(row);
  row.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  if(j==0 && i == 0) {CPPUNIT_ASSERT( test_field->get(i,j) );}
	  else {CPPUNIT_ASSERT( !(test_field->get(i,j)) );}
	}
    }
  delete test_field;

  // A single block in the middle.
  row.resize(10,false);
  row[5]=true;
  test_field=new Field(row);
  row.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  if(i==5 && j==0) { CPPUNIT_ASSERT( test_field->get(i,j) ); }
	  else { CPPUNIT_ASSERT( !(test_field->get(i,j)) ); }
	}
    }
  delete test_field;

  // The full row should automatically clear itself, therefore:
  // The field should be empty, i.e. all gets should return false.
  row.resize(10,true);
  test_field = new Field(row);
  row.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  CPPUNIT_ASSERT( (!test_field->get(i,j)) );
	}
    }
  delete test_field;

  /* Constructor with full field
   */

  // Empty field
  blocks.resize(10);
  for(i=0;i<10;++i) {blocks[i].resize(22,false);}
  test_field= new Field(blocks);
  blocks.clear();
  for(i=0;i<10;++i)
    {
      for (j=0;j<22;++j)
	{
	  CPPUNIT_ASSERT( !(test_field->get(i,j)) );
	}
    }
  delete test_field;

  // Check the extreme corners
  blocks.resize(10);
  for(i=0;i<10;++i) {blocks[i].resize(22,false);}
  blocks[0][0]=true;
  blocks[9][21]=true;
  test_field= new Field(blocks);
  blocks.clear();
  for(i=0;i<10;++i)
    {
      for (j=0;j<22;++j)
	{
	  if ( (i==0 && j == 0) || (i==9 && j==21) )
	    {
	      CPPUNIT_ASSERT( test_field->get(i,j) );
	    }
	  else { CPPUNIT_ASSERT( !(test_field->get(i,j)) ); }
	}
    }
  delete test_field;

  // A non-full row placed over a full row. The non-full row should drop into
  // the space opened when the full row is cleared.
  blocks.resize(10);
  for(i=0;i<10;++i) {blocks[i].resize(22,false);}
  for(i=0;i<10;++i)
    {
      blocks[i][0]=true;
      blocks[i][1]= ((i%2)?true:false);
    }
  test_field= new Field(blocks);
  blocks.clear();
  for(i=0;i<10;++i)
    {
      for (j=0;j<22;++j)
	{
	  if ( i==0  )
	    {
	      CPPUNIT_ASSERT( (i%2)?test_field->get(i,j):(!test_field->get(i,j)) );
	    }
	  else { CPPUNIT_ASSERT( !(test_field->get(i,j)) ); }
	}
    }
  delete test_field;

  // Clear multiple rows. 

  // All rows full. The full rows should auto-clear.
  blocks.resize(10);
  for(i=0;i<10;++i) {blocks[i].resize(22,true);}
  test_field= new Field(blocks);
  blocks.clear();
  for(i=0;i<10;++i)
    {
      for (j=0;j<22;++j)
	{
	  CPPUNIT_ASSERT( !(test_field->get(i,j)) );
	}
    }
  delete test_field;  
  CPPUNIT_FAIL( "test case not complete" );
}

void FieldTest::testSet()
{
  CPPUNIT_FAIL( "not implemented" );
}

void FieldTest::testFieldScore()
{
  CPPUNIT_FAIL( "not implemented" );
}

void FieldTest::testFieldSizeException()
{
  CPPUNIT_FAIL( "not implemented" );
}
