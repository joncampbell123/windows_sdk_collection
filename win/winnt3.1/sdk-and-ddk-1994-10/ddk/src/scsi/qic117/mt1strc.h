/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

   mt1strc.h

Abstract:

Revision History:


--*/

#pragma pack(1)

//
//   Tape Drive Status
//    returned from Report Drive Status command.
//

struct DriveStatus {
    UBYTE Ready:1;              // drive ready
    UBYTE Error:1;              // error detected
    UBYTE CartPresent:1;       // cartridge present
    UBYTE WriteProtect:1;  // cartridge write protected
    UBYTE NewCart:1;           // new cartridge
    UBYTE Referenced:1;     // cartridge referenced
    UBYTE BOT:1;                    // at physical beginning of tape
    UBYTE EOT:1;                    // at physical end of tape
};


//
//   CMS Status
//    returned from Report CMS Status command.
//

struct CmsStatus {
    UBYTE NoBurst:1;           // no burst seek if 1
    UBYTE CmsMode:1;           // cms mode if 1
    UBYTE Threshold:1;         // Threshold load if 1
    UBYTE DriveType:1;         // qic40=1, qic80=0
    UBYTE BurstOnly:1;         // Burst only gain set if 1
    UBYTE Pegasus:1;           // If Pegasus tape 1, else 0
    UBYTE Eagle:1;             // Eagle
    UBYTE Reserved2:1;         // not defined
};

//
//   Tape Drive Configuration
//    returned from Report Drive Configuration command.
//

struct DriveConfiguration {
    UBYTE Reserved:3;   // reserved (response = 0)
    UBYTE XferRate:2;  // Transfer Rate    =   00  250 Kbps
                            //                          01  Reserved
                            //                          10  500 Kbps
                            //                          11  Reserved
    UBYTE Reserved1:1;
    UBYTE XL_Tape:1;    // extra length tape detected
    UBYTE QIC80:1;      // QIC-80 mode
};


//
//   Tape Drive Error Status
//    returned from Report Error Code command.
//

struct DriveError {
    UWORD Error:8;      // error code
    UWORD Command:8;    // associated command
};

struct TapeFormatLgth {
    UWORD Format:4;          /* Format of the tape */
    UWORD Length:4;          /* Length of the tape */
};


//
//   Tape Drive Parameters
//

struct DriveParameters {
    UCHAR Flavor;                   // CMS or ALIEN
    UCHAR Version;                  // Firmware Version
    UCHAR DriveType;                // QIC-40 or QIC-80
    UCHAR SeekMode;                 // seek mode supported by the drive
    SHORT DriveSelect;              // current drive identifier
    CHAR Mode;                      // drive mode (Primary, Format, Verify)
    USHORT ArchiveNativeMode;       // Archive Native Mode Data
    struct DriveStatus Status;      // last read tape drive status byte
};

typedef struct DriveParameters DRIVE_PARAMETERS;

//
//   Tape Parameters
//

struct TapeParameters {
    CHAR TapeType;          //  QIC40/QIC80 SHORT/LONG
    SHORT FsectSeg;         //  floppy sectors per segment
    SHORT SegFtrack;        //  segments per floppy track
    UWORD FsectFtrack;      //  floppy sectors per floppy track
    UWORD RwGapLength;      //  write gap length
    USHORT NumTtrack;       //  number of tape tracks
    SHORT FtrackFside;      //  floppy tracks per floppy side
    USHORT SegTtrack;       //  segments per tape track
    ULONG FsectFside;       //  floppy sectors per floppy side
    ULONG LogSectors;       //  number of logical sectors on a tape
    ULONG FsectTtrack;      //  floppy sectors per tape track
    USHORT TapeFormatCode;  //  the tape format code
    USHORT FormattableSegs; //  the number of formattable segments
    QIC_TIME TimeOut[3];    //  time_out for the QIC-117 commands
                            //  time_out[0] = logical_slow, time_out[1] = logical_fast,
                            //  time[2] = physical.
};

typedef struct TapeParameters TAPE_PARAMETERS;

//
//   Drive Select Parameters
//    These are select/deselect bytes used to select and deselect the tape
//    drive.
//

struct DriveSelect {
    CHAR    Selected;           // drive selected flag.
    UBYTE   DeselectByte;      // bits to make drive select line inactive
    UBYTE   SelectByte;        // bits to make drive select line   active
};

typedef struct DriveSelect DRIVE_SELECT;

//
//   Transfer Rate Parameters
//

struct TransferRate {
    UBYTE XferRate;    // current transfer rate (SLOW or FAST)
    UBYTE MaxRate;     // maximum transfer rate (SLOW or FAST)
    UBYTE TapeSlow;    // Program tape drive slow (250 or 500 Kbps)
    UBYTE TapeFast;    // Program tape drive fast (500 or 1000 Kbps)
    UBYTE FDC_Slow;     // Program FDC slow (250 or 500 Kbps)
    UBYTE FDC_Fast;     // Program FDC fast (500 or 1000 Kbps)
    UBYTE SRT_Slow;     // FDC step rate for slow xfer rate
    UBYTE SRT_Fast;     // FDC step rate for fast xfer rate
};

typedef struct TransferRate TRANSFER_RATE;

//
//   Tape Position Parameters
//

struct TapePosition {
    CHAR    LogFwd;                // indicates that the driver thinks that
                                    //  the tape is going logical forward
    SHORT   C_Segment;          // current tape block, (floppy track)
    SHORT   C_Track;            // current physical track
    SHORT   D_Segment;          // desired tape segment, (floppy track)
    SHORT   D_Track;                // desired physical tape track
};

typedef struct TapePosition TAPE_POSITION;


//
//   Commands to the Floppy Controller.  FDC commands and the corresponding
//    driver structures are listed below.
//
//         FDC Command                         Command Struct      Response Struct
//         -----------                         --------------      ---------------
//         Read Data                           rdv_command         stat
//         Read Deleted Data                   N/A                 N/A
//         Write Data                          rdv_command         stat
//         Write Deleted Data              rdv_command         stat
//         Read a Track                        N/A                 N/A
//         Verify (82077)                      N/A                 N/A
//         Version (82077)                 version_cmd         N/A
//         Read ID                             read_id_cmd         stat
//         Format a Track                      format_cmd          stat
//         Scan Equal (765)                    N/A                 N/A
//         Scan Low or Equal (765)         N/A                 N/A
//         Scan High or Equal (765)        N/A                 N/A
//         Recalibrate                         N/A                 N/A
//         Sense Interrupt Status          sns_int_cmd         fdc_result
//         Specify                             specify_cmd         N/A
//         Sense Drive Status              sns_stat_cmd        stat
//         Seek                                    seek_cmd                N/A
//         Configure (82077)                   config_cmd          N/A
//         Relative Seek (82077)           N/A                 N/A
//         Dump Registers (82077)          N/A                 N/A
//         Perpendicular Mode (82077)      N/A                 N/A
//         Invalid                             invalid_cmd         N/A
//


struct  rdv_command {
    UBYTE   command;                // command UBYTE
    UBYTE   drive;                  // drive specifier
    UBYTE   C;                      // cylinder number
    UBYTE   H;                      // head address
    UBYTE   R;                      // record (sector number)
    UBYTE   N;                      // number of UBYTEs per sector
    UBYTE   EOT;                    // end of track
    UBYTE   GPL;                    // gap length
    UBYTE   DTL;                    // data length
};

typedef struct rdv_command RDV_COMMAND;

struct  read_id_cmd {
    UBYTE command;              // command byte
    UBYTE drive;                    // drive specifier
};

struct  format_cmd {
    UBYTE command;              // command byte
    UBYTE drive;                    // drive specifier
    UBYTE N;                        // number of bytes per sector
    UBYTE SC;                       // sectors per track (segment)
    UBYTE GPL;                  // gap length
    UBYTE D;                        // format filler byte
};

typedef struct  format_cmd FORMAT_CMD;

struct  sns_int_cmd {
    UBYTE command;              // command byte
};

struct  version_cmd {
    UBYTE command;              // command byte
};

struct  national_cmd    {
    UBYTE command;              // command byte
};

struct  specify_cmd {
    UBYTE command;              // command byte
    UBYTE SRT_HUT;              // step rate time (bits 7-4)
                                // head unload time bits (3-0)
    UBYTE HLT_ND;               // head load time (bits 7-1)
                                // non-DMA mode flag (bit 0)
};

struct  sns_stat_cmd    {
    UBYTE command;              // command byte
    UBYTE drive;                // drive specifier
};

struct  recalibrate_cmd    {
    UBYTE command;              // command byte
    UBYTE drive;                // drive specifier
};

struct  seek_cmd {
    UBYTE cmd;                  // command byte
    UBYTE drive;                // drive specifier
    UBYTE NCN;                  // new cylinder number
};

struct config_cmd {
    UBYTE cmd;                  // command byte
    UBYTE czero;                // null byte
    UWORD FIFOTHR:4;            // FIFO threshold
    UWORD POLL:1;               // Disable polling of drives
    UWORD EFIFO:1;              // Enable FIFO
    UWORD EIS:1;                // Enable implied seek
    UWORD reserved:1;           // reserved
    UWORD PRETRK:8;             // Pre-compensation start track number
};

struct  invalid_cmd {
    UBYTE command;              // command byte
};

struct _FDC_STATUS {
    UBYTE ST0;                  // status register 0
    UBYTE ST1;                  // status register 1
    UBYTE ST2;                  // status register 2
    UBYTE C;                    // cylinder number
    UBYTE H;                    // head address
    UBYTE R;                    // record (sector number)
    UBYTE N;                    // number of bytes per sector
};

typedef struct _FDC_STATUS FDC_STATUS;


struct fdc_result {
    UBYTE ST0;                  // status register 0
    UBYTE PCN;                  // present cylinder number
};

//
//   FDC Sector Header Data used for formatting
//

union format_header {
    struct {
        UBYTE C;                    // cylinder number
        UBYTE H;                    // head address
        UBYTE R;                    // record (sector number)
        UBYTE N;                    // bytes per sector
    } hdr_struct;
    ULONG hdr_all;
};

// This command is only valid on the 82078 Enhanced controller

struct part_id_cmd {
        UBYTE command;
};

union drvspec {
    struct {
        UBYTE drive_density:2;
        UBYTE data_rate_table:2;
        UBYTE precomp:1;
        UBYTE floppy_select:2;
        UBYTE reserved:1;
    } spec;
    struct {
        UBYTE reserved:6;
        UBYTE no_report:1;
        UBYTE cmd_done:1;
    } spec_end;
    UBYTE spec_all;
};

typedef union drvspec DrvSpec;

// This command is only valid on the 82078 Enhanced controller

struct drive_specification {
        UBYTE    command;
        DrvSpec  drive[5];
};

struct drive_specification_status {
        DrvSpec    drive[4];
};

typedef struct drive_specification_status DrvSpecStatus;

struct PerpMode {
        UBYTE command;
        UBYTE wgate:1;
        UBYTE gap:1;
        UBYTE drive_select:4;
        UBYTE reserved:1;
        UBYTE over_write:1;
};

// This command is only valid on the 82078 64 pin Enhanced controller

struct SaveCmd {
        UBYTE command;
};

struct SaveResult {
        UBYTE reserved1:7;
        UBYTE clk48:1;
        UBYTE reserved2;
        UBYTE reserved3;
        UBYTE reserved4;
        UBYTE reserved5;
        UBYTE reserved6;
        UBYTE reserved7;
        UBYTE reserved8;
        UBYTE reserved9;
        UBYTE reserved10;
        UBYTE reserved11;
        UBYTE reserved12;
        UBYTE reserved13;
        UBYTE reserved14;
        UBYTE reserved15;
        UBYTE reserved16;
};


struct FormatParameters {
    SHORT  Segments;    // tape track segment counter
    UCHAR  Cylinder;    // floppy cylinder number
    UCHAR  Head;        // floppy head number
    UCHAR  Sector;      // floppy sector number
    PMDL   MdlAddress;  // Memory desciptor list for format data
    UBYTE  NCN;         // new cylinder number
    ULONG  *HdrPtr;     // pointer to sector id data for format
    STATUS retval;      // Format status
};

typedef struct FormatParameters FMT_PARAMETERS;

#pragma pack()



//
//    Structure for the miscellaneous drive information
//

struct MiscDriveInfo {
        CHAR DriveType;                        // Drive_type: QIC40 or QIC80
        UCHAR ROM_Version;      // ROM Version of the drive's firmware
        CHAR InfoExists;               // Indicates the existance of the drive train info
        UCHAR SerialNumber[4]; // Four byte array used for the drive serial number
        UCHAR ManDate[2];          // Two byte field used for the date of manufacture
        CHAR Oem[20];                           // Twenty byte field used for the OEM name
};

typedef struct MiscDriveInfo MISC_DRIVE_INFO;

