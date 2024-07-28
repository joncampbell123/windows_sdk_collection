;/*     GDITYPE.INC


;       basic types used by gdi


MAXINT  equ     7fffh
MININT  equ     8000h



        if      0
*/

#define Byte    BYTE
#define Word    WORD

#define MAXINT  0x7fff
#define MININT  0x8000

/*
        endif



PTTYPE          struc                                                       ;*/ typedef     struct {        /*
  xcoord        dw      0               ;x coordinate of point  : INTEGER;  ;*/ short int xcoord;           /*
  ycoord        dw      0               ;y coordinate of point  : INTEGER;  ;*/ short int ycoord;           /*
PTTYPE          ends                                                        ;*/ } PTTYPE;                   /*
                                                                            ;*/ typedef PTTYPE *PPOINT;     /*
                                                                            ;*/ typedef PTTYPE FAR *LPPOINT;/*
        if      0
*/
#define POINT   PTTYPE
/*
        endif



BOXTYPE         struc                                                       ;*/ typedef     struct {        /*
  min           db      SIZE pttype dup (?)     ;x,y starting coord : PTTYPE;*/ PTTYPE min;                 /*
  ext           db      SIZE pttype dup (?)     ;x,y extents        : PTTYPE;*/ PTTYPE ext;                 /*
BOXTYPE         ends                                                        ;*/ } BOXTYPE;                  /*


RECT            struc                                                       ;*/ typedef struct {            /*
  left          dw      0                                                   ;*/ short int left,             /*
  top           dw      0                                                   ;*/           top,              /*
  right         dw      0                                                   ;*/           right,            /*
  bottom        dw      0                                                   ;*/           bottom;           /*
RECT            ends                                                        ;*/ } RECT;                     /*
                                                                            ;*/ typedef RECT  *PRECT;       /*
                                                                            ;*/ typedef RECT  FAR * LPRECT; /*

ilObjHead       struc                                                       ;*/ typedef     struct {           /*
                dw      ?           ; pointer to next obj in chain          ;*/ unsigned short int nextinchain;/*
  ilObjType     dw      ?           ; defines type of object                ;*/ short int ilObjType;           /*
  ilObjCount    dd      ?           ; the count of the object               ;*/ long  int ilObjCount;          /*
ilObjHead       ends                                                        ;*/ } ILOBJHEAD;                   /*



ipObjHead       struc                                                       ;*/ typedef     struct {           /*
                dw      ?           ; pointer to next obj in chain          ;*/ unsigned short int nextinchain;/*
ipObjHead       ends                                                        ;*/ } IPOBJHEAD;                   /*

;*/



;/*
;       Memory Manager Definitions


;sBegin  data
;
;	 externw LOCALMOVABLE
;	 externw GLOBALMOVABLE
;	 externw GLOBALMOVABLENONSHARED
;
;sEnd	 data



        if      0
*/

        extern  short LOCALMOVABLE;
        extern  short GLOBALMOVABLE;
        extern  short GLOBALMOVABLENONSHARED

/*
        endif

;*/
