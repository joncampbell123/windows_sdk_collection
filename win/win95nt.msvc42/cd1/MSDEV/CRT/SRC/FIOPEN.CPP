// fiopen -- filebuf::_Fiopen(const char *, ios::openmode)
#include <fstream>

FILE *__cdecl filebuf::_Fiopen(const char *name, ios_base::openmode mode)
	{	// open a file
	static const char *mods[] = {
		"r", "w", "a", "rb", "wb", "ab", "r+", "w+", "a+",
			"r+b", "w+b", "a+b", 0};
	static const int valid[] = {
		ios_base::in,
		ios_base::out | ios_base::trunc,
		ios_base::out | ios_base::app,
		ios_base::in | ios_base::binary,
		ios_base::out | ios_base::trunc | ios_base::binary,
		ios_base::out | ios_base::app | ios_base::binary,
		ios_base::in | ios_base::out,
		ios_base::in | ios_base::out | ios_base::trunc,
		ios_base::in | ios_base::out | ios_base::app,
		ios_base::in | ios_base::out | ios_base::binary,
		ios_base::in | ios_base::out | ios_base::trunc
			| ios_base::binary,
		ios_base::in | ios_base::out | ios_base::app
			| ios_base::binary,
		0};
	FILE *fp;
	int n;
	ios_base::openmode atefl = mode & ios_base::ate;
	mode &= ~ios_base::ate;
	for (n = 0; valid[n] != 0 && valid[n] != mode; ++n)
		;
	if (valid[n] == 0 || (fp = fopen(name, mods[n])) == 0)
		return (0);
	if (!atefl || fseek(fp, 0, SEEK_END) == 0)
		return (fp);
	fclose(fp);	// can't position at end
	return (0);
	}

/*
 * Copyright (c) 1994 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */
