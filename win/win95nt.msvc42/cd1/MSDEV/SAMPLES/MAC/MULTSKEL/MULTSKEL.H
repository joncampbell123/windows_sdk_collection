/*
	TransSkel multiple-window demonstration: Header file.
	
	#includes standard header files used by all demo modules, #defines
	demo resource numbers and some values used many places, such as nil.

	14 Jun 1986	Paul DuBois
	29 Jan 1989 Conversion for TransSkel 2.0.  Integer should be a
				typedef for compiler 2-byte integer type.  Longint should
				be a typedef for the 4-byte integer type.
*/

/* to work with new headers */
#define dangerousPattern

# include	<macos\msvcmac.h>
# include	<macos\Desk.h>
# include	<macos\Dialogs.h>
# include	<macos\Memory.h>
# include	<macos\Menus.h>

# include	<macos\Windows.h>	/* pulls in MacTypes, QuickDraw */

typedef	short	Integer;
typedef long	Longint;


/*
	resource numbers
*/

# define	aboutAlrt		1000
# define	getInfoDlog		1001

# define	fileMenuRes		1001
# define	editMenuRes		1002

# define	helpWindRes		1000
# define	editWindRes		1001
# define	zoomWindRes		1002
# define	rgnWindRes		1003

# define	helpTextRes		1000


extern	MenuHandle	editMenu;

extern	WindowPtr	helpWind;
extern	WindowPtr	editWind;
extern	WindowPtr	zoomWind;
extern	WindowPtr	rgnWind;
