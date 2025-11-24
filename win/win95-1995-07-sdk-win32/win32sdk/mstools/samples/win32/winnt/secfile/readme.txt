
This sample demonstrates the following security related activities:

	- Constructing well-known security identifiers (SIDs)
	- Obtaining the current user's SID
	- Building a discretionary access control list (DACL)
	- Building a security descriptor (SD)

You should use this sample in conjunction with the Win32 API Reference.
Looking at the documentation while also seeing the API call used in a
piece of code should make both the documentation and the sample more
clear.

This sample will only work on a NTFS partition.  No other file system on
Windows NT supports security.

You can use File Manager to check the security on the file after you have
run the sample.  One thing you may notice is that the user "Everyone",
instead of being shown with "Read" access, has "Special" access.  That's
because File Manager considers "Read" access to include both read and
execute rights.  So, to modify the sample to give "Everyone" the File
Manager definition of "Read" access, change the line that says:

	#define WORLD_ACCESS	GENERIC_READ

to say:

	#define WORLD_ACCESS	(GENERIC_READ | GENERIC_EXECUTE)

