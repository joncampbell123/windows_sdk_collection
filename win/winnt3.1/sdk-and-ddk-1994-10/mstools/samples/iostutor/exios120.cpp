// exios120.cpp
// The seekg member function
#include <fstream.h>

void main()
{
   char ch;

   ifstream tfile( "payroll", ios::binary | ios::nocreate );
   if( tfile ) {
      tfile.seekg( 8 );      // Seek eight bytes in (past salary)
      while ( tfile.good() ) { // EOF or failure stops the reading
         tfile.get( ch );
         if( !ch ) break; // quit on null
         cout << ch;
      }
   }
   else {
      cout << "ERROR: Cannot open file 'payroll'." << endl;
   }
}
