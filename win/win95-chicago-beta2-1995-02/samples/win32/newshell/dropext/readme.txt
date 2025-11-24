See how it works after building DROPEXT.DLL, please update the registry
in following steps.

1. Build DROPEXT.DLL
2-a. Copy DROPEXT.DLL to the system directory (e.g., c:\windows\system)
 or
2-b. Modify DROPEXT.REG file to points to a fully qualified path to the DLL
3. Run "regedit dropext.reg"

Then, using the right-mouse button, drag a file (or multiple files)
from one folder to another, you'll see additional menuitems on drop.

