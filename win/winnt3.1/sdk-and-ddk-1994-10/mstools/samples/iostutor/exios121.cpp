// exios121.cpp
// The tellg function
#include <fstream.h>

void main()
{
   char ch;

   ifstream tfile( "payroll", ios::binary | ios::nocreate );
   if( tfile ) {
       while ( tfile.good() ) {
          streampos here = tfile.tellg();
          tfile.get( ch );
          if ( ch == ' ' )
             cout << "\nPosition " << here << " is a space";
       }
   }
   else {
      cout << "ERROR: Cannot open file 'payroll'." << endl;
   }
}
