// exios201.cpp
// A custom manipulator with an integer parameter
#include <iostream.h>
#include <iomanip.h>

ostream& fb( ostream& os, int l )
{
   for( int i=0; i < l; i++ )
        os << ' ';
   return os;
}

OMANIP(int) fillblank( int l )
{
   return OMANIP(int) ( fb, l );
}

void main()
{
    cout << "10 blanks follow" << fillblank( 10 ) << ".\n";
}
