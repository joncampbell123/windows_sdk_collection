//--------------------------------------------------------------------------
//
// Module Name:  MAPPING.H
//
// Brief Description:  This module contains defines and structures
//               necessary for the PSCRIPT driver's character
//               mapping between Adobe's encoding vectors and
//               UNICODE.
//
// Author:  Kent Settle (kentse)
// Created: 07-Mar-1991
//
// Copyright (c) 1991 Microsoft Corporation
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// NT PostScript Driver character mapping scheme.
//
// UCMap contains three fields.  these three fields contain four pieces
// of information, necessary for optimal mapping between Adobe encoding
// vectors and Unicode.  the first field is a pointer to a string which
// contains the ASCII name of the PostScript character.  usPSValue
// contains two pieces of information.    If the high bit is set, then
// the current character is not in the Adobe standard encoding vector.
// what this means is that if we try to print one of these character,
// we must first re-encode the font to match our tables.  the reason
// we set this bit, as opposed to always redefining the font, is for
// speed.  many applications will simply print with the standard ASCII
// characters, and will never need to redefine the font.  the remainder
// of usPSValue is the character index in our internal, remapping scheme.
// for example, if the high bit is not set, then the character index in
// usPSValue for the given character is the same as the Adobe standard.
// the third field is usUCValue.  this is the Unicode index of the given
// character.  it may be noted that some character exists under PostScript,
// which do not exist under Unicode.  in this case, it is not worth
// dragging the extra data around, however, I have commented those
// characters out in place, in case they do become defined in Unicode
// at a later date.
//
//   11-Mar-1991     -by-     Kent Settle     (kentse)
//--------------------------------------------------------------------------


// LatinMap provides a standardized mapping which contains all currently
// known Adobe Latin characters. SymbolMap contains all the currently
// known symbol characters.  each character in this table is, in fact,
// at the same location as defined by Adobe's Symbol font.  therefore,
// it will never be necessary to remap the symbol font.  DingbatsMap
// contains all the currently know ZapfDingbats characters.  as with
// the symbol font, it will never be necessary to remap Dingbats.
//
// these three tables will provide the same font abilities as Windows
// and PM.  as more Adobe encoding vectors become known, such as for
// Chinese, Japanese, etc, mapping tables can be added here for each
// of them.
//
// NOTE: each table is sorted by UNICODE value.

//!!! put into resource - kentse.

static UCMap  LatinMap[] =
{
    "space",            0x0020,         0x0020,
    "exclam",           0x0021,         0x0021,
    "quotedbl",         0x0022,         0x0022,
    "numbersign",       0x0023,         0x0023,
    "dollar",           0x0024,         0x0024,
    "percent",          0x0025,         0x0025,
    "ampersand",        0x0026,         0x0026,
    "quotesingle",      0x808F,         0x0027,
    "parenleft",        0x0028,         0x0028,
    "parenright",       0x0029,         0x0029,
    "asterisk",         0x002A,         0x002A,
    "plus",             0x002B,         0x002B,
    "comma",            0x002C,         0x002C,
    "hyphen",           0x002D,         0x002D,
    "period",           0x002E,         0x002E,
    "slash",            0x002F,         0x002F,
    "zero",             0x0030,         0x0030,
    "one",              0x0031,         0x0031,
    "two",              0x0032,         0x0032,
    "three",            0x0033,         0x0033,
    "four",             0x0034,         0x0034,
    "five",             0x0035,         0x0035,
    "six",              0x0036,         0x0036,
    "seven",            0x0037,         0x0037,
    "eight",            0x0038,         0x0038,
    "nine",             0x0039,         0x0039,
    "colon",            0x003A,         0x003A,
    "semicolon",        0x003B,         0x003B,
    "less",             0x003C,         0x003C,
    "equal",            0x003D,         0x003D,
    "greater",          0x003E,         0x003E,
    "question",         0x003F,         0x003F,
    "at",               0x0040,         0x0040,
    "A",                0x0041,         0x0041,
    "B",                0x0042,         0x0042,
    "C",                0x0043,         0x0043,
    "D",                0x0044,         0x0044,
    "E",                0x0045,         0x0045,
    "F",                0x0046,         0x0046,
    "G",                0x0047,         0x0047,
    "H",                0x0048,         0x0048,
    "I",                0x0049,         0x0049,
    "J",                0x004A,         0x004A,
    "K",                0x004B,         0x004B,
    "L",                0x004C,         0x004C,
    "M",                0x004D,         0x004D,
    "N",                0x004E,         0x004E,
    "O",                0x004F,         0x004F,
    "P",                0x0050,         0x0050,
    "Q",                0x0051,         0x0051,
    "R",                0x0052,         0x0052,
    "S",                0x0053,         0x0053,
    "T",                0x0054,         0x0054,
    "U",                0x0055,         0x0055,
    "V",                0x0056,         0x0056,
    "W",                0x0057,         0x0057,
    "X",                0x0058,         0x0058,
    "Y",                0x0059,         0x0059,
    "Z",                0x005A,         0x005A,
    "bracketleft",      0x005B,         0x005B,
    "backslash",        0x005C,         0x005C,
    "bracketright",     0x005D,         0x005D,
    "asciicircum",      0x005E,         0x005E,
    "underscore",       0x005F,         0x005F,
    "grave",            0x8000,         0x0060,
    "a",                0x0061,         0x0061,
    "b",                0x0062,         0x0062,
    "c",                0x0063,         0x0063,
    "d",                0x0064,         0x0064,
    "e",                0x0065,         0x0065,
    "f",                0x0066,         0x0066,
    "g",                0x0067,         0x0067,
    "h",                0x0068,         0x0068,
    "i",                0x0069,         0x0069,
    "j",                0x006A,         0x006A,
    "k",                0x006B,         0x006B,
    "l",                0x006C,         0x006C,
    "m",                0x006D,         0x006D,
    "n",                0x006E,         0x006E,
    "o",                0x006F,         0x006F,
    "p",                0x0070,         0x0070,
    "q",                0x0071,         0x0071,
    "r",                0x0072,         0x0072,
    "s",                0x0073,         0x0073,
    "t",                0x0074,         0x0074,
    "u",                0x0075,         0x0075,
    "v",                0x0076,         0x0076,
    "w",                0x0077,         0x0077,
    "x",                0x0078,         0x0078,
    "y",                0x0079,         0x0079,
    "z",                0x007A,         0x007A,
    "braceleft",        0x007B,         0x007B,
    "bar",              0x007C,         0x007C,
    "braceright",       0x007D,         0x007D,
    "asciitilde",       0x007E,         0x007E,
    "space",            0x0020,         0x00A0,
    "exclamdown",       0x00A1,         0x00A1,
    "cent",             0x00A2,         0x00A2,
    "sterling",         0x00A3,         0x00A3,
    "currency",         0x80A4,         0x00A4,
    "yen",              0x80A5,         0x00A5,
    "brokenbar",        0x80A6,         0x00A6,
    "section",          0x80A7,         0x00A7,
    "dieresis",         0x800B,         0x00A8,
    "copyright",        0x80A9,         0x00A9,
    "ordfeminine",      0x80AA,         0x00AA,
    "guillemotleft",    0x80AB,         0x00AB,
    "logicalnot",       0x80AC,         0x00AC,
    "hyphen",           0x002D,         0x00AD,
    "registered",       0x80AE,         0x00AE,
    "macron",           0x8004,         0x00AF,
    "degree",           0x80B0,         0x00B0,
    "plusminus",        0x80B1,         0x00B1,
    "twosuperior",      0x80B2,         0x00B2,
    "threesuperior",    0x80B3,         0x00B3,
    "acute",            0x8001,         0x00B4,
    "mu",               0x80B5,         0x00B5,
    "paragraph",        0x80B6,         0x00B6,
    "periodcentered",   0x80B7,         0x00B7,
    "cedilla",          0x8007,         0x00B8,
    "onesuperior",      0x80B9,         0x00B9,
    "ordmasculine",     0x80BA,         0x00BA,
    "guillemotright",   0x80BB,         0x00BB,
    "onequarter",       0x80BC,         0x00BC,
    "onehalf",          0x80BD,         0x00BD,
    "threequarters",    0x80BE,         0x00BE,
    "questiondown",     0x80BF,         0x00BF,
    "Agrave",           0x80C0,         0x00C0,
    "Aacute",           0x80C1,         0x00C1,
    "Acircumflex",      0x80C2,         0x00C2,
    "Atilde",           0x80C3,         0x00C3,
    "Adieresis",        0x80C4,         0x00C4,
    "Aring",            0x80C5,         0x00C5,
    "AE",               0x80C6,         0x00C6,
    "Ccedilla",         0x80C7,         0x00C7,
    "Egrave",           0x80C8,         0x00C8,
    "Eacute",           0x80C9,         0x00C9,
    "Ecircumflex",      0x80CA,         0x00CA,
    "Edieresis",        0x80CB,         0x00CB,
    "Igrave",           0x80CC,         0x00CC,
    "Iacute",           0x80CD,         0x00CD,
    "Icircumflex",      0x80CE,         0x00CE,
    "Idieresis",        0x80CF,         0x00CF,
    "Eth",              0x80D0,         0x00D0,
    "Ntilde",           0x80D1,         0x00D1,
    "Ograve",           0x80D2,         0x00D2,
    "Oacute",           0x80D3,         0x00D3,
    "Ocircumflex",      0x80D4,         0x00D4,
    "Otilde",           0x80D5,         0x00D5,
    "Odieresis",        0x80D6,         0x00D6,
    "multiply",         0x80D7,         0x00D7,
    "Oslash",           0x80D8,         0x00D8,
    "Ugrave",           0x80D9,         0x00D9,
    "Uacute",           0x80DA,         0x00DA,
    "Ucircumflex",      0x80DB,         0x00DB,
    "Udieresis",        0x80DC,         0x00DC,
    "Yacute",           0x80DD,         0x00DD,
    "Thorn",            0x80DE,         0x00DE,
    "germandbls",       0x80DF,         0x00DF,
    "agrave",           0x80E0,         0x00E0,
    "aacute",           0x80E1,         0x00E1,
    "acircumflex",      0x80E2,         0x00E2,
    "atilde",           0x80E3,         0x00E3,
    "adieresis",        0x80E4,         0x00E4,
    "aring",            0x80E5,         0x00E5,
    "ae",               0x80E6,         0x00E6,
    "ccedilla",         0x80E7,         0x00E7,
    "egrave",           0x80E8,         0x00E8,
    "eacute",           0x80E9,         0x00E9,
    "ecircumflex",      0x80EA,         0x00EA,
    "edieresis",        0x80EB,         0x00EB,
    "igrave",           0x80EC,         0x00EC,
    "iacute",           0x80ED,         0x00ED,
    "icircumflex",      0x80EE,         0x00EE,
    "idieresis",        0x80EF,         0x00EF,
    "eth",              0x80F0,         0x00F0,
    "ntilde",           0x80F1,         0x00F1,
    "ograve",           0x80F2,         0x00F2,
    "oacute",           0x80F3,         0x00F3,
    "ocircumflex",      0x80F4,         0x00F4,
    "otilde",           0x80F5,         0x00F5,
    "odieresis",        0x80F6,         0x00F6,
    "divide",           0x80F7,         0x00F7,
    "oslash",           0x80F8,         0x00F8,
    "ugrave",           0x80F9,         0x00F9,
    "uacute",           0x80FA,         0x00FA,
    "ucircumflex",      0x80FB,         0x00FB,
    "udieresis",        0x80FC,         0x00FC,
    "yacute",           0x80FD,         0x00FD,
    "thorn",            0x80FE,         0x00FE,
    "ydieresis",        0x80FF,         0x00FF,
    "dotlessi",         0x800C,         0x0131,
    "Lslash",           0x8092,         0x0141,
    "lslash",           0x809B,         0x0142,
    "OE",               0x8093,         0x0152,
    "oe",               0x8094,         0x0153,
    "Scaron",           0x8095,         0x0160,
    "scaron",           0x8096,         0x0161,
    "Ydieresis",        0x809C,         0x0178,
    "Zcaron",           0x8098,         0x017D,
    "zcaron",           0x8099,         0x017E,
    "florin",           0x8081,         0x0192,
    "circumflex",       0x8002,         0x02C6,
    "breve",            0x8005,         0x02D8,
    "tilde",            0x8003,         0x02DC,
    "grave",            0x8000,         0x0300,
    "dotaccent",        0x800A,         0x0307,
    "dieresis",         0x800B,         0x0308,
    "ring",             0x8006,         0x030A,
    "hungarumlaut",     0x800D,         0x030B,
    "caron",            0x8009,         0x030C,
    "ogonek",           0x8008,         0x0328,
    "hyphen",           0x002D,         0x2012,
    "endash",           0x8088,         0x2013,
    "emdash",           0x8089,         0x2014,
    "quoteleft",        0x0060,         0x2018,
    "quoteright",       0x0027,         0x2019,
    "quotesinglbase",   0x808D,         0x201A,
    "quotedblleft",     0x8082,         0x201C,
    "quotedblright",    0x8083,         0x201D,
    "quotedblbase",     0x808E,         0x201E,
    "dagger",           0x808A,         0x2020,
    "daggerdbl",        0x808B,         0x2021,
    "bullet",           0x808C,         0x2022,
    "ellipsis",         0x8090,         0x2026,
    "perthousand",      0x8091,         0x2030,
    "guilsinglleft",    0x8084,         0x2039,
    "guilsinglright",   0x8085,         0x203A,
    "fraction",         0x8080,         0x20DB,
    "trademark",        0x8097,         0x2122,
    "minus",            0x809A,         0x2212,
    NULL,               0x0000,         0x0000
};

static UCMap  SymbolMap[] =
{
    "space",            0x0020,         0x0020,
    "exclam",           0x0021,         0x0021,
    "universal",        0x0022,         0x0022,
    "numbersign",       0x0023,         0x0023,
    "existential",      0x0024,         0x0024,
    "percent",          0x0025,         0x0025,
    "ampersand",        0x0026,         0x0026,
    "suchthat",         0x0027,         0x0027,
    "parenleft",        0x0028,         0x0028,
    "parenright",       0x0029,         0x0029,
    "asteriskmath",     0x002A,         0x002A,
    "plus",             0x002B,         0x002B,
    "comma",            0x002C,         0x002C,
    "minus",            0x002D,         0x002D,
    "period",           0x002E,         0x002E,
    "slash",            0x002F,         0x002F,
    "zero",             0x0030,         0x0030,
    "one",              0x0031,         0x0031,
    "two",              0x0032,         0x0032,
    "three",            0x0033,         0x0033,
    "four",             0x0034,         0x0034,
    "five",             0x0035,         0x0035,
    "six",              0x0036,         0x0036,
    "seven",            0x0037,         0x0037,
    "eight",            0x0038,         0x0038,
    "nine",             0x0039,         0x0039,
    "colon",            0x003A,         0x003A,
    "semicolon",        0x003B,         0x003B,
    "less",             0x003C,         0x003C,
    "equal",            0x003D,         0x003D,
    "greater",          0x003E,         0x003E,
    "question",         0x003F,         0x003F,
    "congruent",        0x0040,         0x0040,
    "Alpha",            0x0041,         0x0041,
    "Beta",             0x0042,         0x0042,
    "Chi",              0x0043,         0x0043,
    "Delta",            0x0044,         0x0044,
    "Epsilon",          0x0045,         0x0045,
    "Phi",              0x0046,         0x0046,
    "Gamma",            0x0047,         0x0047,
    "Eta",              0x0048,         0x0048,
    "Iota",             0x0049,         0x0049,
    "theta1",           0x004A,         0x004A,
    "Kappa",            0x004B,         0x004B,
    "Lambda",           0x004C,         0x004C,
    "Mu",               0x004D,         0x004D,
    "Nu",               0x004E,         0x004E,
    "Omicron",          0x004F,         0x004F,
    "Pi",               0x0050,         0x0050,
    "Theta",            0x0051,         0x0051,
    "Rho",              0x0052,         0x0052,
    "Sigma",            0x0053,         0x0053,
    "Tau",              0x0054,         0x0054,
    "Upsilon",          0x0055,         0x0055,
    "sigma1",           0x0056,         0x0056,
    "Omega",            0x0057,         0x0057,
    "Xi",               0x0058,         0x0058,
    "Psi",              0x0059,         0x0059,
    "Zeta",             0x005A,         0x005A,
    "bracketleft",      0x005B,         0x005B,
    "therefore",        0x005C,         0x005C,
    "bracketright",     0x005D,         0x005D,
    "perpendicular",    0x005E,         0x005E,
    "underscore",       0x005F,         0x005F,
    "radicalex",        0x0060,         0x0060,
    "alpha",            0x0061,         0x0061,
    "beta",             0x0062,         0x0062,
    "chi",              0x0063,         0x0063,
    "delta",            0x0064,         0x0064,
    "epsilon",          0x0065,         0x0065,
    "phi",              0x0066,         0x0066,
    "gamma",            0x0067,         0x0067,
    "eta",              0x0068,         0x0068,
    "iota",             0x0069,         0x0069,
    "phi1",             0x006A,         0x006A,
    "kappa",            0x006B,         0x006B,
    "lambda",           0x006C,         0x006C,
    "mu",               0x006D,         0x006D,
    "nu",               0x006E,         0x006E,
    "omicron",          0x006F,         0x006F,
    "pi",               0x0070,         0x0070,
    "theta",            0x0071,         0x0071,
    "rho",              0x0072,         0x0072,
    "sigma",            0x0073,         0x0073,
    "tau",              0x0074,         0x0074,
    "upsilon",          0x0075,         0x0075,
    "omega1",           0x0076,         0x0076,
    "omega",            0x0077,         0x0077,
    "xi",               0x0078,         0x0078,
    "psi",              0x0079,         0x0079,
    "zeta",             0x007A,         0x007A,
    "braceleft",        0x007B,         0x007B,
    "bar",              0x007C,         0x007C,
    "braceright",       0x007D,         0x007D,
    "similar",          0x007E,         0x007E,
    "Upsilon1",         0x00A1,         0x00A1,
    "minute",           0x00A2,         0x00A2,
    "lessequal",        0x00A3,         0x00A3,
    "fraction",         0x00A4,         0x00A4,
    "infinity",         0x00A5,         0x00A5,
    "florin",           0x00A6,         0x00A6,
    "club",             0x00A7,         0x00A7,
    "diamond",          0x00A8,         0x00A8,
    "heart",            0x00A9,         0x00A9,
    "spade",            0x00AA,         0x00AA,
    "arrowboth",        0x00AB,         0x00AB,
    "arrowleft",        0x00AC,         0x00AC,
    "arrowup",          0x00AD,         0x00AD,
    "arrowright",       0x00AE,         0x00AE,
    "arrowdown",        0x00AF,         0x00AF,
    "degree",           0x00B0,         0x00B0,
    "plusminus",        0x00B1,         0x00B1,
    "second",           0x00B2,         0x00B2,
    "greaterequal",     0x00B3,         0x00B3,
    "multiply",         0x00B4,         0x00B4,
    "proportional",     0x00B5,         0x00B5,
    "partialdiff",      0x00B6,         0x00B6,
    "bullet",           0x00B7,         0x00B7,
    "divide",           0x00B8,         0x00B8,
    "notequal",         0x00B9,         0x00B9,
    "equivalence",      0x00BA,         0x00BA,
    "approxequal",      0x00BB,         0x00BB,
    "ellipsis",         0x00BC,         0x00BC,
    "arrowvertex",      0x00BD,         0x00BD,
    "arrowhorizex",     0x00BE,         0x00BE,
    "carriagereturn",   0x00BF,         0x00BF,
    "aleph",            0x00C0,         0x00C0,
    "Ifraktur",         0x00C1,         0x00C1,
    "Rfraktur",         0x00C2,         0x00C2,
    "weierstrass",      0x00C3,         0x00C3,
    "circlemultiply",   0x00C4,         0x00C4,
    "circleplus",       0x00C5,         0x00C5,
    "emptyset",         0x00C6,         0x00C6,
    "intersection",     0x00C7,         0x00C7,
    "union",            0x00C8,         0x00C8,
    "propersuperset",   0x00C9,         0x00C9,
    "reflexsuperset",   0x00CA,         0x00CA,
    "notsubset",        0x00CB,         0x00CB,
    "propersubset",     0x00CC,         0x00CC,
    "reflexsubset",     0x00CD,         0x00CD,
    "element",          0x00CE,         0x00CE,
    "notelement",       0x00CF,         0x00CF,
    "angle",            0x00D0,         0x00D0,
    "gradient",         0x00D1,         0x00D1,
    "registerserif",    0x00D2,         0x00D2,
    "copyrightserif",   0x00D3,         0x00D3,
    "trademarkserif",   0x00D4,         0x00D4,
    "product",          0x00D5,         0x00D5,
    "radical",          0x00D6,         0x00D6,
    "dotmath",          0x00D7,         0x00D7,
    "logicalnot",       0x00D8,         0x00D8,
    "logicaland",       0x00D9,         0x00D9,
    "logicalor",        0x00DA,         0x00DA,
    "arrowdblboth",     0x00DB,         0x00DB,
    "arrowdblleft",     0x00DC,         0x00DC,
    "arrowdblup",       0x00DD,         0x00DD,
    "arrowdblright",    0x00DE,         0x00DE,
    "arrowdbldown",     0x00DF,         0x00DF,
    "lozenge",          0x00E0,         0x00E0,
    "angleleft",        0x00E1,         0x00E1,
    "registersans",     0x00E2,         0x00E2,
    "copyrightsans",    0x00E3,         0x00E3,
    "trademarksans",    0x00E4,         0x00E4,
    "summation",        0x00E5,         0x00E5,
    "parenlefttp",      0x00E6,         0x00E6,
    "parenleftex",      0x00E7,         0x00E7,
    "parenleftbt",      0x00E8,         0x00E8,
    "bracketlefttp",    0x00E9,         0x00E9,
    "bracketleftex",    0x00EA,         0x00EA,
    "bracketleftbt",    0x00EB,         0x00EB,
    "bracelefttp",      0x00EC,         0x00EC,
    "braceleftmid",     0x00ED,         0x00ED,
    "braceleftbt",      0x00EE,         0x00EE,
    "braceex",          0x00EF,         0x00EF,
    "angleright",       0x00F1,         0x00F1,
    "integral",         0x00F2,         0x00F2,
    "integraltp",       0x00F3,         0x00F3,
    "integralex",       0x00F4,         0x00F4,
    "integralbt",       0x00F5,         0x00F5,
    "parenrighttp",     0x00F6,         0x00F6,
    "parenrightmid",    0x00F7,         0x00F7,
    "parenrightbt",     0x00F8,         0x00F8,
    "bracketrighttp",   0x00F9,         0x00F9,
    "bracketrightex",   0x00FA,         0x00FA,
    "bracketrightbt",   0x00FB,         0x00FB,
    "bracerighttp",     0x00FC,         0x00FC,
    "bracerightmid",    0x00FD,         0x00FD,
    "bracerightbt",     0x00FE,         0x00FE,
    NULL,               0x0000,         0x0000
};

#if 0
static UCMap  SymbolMap[] =
{
    "space",            0x0020,         0x0020,
    "exclam",           0x0021,         0x0021,
    "numbersign",       0x0023,         0x0023,
    "percent",          0x0025,         0x0025,
    "ampersand",        0x0026,         0x0026,
    "parenleft",        0x0028,         0x0028,
    "parenright",       0x0029,         0x0029,
    "plus",             0x002B,         0x002B,
    "comma",            0x002C,         0x002C,
    "period",           0x002E,         0x002E,
    "slash",            0x002F,         0x002F,
    "zero",             0x0030,         0x0030,
    "one",              0x0031,         0x0031,
    "two",              0x0032,         0x0032,
    "three",            0x0033,         0x0033,
    "four",             0x0034,         0x0034,
    "five",             0x0035,         0x0035,
    "six",              0x0036,         0x0036,
    "seven",            0x0037,         0x0037,
    "eight",            0x0038,         0x0038,
    "nine",             0x0039,         0x0039,
    "colon",            0x003A,         0x003A,
    "semicolon",        0x003B,         0x003B,
    "less",             0x003C,         0x003C,
    "equal",            0x003D,         0x003D,
    "greater",          0x003E,         0x003E,
    "question",         0x003F,         0x003F,
    "bracketleft",      0x005B,         0x005B,
    "bracketright",     0x005D,         0x005D,
    "underscore",       0x005F,         0x005F,
    "braceleft",        0x007B,         0x007B,
    "bar",              0x007C,         0x007C,
    "braceright",       0x007D,         0x007D,
    "copyrightserif",   0x00D3,         0x00A9,
    "copyrightsans",    0x00E3,         0x00A9,
    "logicalnot",       0x00D8,         0x00AC,
    "registerserif",    0x00D2,         0x00AE,
    "registersans",     0x00E2,         0x00AE,
    "degree",           0x00B0,         0x00B0,
    "plusminus",        0x00B1,         0x00B1,
    "multiply",         0x00B4,         0x00D7,
    "divide",           0x00B8,         0x00F7,
    "florin",           0x00A6,         0x0192,
    "Alpha",            0x0041,         0x0391,
    "Beta",             0x0042,         0x0392,
    "Gamma",            0x0047,         0x0393,
    "Delta",            0x0044,         0x0394,
    "Epsilon",          0x0045,         0x0395,
    "Zeta",             0x005A,         0x0396,
    "Eta",              0x0048,         0x0397,
    "Theta",            0x0051,         0x0398,
    "Iota",             0x0049,         0x0399,
    "Kappa",            0x004B,         0x039A,
    "Lambda",           0x004C,         0x039B,
    "Mu",               0x004D,         0x039C,
    "Nu",               0x004E,         0x039D,
    "Xi",               0x0058,         0x039E,
    "Omicron",          0x004F,         0x039F,
    "Pi",               0x0050,         0x03A0,
    "Rho",              0x0052,         0x03A1,
    "Sigma",            0x0053,         0x03A3,
    "Tau",              0x0054,         0x03A4,
    "Upsilon",          0x0055,         0x03A5,
    "Phi",              0x0046,         0x03A6,
    "Chi",              0x0043,         0x03A7,
    "Omega",            0x0057,         0x03A9,
    "Psi",              0x0059,         0x03A8,
    "alpha",            0x0061,         0x03B1,
    "beta",             0x0062,         0x03B2,
    "gamma",            0x0067,         0x03B3,
    "delta",            0x0064,         0x03B4,
    "epsilon",          0x0065,         0x03B5,
    "zeta",             0x007A,         0x03B6,
    "eta",              0x0068,         0x03B7,
    "theta",            0x0071,         0x03B8,
    "iota",             0x0069,         0x03B9,
    "kappa",            0x006B,         0x03BA,
    "lambda",           0x006C,         0x03BB,
    "mu",               0x006D,         0x03BC,
    "nu",               0x006E,         0x03BD,
    "xi",               0x0078,         0x03BE,
    "omicron",          0x006F,         0x03BF,
    "rho",              0x0072,         0x03C1,
    "sigma1",           0x0056,         0x03C2,
    "sigma",            0x0073,         0x03C3,
    "tau",              0x0074,         0x03C4,
    "upsilon",          0x0075,         0x03C5,
    "phi",              0x0066,         0x03C6,
    "chi",              0x0063,         0x03C7,
    "psi",              0x0079,         0x03C8,
    "omega",            0x0077,         0x03C9,

    "theta1",           0x004A,         0x03D1,
    "Upsilon1",         0x00A1,         0x03D2,
    "phi1",             0x006A,         0x03D5,
    "omega1",           0x0076,         0x03D6,
    "bullet",           0x00B7,         0x2022,
    "ellipsis",         0x00BC,         0x2026,
    "minute",           0x00A2,         0x2032,
    "second",           0x00B2,         0x2033,
    "fraction",         0x00A4,         0x20DB,
    "Ifraktur",         0x00C1,         0x2111,
    "weierstrass",      0x00C3,         0x2118,
    "Rfraktur",         0x00C2,         0x211C,
    "trademarkserif",   0x00D4,         0x2122,
    "trademarksans",    0x00E4,         0x2122,
    "aleph",            0x00C0,         0x2128,
    "arrowleft",        0x00AC,         0x2190,
    "arrowup",          0x00AD,         0x2191,
    "arrowright",       0x00AE,         0x2192,
    "arrowdown",        0x00AF,         0x2193,
    "arrowboth",        0x00AB,         0x2194,
    "carriagereturn",   0x00BF,         0x21B5,
    "arrowdblleft",     0x00DC,         0x21D0,
    "arrowdblup",       0x00DD,         0x21D1,
    "arrowdblright",    0x00DE,         0x21D2,
    "arrowdbldown",     0x00DF,         0x21D3,
    "arrowdblboth",     0x00DB,         0x21D4,
    "universal",        0x0022,         0x2200,
    "partialdiff",      0x00B6,         0x2202,
    "existential",      0x0024,         0x2203,
    "emptyset",         0x00C6,         0x2205,
    "gradient",         0x00D1,         0x2207,
    "element",          0x00CE,         0x220B,
    "notelement",       0x00CF,         0x220C,
    "suchthat",         0x0027,         0x220D,
    "product",          0x00D5,         0x220F,
    "summation",        0x00E5,         0x2211,
    "minus",            0x002D,         0x2212,
    "asteriskmath",     0x002A,         0x2217,
    "dotmath",          0x00D7,         0x2219,
    "radical",          0x00D6,         0x221A,
    "proportional",     0x00B5,         0x221D,
    "infinity",         0x00A5,         0x221E,
    "angle",            0x00D0,         0x2220,
    "logicaland",       0x00D9,         0x2227,
    "logicalor",        0x00DA,         0x2228,
    "intersection",     0x00C7,         0x2229,
    "union",            0x00C8,         0x222A,
    "integral",         0x00F2,         0x222B,
    "therefore",        0x005C,         0x2234,
    "similar",          0x007E,         0x223C,
    "congruent",        0x0040,         0x2245,
    "approxequal",      0x00BB,         0x2248,
    "notequal",         0x00B9,         0x2260,
    "equivalence",      0x00BA,         0x2261,
    "greaterequal",     0x00B3,         0x2265,
    "propersubset",     0x00CC,         0x2282,
    "propersuperset",   0x00C9,         0x2283,
    "notsubset",        0x00CB,         0x2284,
    "reflexsubset",     0x00CD,         0x2286,
    "reflexsuperset",   0x00CA,         0x2287,
    "circleplus",       0x00C5,         0x2295,
    "circlemultiply",   0x00C4,         0x2297,
    "perpendicular",    0x005E,         0x22A5,
    "integraltp",       0x00F3,         0x2320,
    "integralbt",       0x00F5,         0x2321,
    "lozenge",          0x00E0,         0x25CA,
    "spade",            0x00AA,         0x2660,
    "club",             0x00A7,         0x2663,
    "heart",            0x00A9,         0x2665,
    "diamond",          0x00A8,         0x2666,
    "angleleft",        0x00E1,         0x3008,
    "angleright",       0x00F1,         0x3009,
    NULL,               0x0000,         0x0000
};
#endif

static UCMap  DingbatsMap[] =
{
    "space",            0x0020,     0x0020,
    "a1",               0x0021,     0x0021, // upper blade scissors.
    "a2",               0x0022,     0x0022, // black scissors.
    "a202",             0x0023,     0x0023, // lower blade scissors.
    "a3",               0x0024,     0x0024, // white scissors.
    "a4",               0x0025,     0x0025, // black telephone.
    "a5",               0x0026,     0x0026, // telephone location sign.
    "a119",             0x0027,     0x0027, // tape drive.
    "a118",             0x0028,     0x0028, // airplane.
    "a117",             0x0029,     0x0029, // envelope.
    "a11",              0x002A,     0x002A, // black right pointing index.
    "a12",              0x002B,     0x002B, // white right pointing index.
    "a13",              0x002C,     0x002C, // victory hand.
    "a14",              0x002D,     0x002D, // writing hand.
    "a15",              0x002E,     0x002E, // pencil pointing down.
    "a16",              0x002F,     0x002F, // horizontal pencil.
    "a105",             0x0030,     0x0030, // pencil pointing up.
    "a17",              0x0031,     0x0031, // white nib.
    "a18",              0x0032,     0x0032, // black nib.
    "a19",              0x0033,     0x0033, // check mark 1.
    "a20",              0x0034,     0x0034, // check mark 2.
    "a21",              0x0035,     0x0035, // ballot cross 1.
    "a22",              0x0036,     0x0036, // ballot cross 2.
    "a23",              0x0037,     0x0037, // ballot cross 3.
    "a24",              0x0038,     0x0038, // ballot cross 4.
    "a25",              0x0039,     0x0039, // black cross 1.
    "a26",              0x003A,     0x003A, // black cross 2.
    "a27",              0x003B,     0x003B, // black cross 3.
    "a28",              0x003C,     0x003C, // black cross 4.
    "a6",               0x003D,     0x003D, // latin cross 1.
    "a7",               0x003E,     0x003E, // latin cross 2.
    "a8",               0x003F,     0x003F, // latin cross 3.
    "a9",               0x0040,     0x0040, // maltese cross.
    "a10",              0x0041,     0x0041, // star of david.
    "a29",              0x0042,     0x0042, // black cross 5.
    "a30",              0x0043,     0x0043, // black cross 6.
    "a31",              0x0044,     0x0044, // black cross 7.
    "a32",              0x0045,     0x0045, // black cross 8.
    "a33",              0x0046,     0x0046, // black four pointed star.
    "a34",              0x0047,     0x0047, // white four pointed star.
    "a35",              0x0048,     0x0048, // black star.
    "a36",              0x0049,     0x0049, // white star.
    "a37",              0x004A,     0x004A, // circled white star.
    "a38",              0x004B,     0x004B, // white star.
    "a39",              0x004C,     0x004C, // white star.
    "a40",              0x004D,     0x004D, // white star.
    "a41",              0x004E,     0x004E, // white star.
    "a42",              0x004F,     0x004F, // white star.
    "a43",              0x0050,     0x0050, // white star.
    "a44",              0x0051,     0x0051, // black star.
    "a45",              0x0052,     0x0052, // black star.
    "a46",              0x0053,     0x0053, // black star.
    "a47",              0x0054,     0x0054, // black star.
    "a48",              0x0055,     0x0055, // black star.
    "a49",              0x0056,     0x0056, // black star.
    "a50",              0x0057,     0x0057, // black star.
    "a51",              0x0058,     0x0058, // black star.
    "a52",              0x0059,     0x0059, // black star.
    "a53",              0x005A,     0x005A, // black star.
    "a54",              0x005B,     0x005B, // black star.
    "a55",              0x005C,     0x005C, // black star.
    "a56",              0x005D,     0x005D, // black star.
    "a57",              0x005E,     0x005E, // black star.
    "a58",              0x005F,     0x005F, // black florette 1.
    "a59",              0x0060,     0x0060, // white florette 1.
    "a60",              0x0061,     0x0061, // black florette 2.
    "a61",              0x0062,     0x0062, // black florette 3.
    "a62",              0x0063,     0x0063, // white florette 2.
    "a63",              0x0064,     0x0064, // snowflake.
    "a64",              0x0065,     0x0065, // snowflake.
    "a65",              0x0066,     0x0066, // snowflake.
    "a66",              0x0067,     0x0067, // snowflake.
    "a67",              0x0068,     0x0068, // snowflake.
    "a68",              0x0069,     0x0069, // snowflake.
    "a69",              0x006A,     0x006A, // snowflake.
    "a70",              0x006B,     0x006B, // snowflake.
    "a71",              0x006C,     0x006C, // black circle.
    "a72",              0x006D,     0x006D, // white circle.
    "a73",              0x006E,     0x006E, // black square.
    "a74",              0x006F,     0x006F, // white square 1.
    "a203",             0x0070,     0x0070, // white square 2.
    "a75",              0x0071,     0x0071, // white square 3.
    "a204",             0x0072,     0x0072, // white square 4.
    "a76",              0x0073,     0x0073, // black up pointing triangle.
    "a77",              0x0074,     0x0074, // black down pointing triangle.
    "a78",              0x0075,     0x0075, // black diamond.
    "a79",              0x0076,     0x0076, // black diamond minus white X.
    "a81",              0x0077,     0x0077, // right half black circle.
    "a82",              0x0078,     0x0078, // black rectangle 1.
    "a83",              0x0079,     0x0079, // black rectangle 2.
    "a84",              0x007A,     0x007A, // black rectangle 3.
    "a97",              0x007B,     0x007B, // single turned comma quotation.
    "a98",              0x007C,     0x007C, // single comma quotation mark.
    "a99",              0x007D,     0x007D, // double turned comma quotation.
    "a100",             0x007E,     0x007E, // double comma quotation mark.
    "a112",             0x00A8,     0x00A8, // black club suit.
    "a111",             0x00A9,     0x00A9, // black diamond suit.
    "a110",             0x00AA,     0x00AA, // black heart suit.
    "a109",             0x00AB,     0x00AB, // black spade suit.
    "a120",             0x00AC,     0x00AC, // circled one 1.
    "a121",             0x00AD,     0x00AD, // circled two 1.
    "a122",             0x00AE,     0x00AE, // circled three 1.
    "a123",             0x00AF,     0x00AF, // circled four 1.
    "a124",             0x00B0,     0x00B0, // circled five 1.
    "a125",             0x00B1,     0x00B1, // circled six 1.
    "a126",             0x00B2,     0x00B2, // circled seven 1.
    "a127",             0x00B3,     0x00B3, // circled eight 1.
    "a128",             0x00B4,     0x00B4, // circled nine 1.
    "a129",             0x00B5,     0x00B5, // circled ten 1.
    "a130",             0x00B6,     0x00B6, // circled one 2.
    "a131",             0x00B7,     0x00B7, // circled two 2.
    "a132",             0x00B8,     0x00B8, // circled three 2.
    "a133",             0x00B9,     0x00B9, // circled four 2.
    "a134",             0x00BA,     0x00BA, // circled five 2.
    "a135",             0x00BB,     0x00BB, // circled six 2.
    "a136",             0x00BC,     0x00BC, // circled seven 2.
    "a137",             0x00BD,     0x00BD, // circled eight 2.
    "a138",             0x00BE,     0x00BE, // circled nine 2.
    "a139",             0x00BF,     0x00BF, // circled ten 2.
    "a140",             0x00C0,     0x00C0, // circled one 3.
    "a141",             0x00C1,     0x00C1, // circled two 3.
    "a142",             0x00C2,     0x00C2, // circled three 3.
    "a143",             0x00C3,     0x00C3, // circled four 3.
    "a144",             0x00C4,     0x00C4, // circled five 3.
    "a145",             0x00C5,     0x00C5, // circled six 3.
    "a146",             0x00C6,     0x00C6, // circled seven 3.
    "a147",             0x00C7,     0x00C7, // circled eight 3.
    "a148",             0x00C8,     0x00C8, // circled nine 3.
    "a149",             0x00C9,     0x00C9, // circled ten 3.
    "a150",             0x00CA,     0x00CA, // circled one 4.
    "a151",             0x00CB,     0x00CB, // circled two 4.
    "a152",             0x00CC,     0x00CC, // circled three 4.
    "a153",             0x00CD,     0x00CD, // circled four 4.
    "a154",             0x00CE,     0x00CE, // circled five 4.
    "a155",             0x00CF,     0x00CF, // circled six 4.
    "a156",             0x00D0,     0x00D0, // circled seven 4.
    "a157",             0x00D1,     0x00D1, // circled eight 4.
    "a158",             0x00D2,     0x00D2, // circled nine 4.
    "a159",             0x00D3,     0x00D3, // circled ten 4.
    "a160",             0x00D4,     0x00D4, // right arrow.
    "a161",             0x00D5,     0x00D5, // right arrow.
    "a163",             0x00D6,     0x00D6, // horizontal arrow both ways.
    "a164",             0x00D7,     0x00D7, // vertical arrow both ways.
    "a196",             0x00D8,     0x00D8, // right-down arrow.
    "a165",             0x00D9,     0x00D9, // right arrow.
    "a192",             0x00DA,     0x00DA, // right-up arrow.
    "a166",             0x00DB,     0x00DB, // right arrow.
    "a167",             0x00DC,     0x00DC, // right arrow.
    "a168",             0x00DD,     0x00DD, // right arrow.
    "a169",             0x00DE,     0x00DE, // right arrow.
    "a170",             0x00DF,     0x00DF, // right arrow.
    "a171",             0x00E0,     0x00E0, // right arrow.
    "a172",             0x00E1,     0x00E1, // right arrow.
    "a173",             0x00E2,     0x00E2, // right arrow.
    "a162",             0x00E3,     0x00E3, // right arrow.
    "a174",             0x00E4,     0x00E4, // right arrow.
    "a175",             0x00E5,     0x00E5, // turning arrow.
    "a176",             0x00E6,     0x00E6, // turning arrow.
    "a177",             0x00E7,     0x00E7, // right arrow.
    "a178",             0x00E8,     0x00E8, // right arrow.
    "a179",             0x00E9,     0x00E9, // white right arrow.
    "a193",             0x00EA,     0x00EA, // white right arrow.
    "a180",             0x00EB,     0x00EB, // white right arrow.
    "a199",             0x00EC,     0x00EC, // white right arrow.
    "a181",             0x00ED,     0x00ED, // white right arrow.
    "a200",             0x00EE,     0x00EE, // white right arrow.
    "a182",             0x00EF,     0x00EF, // white right arrow.
    "a201",             0x00F1,     0x00F1, // white right arrow.
    "a183",             0x00F2,     0x00F2, // white right arrow.
    "a184",             0x00F3,     0x00F3, // right arrow.
    "a197",             0x00F4,     0x00F4, // right-down arrow.
    "a185",             0x00F5,     0x00F5, // right arrow.
    "a194",             0x00F6,     0x00F6, // right-up arrow.
    "a198",             0x00F7,     0x00F7, // right down arrow.
    "a186",             0x00F8,     0x00F8, // right arrow.
    "a195",             0x00F9,     0x00F9, // right-up arrow.
    "a187",             0x00FA,     0x00FA, // right arrow.
    "a188",             0x00FB,     0x00FB, // right arrow.
    "a189",             0x00FC,     0x00FC, // right arrow.
    "a190",             0x00FD,     0x00FD, // right arrow.
    "a191",             0x00FE,     0x00FE, // double right arrow.
    NULL,               0x0000,     0x0000
};

#if 0
static UCMap  DingbatsMap[] =
{
    "space",            0x0020,     0x0020,
    "a97",              0x007B,     0x2018, // single turned comma quotation.
    "a98",              0x007C,     0x2019, // single comma quotation mark.
    "a99",              0x007D,     0x201C, // double turned comma quotation.
    "a100",             0x007E,     0x201D, // double comma quotation mark.
    "a160",             0x00D4,     0x2192, // right arrow.
    "a161",             0x00D5,     0x2192, // right arrow.
    "a165",             0x00D9,     0x2192, // right arrow.
    "a166",             0x00DB,     0x2192, // right arrow.
    "a167",             0x00DC,     0x2192, // right arrow.
    "a168",             0x00DD,     0x2192, // right arrow.
    "a169",             0x00DE,     0x2192, // right arrow.
    "a170",             0x00DF,     0x2192, // right arrow.
    "a171",             0x00E0,     0x2192, // right arrow.
    "a172",             0x00E1,     0x2192, // right arrow.
    "a173",             0x00E2,     0x2192, // right arrow.
    "a162",             0x00E3,     0x2192, // right arrow.
    "a174",             0x00E4,     0x2192, // right arrow.
    "a177",             0x00E7,     0x2192, // right arrow.
    "a178",             0x00E8,     0x2192, // right arrow.
    "a184",             0x00F3,     0x2192, // right arrow.
    "a185",             0x00F5,     0x2192, // right arrow.
    "a186",             0x00F8,     0x2192, // right arrow.
    "a187",             0x00FA,     0x2192, // right arrow.
    "a188",             0x00FB,     0x2192, // right arrow.
    "a189",             0x00FC,     0x2192, // right arrow.
    "a190",             0x00FD,     0x2192, // right arrow.
    "a163",             0x00D6,     0x2194, // horizontal arrow both ways.
    "a164",             0x00D7,     0x2195, // vertical arrow both ways.
    "a192",             0x00DA,     0x2197, // right-up arrow.
    "a194",             0x00F6,     0x2197, // right-up arrow.
    "a195",             0x00F9,     0x2197, // right-up arrow.
    "a196",             0x00D8,     0x2198, // right-down arrow.
    "a197",             0x00F4,     0x2198, // right-down arrow.
    "a198",             0x00F7,     0x2198, // right down arrow.
    "a176",             0x00E6,     0x21B1, // turning arrow.
    "a175",             0x00E5,     0x21B3, // turning arrow.
    "a179",             0x00E9,     0x21E8, // white right arrow.
    "a193",             0x00EA,     0x21E8, // white right arrow.
    "a180",             0x00EB,     0x21E8, // white right arrow.
    "a199",             0x00EC,     0x21E8, // white right arrow.
    "a181",             0x00ED,     0x21E8, // white right arrow.
    "a200",             0x00EE,     0x21E8, // white right arrow.
    "a182",             0x00EF,     0x21E8, // white right arrow.
    "a201",             0x00F1,     0x21E8, // white right arrow.
    "a183",             0x00F2,     0x21E8, // white right arrow.
    "a191",             0x00FE,     0x21D2, // double right arrow.
    "a120",             0x00AC,     0x2460, // circled one 1.
    "a130",             0x00B6,     0x2460, // circled one 2.
    "a140",             0x00C0,     0x2460, // circled one 3.
    "a150",             0x00CA,     0x2460, // circled one 4.
    "a121",             0x00AD,     0x2461, // circled two 1.
    "a131",             0x00B7,     0x2461, // circled two 2.
    "a141",             0x00C1,     0x2461, // circled two 3.
    "a151",             0x00CB,     0x2461, // circled two 4.
    "a122",             0x00AE,     0x2462, // circled three 1.
    "a132",             0x00B8,     0x2462, // circled three 2.
    "a142",             0x00C2,     0x2462, // circled three 3.
    "a152",             0x00CC,     0x2462, // circled three 4.
    "a123",             0x00AF,     0x2463, // circled four 1.
    "a133",             0x00B9,     0x2463, // circled four 2.
    "a143",             0x00C3,     0x2463, // circled four 3.
    "a153",             0x00CD,     0x2463, // circled four 4.
    "a124",             0x00B0,     0x2464, // circled five 1.
    "a134",             0x00BA,     0x2464, // circled five 2.
    "a144",             0x00C4,     0x2464, // circled five 3.
    "a154",             0x00CE,     0x2464, // circled five 4.
    "a125",             0x00B1,     0x2465, // circled six 1.
    "a135",             0x00BB,     0x2465, // circled six 2.
    "a145",             0x00C5,     0x2465, // circled six 3.
    "a155",             0x00CF,     0x2465, // circled six 4.
    "a126",             0x00B2,     0x2466, // circled seven 1.
    "a136",             0x00BC,     0x2466, // circled seven 2.
    "a146",             0x00C6,     0x2466, // circled seven 3.
    "a156",             0x00D0,     0x2466, // circled seven 4.
    "a127",             0x00B3,     0x2467, // circled eight 1.
    "a137",             0x00BD,     0x2467, // circled eight 2.
    "a147",             0x00C7,     0x2467, // circled eight 3.
    "a157",             0x00D1,     0x2467, // circled eight 4.
    "a128",             0x00B4,     0x2468, // circled nine 1.
    "a138",             0x00BE,     0x2468, // circled nine 2.
    "a148",             0x00C8,     0x2468, // circled nine 3.
    "a158",             0x00D2,     0x2468, // circled nine 4.
    "a129",             0x00B5,     0x2469, // circled ten 1.
    "a139",             0x00BF,     0x2469, // circled ten 2.
    "a149",             0x00C9,     0x2469, // circled ten 3.
    "a159",             0x00D3,     0x2469, // circled ten 4.
    "a73",              0x006E,     0x25A0, // black square.
    "a74",              0x006F,     0x25A1, // white square 1.
    "a203",             0x0070,     0x25A1, // white square 2.
    "a75",              0x0071,     0x25A1, // white square 3.
    "a204",             0x0072,     0x25A1, // white square 4.
    "a82",              0x0078,     0x25AE, // black rectangle 1.
    "a83",              0x0079,     0x25AE, // black rectangle 2.
    "a84",              0x007A,     0x25AE, // black rectangle 3.
    "a76",              0x0073,     0x25B2, // black up pointing triangle.
    "a77",              0x0074,     0x25BC, // black down pointing triangle.
    "a78",              0x0075,     0x25C6, // black diamond.
    "a79",              0x0076,     0x25C9, // black diamond minus white X.
    "a72",              0x006D,     0x25CB, // white circle.
    "a71",              0x006C,     0x25CF, // black circle.
    "a81",              0x0077,     0x25D7, // right half black circle.
    "a34",              0x0047,     0x2603, // white four pointed star.
    "a33",              0x0046,     0x2604, // black four pointed star.
    "a35",              0x0048,     0x2605, // black star.
    "a44",              0x0051,     0x2605, // black star.
    "a45",              0x0052,     0x2605, // black star.
    "a46",              0x0053,     0x2605, // black star.
    "a47",              0x0054,     0x2605, // black star.
    "a48",              0x0055,     0x2605, // black star.
    "a49",              0x0056,     0x2605, // black star.
    "a50",              0x0057,     0x2605, // black star.
    "a51",              0x0058,     0x2605, // black star.
    "a52",              0x0059,     0x2605, // black star.
    "a53",              0x005A,     0x2605, // black star.
    "a54",              0x005B,     0x2605, // black star.
    "a55",              0x005C,     0x2605, // black star.
    "a56",              0x005D,     0x2605, // black star.
    "a57",              0x005E,     0x2605, // black star.
    "a36",              0x0049,     0x2606, // white star.
    "a38",              0x004B,     0x2606, // white star.
    "a39",              0x004C,     0x2606, // white star.
    "a40",              0x004D,     0x2606, // white star.
    "a41",              0x004E,     0x2606, // white star.
    "a42",              0x004F,     0x2606, // white star.
    "a43",              0x0050,     0x2606, // white star.
    "a37",              0x004A,     0x2607, // circled white star.
    "a58",              0x005F,     0x2608, // black florette 1.
    "a59",              0x0060,     0x2609, // white florette 1.
    "a60",              0x0061,     0x2608, // black florette 2.
    "a61",              0x0062,     0x2608, // black florette 3.
    "a62",              0x0063,     0x2609, // white florette 2.
    "a63",              0x0064,     0x260B, // snowflake.
    "a64",              0x0065,     0x260B, // snowflake.
    "a65",              0x0066,     0x260B, // snowflake.
    "a66",              0x0067,     0x260B, // snowflake.
    "a67",              0x0068,     0x260B, // snowflake.
    "a68",              0x0069,     0x260B, // snowflake.
    "a69",              0x006A,     0x260B, // snowflake.
    "a70",              0x006B,     0x260B, // snowflake.
    "a19",              0x0033,     0x260C, // check mark 1.
    "a20",              0x0034,     0x260C, // check mark 2.
    "a21",              0x0035,     0x260D, // ballot cross 1.
    "a22",              0x0036,     0x260D, // ballot cross 2.
    "a23",              0x0037,     0x260D, // ballot cross 3.
    "a24",              0x0038,     0x260D, // ballot cross 4.
    "a4",               0x0025,     0x260E, // black telephone.
    "a5",               0x0026,     0x2610, // telephone location sign.
    "a119",             0x0027,     0x2611, // tape drive.
    "a18",              0x0032,     0x2612, // black nib.
    "a17",              0x0031,     0x2613, // white nib.
    "a15",              0x002E,     0x2614, // pencil pointing down.
    "a16",              0x002F,     0x2614, // horizontal pencil.
    "a105",             0x0030,     0x2614, // pencil pointing up.
    "a117",             0x0029,     0x2615, // envelope.
    "a3",               0x0024,     0x2616, // white scissors.
    "a2",               0x0022,     0x2617, // black scissors.
    "a1",               0x0021,     0x2618, // upper blade scissors.
    "a202",             0x0023,     0x2619, // lower blade scissors.
    "a11",              0x002A,     0x261B, // black right pointing index.
    "a12",              0x002B,     0x261E, // white right pointing index.
    "a14",              0x002D,     0x2620, // writing hand.
    "a13",              0x002C,     0x2621, // victory hand.
    "a25",              0x0039,     0x2623, // black cross 1.
    "a26",              0x003A,     0x2623, // black cross 2.
    "a27",              0x003B,     0x2623, // black cross 3.
    "a28",              0x003C,     0x2623, // black cross 4.
    "a29",              0x0042,     0x2623, // black cross 5.
    "a30",              0x0043,     0x2623, // black cross 6.
    "a31",              0x0044,     0x2623, // black cross 7.
    "a32",              0x0045,     0x2623, // black cross 8.
    "a6",               0x003D,     0x2626, // latin cross 1.
    "a7",               0x003E,     0x2626, // latin cross 2.
    "a8",               0x003F,     0x2626, // latin cross 3.
    "a9",               0x0040,     0x2629, // maltese cross.
    "a10",              0x0041,     0x262A, // star of david.

    "a109",             0x00AB,     0x2660, // black spade suit.
    "a112",             0x00A8,     0x2663, // black club suit.
    "a110",             0x00AA,     0x2665, // black heart suit.
    "a111",             0x00A9,     0x2666, // black diamond suit.
    "a118",             0x0028,     0x2674, // airplane.
    NULL,               0x0000,     0x0000
};
#endif
