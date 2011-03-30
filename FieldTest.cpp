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
  std::vector< std::vector<bool> > rows;
  // Default constructor
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

  // Constructor with a single bottom row
  row.resize(10,true);
  row[9]=false;
  test_field = new Field(row);
  row.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  // The bottom row should be missing only the far right block
	  if(j==0 && i < 9) {CPPUNIT_ASSERT( (test_field->get(i,j)) );}
	  // All other rows should be empty.
	  else {CPPUNIT_ASSERT( (!test_field->get(i,j)) );}
	}
    }
  delete test_field;

  row.resize(10,false);
  row[0]=true;
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

  row.resize(10,true);
  test_field = new Field(row);
  row.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  // The full row should automatically clear itself, therefore:
	  // The field should be empty, i.e. all gets should return false.
	  CPPUNIT_ASSERT( (!test_field->get(i,j)) );
	}
    }
  delete test_field;

  // Constructor with bottom N rows
  rows.resize(22);
  for(j=0;j<22;++j) {rows[j].resize(10,false);}
  test_field= new Field(rows);
  rows.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  CPPUNIT_ASSERT( !(test_field->get(i,j)) );
	}
    }
  delete test_field;

  rows.resize(22);
  for(j=0;j<22;++j) {rows[j].resize(10,false);}
  rows[0][0]=true;
  rows[21][9]=true;
  test_field= new Field(rows);
  rows.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  if ( (i==0 && j == 0) || (i==9 && j==21) )
	    {
	      CPPUNIT_ASSERT( test_field->get(i,j) );
	    }
	  else { CPPUNIT_ASSERT( !(test_field->get(i,j)) ); }
	}
    }
  delete test_field;

  rows.resize(22);
  for(j=0;j<22;++j) {rows[j].resize(10,true);}
  test_field= new Field(rows);
  rows.clear();
  for(j=0;j<22;++j)
    {
      for (i=0;i<10;++i)
	{
	  // Full rows should auto-clear.
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
