Building a Help Index

This file contains complete details on how to build a help index which
spans either a single, or multiple help files.  This functionality allows a user
to search for any word in a help file.

***NOTE: The Full Text Search functionality is specific to Windows NT. If
you intend to ship your application on other platforms, such as Windows 3.1, 
then you should not install the .IND file.  The .HLP files will still work, but
the Find button will not be registered.

1.  Copy the complete HLPINDEX directory to your local hard drive.

2.  Copy NMAKE.EXE from your \MSTOOLS\BIN directory into the HLPINDEX directory.

3.  Edit the BAG.INI file to correspond to your help project.  The sample file
    looks like this:

	[bag.ini]
	groupcount=2
	group1=hlpfile1
	group2=hlpfile2

	[hlpfile1]
	Indexfile=hlpfile1.ind
	Title=Sample Help File 1

	[hlpfile2]
	Indexfile=hlpfile1.ind
	Title=Sample Help File 2

    This BAG.INI will create an index file, hlpfile1.ind which contains the
    full text search index for the two help files HLPFILE1.HLP and HLPFILE2.HLP.
    Following are the descriptions of each element of the BAG.INI:

	groupcount -	The number of helpfiles to cross index.  Must be less 
			than 15.

	group[1-15] -	The name of each individual help file.

	[hlpfile1] -	Each individual group needs a separate configuration
			section, identified by the helpfile name.

	Indexfile -	The name of the resulting .IND file.  This must be the
			same for all groups.

	Title - 	The full name of the help file.  This appears in the
			Found dialog alongside each hit.

4.  Copy the BAG.INI into your development directory (wherever you build the 
    help file)

5.  Add the following lines to your .HPJ files (found in GENERIC.HPJ):

	[Baggage]
	bag.ini

	[config]
	RegisterRoutine("ftui","InitRoutines","SU")
	InitRoutines(qchPath,1)
	RegisterRoutine("ftui","SwitchToTopicsFound","U")
	AddAccelerator(0x46, 2, "SwitchToTopicsFound(hwndApp)")
	RegisterRoutine("ftui","ExecFullTextSearch","USSS")
	CreateButton("ftSearch","F&ind","ExecFullTextSearch(hwndApp,qchPath,`',`')")
			
    This will tell the help compiler to build in the hooks for the index file,
    and to provide the Find button in each help file.

6.  Rebuild all of your help files.

7.  Copy all of the help files into the HLPINDEX directory.

8.  Copy all of the GENERIC files to the same name as your index file.  For
    example if your index file was FOO.IND, then you would copy GENERIC.* to 
    FOO.*.

	NOTE: The GENERIC.STP file contains a list of words which will not be 
	      indexed.  You can add your own words to this list.

9.  Run the HLPINDEX batch file as follows:

	HLPINDEX FOO -	  Creates FOO.IND for one HLP file.
	HLPINDEX /m FOO - Creates FOO.IND to cross-index multiple HLP files.

10. You can now simply put all of the .HLP files and the .IND file in the same
    directory.  Start winhlp32 foo.hlp and hit the Find button for Full-Text
    Search.
