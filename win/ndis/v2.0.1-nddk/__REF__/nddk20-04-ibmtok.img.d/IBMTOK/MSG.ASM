                page    88,122
                title   MSG - Messages and Banners (2.0.1 Spec)

;****************************************************************************;
;*                                                                          *;
;*      (C) Copyright 1988,1989 by 3Com Corporation                         *;
;*                                                                          *;
;*      This IBM Token Ring NDIS Driver source is the property of Microsoft *;
;*      Corporation and is provided only as an example of an NDIS MAC       *;
;*      driver for use with Microsoft's OS/2 LAN Manager, DOS LAN Manager,  *;
;*      or LM/X products.                                                   *;
;*                                                                          *;
;****************************************************************************;

;------ MSG.ASM -------------------------------------------------------------;
;                                                                            ;
;       Language dependent data and code.                                    ;
;                                                                            ;
;       This file contains all of the messages, banners and keyword names    ;
;       used by the MAC driver.  These are language-dependent and therefore  ;
;       multiple linkable versions of this module may exist.                 ;
;                                                                            ;
;       This is the only file that should need any changing for the          ;
;       internationalization of the MAC driver.  It contains all of the      ;
;       messages, banners, keyword names and text strings.  It also          ;
;       contains a few functions which handle simple conversions that may    ;
;       be dependent on alphabet.                                            ;
;                                                                            ;
;       09-21-88 Connery                                                     ;
;       Original version.                                                    ;
;                                                                            ;
;       03-17-89 Connery                                                     ;
;       Now version 1.1a.  Added EARLYRELEASE keyword as part of the 16/4    ;
;       adapter support.  Added upcase, hexchar and charhex functions to     ;
;       remove alphabet dependencies from the init code.                     ;
;                                                                            ;
;----------------------------------------------------------------------------;

.xlist
include includes.mac
.list
.lfcond
.sall

;------ Exported Interfaces -------------------------------------------------;

                public  msg_signon              ; INIT time only
                public  msg_generalfailure      ; "
                public  msg_nosuchkeyword       ; "
                public  msg_invalidrhs          ; "
                public  msg_badconfigure        ; "
                public  msg_nohardware          ; "
                public  msg_pressakey           ; "
                public  msg_crlf                ; "
                public  msg_equals              ; "
                public  msg_quote               ; "
                public  msg_openparen           ; "
                public  msg_closeparen          ; "
                public  msg_preamble            ; "
                public  msg_moretocome          ; "
                public  keyn_drivername         ; "
                public  keyn_primary            ; "
                public  keyn_alternate          ; "
                public  keyn_recvbufs           ; "
                public  keyn_recvbuflen         ; "
                public  keyn_xmitbufs           ; "
                public  keyn_xmitbuflen         ; "
                public  keyn_ram                ; "
                public  keyn_productid          ; "
                public  keyn_netaddr            ; "
                public  keyn_transmits          ; "
                public  keyn_earlyrelease       ; "

                public  upcase                  ; "
                public  hexchar                 ; "
                public  charhex                 ; "

NEWPAGE <Banner>
                BEGIN_INIT_DATA
;
; copyright message.  not displayed
;
                db      '(C) Copyright 3Com Corporation, 1988.',CR,LF
                db      '(C) Copyright Microsoft Corporation, 1988.',CR,LF
                db      'All rights reserved.',CR,LF,0
;
; signon message.  will have to be modified to handle version numbers over 9.
; prefixed by '%VER ' to allow a generic utility to locate/dump the message.
;
ALPHA_MAJOR     equ     '0'+MAJOR_MAC_VERSION
ALPHA_MINOR     equ     '0'+MINOR_MAC_VERSION
ALPHA_SUB       equ     'a'+SUB_MAC_VERSION-1

                db      '%VER '
msg_signon      db      'MS ',OSNAME,' LAN Manager Network Driver '
                db      'for IBM Token Ring '
                db      'v',ALPHA_MAJOR,'.',ALPHA_MINOR
IFNDEF DEV
                db      0
ENDIF
                db      ALPHA_SUB
                db      0,0

NEWPAGE <Messages>
;
; initialization error messages.
;
msg_generalfailure  db  'MAC0001: Initialization failure',CR,LF,0
msg_invalidrhs      db  'MAC0004: Syntax error in PROTOCOL.INI - '
                    db  'Invalid Parameter Value',CR,LF,0
msg_nosuchkeyword   db  'MAC0005: Syntax error in PROTOCOL.INI - '
                    db  'Unrecognized Keyword',CR,LF,0
msg_nohardware      db  'MAC0006: Network Interface Hardware Not Found or Not Responding',CR,LF,0
msg_badconfigure    db  'MAC0007: Configuration error in PROTOCOL.INI',CR,LF,0
msg_pressakey       db  'Press any key to continue...',CR,LF,0
;
; miscellaneous message parts.
;
msg_crlf        db      CR,LF,0
msg_equals      db      ' = ',0
msg_quote       db      '"',0
msg_openparen   db      ' (0x',0
msg_closeparen  db      ')',0
msg_preamble    db      '-->      ',0
msg_moretocome  db      ', ...',0

NEWPAGE <Keywords>
;
; keyword names, 15 characters maximum.  the protocol manager truncates the
; keywords when they are read from PROTOCOL.INI so for StringEquals to work,
; these must be truncated at 15 characters as well.
;
keyn_drivername     db  'DRIVERNAME',0
keyn_primary        db  'PRIMARY',0
keyn_alternate      db  'ALTERNATE',0
keyn_recvbufs       db  'RECVBUFS',0
keyn_recvbuflen     db  'RECVBUFSIZE',0
keyn_xmitbufs       db  'XMITBUFS',0
keyn_xmitbuflen     db  'XMITBUFSIZE',0
keyn_ram            db  'RAM',0
keyn_productid      db  'PRODUCTID',0
keyn_netaddr        db  'NETADDRESS',0
keyn_transmits      db  'MAXTRANSMITS',0
keyn_earlyrelease   db  'EARLYRELEASE',0

                END_INIT_DATA
                BEGIN_INIT_CODE

NEWPAGE <upcase/hexchar/charhex>
;------ upcase --------------------------------------------------------------;
;                                                                            ;
;       Converts a character in AL from lowercase to uppercase.              ;
;                                                                            ;
;       This implementation is for U.S. ASCII.                               ;
;                                                                            ;
;       Assumes:                                                             ;
;       AL = character to convert, or not                                    ;
;                                                                            ;
;       Returns:                                                             ;
;       AL = uppercase character                                             ;
;                                                                            ;
;       Preserves all other registers.                                       ;
;                                                                            ;
;----------------------------------------------------------------------------;

upcase          proc    near
                cmp     al, 'a'
                jb      @F
                cmp     al, 'z'
                ja      @F
                sub     al, 'a'-'A'
@@:             ret
upcase          endp

;------ hexchar -------------------------------------------------------------;
;                                                                            ;
;       Converts a binary digit in AL to a hex character.                    ;
;                                                                            ;
;       This implementation is for U.S. ASCII.                               ;
;                                                                            ;
;       Assumes:                                                             ;
;       AL = binary digit to convert                                         ;
;                                                                            ;
;       Returns:                                                             ;
;       AL = hex character, '0' thru 'F'                                     ;
;                                                                            ;
;       Preserves all other registers.                                       ;
;                                                                            ;
;----------------------------------------------------------------------------;

hexchar         proc    near
                add     al, '0'                 ; convert to ASCII
                cmp     al, '9'                 ; greater than 9?
                jbe     @F                      ; no, not in A-F range
                add     al, 'A'-'9'-1           ; map to uppercase letter
@@:             ret
hexchar         endp

;------ charhex -------------------------------------------------------------;
;                                                                            ;
;       Converts a hex character in AL to a binary digit.                    ;
;                                                                            ;
;       This implementation is for U.S. ASCII.                               ;
;                                                                            ;
;       Assumes:                                                             ;
;       AL = hex character, '0' thru 'F'                                     ;
;                                                                            ;
;       Returns:                                                             ;
;       AL = binary digit                                                    ;
;       carry set if an invalid character                                    ;
;                                                                            ;
;       Preserves all other registers.                                       ;
;                                                                            ;
;----------------------------------------------------------------------------;

charhex         proc    near
                cmp     al,'0'
                jb      ch_fail
                cmp     al,'9'
                ja      @F
                sub     al,'0'
                ret
@@:             call    upcase
                cmp     al,'A'
                jb      ch_fail
                cmp     al,'F'
                ja      ch_fail
                sub     al,'A' - 0Ah
                ret
ch_fail:        stc
                ret
charhex         endp

                END_INIT_CODE
                end
