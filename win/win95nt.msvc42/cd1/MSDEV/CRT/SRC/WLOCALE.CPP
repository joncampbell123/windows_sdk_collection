// wlocale -- class locale wide member functions
#include <istream>
#include <locale>

typedef char_traits<wchar_t> _Wtraits;
typedef istreambuf_iterator<wchar_t, _Wtraits> _Winiter;
typedef ostreambuf_iterator<wchar_t, _Wtraits> _Woutiter;

	// facets associated with C categories
#define ADDFAC(T, cat, pi, pl) \
	if ((_CATMASK(T::_Getcat()) & cat) == 0) \
		; \
	else if (pl == 0) \
		pi->_Addfac(new T(_Lobj), T::id); \
	else \
		pi->_Addfac((locale::facet *)&_USE(*pl, T), T::id); 
typedef ctype<wchar_t> _Tw1;
typedef num_get<wchar_t, _Winiter> _Tw2;
typedef num_put<wchar_t, _Woutiter> _Tw3;
typedef numpunct<wchar_t> _Tw4;
typedef collate<wchar_t> _Tw5;
typedef messages<wchar_t> _Tw6;
typedef money_get<wchar_t, false, _Winiter> _Tw7;
typedef money_get<wchar_t, true, _Winiter> _Tw8;
typedef money_put<wchar_t, false, _Woutiter> _Tw9;
typedef money_put<wchar_t, true, _Woutiter> _Tw10;
typedef moneypunct<wchar_t, false> _Tw11;
typedef moneypunct<wchar_t, true> _Tw12;
typedef time_get<wchar_t, _Winiter> _Tw13;
typedef time_put<wchar_t, _Woutiter> _Tw14;
//....
typedef codecvt<char, wchar_t, mbstate_t> _Twc1;
typedef codecvt<wchar_t, char, mbstate_t> _Twc2;

void __cdecl locale::_Locimp::_Makewloc(const _Locinfo& _Lobj,
	locale::category _C, _Locimp *_Pi, const locale *_Pl)
	{	// setup wide part of a new locale
	ADDFAC(_Tw1, _C, _Pi, _Pl);
	ADDFAC(_Tw2, _C, _Pi, _Pl);
	ADDFAC(_Tw3, _C, _Pi, _Pl);
	ADDFAC(_Tw4, _C, _Pi, _Pl);
	ADDFAC(_Tw5, _C, _Pi, _Pl);
	ADDFAC(_Tw6, _C, _Pi, _Pl);
	ADDFAC(_Tw7, _C, _Pi, _Pl);
	ADDFAC(_Tw8, _C, _Pi, _Pl);
	ADDFAC(_Tw9, _C, _Pi, _Pl);
	ADDFAC(_Tw10, _C, _Pi, _Pl);
	ADDFAC(_Tw11, _C, _Pi, _Pl);
	ADDFAC(_Tw12, _C, _Pi, _Pl);
	ADDFAC(_Tw13, _C, _Pi, _Pl);
	ADDFAC(_Tw14, _C, _Pi, _Pl);
	//...
	ADDFAC(_Twc1, _C, _Pi, _Pl);
	ADDFAC(_Twc2, _C, _Pi, _Pl);
	}


/*
 * Copyright (c) 1995 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */
