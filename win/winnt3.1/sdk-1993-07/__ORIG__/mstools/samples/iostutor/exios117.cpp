// exios117.cpp
// Using get with a buffer and length
#include <iostream.h>

void main()
{
   char line[25];
   cout << " Type a line terminated by carriage return\n>";
   cin.get( line, 25 );
   cout << ' ' << line;
}
