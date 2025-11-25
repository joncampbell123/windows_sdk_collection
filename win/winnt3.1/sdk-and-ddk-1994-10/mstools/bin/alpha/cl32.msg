		            CLAXP COMPILER OPTIONS

				-OPTIMIZATION-
/Ox Like /O, but /Ob1 (default has -Oi-) /O  Maximum speed, /Oi /Ob2
/O2 Maximum speed (same as /O)           /Ob<0|1|2> control inline expansion
/O1 Speed without big code               /Od no opt (default if /Zi and no /O*)
/Oi- no intrinsic functions (default)    /Oi enable intrinsic functions
/Op precision-sensitive floats (default) /Op- trade float precision for speed

			       -CODE GENERATION-
/Ge enable stack limit checks (default)  /Gs suppress stack limit checks
/Gf enable string pooling                /Gz use standard calling convention
/Gt<n> threshold for gp-relative data

				-OUTPUT FILES-
/FA<a|c|s> select <asm|codebytes|source> /Fa[file] file for /FA-selected info
/Fm[file] linker map file                /Fe[file] executable file
/Fp[file] precompiled header file        /Fo[file] object file
/Fr[file] source browser info file       /FR[file] extended source browser info

				-PREPROCESSOR-
/C don't strip comments                  /D<name>[=|#text] define macro
/E preprocess to stdout                  /U<name> remove predefined macro
/EP preprocess to stdout, no #line       /u remove all predefined macros
/P preprocess to file, no #line          /I<dir> add to include search path
/X ignore "standard places"              /FI<file> include file at line 0

				  -LANGUAGE-
/Za disable non ansi extensions          /Zs syntax check only
/Ze enable extensions                    /Zg generate function prototypes
/Zd include line number debug info       /Zp[n] pack structs on n-byte boundary
/Zi include Windbg debug info            /Zh home arguments (low-level debug)
/Z7 include Windbg debug info

				-MISCELLANEOUS-
/c compile only, no link                 /nologo suppress copyright message
/H<number> max external name length      /J default char type is unsigned
/Yc<file> create .PCH file               /Yd put debug info in .PCH file
/Yu<file> use .PCH file                  /YX use automatic precompiled headers
/Tc<source file> compile file as .c      /Tp<source file> compile file as .cpp
/W<n> warning level 0, 1, 2, 3, 4        /WX treat a warning as an error
/w Suppress warnings (same as /W0)       /V<string> set version string
/Zn turn off SBRPACK utility             /batch specify batch mode compilation
/help,/HELP,/?  print this message

				  -LINKING-
/F<number> stack size (hex. bytes)       /link pass rest of line to the linker
