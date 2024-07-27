; This module defines the OEM dependent Virtual keys. If an OEM has the
;  following symbols on a single keycap, then the following are suggested
;  as part of the country-specific OEM defined key set. Though 4 of them
;  can be found in each country as corresponding to the same symbol (+,-.),
;  they must be used very carefully in any application.
;
; Table defining the equivalence between Virtual keys and the keycaps
; for the supported countries:

;Virtual key     U.S.   U.K.  Italy    France   Germany  Spain

;VK_OEM_PLUS     = +    = +    +  *     =  +     +  *     =  +
;VK_OEM_COMMA    , <    , <    ,  ;     ,  ?     ,  ;     ,  ?
;VK_OEM_MINUS    - _    - _    -  _     -  _     -  _     -  _
;VK_OEM_PERIOD   . >    . >    .  :     ;  .     .  :     .  !
;VK_OEM_1        ; :    ; :    <  >     $  *     <  >     ;  :
;VK_OEM_2        / ?    / ?    '  ?     :  /     ss ?     <  >
;VK_OEM_3       bq ~    ' @   ugr sec   mu lb    ac gr   gr  cr
;VK_OEM_4        [ {    [ {   agr #     )  deg    Aum    ac  um
;VK_OEM_5        \ |    \ |   ogr @    ugr %      Oum     Ntl
;VK_OEM_6        ] }    ] }   egr eac   <  >      Uum     Ccd
;VK_OEM_7        ' "    # ~   igr ^     cr um    #  ^     '  "

;Virtual key   Sweden   Norway  Denmark

;VK_OEM_PLUS    +  ?     +  ?    +  ?
;VK_OEM_COMMA   ,  ;     ,  ;    ,  ;
;VK_OEM_MINUS   -  _     -  _    -  _
;VK_OEM_PERIOD  .  :     .  :    .  :
;VK_OEM_1       '  *     '  *    '  *
;VK_OEM_2       <  >     <  >    <  >
;VK_OEM_3       ac gr    ac gr   ac gr
;VK_OEM_4        Aum      AE      AE
;VK_OEM_5        Oum      Osl     Osl
;VK_OEM_6        Arn      Arn     Arn
;VK_OEM_7       um cr    um cr   um cr


; The RT keyboards have different equivalences:
;

;Virtual key      U.K.  Italy    France   Germany  Spain

;VK_OEM_PLUS      =  +    +  *     =  +     +  *     +  *
;VK_OEM_COMMA     ,  <    ,  ;     ,  ?     ,  ;     ,  ;
;VK_OEM_MINUS     -  _    -  _     ! sec    -  _     -  _
;VK_OEM_PERIOD    .  >    .  :     ;  .     .  :     .  : 
;VK_OEM_1         ;  :    <  >     $  lb    <  >     ip iq
;VK_OEM_2         /  ?    '  ?     :  /     ss ?     <  >
;VK_OEM_3         '  @   ugr sec   *  mu    ac gr   gr  cr
;VK_OEM_4         [  {   agr deg   )  deg    Aum    ac  um
;VK_OEM_5         \  |   ogr Ccd  ugr %      Oum     Ntl
;VK_OEM_6         ]  }   egr eac   <  >      Uum     Ccd
;VK_OEM_7         #  ~   igr ^     cr um    #  '     '  "
;VK_OEM_8        gr lnot  \  |    sc2 sc2   ^ deg   sco sca

; Notes:
; ------  1) "'" (close quote) is not the acute accent.
;         2) The open quote (also named back quote) and "^" are
;             respectively the grave and circumflex accents.
;         3) The U.K. layout is the IBM PC keyboard layout. Others
;            may differ.
;         4) The Finnish layout is the same than the Swedish one.
;         5) If you need more Virtual OEM keys, just take the next
;            number available (presently 9) but the value must be
;            new and into the interval (80h - 0FFh)
;         6) Here are the definition of the abreviations:

; ss :  German sharp S            mu :  Greek mu
; lb :  British pound             ac :  Acute accent
; gr :  Grave accent              cr :  Circumflex accent
; ugr:  u grave                   sec:  Section mark
; deg:  degree symbol             Aum:  A umlaut (lower and upper case)
; um :  umlaut accent             agr:  a grave
; Oum:  O umlaut (l.c. and u.c.)  Ntl:  N tilde (l.c. and u.c.)
; ogr:  o grave                   Uum:  U umlaut (l.c. and u.c.)
; Ccd:  C cedilla (l.c. and u.c.) egr:  e grave
; eac:  e acute                   igr:  i grave
; Arn:  A ring (l.c. and u.c.)    AE :  ae (l.c. and u.c.)
; Osl:  O slashed (l.c and u.c.)  bq :  back quote
; ip :  inverted point            iq :  inverted question mark
; sco:  o underlined superscript  sca:  a underlined superscript
; sc2:  2 superscript


VK_OEM_NUMBER   = 090H          ; NumLock
VK_OEM_SCROLL   = 091H          ; ScrollLock
VK_OEM_SHIFT    = 092H          ; ShiftLock
VK_OEM_1        = 0BAH          ; ';:' for US
VK_OEM_PLUS     = 0BBH          ; '+' any country
VK_OEM_COMMA    = 0BCH          ; ',' any country
VK_OEM_MINUS    = 0BDH          ; '-' any country (except French Rt keyboard)
VK_OEM_PERIOD   = 0BEH          ; '.' any country
VK_OEM_2        = 0BFH          ; '/?' for US
VK_OEM_3        = 0C0H          ; '`~' for US
VK_OEM_4        = 0DBH          ; '[{' for US
VK_OEM_5        = 0DCH          ; '\|' for US
VK_OEM_6        = 0DDH          ; ']}' for US
VK_OEM_7        = 0DEH          ; ''"' for US
VK_OEM_8        = 0DFH          ; none for US
VK_OEM_9        = 0E0H          ; next available
