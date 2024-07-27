/*
This file contains the definitions of the numerical indexes to the menu
items (imi) used by Windows Word.
Each imi should be unique and be continuous within the same
menu. The middle 2 bytes of imi are masked against MENUMASK to
provide the submenu index.
IF any of the menu item order has changed, menu.c has
to be modified also -- rgmfAllItem, rgmfScrap, SetMenuFlags etc.
*/

/* number of pulldown submenus */
#define CMENUS 6

/* menu index */
#define FILE 0
#define EDIT 1
#define FIND 2
#define CHARACTER 3
#define PARA 4
#define DIV 5

#define MENUMASK	   0x0ff0
#define FILEMENU	   0x0000
#define EDITMENU	   0x0010
#define FINDMENU	   0x0020
#define CHARMENU	   0x0040
#define PARAMENU	   0x0080
#define DOCUMENU	   0x0100

#define fMenuItem			0x1000

#define imiNil				0x1fff
#define imiHelp 			0xf2f0

/* Menu items */
#define imiAbout   			0x1000
#define imiNew				0x1001
#define imiOpen 			0x1002
#define imiSave 			0x1003
#define imiSaveAs   		0x1004
#define imiPrint			0x1005
#define imiPrintSetup		0x1006
#define imiRepaginate		0x1007
#define imiQuit    			0x1008
#define imiFileMin			(imiAbout)
#define imiFileMax			(imiQuit + 1)

#define imiUndo 			0x1010
#define imiCut				0x1011
#define imiCopy 			0x1012
#define imiPaste			0x1013
#define imiMovePicture			0x1014
#define imiSizePicture			0x1015
#define imiEditMin	   (imiUndo)
#define imiEditMax	   (imiSizePicture + 1)

#define imiFind 			0x1020
#define imiFindAgain	   0x1021
#define imiChange			0x1022
#define imiGoTo 			 0x1023
#define imiFindMin	   (imiFind)
#define imiFindMax	   (imiGoTo + 1)

#define imiCharNormal	   0x1040
#define imiBold 				0x1041
#define imiItalic			0x1042
#define imiUnderline			0x1043
#define imiSuper			0x1044
#define imiSub				0x1045
#define imiFont1	   0x1046
#define imiFont2	   0x1047
#define imiFont3	   0x1048
#define imiSmFont	   0x1049
#define imiLgFont	   0x104a
#define imiCharFormats		0x104b
#define imiCharMin	   (imiCharNormal)
#define imiCharMax	   (imiCharFormats + 1)

/* special accelerator key
#define imiAccelBold	   0x104c
#define imiAccelItalic	   0x104d
#define imiAccelUnderline  0x104e*/

#ifdef KANJI
#define imiParaNormal			0x1080
#define imiLeft 			0x1081
#define imiCenter			0x1082
#define imiRight			0x1083
#define imiSingleSpace			0x1084
#define imiOneandhalfSpace		0x1085
#define imiDoubleSpace			0x1086
#define imiParaFormats			0x1087
#define imiParaMin			(imiParaNormal)
#define imiParaMax			(imiParaFormats + 1)
#else
#define imiParaNormal			0x1080
#define imiLeft 			0x1081
#define imiCenter			0x1082
#define imiRight			0x1083
#define imiJustified			0x1084
#define imiSingleSpace			0x1085
#define imiOneandhalfSpace		0x1086
#define imiDoubleSpace			0x1087
#define imiParaFormats			0x1088
#define imiParaMin			(imiParaNormal)
#define imiParaMax			(imiParaFormats + 1)
#endif /*  if-else-def KANJI  */

#define imiHeader		 0x1100
#define imiFooter		0x1101
#define imiShowRuler		     0x1102
#define imiTabs 		0x1103
#define imiDivFormats		0x1104
#define imiDocuMin	   (imiHeader)
#define imiDocuMax	   (imiDivFormats + 1)

#ifdef CASHMERE
#define imiFootnote
#define imiPreferences
#endif

#ifdef ENABLE /* CFILE, CEDIT ... */
/* number of items in each submenu */
#define CFILE  (imiFileMax - imiFileMin)
#define CEDIT  (imiEditMax - imiEditMin)
#define CFIND  (imiFindMax - imiFindMin)
#define CCHAR  (imiCharMax - imiCharMin)
#define CPARA  (imiParaMax - imiParaMin)
#define CDOCU  (imiDocuMax - imiDocuMin)
#endif
