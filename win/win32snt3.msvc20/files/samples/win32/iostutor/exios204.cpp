// exios204.cpp
// hstream Driver program copies 'cin' to 'cout' until end-of-file
#include "hstream.h"

hstreambuf hsb( 1 ); // 1=stdout

void main()
{
    char line[200];
    cout = &hsb; // Associates the HP LaserJet streambuf to cout
    while( 1 ) {
       cin.getline( line, 200 );
       if( !cin.good() ) break;
       cout << line << endl;
    }
}
