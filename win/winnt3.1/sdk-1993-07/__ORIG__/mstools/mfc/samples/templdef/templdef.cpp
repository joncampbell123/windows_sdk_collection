// templdef.cpp : A simple C++ template expansion utility.
//
//   This utility takes a template file which uses a syntax
//   subset similar to the proposed future C++ template
//   (parameterized type) syntax, and expands it into the
//   equivalent current C++ syntax.
//
//   Note that this is intended as an example program, and
//   handles only a specific syntax in the template files.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include <ctype.h>
#include <afxcoll.h>

#define istringTypeParamMax 10

CString string_h;
CString string_cpp;
CString string_ctt;

CString stringTemplate = "template";
CString stringClass = "class";
CString stringTemplateName;
CString stringTypedefName;

const char chMoreThanOne = '\0';
const char chNot = '\0';

/////////////////////////////////////////////////////////////////////////////

// UsageErr:
// A utility function.  Write an error message, followed by a usage summary.
//
void UsageErr(const char* szErrorMessage = NULL,
			  const char* szErrorParam = NULL)
{
	if (szErrorMessage != NULL)
	{
		fprintf(stderr, "templdef : error: ");
		fprintf(stderr, szErrorMessage, szErrorParam);
		fprintf(stderr, ".\n\n");
	}

	fprintf(stderr, "templdef : usage:\n\n"
		"  Suppose a class CMap exists in the template file map.ctt, which\n"
		"  has two parameterized types Foo and Bar. To create a new class\n"
		"  CMapFooToBar, declared in footobar.h and defined in footobar.cpp,\n"
		"  use the following command:\n"
		"\n"
		"  templdef \"CMap<Foo, Bar> CMapFooToBar\" map.ctt footobar.h footobar.cpp\n"
		"\n"
		"  This command is similar to the proposed future C++ syntax for templates:\n"
		"\n"
		"  typedef CMap<Foo, Bar> CMapFooToBar;\n");

	exit(1);
}

void CheckSyntax(BOOL fGoodSyntax, const char* sz)
{
	if (!fGoodSyntax)
		UsageErr("expecting %s", sz);
}

/////////////////////////////////////////////////////////////////////////////
// CTokenFile
// For the purpose of reading source code, this sort of CStdioFile derivative
// proves quite handy.  This could be made really powerful, but this is a
// start.

class CTokenFile : public CStdioFile
{
private:
	enum { ichMax = 1024 };
	char ach[ichMax];
	int ich;
	static CString stringToken;
	BOOL fComment;
	void GetBuf();
	char GetChar() { if (ach[ich] == chNot) GetBuf(); return ach[ich++]; }
	char PeekChar() { return ach[ich]; }
	void UnGetChar() { --ich; } // note: really a single char pushback
public:
	void InitBuf(const char* sz);
	static CString& TokenString() { return stringToken; }
	CTokenFile();
	char GetToken();
	char GetPrintToken();
	char GetTypeToken();
	char GetNameToken();
	void PutToken(char ch) { fputc(ch, m_pStream); }
	void PutToken(const CString& string) { WriteString(string); }
	void PutToken() { WriteString(stringToken); }
	BOOL AtFileEnd() { return ach[0] == chNot; }
	BOOL AtBufEnd() { return ach[ich] == chNot; }
	void SafeOpen(const CString& string, UINT nStyleFlags);
};
CString CTokenFile::stringToken;

CTokenFile::CTokenFile()
{
	ach[0] = '\n';
	ach[1] = chNot;
	ich = 1;
	fComment = FALSE;
};

void CTokenFile::GetBuf()
{
	if (ReadString(ach, ichMax) == NULL)
		ach[0] = chNot;

	ich = 0;
}

void CTokenFile::InitBuf(const char* sz)
{
	strncpy(ach, sz, ichMax);
	ich = 0;
}

void CTokenFile::SafeOpen(const CString& string, UINT nStyleFlags)
{
	BOOL fSuccess = Open(string, nStyleFlags, 0);

	if (!fSuccess)
		UsageErr("can't open file \"%s\"", string);
}

char CTokenFile::GetToken()
{
	if (AtBufEnd())
		GetBuf();
	
	if (AtFileEnd())
		exit(0);

	fComment = FALSE;

	char ch = GetChar();
	char* pch = stringToken.GetBuffer(1024);
	char* pchInit = pch;
	*pch++ = ch;
	*pch = '\0';

	// assuming this doesn't "really" release the buffer!
	stringToken.ReleaseBuffer(1);

	if (!isalnum(ch) &&
		ch != '/'    &&
		ch != '_'    &&
		ch != '\''   &&
		ch != '"'    &&
		ch != '#')
	{
		return ch;
	}

	if ((ch == '\'') || (ch == '"'))
	{
		char ch2;
		while ((ch2 = GetChar()) != ch)
		{
			*pch++ = ch2;
		}
		*pch++ = ch2;
		stringToken.ReleaseBuffer(pch - pchInit);
		return chMoreThanOne;
	}

	if (ch == '/')
	{   
		char ch2 = GetChar();
		if (AtFileEnd()) exit(0);
		if (ch2 == '/')
		{
			char ch3 = GetChar();
			if (ch3 != '$')
			{
				UnGetChar();
				*pch++ = '/';
				fComment = TRUE;
				while ((ch3 = GetChar()) != '\n')
					*pch++ = ch3;
			}
			else
			{
				stringToken.ReleaseBuffer(0);
			}
			*pch++ = '\n';
			stringToken.ReleaseBuffer(pch - pchInit);
			return chMoreThanOne;
		}
		else if (ch2 == '*')
		{
			char ch3 = chNot;
			char ch4 = chNot;
			*pch++ = '*';
			fComment = TRUE;
			while (ch4 != '/')
			{
				while ((ch3 = GetChar()) != '*')
					*pch++ = ch3;
				*pch++ = '*';   

				ch4 = GetChar();
				if (ch4 != '/')
					UnGetChar();
			}
			*pch++ = '/';
			stringToken.ReleaseBuffer(pch - pchInit);
			return chMoreThanOne;
		}
		else
		{
			UnGetChar();
			return ch;
		}
	}

	if (isdigit(ch))
	{
		char ch2;
		ch2 = GetChar();
		if (!isalpha(ch2))
		{
			UnGetChar();
			return ch;
		}

		*pch++ = ch2;
		stringToken.ReleaseBuffer(pch - pchInit);
		return chMoreThanOne;
	}

	while ((!AtFileEnd()) &&
		(ch = GetChar(), (isalnum(ch) || (ch == '_'))))
	{
		*pch++ = ch;
	}

	if (!AtFileEnd())
		UnGetChar();

	stringToken.ReleaseBuffer(pch - pchInit);
	return chMoreThanOne;
}

char CTokenFile::GetPrintToken()
{
	char chToken;
	while (((chToken = GetToken()) != chMoreThanOne) &&
		   (isspace(chToken)) || fComment)
		/* try again */ ;

	return chToken;
}

char CTokenFile::GetTypeToken()
{
	char chToken = GetPrintToken();
	CString typeStr = stringToken;
	char chFirst = typeStr[0];

	CheckSyntax( 
		isalnum(chFirst) || 
		chFirst == '_'   || 
		chFirst == '\''  || 
		chFirst == '"',
		"a type description or constant starting with an\n"
		"alphanumeric or an underbar, or a string or char constant");

	while ((chToken = GetPrintToken()) != ',' && chToken != '>')
	{
		if (chToken != '*' && chToken != '&')
			typeStr += ' ';

		typeStr += stringToken;
	}

	stringToken = typeStr;
	return chToken;
}

char CTokenFile::GetNameToken()
{
	GetPrintToken();

	CheckSyntax((isalpha(stringToken[0]) || stringToken[0] == '_'),
		"a name token starting in an alpha char or underbar");

	int l = stringToken.GetLength();
	for (int i=1; i<l; ++i)
	{
		CheckSyntax(isalnum(stringToken[i]) || stringToken[i] == '_',
			"a name token consisting of alphanumerics or underbars");
	}

	CheckSyntax(stringToken != stringTemplate, "a name token");
	CheckSyntax(stringToken != stringClass, "a name token");

	return chMoreThanOne;
}

/////////////////////////////////////////////////////////////////////////////

CMapStringToString map;

BOOL isTailMatch(CString string, CString tail)
{
	BOOL retval = string.Right(tail.GetLength()) == tail;
	ASSERT(retval);
	return retval;
}

CTokenFile file_h;
CTokenFile file_cpp;
CTokenFile file_ctt;

static BOOL fEnableIfElseEating = FALSE;

// TranslateTo:
// Copy file_ctt to file_out, translating words as necessary.
// On the way, swallow things outside of blocks and after
// the "template" word inside of '<' '>' brackets.
//
// Also, on the way, any #if or #else statements that are conditional
// on a constant "1" or "0" template parameter will be swallowed as
// appropriate.
//
void TranslateTo(CTokenFile& file_out)
{
	char chToken;
	static BOOL fswallowParams = TRUE;
	BOOL fswallow = FALSE;
	BOOL finIf = FALSE;
	BOOL finElse = FALSE;
	BOOL fswallowIf = FALSE;
	BOOL fswallowElse = FALSE;
	CString stringTokenOut;

	while ((chToken = file_ctt.GetToken()) != chMoreThanOne ||
		   file_ctt.TokenString() != "IMPLEMENT_TEMPLATE")
	{
		if (chToken != chMoreThanOne)
		{
			if (chToken == '{')
				fswallowParams = FALSE;
			if (fswallowParams && (chToken == '<'))
			{
				while (chToken != '>')
					chToken = file_ctt.GetToken();
			}
			else
			{
				if (!fswallow)
					file_out.PutToken();
			}
		}
		else
		{
			if (fEnableIfElseEating && file_ctt.TokenString()[0] == '#')
			{
				if (file_ctt.TokenString() == "#if")
				{   
					file_ctt.GetPrintToken();
					if (map.Lookup(file_ctt.TokenString(), stringTokenOut))
					{
						if (stringTokenOut == "0")
						{
							fswallowIf = TRUE;
							fswallowElse = FALSE;
						}
						else
						{
							if (stringTokenOut == "1");
							{
								fswallowElse = TRUE;
								fswallowIf = FALSE;
							}
						}

						if (fswallowIf || fswallowElse)
						{
							file_ctt.TokenString() = "";
							finIf = TRUE;
						}

						fswallow = fswallowIf;
					}
					else
					{
						file_out.PutToken("#if ");
					}
				}
				else if (file_ctt.TokenString() == "#else")
				{
					if (finIf)
					{
						file_ctt.TokenString() = "";
						finIf = FALSE;
						finElse = TRUE;
					}

					fswallow = fswallowElse;
				}
				else if (file_ctt.TokenString() == "#endif")
				{
					if (finIf || finElse)
					{
						file_ctt.TokenString() = "";

						// eat any evil
						//junk after the #endif
						while ((chToken = file_ctt.GetToken()) != '\n' &&
							file_ctt.TokenString()[0] != '/')
							/* spin */ ;

						if (chToken != '\n')
							file_ctt.GetToken();
					}
					finIf = FALSE;
					finElse = FALSE;
					fswallowIf = FALSE;
					fswallowElse = FALSE;
					fswallow = FALSE;
				}
			}

			if (file_ctt.TokenString() == stringTemplate)
				fswallowParams = TRUE;

			if (map.Lookup(file_ctt.TokenString(), stringTokenOut))
			{
				if (!fswallow)
					file_out.PutToken(stringTokenOut);
			}
			else
			{
				if (!fswallow)
					file_out.PutToken();
			}
		}
	}
}

// main:
// Gets the arguments, checks them, then processes the files.
//
main(int argc, char* argv[])
{
	CString stringTypes;
	{
		for (int i=1; i<(argc-3); ++i)
		{
			stringTypes += CString(argv[i]) + ' ';
		}
	}

	if (argc < 4)
		UsageErr(NULL, NULL);

	// Copy the template, header and source file name arguments.
	//
	string_ctt = argv[argc-3];
	string_h   = argv[argc-2];
	string_cpp = argv[argc-1];

	// Check to make sure that the args are in the right order.
	//
	if (!isTailMatch(string_cpp, ".cpp") &&
		!isTailMatch(string_cpp, ".cxx"))
		UsageErr("module file should have a .cpp or .cxx extension");

	if (!isTailMatch(string_ctt, ".ctt"))
		UsageErr("template file should have a .ctt extension");

	if (!isTailMatch(string_h, ".h") &&
		!isTailMatch(string_h, ".hpp") &&
		!isTailMatch(string_h, ".hxx"))
		UsageErr("header file should have an .h, .hpp or .hxx extension");

	// Open the files.
	//
	file_ctt.SafeOpen(string_ctt, CTokenFile::modeRead);
	file_h.SafeOpen(string_h,
		CTokenFile::modeWrite | CTokenFile::modeCreate);
	file_cpp.SafeOpen(string_cpp,
		CTokenFile::modeWrite | CTokenFile::modeCreate);

	// Push the command line onto the file buffer, so that we can parse it
	// using our standard tool set.
	//
	file_ctt.InitBuf(stringTypes);

	int chToken;
	CString astringTypeParam[istringTypeParamMax];
	int istringTypeParam = 0;

	file_ctt.GetNameToken();
	stringTemplateName = file_ctt.TokenString();

	chToken = file_ctt.GetPrintToken();
	CheckSyntax(chToken == '<', "'<'");

	do
	{
		chToken = file_ctt.GetTypeToken();
		if ((file_ctt.TokenString() == "0") ||
			(file_ctt.TokenString() == "1"))
		{
			fEnableIfElseEating = TRUE;
		}
		astringTypeParam[istringTypeParam++] = file_ctt.TokenString();
		CheckSyntax(istringTypeParam < istringTypeParamMax,
			"fewer parameterized types (program limit)");
	} while (chToken == ',');

	CheckSyntax(chToken == '>', "'>'");

	file_ctt.GetNameToken();
	stringTypedefName = file_ctt.TokenString();

	map.SetAt(stringTemplate, " ");
	map.SetAt(stringTemplateName, stringTypedefName);

	// Done processing the command line part, now eat any initial comments
	// appearing before the //$DECLARE_TEMPLATE flag.
	//
	while ((chToken = file_ctt.GetPrintToken()) != chMoreThanOne ||
		   file_ctt.TokenString() != "DECLARE_TEMPLATE")
	{
		/* spin */ ;
	}

	while ((file_ctt.GetToken() != chMoreThanOne) ||
		   (file_ctt.TokenString() != stringTemplate))
	{   
		file_h.PutToken();
	}

	// Token must now be "template".
	
	// Eat opening '<'.
	//
	chToken = file_ctt.GetPrintToken();

	// Now get a list of type parameters.
	//
	int istringTypeParamMaxPrev = istringTypeParam;
	istringTypeParam = 0;
	while (chToken !='>')
	{
		CString stringParamName;

		// The parameter name is the last thing before a ',' or '>'.
		//
		while ((chToken = file_ctt.GetPrintToken()) != ',' && chToken != '>')
		{
			stringParamName = file_ctt.TokenString();
		}

		map.SetAt(stringParamName, astringTypeParam[istringTypeParam++]);
	}

	CheckSyntax(istringTypeParam == istringTypeParamMaxPrev,
		"same number of template parameters");

	// Copy template to header file, translating words as necessary,
	// terminating when the //$IMPLEMENT_TEMPLATE flag is hit.
	//
	TranslateTo(file_h);

	// Copy template to source file, translating words as necessary.
	//
	TranslateTo(file_cpp);

	return 0;
}
