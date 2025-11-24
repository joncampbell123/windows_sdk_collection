All the TestProt Test Scripts are located in the 
\ddk\net\scripts directory.  The scripts directory, 
in its entirety, will need to be moved to the \tps 
directory when you install the Test Protocol.

The scripts directory is at this location because of 
limitations to the CDROM directory structure (must be 
nested less than 8 levels deep).  The final TPS directory 
structure when installed on your test machine should look 
like this:

\tps
    \scripts
            \1
              \1
                \*.*
              \2
                \*.*
              \3
                \*.*
            \2
              \1
                \*.*

