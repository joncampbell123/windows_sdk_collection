#include "windows.h"
#include "winexp.h"

#define errNoPIF        1
#define errNoFile       2
#define errNoCreate     3
#define errMaxMin       4
#define errHiLow        5
#define warSave         7
#define warOverwrite    8

#define errTitle        0
#define EINSMEMORY      9
#define EFILEEXISTS      10
#define EINVALIDFILE    11
#define errDiskFull     12
#define IABOUT          13
#define errBadExt       14
#define errBadProgram   15
#define errBadNumber    16
#define HELPTOPIC1      17
#define CHELPTOPICS     10

#define ID_PIFICON     1
#define ID_PIFMENU     1
#define ID_PIFTEMPLATE 1


/* menu mnemonic for use in PifMenuMnemonic */
/* must be mnemonic for toplevel menu items, capitalized */
#define MENUMNEMONIC1   'F'


/* Menu commands */
#define M_OPEN      10
#define M_SAVE      11
#define M_NEW       12
#define M_SAVEAS    13
#define M_ABOUT     14
#define M_EXIT      15
#define M_HELP      16

/* edit fields IDs */
#define IDI_ENAME       100
#define IDI_ETITLE      101
#define IDI_EPARM       102
#define IDI_EPATH       103
#define IDI_MEMREQ      104
#define IDI_MEMDES      105

/* "directly modifies" checkbox group */
#define IDI_DMSCREEN    200
#define IDI_DMCOM1      201
#define IDI_DM8087      202
#define IDI_DMKBD       203
#define IDI_DMCOM2      204
#define IDI_DMMEM       205

/* Program switch radio group */
#define IDI_PSFIRST     300
#define IDI_PSNONE      300
#define IDI_PSTEXT      301
#define IDI_PSGRAPH     302
#define IDI_PSLAST      302

/* Screen exchange radio group */
#define IDI_SEFIRST     400
#define IDI_SENONE      400
#define IDI_SETEXT      401
#define IDI_SEGRAPH     402
#define IDI_SELAST      402

/* Close window checkbox group */
#define IDI_EXIT        500

#define PIFNAMESIZE 30
#define PIFSTARTLOCSIZE 63
#define PIFDEFPATHSIZE 64
#define PIFPARAMSSIZE 64
#define PIFSHPROGSIZE 64
#define PIFSHDATASIZE 64

#define PIFSIZE 367

typedef struct {
    char unknown;
    char id;
    char name[PIFNAMESIZE];
    unsigned maxmem;
    unsigned minmem;
    char startfile[PIFSTARTLOCSIZE];
    char MSflags;
    char reserved;
    char defpath[PIFDEFPATHSIZE];
    char params[PIFPARAMSSIZE];
    char screen;
    char cPages;
    unsigned char lowVector;
    unsigned char highVector;
    char rows;
    char cols;
    char rowoff;
    char coloff;
    unsigned sysmem;
    char shprog[PIFSHPROGSIZE];
    char shdata[PIFSHDATASIZE];
    unsigned char behavior;
    char sysflags;
    } PIFSTRUCT;


#define SWAPS 0x20
#define SWAPMASK 0x20
#define NOTSWAPMASK 0xdf

#define PARMS 0x40
#define PARMMASK 0x40
#define NOTPARMMASK 0xbf

#define SCR 0xC0
#define SCRMASK 0xC0
#define NOTSCRMASK 0x3f

#define MASK8087 0x20
#define NOTMASK8087 0xdf
#define KEYMASK 0x10
#define NOTKEYMASK 0xef

/* Microsoft PIF flags */

#define MEMMASK 0x01
#define NOTMEMMASK 0xfe

#define GRAPHMASK 0x02
#define TEXTMASK 0xfd

#define PSMASK 0x04
#define NOTPSMASK 0xfb

#define SGMASK 0x08
#define NOTSGMASK 0xf7

#define EXITMASK 0x10
#define NOTEXITMASK 0xef

#define DONTUSE 0x20

#define COM2MASK 0x40
#define NOTCOM2MASK 0xbf

#define COM1MASK 0x80
#define NOTCOM1MASK 0x7f


extern char hextobyte(LPSTR);

#define DTOPEN      10
#define DTSAVE      11
#define DTABOUT     12
#define DTHELP      13

#define ID_PATH     10
#define ID_EDIT     11
#define ID_LISTBOX  12
#define IDTOPICS    13
#define IDNEXT      14
#define IDPREVIOUS  15
#define IDSCROLL    16
#define IDTEXTBOX   17

#define RT_TEXTTYPE 20
#define HELPTEXT 1
