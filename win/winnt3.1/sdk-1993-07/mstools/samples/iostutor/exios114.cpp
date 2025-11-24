// exios114.cpp
// Overloading the << operator
#include <iostream.h>

class Date
{
   int mo, da, yr;
public:
   Date( int m, int d, int y )
   {
      mo = m; da = d; yr = y;
   }
   friend ostream& operator<< ( ostream& os, Date& dt );
};

ostream& operator<< ( ostream& os, Date& dt )
{
   os << dt.mo << '/' << dt.da << '/' << dt.yr;
   return os;
}

void main()
{
   Date dt( 5, 6, 77 );
   cout << dt;
}
