
                          A Word About .REG Files:

The OLE2 registration files contained in this sample tree all register their
servers using relative paths.  In order for OLE2 to successfully locate these
servers, they must be copied to the %systemroot%\system directory.  If this
is not done then OLE2 may not be able to locate the servers and the samples may
not run correctly.

The one exception to this rule is the MFract sample, which doesn't register
true OLE2 servers.  The fractal server DLLs it registers do not need to be
copied to the system directory; it is enough to ensure that the DLLs are in
the same directory as the MFract.EXE application file.

Registry entries for "real products" should not use relative paths.  Products
should register their servers with full paths so that they will not need to
copy anything to the system directory in order to function.


To build these samples, type "nmake -a" from the command prompt.
