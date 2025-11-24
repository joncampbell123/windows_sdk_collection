// exios108.cpp
// The write member function
#include <fstream.h>

struct Date
{
   int mo, da, yr;
};

void main()
{
   Date dt = { 6, 10, 91 };
   ofstream tfile( "date.dat" , ios::binary );
   tfile.write( (char *) &dt, sizeof dt );
}
