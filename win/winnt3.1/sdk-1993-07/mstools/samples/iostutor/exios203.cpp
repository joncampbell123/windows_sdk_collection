// exios203.cpp
// 2-argument manipulator example
#include <iostream.h>
#include <iomanip.h>

struct fillpair {
       char ch;
       int  cch;
};

IOMANIPdeclare( fillpair );

ostream& fp( ostream& os, fillpair pair )
{
    for ( int c = 0; c < pair.cch; c++ ) {
        os << pair.ch;
    }
    return os;
}

OMANIP(fillpair) fill( char ch, int cch )
{
    fillpair pair;

    pair.cch = cch;
    pair.ch  = ch;
    return OMANIP (fillpair)( fp, pair );
}


void main()
{
    cout << "10 dots coming" << fill( '.', 10 ) << "done" << endl;
}
