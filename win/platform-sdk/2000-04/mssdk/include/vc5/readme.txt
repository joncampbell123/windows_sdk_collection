These versions of rpcndr.h and rpcproxy.h are provided if you need to work in both
the Platform SDK build environment and with the Microsoft Visual C++ 5.0. You will 
need to use these files only if you distribute the generated files--built with MIDL.EXE
from the Platform SDK--to other developers that are not using the updated build environment.

To use them for a specific project you can either replace them in your SDK\Include directory
or you can change you INCLUDE path to have these first.

Please keep in mind these are only need for distributing the generated build files. They
are not needed for targeting different versions of Windows.
