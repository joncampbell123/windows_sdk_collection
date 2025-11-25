// exios116.cpp
// The istream get member function
#include <iostream.h>

void main()
{
   char line[100], ch = 0, *cp;

   cout << " Type a line terminated by 'x'\n>";
   cp = line;
   while ( ch != 'x' )
   {
      cin >> ch;
      if( !cin.good() ) break; // Exits on EOF or failure
      *cp++ = ch;
   }
   *cp = '\0';
   cout << ' ' << line;
   cin.seekg( 0L, ios::end ); // Empties the input stream
   cout << "\n Type another one\n>";
   cp = line;
   ch = 0;
   while ( ch != 'x' )
   {
      cin.get( ch );
      if( !cin.good() ) break; // Exits on EOF or failure
      *cp++ = ch;
   }
   *cp = '\0';
   cout << ' ' << line;
}
