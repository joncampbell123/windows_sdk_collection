// xlocale -- class locale exotic char member functions
#include <istream>
#include <locale>

typedef char_traits<char> _Traits;
typedef istreambuf_iterator<char, _Traits> _Initer;
typedef ostreambuf_iterator<char, _Traits> _Outiter;

	// facets associated with C categories
#define ADDFAC(T, cat, pi, pl) \
	if ((_CATMASK(T::_Getcat()) & cat) == 0) \
		; \
	else if (pl == 0) \
		pi->_Addfac(new T(_Lobj), T::id); \
	else \
		pi->_Addfac((locale::facet *)&_USE(*pl, T), T::id); 
typedef collate<char> _T5;
typedef messages<char> _T6;
typedef money_get<char, false, _Initer> _T7;
typedef money_get<char, true, _Initer> _T8;
typedef money_put<char, false, _Outiter> _T9;
typedef money_put<char, true, _Outiter> _T10;
typedef moneypunct<char, false> _T11;
typedef moneypunct<char, true> _T12;
typedef time_get<char, _Initer> _T13;
typedef time_put<char, _Outiter> _T14;

void __cdecl locale::_Locimp::_Makexloc(const _Locinfo& _Lobj,
	locale::category _C, _Locimp *_Pi, const locale *_Pl)
	{	// setup exotic char part of a new locale
	ADDFAC(_T5, _C, _Pi, _Pl);
	ADDFAC(_T6, _C, _Pi, _Pl);
	ADDFAC(_T7, _C, _Pi, _Pl);
	ADDFAC(_T8, _C, _Pi, _Pl);
	ADDFAC(_T9, _C, _Pi, _Pl);
	ADDFAC(_T10, _C, _Pi, _Pl);
	ADDFAC(_T11, _C, _Pi, _Pl);
	ADDFAC(_T12, _C, _Pi, _Pl);
	ADDFAC(_T13, _C, _Pi, _Pl);
	ADDFAC(_T14, _C, _Pi, _Pl);
	}


/*
 * Copyright (c) 1995 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */
