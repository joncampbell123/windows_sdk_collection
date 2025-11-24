// exios110.cpp
#include <fstream.h>
int iarray[2] = { 99, 10 };
void main()
{
    ofstream os( "test.dat" );
    os.write( (char *) iarray, sizeof( iarray ) );
}
