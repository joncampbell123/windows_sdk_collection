Summary:

    This sample demonstrates how to start a new process with various
    process attributes. It is a functional replacement for the
    "start" command.

More Information:

Starts a specified program, batch, or command file.
STARTP [/Ttitle] [/Dpath] [/l] [/h] [/r] [/min] [/max] [/w]
       [/c] [/n] [program] [parameters]

    title       Title to display in window title bar. Quote the
                entire paramter to include spaces in the title,
                i.e. startp "/Ttest job"
    path        Starting directory
    l           Set default to low priority
    h           Set default to high priority
    r           Set default to realtime priority
    min         Start window minimized
    max         Start window maximized
    w           Wait for started process to end before returning
                control to the command processor. This option starts
                the process synchronously
    c           Use current console instead of creating a new console
    b           Start detached with no console at all
    program     A batch file or program to run as either a GUI
                application or a console application
    parameters  These are the parameters passed to the program

Note that the priority parameters may have no effect if the program
changes its own priority.
