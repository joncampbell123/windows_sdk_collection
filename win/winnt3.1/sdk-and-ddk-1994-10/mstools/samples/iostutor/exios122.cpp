// exios122.cpp
// An fstream file
#include <fstream.h>
#include <ctype.h>

void main()
{
   fstream tfile( "test.dat", ios::in | ios::app );
   char tdata[100];
   int i = 0;
   char ch;

   while ( i < 100 ) {
      tfile.get( ch );
      if ( tfile.istream::eof() ) break;
      tdata[i++] = ch;
   }
   tfile.istream::clear();
   for ( int j = 0; j < i; j++ ) {
      tfile.put( (char)toupper( tdata[j] ) );
   }
}
