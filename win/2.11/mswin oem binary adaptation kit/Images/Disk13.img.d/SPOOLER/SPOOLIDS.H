/* comm driver stuff */

#define IDS_LENGTH          40

#define COMM_ERR_BIT        0x8000
#define BAUDRATE            0
#define PARITY              1
#define BYTESIZE            2
#define STOPBITS            3
#define REPEAT              4

#define SP_CANCEL   (-1)

#define IDS_NAME            0
#define IDS_NOPRINTER       1
#define IDS_MULTIPRINTER    2
#define IDS_TERMINATE       3
#define IDS_TERMALL         4
#define IDS_PAUSED          5
#define IDS_ACTIVE          6
#define IDS_ABOUT           7
#define IDS_TEMPFILE        8
#define IDS_MAXJOB          9
#define IDS_COMMERROR      11
#define IDS_OOP            12
#define IDS_DNS            13
#define IDS_MULTSPOOLER    14
#define IDS_TOOMANYPORTS   15
#define IDS_OUTOFDISK      16
#define IDS_OUTOFMEMORY    17
#define IDS_NOSPOOLER      18
#define IDS_MERGE          19

#define COMM_OPEN          20
#define COMM_SETTING       21
#define COMM_HARDWARE      22
#define COMM_INVALIDPORT   23
#define SP_ABOUT          999

/****************** get these out of the common file spool.h from \inc
    #define TXTIMEOUT           45000                milliseconds
    #define DNSTIMEOUT          15000               milliseconds
*********************************************/

#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_APPEND        0x0008  /* writes done at eof */

#define O_CREAT         0x0100  /* create and open file */
#define O_TRUNC         0x0200  /* open with truncation */
#define O_EXCL          0x0400  /* exclusive open */
