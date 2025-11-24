// exios118.cpp
// The istream getline member function
#include <iostream.h>

void main()
{
   char line[100];
   cout << " Type a line terminated by 't'" << endl;
   cin.getline( line, 100, 't' );
   cout << line;
}
