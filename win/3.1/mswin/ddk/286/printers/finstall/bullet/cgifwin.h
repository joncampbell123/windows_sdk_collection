/* cgifwin.h  -- a windows version of cgif.h */
/* Copyright (C) 1989, 1990 by Agfa Compugraphic, Inc. All rights reserved. */
/*   08-13-90   jfd   Re-arranged the CGIFpcleo declarations so that        */
/*                    if MULTICALLER, version of CGIFpcleo with three       */
/*                    arguments is declared, rather than two                */

typedef unsigned long  ULONG;        /* windows.h does not define ULONG */
typedef unsigned char  UBYTE;        /*                        or UBYTE */
typedef unsigned short UWORD;        /*                        or UWORD */

typedef ULONG        MEM_HANDLE;
typedef MEM_HANDLE   HIFFONT;
typedef MEM_HANDLE   HIFBITMAP;
#define NIL_MH       ((MEM_HANDLE)0L)
/*----------------------------------*/
/*         Memory Pools             */
/*----------------------------------*/

#define CACHE_POOL   0
#define BUFFER_POOL  1

/*----------------------------------*/
/*      CGIFkern() query codes        */
/*----------------------------------*/
#define  TEXT_KERN    0
#define  DESIGN_KERN  1

/*-----------------------------------*/
/* CGIFsegments() data segment codes */
/*-----------------------------------*/
#define FACE_GLOBAL_KEY   0    /* Entire face global segment */
#define GIF_KEY         100    /* Global Intellifont segment */
#define TRACK_KERN_KEY  101    /* track kerning              */
#define TEXT_KERN_KEY   102    /* text kerning               */
#define DESIGN_KERN_KEY 103    /* designer kerning           */
#define WIDTH_KEY       104    /* character width segment    */
#define ATTRIBUTE_KEY   105    /* Attribute header           */
#define RASTER_KEY      106    /* Raster parameter           */
#define TF_HEADER_KEY   107    /* Typeface Header segment    */
#define COMPOUND_KEY    108    /* Compound character         */
#define DISPLAY_KEY     109    /* Display header             */
#define FONT_ALIAS_KEY  110    /* Font Alias segment         */
#define COPYRIGHT_KEY   111    /* Copyright notice           */

/*----------------------------------*/
/*        Error Return Codes        */
/*----------------------------------*/
#define ERR_bad_bitmap_width   1
#define ERR_no_font_index      3   /* can't open font index file */
#define ERR_rd_font_index      4   /* can't allocate or read font index */
#define ERR_missing_plugin     5   /* missing one or more plugin lib    */
#define ERR_no_symbol_set      6   /* can't open symbol set file        */
#define ERR_rd_symbol_set      7   /* can't allocate or read symbol set */
#define  ERR_bad_pool          21
#define  ERR_CACinit           32
#define  ERR_no_font           53
#define  ERR_IXnew_ss          54
#define  ERR_bad_chID          61
#define  ERR_bm_buff           63
#define  ERR_no_cgnum          64  /* ss id does not map to a cgnum */
#define  ERR_no_fi_entry       65  /* no font index */
#define  ERR_open_lib          66  /* Error opening library file */
#define  ERR_mem_face_hdr      67  /* Mallac failed for face header */
#define  ERR_face_abort        68
#define  ERR_find_cgnum        69  /* Can't find cgnum in char. index */
#define  ERR_rd_char_data      70  /* Can't read character data       */
#define  ERR_buffer_too_small  71
#define  ERR_bad_kern_code     81
/* pcleo.c */
#define ERRnot_ready_lpt      100  /* printer not ready during write   */
#define ERRwrite              101  /* fwrite() error                   */
#define ERRopen_printer       102  /* LPT not ready during open_file() */
#define ERRopen_file          103  /* fopen() error   "      "    "    */
#define ERRfstat              104  /* fstat() error   "      "    "    */
#define ERRrastparam_missing  105  /* missing raster parameter segment */
#define ERRattribseg_missing  106  /*    "    font attribute      "    */
#define ERRdispheader_missing 107  /*    "    display header      "    */
#define ERRfat_missing        108  /*    "    font alias table         */
#define ERRbadchar            109  /* Inconsistent skeletal char data  */
#define ERRvertesc            110  /* Non-zero vertical escapement     */
/*---------*/
#define  ERR_bm_gt_oron       599  /* Char bitmap too big for on array */
#define  ERR_bm_too_big       600  /* Char bitmap too big */
#define  ERR_mem_char_buf     602  /* Can't malloc character buffer */
#define  ERR_if_init_glob     605  /* if_init_glob() failed */
#define  ERR_comp_pix         606  /* com_pix() failed */
#define  ERR_fixed_space      607  /* character is a fixed space */
#define  ERR_skeletal_init    610
#define  ERR_y_skel_proc      611
#define  ERR_x_skel_proc      612
#define  ERR_bold_buf         613
#define  ERR_no_contour       701
#define  ERR_not_hq3          702
#define  ERR_ov_char_buf      703
#define  ERR_out_of_handles   800
#define  ERR_bad_handle       801
#define  ERR_lost_mem         802  /* CGIFdefund() didnot free whole block */
#define  ERR_IXopen_file      803
#define  ERR_no_buck_mem      804  /* BUCKnew couldn't get memory */
#define  ERR_cc_complex       805  /* Compound character hs too many parts */
#define  ERR_no_cc_part       806  /* Can't find compound character part */
#define  ERR_missing_block    807  /* CGIFdefund() can't find the block */
#define  ERR_max_callers      900  /* maximum number of callers         */
#define  ERR_bad_ID           901  /* Caller ID is out of range         */
#define  ERR_not_valid_ID     902  /* ID passed is not active           */
#define  ERR_ccBuff_alloc     903  /* cannot allocate comp-char buffer  */
#define  ERRtoo_many_dup_skel 904  /* too many duplicate skel points    */

/*----------------------------------*/
/* Intellifont Configuration Block  */
/*----------------------------------*/
#define PATHNAMELEN  63

typedef struct
{
    UWORD  max_char_size;   /* max cached character bitmap size  */
    UWORD  bit_map_width;   /* width of char bitmap in bytes     */
    UBYTE far *cc_buf_ptr;  /* compound character buffer pointer */
    UWORD  cc_buf_size;     /*    "        "        "       "    */
    UWORD  num_files;       /* max number of open library files  */
    BYTE   bulletPath[PATHNAMELEN];  /* location of Bullet files.         */
    BYTE   typePath[PATHNAMELEN];    /* location of typeface files.    */
} IFCONFIG;

typedef long  SLONG;   /* windows.h does not define a signed long */
typedef int   SWORD;   /*                        or a signed word */
typedef LONG  FIXED;   /* 32 bit fixed point number               */
                       /* with 16 bit fractional part             */
typedef struct
{
    FIXED x;
    FIXED y;
} POINTFX;


/*----------------------------------*/
/*       Font Context               */
/*----------------------------------*/
/*  56 bytes */
typedef struct
{
    SLONG   font_id;      /* font number                                */
    SWORD   point_size;   /* point size of this character in 1/8th pts  */
    SWORD   set_size;     /* set   size of this character in 1/8th pts  */
    POINTFX shear;        /* cosine & sine of vertical stroke angle     */
    POINTFX rotate;       /* cosine & sine of baseline rotation         */
    ULONG   xres;         /* output pixels per meter                    */
    ULONG   yres;
    FIXED   xspot;
    FIXED   yspot;
    SWORD   xbold;
    SWORD   ybold;
    UWORD   ssnum;
    UWORD   format;       /* output format                              */
                          /* bit 15-14    0  Auto-quality               */
                          /*              1  quality level 1            */
                          /*              2  quality level 2            */
                          /*              3  quality level 3            */

                   
}
FONTCONTEXT;



/*----------------------------------*/
/*    Character Bitmap              */
/*----------------------------------*/

/*----------------------*/
/*  Doubly Linked Lists */
/*----------------------*/
typedef struct
{ 
    MEM_HANDLE  f;  /* forward pointer */
    MEM_HANDLE  b;  /* backward pointer */
} DLL;

/* 34 Bytes plus bitmap */
typedef struct
{
    DLL     link;          /* doubly linked list pointers                  */
    UWORD   notmru;        /* set to 0 if recently made most recently used */
    HIFFONT hiffont;       /* Font handle of the owner of this IFBITMAP    */
    SWORD   index;         /* index to IFBITMAP in above FONT's table      */

    SWORD   width;         /* bit map width (bytes)                 */
    SWORD   depth;         /*  "   "  depth (pixels)                */
    SWORD   left_indent;
    SWORD   top_indent;
    SWORD   black_width;
    SWORD   black_depth;
    SWORD   xorigin;       /*  (1/16th pixel)  */
    SWORD   yorigin;       /*  (1/16th pixel)  */
    SWORD   escapement;    /* character body width (design units)  */

    UBYTE   bm[4];         /* character bit map                     */
} IFBITMAP;


/*----------------------------------*/
/*    Kerning Pair                  */
/*----------------------------------*/

typedef struct
{
    UWORD chId0, chId1;
    SWORD adj;
}  KERN_PAIR;




/*----------------------------------*/
/*    Windows specific typedefs     */
/*----------------------------------*/


typedef KERN_PAIR * PKERN_PAIR;
typedef KERN_PAIR far * FPKERN_PAIR;

typedef IFBITMAP * PIFBITMAP;
typedef IFBITMAP ** PPIFBITMAP;
typedef IFBITMAP far * FPIFBITMAP;
typedef IFBITMAP far * far * FPFPIFBITMAP;

typedef FONTCONTEXT * PFONTCONTEXT;
typedef FONTCONTEXT far  * FPFONTCONTEXT;

typedef IFCONFIG * PIFCONFIG;
typedef IFCONFIG far * FPIFCONFIG;

typedef char  FAR *FPBYTE;
typedef short FAR *FPWORD;
typedef unsigned char  FAR *FPUBYTE;
typedef unsigned short FAR *FPUWORD;

#if MULTICALLER
WORD  FAR PASCAL  CGIFinit(LPWORD, LPDWORD, LPDWORD);
WORD  FAR PASCAL  CGIFconfig(WORD, FPIFCONFIG);
WORD  FAR PASCAL  CGIFfund(WORD, WORD, FPBYTE, LONG, FPWORD);
WORD  FAR PASCAL  CGIFenter(WORD);
VOID  FAR PASCAL  CGIFexit(WORD);
WORD  FAR PASCAL  CGIFfont(WORD, FPFONTCONTEXT);
WORD  FAR PASCAL  CGIFwidth(WORD, WORD, WORD, WORD, FPWORD);
WORD  FAR PASCAL  CGIFkern(WORD, WORD, WORD, FPKERN_PAIR);
WORD  FAR PASCAL  CGIFmove_block(WORD, FPBYTE);

#if CACHE
WORD  FAR PASCAL  CGIFchar(WORD, WORD, FPFPIFBITMAP, WORD);
#else
WORD  FAR PASCAL  CGIFchar_size(WORD, WORD, FPWORD, WORD);
WORD  FAR PASCAL  MAKbitMap(FPIFBITMAP, FPBYTE, FPBYTE);
#endif

#if DEFUND
WORD  FAR PASCAL  CGIFdefund(WORD, UWORD);
#endif

#if PCLEO
WORD  FAR PASCAL  CGIFpcleo(UWORD, LPSTR, LPLONG);
#endif

#if SEGACCESS
WORD  FAR PASCAL  CGIFsegments(LONG, WORD, FPWORD, LPBYTE);
#endif

#else

WORD  FAR PASCAL  CGIFinit(VOID);
WORD  FAR PASCAL  CGIFconfig(FPIFCONFIG);
WORD  FAR PASCAL  CGIFfund(WORD, FPBYTE, LONG, FPWORD );
WORD  FAR PASCAL  CGIFenter(VOID);
VOID  FAR PASCAL  CGIFexit(VOID);
WORD  FAR PASCAL  CGIFfont(FPFONTCONTEXT);
WORD  FAR PASCAL  CGIFwidth(WORD, WORD, WORD, FPWORD);
WORD  FAR PASCAL  CGIFkern(WORD, WORD, FPKERN_PAIR );
WORD  FAR PASCAL  CGIFmove_block(UWORD, FPBYTE);

#if CACHE
WORD  FAR PASCAL  CGIFchar(WORD, FPFPIFBITMAP, WORD);
#else
WORD  FAR PASCAL  CGIFchar_size(WORD,   FPWORD , WORD);
WORD  FAR PASCAL  MAKbitMap(FPIFBITMAP,  FPBYTE ,  FPBYTE);
#endif

#if DEFUND
WORD  FAR PASCAL  CGIFdefund(UWORD);
#endif

#if PCLEO
WORD  FAR PASCAL  CGIFpcleo(LPSTR, LPLONG);
#endif

#if SEGACCESS
WORD   FAR PASCAL CGIFsegments(LONG, WORD, FPWORD, LPBYTE);
#endif

#endif

