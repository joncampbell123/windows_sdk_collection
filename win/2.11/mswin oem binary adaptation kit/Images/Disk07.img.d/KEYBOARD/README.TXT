                                                01-August-1988

        We have included the sample keyboard drivers to create the
United States English and German drivers.  If you have a full OEM kit, 
the additional keyboard driver sources are located on another disk in your kit.
If you have the Device Driver kit ONLY and need other languages, please 
send a TAR or Service Request through DIAL or ONLINE to the attention of 
the Windows OEM Kit support group.

                                How to Make

        There is a directory called COMMON which contains files common to 
all keyboard drivers.  You should run the Makefile in this directory first
to create the .OBJ files for use by each specific keyboard driver that you
build.  Make sure that you copy KERNEL.LIB to a directory which is on your
LIB search environment.

Thanks
