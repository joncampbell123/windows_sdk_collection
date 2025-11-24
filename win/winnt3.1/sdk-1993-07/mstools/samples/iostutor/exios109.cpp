// exios109.cpp
// A buffered stream object
#include <iostream.h>
#include <time.h>

void main()
{
   time_t tm = time( NULL ) + 5;
   cout << "Please wait...";
   while ( time( NULL ) < tm )
      ;
   cout << "\nAll done" << endl;
}
