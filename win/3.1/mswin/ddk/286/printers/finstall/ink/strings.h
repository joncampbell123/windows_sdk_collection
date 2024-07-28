
/**[f******************************************************************
* strings.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
/*********************************************************************
* 25 oct 91    RLK(HP) To remove strings from code for localization added the 
*              following: SF_INSTALMSG. Bug 718                                  
* 15 aug 91    RLK(HP) To remove strings from code for localization added the 
*              following: SF_NOMEMORYDLG, SF_AUTOFONT,SF_CARTRIDGEF,SF_
*              SCALABLETF,SF_INSTALLFONT,SF_BUILDFONT,SF_TYPEFILEERR,
*              SF_NOFONTINSTAL,SF_NOSCALABLE,SF_NOEDITSCALE,SF_INSTALLAF,
*              SF_READING,SF_WRITING,SF_SS_OFFSET.
* 17 jul 91    RLK(HP) Added SFADD_DRIVE and SFADD-BADDRIVE for message about
*              Drive xx is invalid or does not contain a floppy disk.
* 25 jun 91    RLK(HP) Added SFADD_STATICTEXT for SFADDFONT Dialog static text.
* 09 nov 89    peterbe Added SFADD_OLDDIRF for old "SFINSTAL.DIR"
* 09 oct 89    peterbe Added SF_INITWAIT
* 27 sep 89    peterbe Added SF_NOSOFT.
* 23 aug 89    peterbe Added SF_HELP.
* 17 jul 89    peterbe Adding string indices for DeskJet dialog.
* 15 apr 89    peterbe Removed SF_ABOUTSTR, SF_SYSABOUT,
*          added SF_ABOUT, SF_SYSABOUT.
*  7 mar 89    peterbe Removed SF_YESEDIT, SF_NOEDIT, some SF_SYS*.
*          Added SF_COPYPORT.
*   1-26-89    jimmat  Created by chopping up HPPCL version.
*/
  
// Dialog item ID's
  
#define SFDL_NOW            16
#define SFDL_STARTUP        32
#define SFDL_PORT           64
  
#define SFDU_PCNT           10
  
#define SF_PRINTER_LEFT     100
#define SF_PRINTER_RIGHT    101
#define SF_POINTER          102
#define SF_LB_LEFT          103
#define SF_LB_RIGHT         104
#define SF_MOVE             105
#define SF_COPY             106
#define SF_ERASE            107
#define SF_EDIT             108
#define SF_EXIT             IDCANCEL
#define SF_PERM_LEFT        110
#define SF_TEMP_LEFT        111
#define SF_PERM_RIGHT       112
#define SF_TEMP_RIGHT       113
#define SF_ADD_RIGHT        114
#define SF_STATUS           115
#define SF_INITLB_LEFT      200
#define SF_IGNORMESSAGES    201
#define SF_COPYPORT         202
#define SF_ABOUT            203
#define SF_HELP             204
  
/*  Width of string above listbox in #chars
*/
#define SFLB_CAPTIONWID     29
  
  
/*  String id's
Strings are grouped into different segments based on ID numbers,
0-15 in one segment, 13-31 in another,etc. .EXE file will be shorter
if use consecutive ID numbers. But since the entire segment of up to
16 strings will be loaded whenever a string from the segment is called
one should only include strings in the segment that are used together.
Blank lines indicate segment boundaries. Gaps in sequence are indicated
by a comment line.
*/
#define SF_STR_BASE         1
  
#define SF_NOMEMORYDLG      (SF_STR_BASE+0)
  
#define SF_LLSRJETIII       (SF_STR_BASE+12)
#define SF_RLSRJETIII       (SF_STR_BASE+13)
#define SF_LPAINTJET        (SF_STR_BASE+14)
#define SF_RPAINTJET        (SF_STR_BASE+15)
#define SF_LDESKJET         (SF_STR_BASE+16)
#define SF_RDESKJET         (SF_STR_BASE+17)
#define SF_LDESKJETPLUS     (SF_STR_BASE+18)
#define SF_RDESKJETPLUS     (SF_STR_BASE+19)
#define SF_COPYRIGHT        (SF_STR_BASE+20)
#define SF_LLASERJET        (SF_STR_BASE+21)
#define SF_RLASERJET        (SF_STR_BASE+22)
#define SF_NOFNT            (SF_STR_BASE+23)
#define SF_POINT            (SF_STR_BASE+24)
#define SF_BOLD             (SF_STR_BASE+25)
#define SF_ITALIC           (SF_STR_BASE+26)
#define SF_PORT             (SF_STR_BASE+27)
#define SF_LAND             (SF_STR_BASE+28)
#define SF_IDSTR            (SF_STR_BASE+29)
#define SF_SOFTFONT         (SF_STR_BASE+30)

#define SF_TKPACKAGE        (SF_STR_BASE+31)
#define SF_TKFAMILY         (SF_STR_BASE+32)
#define SF_TKLDRIVE         (SF_STR_BASE+33)
#define SF_TKPORT           (SF_STR_BASE+34)
#define SF_TKLAND           (SF_STR_BASE+35)
#define SF_NODESCSTR        (SF_STR_BASE+36)
#define SFADD_PATH          (SF_STR_BASE+37)
#define SFADD_DIRFILE       (SF_STR_BASE+38)
#define SFADD_RPTERR        (SF_STR_BASE+39)
#define SFADD_ERRFILE       (SF_STR_BASE+40)
#define SFADD_LBLSPEC       (SF_STR_BASE+41)
#define SFADD_ALLFSPEC      (SF_STR_BASE+42)
#define SFADD_CLOSESTR      (SF_STR_BASE+43)
#define SFADD_ADDSTR        (SF_STR_BASE+44)
#define SFADD_ADDTEXT       (SF_STR_BASE+45)
#define SFADD_MOVETEXT      (SF_STR_BASE+46)

#define SFADD_DRIVETEXT     (SF_STR_BASE+47)
#define SFADD_TARGDIR       (SF_STR_BASE+48)
#define SF_YESPORT          (SF_STR_BASE+49)
#define SF_INITWAIT         (SF_STR_BASE+50)
#define SF_NOPORT           (SF_STR_BASE+51)
#define SF_NOSOFT           (SF_STR_BASE+52)
#define SF_CNCLSTR          (SF_STR_BASE+53)
#define SF_EXITSTR          (SF_STR_BASE+54)
#define SF_ADDREADY         (SF_STR_BASE+55)
#define SF_APPFNT           (SF_STR_BASE+56)
#define SFINSTAL_NM         (SF_STR_BASE+57)
#define SF_QUESTION         (SF_STR_BASE+58)
#define SF_SOFTFONTS        (SF_STR_BASE+59)
#define SF_TKINVALSTR       (SF_STR_BASE+60)
#define SF_TKINVALTK        (SF_STR_BASE+61)
#define SF_TKINVALDRV       (SF_STR_BASE+62)


#define SF_TKEXPEQUAL       (SF_STR_BASE+63)
#define SF_TKINVALLBL       (SF_STR_BASE+64)
#define SF_TKEXPLCMA        (SF_STR_BASE+65)
#define SF_TKEXPLSTR        (SF_STR_BASE+66)
#define SF_TKMAXDRV         (SF_STR_BASE+67)
#define SF_TKBADBRACE       (SF_STR_BASE+68)
#define SF_TKSTRTOOBIG      (SF_STR_BASE+69)
#define SF_TKINVALORIENT    (SF_STR_BASE+70)
#define SF_TKNODLFILE       (SF_STR_BASE+71)
#define SF_TKDUPDRV         (SF_STR_BASE+72)
#define SF_TKNOPFMFILE      (SF_STR_BASE+73)
#define SF_TKBADCLOSEQ      (SF_STR_BASE+74)
#define SF_TKOOMCAP         (SF_STR_BASE+75)
#define SF_TKOOMMSG         (SF_STR_BASE+76)
#define SF_TKEXPFBRC        (SF_STR_BASE+77)
#define SF_TKNOCLOSEBRC     (SF_STR_BASE+78)

#define SF_TKLINENUM        (SF_STR_BASE+79)
#define SF_TKAROUNDCH       (SF_STR_BASE+80)
#define SF_TKAROUNDEOL      (SF_STR_BASE+81)
#define SF_TKERRSFOUND      (SF_STR_BASE+82)
#define SF_TKBADWIDTH       (SF_STR_BASE+83)
#define SF_TKBADHEIGHT      (SF_STR_BASE+84)
#define SF_TKEXPSCRFNM      (SF_STR_BASE+85)
#define SF_TKUNKDRV         (SF_STR_BASE+86)
#define SF_TKCARTRIDGE      (SF_STR_BASE+87)
#define SF_TKNOPCMFILE      (SF_STR_BASE+88)
#define SF_CARTRIDGE        (SF_STR_BASE+89)
#define SF_ADDSUCCESS       (SF_STR_BASE+90)
#define SF_RMVSUCCESS       (SF_STR_BASE+91)
#define SF_PFMDEVNM         (SF_STR_BASE+92)
#define SF_MOVSUCCESS       (SF_STR_BASE+93)
#define SF_CPYSUCCESS       (SF_STR_BASE+94)

#define SF_EDSUCCESS        (SF_STR_BASE+95)
#define SF_NOFNTFOUND       (SF_STR_BASE+96)
#define SF_DIRNOTFOUND      (SF_STR_BASE+97)
#define SF_AUTOFONT         (SF_STR_BASE+98)
#define SFADD_OLDDIRF       (SF_STR_BASE+99)
#define SFADD_DEFPATH       (SF_STR_BASE+100)
#define SFADD_DEFDIRF       (SF_STR_BASE+101)
#define SFADD_DEFERRF       (SF_STR_BASE+102)
#define SFADD_NOPTHCAP      (SF_STR_BASE+103)
#define SFADD_NOPTHMSG      (SF_STR_BASE+104)
#define SFADD_NODIRFMSG     (SF_STR_BASE+105)
#define SFADD_NOERRFMSG     (SF_STR_BASE+106)
#define SFADD_DIRKEYNM      (SF_STR_BASE+107)
#define SFADD_DEFTARG       (SF_STR_BASE+108)
#define SFADD_NEWDIR        (SF_STR_BASE+109)
#define SFADD_BADDIRCAP     (SF_STR_BASE+110)

#define SFADD_BADDIRMSG     (SF_STR_BASE+111)
#define SFADD_NOADD         (SF_STR_BASE+112)
#define SFADD_NOGENPFM      (SF_STR_BASE+113)
#define SF_CARTRIDGEF       (SF_STR_BASE+114)
#define SF_SCALABLETF       (SF_STR_BASE+115)
#define SFADD_CHNGDSK       (SF_STR_BASE+116)
#define SFADD_PROMPT1       (SF_STR_BASE+117)
#define SFADD_PROMPT2       (SF_STR_BASE+118)
#define SFADD_PROMPT3       (SF_STR_BASE+119)
#define SFADD_NOCOPY        (SF_STR_BASE+120)
#define SFADD_NOFIND        (SF_STR_BASE+121)
#define SFADD_NODEST        (SF_STR_BASE+122)
#define SFADD_ADDING        (SF_STR_BASE+123)
#define SFADD_BLDPFM        (SF_STR_BASE+124)
#define SFADD_SCAN          (SF_STR_BASE+125)
#define SF_RMVING           (SF_STR_BASE+126)

#define SFADD_REPDUP        (SF_STR_BASE+127)
#define SFADD_NOROOM        (SF_STR_BASE+128)
#define SFADD_DISKWITH      (SF_STR_BASE+129)
#define SF_REMVCAP          (SF_STR_BASE+130)
#define SF_REMVMSG          (SF_STR_BASE+131)
#define SF_INSTALLFONT      (SF_STR_BASE+132)
#define SF_BUILDFONT        (SF_STR_BASE+133)
#define SF_TYPEFILEERR      (SF_STR_BASE+134)
#define SF_NOFONTINSTAL     (SF_STR_BASE+135)
#define SF_NOSCALABLE       (SF_STR_BASE+136)
#define SF_NOEDITSCALE      (SF_STR_BASE+137)
#define SF_INSTALMSG        (SF_STR_BASE+138)
/* 139 
*/
#define SF_TKSCRNCAP        (SF_STR_BASE+140)
#define SF_TKSCRNMSG1       (SF_STR_BASE+141)
#define SF_TKSCRNMSG2       (SF_STR_BASE+142)

#define SF_TKSCRNCGA        (SF_STR_BASE+143)
#define SF_TKSCRNEGA        (SF_STR_BASE+144)
#define SF_TKSCRN1TO1       (SF_STR_BASE+145)
#define SF_TKSCRNUNDF       (SF_STR_BASE+146)
#define SF_INSTALLAF        (SF_STR_BASE+147)
#define SF_READING          (SF_STR_BASE+148)
#define SF_WRITING          (SF_STR_BASE+149)
#define SFNODL_FONT         (SF_STR_BASE+150)
#define SFNODL_PFM          (SF_STR_BASE+151)
#define SFNODL_DL           (SF_STR_BASE+152)
#define SFNODL_FILE         (SF_STR_BASE+153)
#define SFNODL_BADFILE      (SF_STR_BASE+154)
/* 155-159
*/  

#define SFCPY_TARGPORT      (SF_STR_BASE+160)
#define SFCPY_NPCAP         (SF_STR_BASE+161)
#define SFCPY_NPMSG         (SF_STR_BASE+162)
#define SF_APPPORTS         (SF_STR_BASE+163)
#define SFCPY_MOVING        (SF_STR_BASE+164)
#define SFCPY_CPYING        (SF_STR_BASE+165)
#define SFED_EDITING        (SF_STR_BASE+166)
/* 167-169
*/  
#define SFED_FILE           (SF_STR_BASE+170)
#define SFED_DESC           (SF_STR_BASE+171)
#define SFED_NAME           (SF_STR_BASE+172)
#define SFED_ID             (SF_STR_BASE+173)
#define SFED_PERM           (SF_STR_BASE+174)

#define SFED_TEMP           (SF_STR_BASE+175)
#define SFED_ROMAN          (SF_STR_BASE+176)
#define SFED_SWISS          (SF_STR_BASE+177)
#define SFED_MODERN         (SF_STR_BASE+178)
#define SFED_SCRIPT         (SF_STR_BASE+179)
#define SFED_DECORATIVE     (SF_STR_BASE+180)
#define SFED_DONTCARE       (SF_STR_BASE+181)
#define SFED_GLOBAL         (SF_STR_BASE+182)
#define IDNEXT              (SF_STR_BASE+183)
#define SFED_DUPIDNM        (SF_STR_BASE+184)
#define SFED_BADIDMSG       (SF_STR_BASE+185)
#define SFED_BADIDCAP       (SF_STR_BASE+186)
#define SFED_BADNMMSG       (SF_STR_BASE+187)
#define SFED_BADNMCAP       (SF_STR_BASE+188)
#define SFED_WRPFMMSG       (SF_STR_BASE+189)
#define SFED_WRPRMCAP       (SF_STR_BASE+190)
/* 191-220
*/  
#define SFDL_DLKEYNM        (SF_STR_BASE+221)
#define SF_DOSECHO          (SF_STR_BASE+222)

#define SF_DOSCOPY          (SF_STR_BASE+223)
#define SF_DOSERASE         (SF_STR_BASE+224)
#define SF_DOSBINARY        (SF_STR_BASE+225)
#define SF_DOSALPHA         (SF_STR_BASE+226)
#define SF_DOSCOMMAND       (SF_STR_BASE+227)
#define SF_DOSREM           (SF_STR_BASE+228)
#define SFDL_TMP1FILNM      (SF_STR_BASE+229)
#define SFDL_TMP2FILNM      (SF_STR_BASE+230)
#define SFDL_TMP3FILNM      (SF_STR_BASE+231)
#define SFDL_HEADER         (SF_STR_BASE+232)
#define SFDL_BATCHHEAD      (SF_STR_BASE+233)
#define SFDL_AUTOEXEC       (SF_STR_BASE+234)
#define SFDL_STYLE          (SF_STR_BASE+235)
#define IDHELP              (SF_STR_BASE+236)
#define SFDL_NOAUTOCAP      (SF_STR_BASE+237)
#define SFDL_NOAUTOMSG      (SF_STR_BASE+238)

#define SFDL_SPOOLNM        (SF_STR_BASE+239)
#define SFDL_DLMSG          (SF_STR_BASE+240)
#define SFDL_AEMSG          (SF_STR_BASE+241)
#define SFDL_YNTEST         (SF_STR_BASE+242)
#define SFDL_YNLABEL        (SF_STR_BASE+243)
#define SFDL_YNFILENM       (SF_STR_BASE+244)
#define SFDL_FAMILY         (SF_STR_BASE+245)
#define SFDL_NOSFDMSG       (SF_STR_BASE+246)
#define SFDL_DIRNAME        (SF_STR_BASE+247)
#define SFDL_DIREXIST       (SF_STR_BASE+248)
#define SFDL_CARTRIDGE      (SF_STR_BASE+249)
#define SYMBOL_SET          (SF_STR_BASE+250)
#define TYP_TARGDIR         (SF_STR_BASE+251)
#define PCLEO_TARGDIR       (SF_STR_BASE+252)
#define TYP_TARGDEST        (SF_STR_BASE+253)
#define TYP_DIRKEYNM        (SF_STR_BASE+254)
#define TYP_DEFTARGDIR      (SF_STR_BASE+255)
/* 256-259
*/
#define SFADD_STATICTEXT    (SF_STR_BASE+260)
#define SFADD_DRIVE         (SF_STR_BASE+261)
#define SFADD_BADDRIVE      (SF_STR_BASE+262)
/* 263-299
*/  
#define SF_CHSET_MATH8      (SF_STR_BASE+300)
#define SF_CHSET_PIFONT     (SF_STR_BASE+301)
#define SF_CHSET_LINEDRAW   (SF_STR_BASE+302)

#define SF_CHSET_PCLINE     (SF_STR_BASE+303)
#define SF_CHSET_USLEGAL    (SF_STR_BASE+304)
  
#define SF_FACE_OFFSET      SF_STR_BASE+305 /* 256 strings reserved */
                                            /* for typeface names   */  
  
#define SFLIB_NONE_PRINTER  (SF_STR_BASE+310)
/* This becomes R,Tms Rmn
*/


#define SF_SS_OFFSET        (SF_STR_BASE+575)/* 16 strings reserved  */
                                             /* for symbol set names */
