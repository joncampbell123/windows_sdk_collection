// exios119.cpp
// The istream read function
#include <fstream.h>
#include <fcntl.h>
#include <io.h>

void main()
{
   struct
   {
      double salary;
      char name[23];
   } employee;

   ifstream is( "payroll", ios::binary | ios::nocreate );
   if( is ) {  // ios::operator void*()
      is.read( (char *) &employee, sizeof( employee ) );
      cout << employee.name << ' ' << employee.salary << endl;
   }
   else {
      cout << "ERROR: Cannot open file 'payroll'." << endl;
   }
}
