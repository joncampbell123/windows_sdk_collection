/* Header file for procedures and data types defined by DISK.ASM */

/* Internal structure of a BPB for MSDOS.  This is valid for
   versions 2.* and above but is not the extended BPB for 5.0.
*/

struct bpbType {
    WORD cbSec; 	       /* number of bytes per sector */
    BYTE secPerClus;	       /* number of sectors per cluster */
    WORD cSecRes;	       /* number of reserved sectors */
    BYTE cFAT;		       /* number of FATS */
    WORD cDir;		       /* number of directory entries in root */
    WORD cSec;		       /* total number of sectors in image */
    BYTE bMedia;	       /* media descriptor */
    WORD secPerFAT;	       /* number of sectors per FAT */
    WORD secPerTrack;	       /* number of sectors per track */
    WORD cHead; 	       /* number of heads */
    WORD cSecHidden;	       /* number of hidden sectors */
    };

#define SS48	    2
#define DS48	    3
#define DS96	    4

/* Structure of internal MSDOS drive parameter block.
   Note that this is NOT a complete definition and
   is ONLY valid for versions 2.* through 3.*
*/

struct dpbType {
    BYTE drive;
    BYTE unit;
    WORD sector_size;
    BYTE cluster_mask;
    BYTE cluster_shift;
    WORD first_FAT;
    BYTE FAT_count;
    WORD root_entries;
    WORD first_sector;
    WORD max_cluster;
    BYTE FAT_size;
    WORD dir_sector;
    long reserved1;
    BYTE media;
    BYTE first_access;
    BYTE reserved2[ 8 ];
    };

/* structure of MSDOS directory entries */

struct dirType {
    char name[11];
    BYTE attr;
    char pad[10];
    WORD time;
    WORD date;
    WORD first;
    long size;
    };

#define A_RO	0x01
#define A_H	0x02
#define A_S	0x04
#define A_VID	0x08
#define A_DIR	0x10
#define A_A	0x20
#define A_DEV	0x40

/* structure of beginning of MSDOS boot sector
*/
struct bootType {
    char	   jump[3];	    /* 3 byte jump */
    char	   label[8];	    /* oem name and version */
    struct bpbType BPB; 	    /* bpb */
    unsigned char  bootdrive;	    /* int 13 indicator for boot device */
    };


typedef struct
	{
	char		    dontcare1[4];
	int		    NumCyls;
	char		    dontcare3;
	struct	bpbType     BPB;
	char		    dontcare2[162];
	}DevPB;

/* Global procedures exported by DISK.ASM */

void	PASCAL SetDASD(int, int);
int	PASCAL GetVersionNumber(void);
void	PASCAL GetDeviceParameters(int, DevPB *);
void	PASCAL SetDeviceParameters(int, DevPB *);
int	PASCAL GetCntFloppyDrives( void );
BYTE	PASCAL GetCurDisk( void );
int	PASCAL SetCurDisk( BYTE );
BYTE	PASCAL NormalizeDrive( BYTE );
int	PASCAL GetDriveType( BYTE );
WORD	PASCAL GetClusterSize( BYTE );
void	PASCAL DiskReset( void );
int	PASCAL GetDPB( BYTE, struct dpbType * );
void	PASCAL SetDPB( BYTE, struct bpbType *, struct dpbType * );
int	PASCAL int25( BYTE, LPSTR, WORD, WORD );
int	PASCAL int26( BYTE, LPSTR, WORD, WORD );
int	PASCAL FormatTrackHead( BYTE, BYTE, BYTE, BYTE, LPSTR );
int	PASCAL ReadVolumeID( BYTE, char * );
int	PASCAL WriteVolumeID( struct dpbType *, BYTE, LPSTR, LPSTR );
int	PASCAL WriteBootSec( WORD, WORD, struct bpbType *, LPSTR );
void	PASCAL OpenFAT( BYTE );
WORD	PASCAL FlushFAT( struct dpbType *, LPSTR );
WORD	PASCAL UnpackFAT( struct dpbType *, LPSTR, WORD );
WORD	PASCAL PackFAT( struct dpbType *, LPSTR, WORD, WORD );
int	PASCAL IsFormatted( BYTE, LPSTR );
int	PASCAL EraseDisk( BYTE, LPSTR );

/* (c-ralphp 6/14/88)  begin modification */
int	PASCAL IsHPMachine( void );
/* (c-ralphp) end modification */

#define FAT_READ    1
#define FAT_WRITE   2

/* Global variables */
struct dpbType DPB;	    /* since ds != ss */

struct dbtType
	{
	char	    SRHU;
	char	    HLDMA;
	char	    wait;
	char	    bytespersec;
	char	    lastsector;
	char	    gaplengthrw;
	char	    datalength;
	char	    gaplengthf;
	char	    datavalue;
	char	    HeadSettle;
	char	    MotorStart;
	};
typedef struct dbtType far *LPDBT;
LPDBT	PASCAL GetDBT(void);
