/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    mt1defs..h

Abstract:


Revision History:          




--*/

#define FIRM_VERSION_60             60    // First jumbo B firmware version
#define FIRM_VERSION_63             63    // Cart in problems
#define FIRM_VERSION_64             64    // First Firmware version to support Skip_n_Seg through the Erase Gap
#define FIRM_VERSION_65             65    // First Firmware version to support Pegasus
#define FIRM_VERSION_80             80    // First Firmware version to support Jumbo c
#define FIRM_VERSION_87             87    // First Firmware revision to support QIC-117 C
#define FIRM_VERSION_110            110   // First Firmware version to support Eagle

// EQU's for QIC-40 firmware commands

typedef enum _FIRMWARE_CMD {
    Soft_Reset              =  1,      // soft reset of tape drive
    Rpt_Next_Bit            =  2,      // report next bit (in report subcontext)
    Pause                   =  3,      // pause tape motion
    Micro_Pause             =  4,      // pause and microstep the head
    Alt_Timeout             =  5,      // set alternate command timeout
    Report_Status           =  6,      // report drive status
    Report_Error            =  7,      // report drive error code
    Report_Confg            =  8,      // report drive configuration
    Report_ROM              =  9,      // report ROM version
    Rpt_Signature           =  9,      // report drive signature (model dependant diagnostic mode)
    Logical_Fwd             = 10,      // move tape in logical forward mo
    Physical_Rev            = 11,      // move tape in physical reverse mode
    Physical_Fwd            = 12,      // move tape in physical forward mode
    Seek_Track              = 13,      // seek head to track position
    Seek_LP                 = 14,      // seek load point
    Format_Mode             = 15,      // enter format mode
    Write_Ref               = 16,      // write reference burst
    Verify_Mode             = 17,      // enter verify mode
    Park_Head               = 17,      // park head (model dependant diagnostic mode)
    Stop_Tape               = 18,      // stop the tape
    Read_Noise_Code         = 18,      // check noise on drive (model dependent diagnostic mode)
    Microstep_up            = 21,      // microstep head up
    Disable_WP              = 21,      // disable write protect line (model dependent diagnostic mode)
    Microstep_down          = 22,      // microstep head down
    Set_Gain                = 22,      // set absolute drive gain (model dependant diagnostic mode)
    Read_Port2              = 23,      // read the drive processor port 2 (diagnostic command)
    Report_Vendor           = 24,      // report vendor number
    Skip_N_Rev              = 25,      // skip n segments reverse
    Skip_N_Fwd              = 26,      // skip n segments forward
    Select_Speed            = 27,      // select tape speed
    Diag_1_Mode             = 28,      // enter diagnostic mode 1
    Diag_2_Mode             = 29,      // enter diagnostic mode 2
    Primary_Mode            = 30,      // enter primary mode
    Report_Vendor32         = 32,      // report vendor number (for firmware versions > 33)
    Report_Tape_Stat        = 33,      // reports the tape format of the currently loaded tape
    Skip_N_Rev_Ext          = 34,      // skip n segments reverse (extended format)
    Skip_N_Fwd_Ext          = 35,      // skip n segments forward (extended format)
    Rpt_CMS_Status          = 37,      // report CMS status byte (model dependant - diagnostic mode)
    Read_Ram                = 44,      // read tape drive RAM
    New_Tape                = 45,      // load tape sequence
    Select_Drive            = 46,      // select the tape drive
    Deselect_Drive          = 47,      // deselect the tape drive
    ReportProtoVer          = 50,      // reports firmware prototype version number (model dependant - diagnostic mode)
    Dtrain_Info             = 53,      // enter Drive Train Information mode (model dependant - diagnostic mode)
    Mtn_Select_1            = 23,      // Mountain select byte 1
    Mtn_Select_2            = 20,      // Mountain select byte 2
    Mtn_Deselect            = 24,      // Mountain deselect byte
    Rpt_Archive_Native_Mode = 40       // Archive Native Mode diagnostic command
} FIRMWARE_CMD, *PFIRMWARE_CMD;

// Parameters for the Drive Training Information Commands
#define         Gdesp_Info                      5

// QIC-40 Firmware errors

typedef enum _FIRMWARE_ERROR {
    Drv_Not_Rdy     =  1,          // command received while drive not ready
    No_Cart         =  2,          // cartridge not present or removed
    Speed_Err       =  3,          // motor speed error
    Speed_Anom      =  4,          // motor speed anomaly (motor jammed, or gross speed error)
    Writ_Prot       =  5,          // cartridge write protected
    Undef_Cmd       =  6,          // undefined or reserved command code
    Illegal_Trk     =  7,          // illegal track address specified for Seek
    Cmd_in_Rpt      =  8,          // illegal command in report subcontext
    Bad_Diag_Entry  =  9,          // illegal entry attempt into a diagnostic mode
    Broken_Tape     = 10,          // broken tape detected (based on hole and mo
    Read_Gain_Err   = 11,          // Warning -- read gain setting error
    Cmd_on_Err      = 12,          // command Received while error status pending
    Cmd_on_New_Cart = 13,          // command received while new cartridge pendin
    Bad_Prm_Cmd     = 14,          // command illegal or undefined in primary mo
    Bad_Fmt_Cmd     = 15,          // command illegal or undefined in format mode
    Bad_Vfy_Cmd     = 16,          // command illegal or undefined in verify mode
    Bad_Log_Fwd     = 17,          // logical forward not a logical BOT in format mo
    Early_Fmt_EOT   = 18,          // logical EOT before all segments generated
    Cmd_on_No_Ref   = 19,          // command illegal when cartridge not reference
    Self_Diag       = 20,          // self-diagnostic failed -- NOTE: this error CA
    Uninit_PROM     = 21,          // Warning -- EEPROM not initialized, default set
    Bad_PROM        = 22,          // EEPROM contents corrupted or EEPROM hardware m
    Motion_Timeout  = 23,          // tape motion timeout (max EOT-to-BOT time ex
    Long_Data_Seg   = 24,          // data segment too long -- logical forward
    Xmit_Overrun    = 25,          // command transmit overrun (usually a firmware
    Pwr_On_Reset    = 26,          // power on reset occurred
    Swre_Reset      = 27,          // software initiated reset occurred
    Diag_Err_2      = 28,          // model-dependant diagnostic error
    Diag_Err_1      = 29,          // model-independant diagnostic error
    Non_Int_Cmd     = 30,          // command received during non-interruptible process
    Speed_not_Avail = 31,          // speed selection requested is not ava
    Inval_Media     = 34,          // tape media is invalid for the drive (QIC80 only)
    Head_Ref_Fail   = 35,          // head reference failure (QIC80 only)
    Edge_Seek_Fail  = 36,          // seeking to edge tracks failed (QIC80 only)
    Mssng_Train_Tbl = 37,          // 37 - Missing Training Table
    Invalid_Format  = 38,          // 38 - Invalid Format
    EOT_Sensor_Flr  = 39,          // 39 - EOT/BOT Sensor Failure
    Trn_Tbl_Chksum  = 40,          // 40 - Training Table Checksum Error
    Wtchdog_Reset   = 41,          // 41 - Watchdog Timer Reset Occurred
    Illegal_Error   = 42,          // 42 - Illegal Error Number
    NotRdy          = 106         // Drive Not Ready
} FIRMWARE_ERROR, *PFIRMWARE_ERROR;

// Floppy Disk Port constants

// normal drive B
#define curb                            1
#define selb                0x2d    // 00101101: motor B + enable DMA/IRQ/FDC + sel B
#define dselb               0x0c    // 00001100: enable DMA/IRQ/FDC + sel A
// unselected drive
#define curu                            0
#define selu                0x0d    // 00001101: enable DMA/IRQ/FDC + sel B
#define dselu               0x0c    // 00001100: enable DMA/IRQ/FDC + sel A
// normal drive D
#define curd                            3
#define seld                0x8f    // 10001111: motor D + enable DMA/IRQ/FDC + sel D
#define dseld               0x0e    // 00001110  motor D + enable DMA/IRQ/FDC + sel C
// laptop unselected drive
#define curub                           0
#define selub               0x2d    // 00101101: motor B + enable DMA/IRQ/FDC + sel B
#define dselub              0x0c    // 00001100: enable DMA/IRQ/FDC + sel A

#define alloff              0x08    // no motor + enable DMA/IRQ + disable FDC + sel A
#define fdc_idle            0x0c    // no motor + enable DMA/IRQ/FDC + sel A

#define DRIVE_ID_MASK       0x03

// Floppy configuration parameters

#define FDC_FIFO            15      // FIFO size for an 82077
#define FMT_GPL             233     // gap length for format (QIC-40 205ft & 310ft)
#define WRT_GPL             109     // gap length for write (QIC-40 205ft & 310ft)
#define FMT_BPS             03      // bytes per sector for formatting(1024)
#define WRT_BPS             FMT_BPS // bytes per sector for reading/writing (1024)
#define FSC_SEG             32      // floppy sectors per segment (QIC-40 205ft & 310ft)
#define SEG_FTK             4       // segments per floppy track (QIC-40 205ft & 310ft)
#define FSC_FTK             (FSC_SEG*SEG_FTK)    // floppy sectors per floppy track (QIC-40 205ft & 310ft)
#define SEG_TTRK_40         68      // segments per tape track (QIC-40 205ft)
#define SEG_TTRK_40L        102     // segments per tape track (QIC-40 310ft)
#define SEG_TTRK_80         100     // segments per tape track (QIC-80 205ft)
#define SEG_TTRK_80L        150     // segments per tape track (QIC-80 310ft)
#define SEG_TTRK_QICEST_40  365     // segments per tape track (QIC-40 QICEST)
#define SEG_TTRK_QICEST_80  537     // segments per tape track (QIC-80 QICEST)
#define SEG_TTRK_500        422     // segments per tape track (QIC-500)
#define SEG_TTRK_500L       1574    // segments per tape track (QIC-500)

#define FTK_FSD_40          170     // floppy tracks per floppy side (QIC-40 205ft)
#define FTK_FSD_40L         255     // floppy tracks per floppy side (QIC-40 310ft)
#define FTK_FSD_80          150     // floppy tracks per floppy side (QIC-80 205ft)
#define FTK_FSD_80L         150     // floppy tracks per floppy side (QIC-80 310ft)
#define FTK_FSD_QICEST_40   254     // floppy tracks per floppy side (QIC-40 QICEST) */
#define FTK_FSD_QICEST_80   254     // floppy tracks per floppy side (QIC-80 QICEST) */
#define FTK_FSD_500         254     // floppy tracks per floppy side (QIC-500) */
#define FTK_FSD_QICEST_500  254     // floppy tracks per floppy side (QIC-500) */

#define NUM_TTRK_40         20      // number of tape tracks (QIC-40 205ft & 310ft)
#define NUM_TTRK_80         28      // number of tape tracks (QIC-40 205ft & 310ft)
#define NUM_TTRK_500        40      // number of tape tracks (QIC-500)
#define PHY_SIZ             1024    // number of bytes per sector

//
// Tape Format Types and lengths/Coercivity
//

#define QIC_UNKNOWN         0       // Unknown Tape Format and Length
#define QIC_40              1       // QIC-40 Tape Format
#define QIC_80              2       // QIC-80 Tape Format
#define QIC_500             3       // QIC-500 Tape Format
#define QIC_SHORT           1       // Length = 205 & Coercivity = 550 Oe
#define QIC_LONG            2       // Length = 307.5 & Coercivity = 550 Oe
#define QIC_SHORT_900       3       // Length = 295 & Coercivity = 900 Oe
#define QICEST              4       // Length = 1100 & Coercivity = 550 Oe
#define QICEST_900          5       // Length = 1100 & Coercivity = 900 Oe

// Floppy disk controller misc constants

// 82077 version number
#define VALID_NEC_FDC       0x90    // version number
#define NSC_PRIMARY_VERSION 0x70    // National 8477 verion number
#define NSC_MASK            0xF0    // mask for National version number
#define FDC_82078_44_MASK   0x40    // mask for 82078 44 pin part id

// main status register
#define MSR_RQM             0x80    // request for master
#define MSR_DIO             0x40    // data input/output (0=input, 1=output)
#define MSR_EXM             0x20    // execution mode
#define MSR_CB              0x10    // FDC busy
#define MSR_D3B             0x08    // FDD 3 busy
#define MSR_D2B             0x04    // FDD 2 busy
#define MSR_D1B             0x02    // FDD 1 busy
#define MSR_D0B             0x01    // FDD 0 busy

// status register 0
#define ST0_IC              0xC0    // Interrupt code (00=Normal, 01=Abnormal, 10=Illegal cmd, 11=Abnormal)
#define ST0_SE              0x20    // Seek end
#define ST0_EC              0x10    // Equipment check
#define ST0_NR              0x08    // Not Ready
#define ST0_HD              0x04    // Head Address
#define ST0_US              0x03    // Unit Select (0-3)

// status register 1
#define ST1_EN              0x80    // End of Cylinder
#define ST1_DE              0x20    // Data Error (CRC error)
#define ST1_OR              0x10    // Over Run
#define ST1_ND              0x04    // No Data
#define ST1_NW              0x02    // Not Writable (write protect error)
#define ST1_MA              0x01    // Missing Address Mark

// status register 2
#define ST2_CM              0x40    // Control Mark (Deleted Data Mark)
#define ST2_DD              0x20    // Data Error in Data Field
#define ST2_WC              0x10    // Wrong Cylinder
#define ST2_SH              0x08    // Scan Equal Hit
#define ST2_SN              0x04    // Scan Not Satisfied
#define ST2_BC              0x02    // Bad Cylinder
#define ST2_MD              0x01    // Missing Address Mark in Data Field

// status register 3
#define ST3_FT              0x80    // Fault
#define ST3_WP              0x40    // Write Protected
#define ST3_RY              0x20    // Ready
#define ST3_T0              0x10    // Track 0
#define ST3_TS              0x08    // Two Side
#define ST3_HD              0x04    // Head address
#define ST3_US              0x03    // Unit Select (0-3)

// Misc. constants

#define FWD                 0       // seek in the logical forward direction
#define REV                 1       // seek in the logical reverse direction
#define STOP_LEN            5       // approximate number of blocks used to stop the tape
#define SEEK_SLOP           3       // number of blocks to overshoot at high speed in a seek
#define SEEK_TIMED          0x01    // Perform a timed seek
#define SEEK_SKIP           0x02    // perform a skip N segemnts seek
#define SEEK_SKIP_EXTENDED  0x03    // perform an extended skip N segemnts seek

// number of blocks to overshoot when performing a high speed reverve seek
#define QIC_REV_OFFSET      3
#define QIC_REV_OFFSET_L        4
#define QICEST_REV_OFFSET   14
#define MAX_SKIP            255     // Max number of segments that a Skip N Segs command can skip
#define MAX_SEEK_NIBBLES    3       // Maximum number of nibbles in an extended mode seek

#define NumBad              10      // number of bad READ ID's in row for no_data error
#define OR_TRYS             10      // number of Over Runs ignored per block (system 50)

#define PRIMARY_MODE        0       // tape drive is in primary mode
#define FORMAT_MODE         1       // tape drive is in format mode
#define VERIFY_MODE         2       // tape drive is in verify mode
#define DIAGNOSTIC_1_MODE   3       // tape drive is in diagnostic mode 1
#define DIAGNOSTIC_2_MODE   4       // tape drive is in diagnostic mode 2

#define DMA_WRITE           FALSE   // Program the DMA to write (FDC->DMA->RAM)
#define DMA_READ            TRUE    // Program the DMA to read (RAM->DMA->FDC)

#define READ_BYTE           8       // Number of Bytes to receive from the tape
#define READ_WORD           16      //  drive during communication.

#define HD_SELECT           0x01    // High Density Select bit from the PS/2 DCR

#define TAPE_250Kbps        0       // Program drive for 250 Kbps transfer rate
#define TAPE_2Mbps          1       // Program drive for 2Mbps transfer rate
#define TAPE_500Kbps        2       // Program drive for 500 Kbps transfer rate
#define TAPE_1Mbps          3       // Program drive for 1 Mbps transfer rate
#define FDC_250Kbps         2       // Program FDC for 250 Kbps transfer rate
#define FDC_500Kbps         0       // Program FDC for 500 Kbps transfer rate
#define FDC_1Mbps           3       // Program FDC for 1 Mbps transfer rate
#define FDC_2Mbps           1       // Program FDC for 2 Mbps transfer rate
#define SRT_250Kbps         0xff    // FDC step rate for 250 Kbps transfer rate
#define SRT_500Kbps         0xef    // FDC step rate for 500 Kbps transfer rate
#define SRT_1Mbps           0xdf    // FDC step ratefor 1 Mbps transfer rate
#define SRT_2Mbps           0xcf    // FDC step rate for 2 Mbps transfer rate
#define SPEED_MASK          0x03    // FDC speed mask for lower bits
#define SLOW                0       // current transfer rate is slow
#define FAST                1       // current transfer rate is fast
#define FDC_2MBPS_TABLE     2       // 2 Mbps data rate table for the 82078

#define UNKNOWN             0xFF    // drive_type = unknown
#define CMS                 0       // drive_type = CMS (command set specific) drive
#define SUMMIT              1       // drive_type = Summit
#define WANGTEK             2       // drive_type = Wangtek
#define CORE                3       // drive_type = Core
#define ARCHIVE             4       // drive_type = Archive
#define CMS_ENHANCEMENTS    5       // drive_type = CMS Enhancements
#define IOMEGA              6       // drive_type = IOMega
#define UNSUPPORTED         7       // drive_type = Unsupported
#define CMS_SIG             0xa5    // drive signature for CMS drives
#define CMS_VEND_NO         0x0047  // CMS vendor number
#define SUMMIT_VEND_NO      0x0180  // Summit vendor number
#define IOMEGA_VEND_NO      0x8880  // Iomega vendor number
#define WANGTEK_VEND_NO     0x01c0  // Wangtek vendor number
#define TECHMAR_VEND_NO     0x01c0  // Techmar vendor number
#define CORE_VEND_NO        0x0000  // Core vendor number
#define ARCHIVE_VEND_NO_OLD 0x0005  // Archive vendor number (old mode)
#define ARCHIVE_VEND_NO_NEW 0x0140  // Archive vendor number (new mode)
#define VENDOR_MASK         0xffc0  // Vendor id mask
#define SUMMIT_QIC80        0x0001  // Summit QIC80 Model #
#define WANGTEK_QIC80       0x000a  // Wangtek QIC80 Model #
#define WANGTEK_QIC40       0x0002  // Wangtek QIC40 Model #
#define CORE_QIC80          0x0021  // Core QIC80 Model #


//
// Archive Native mode defines
//

#define ARCHIVE_500KB_XFER  0x0400  // 500 KB xfer rate
#define ARCHIVE_1MB_XFER    0x0800  // 1 MB xfer rate
#define ARCHIVE_20_TRACK    0x0001  // Drive supports 20 tracks
#define ARCHIVE_28_TRACK    0x000e  // Drive supports 28 tracks
#define ARCHIVE_MODEL_5580  0x0002  // Archive Model 5580 series (Hornet)
#define ARCHIVE_MODEL_XKE   0x0004  // Archive 11250 series (1" drives) XKE
#define ARCHIVE_MODEL_XKEII 0x0008  // Archive 11250 series (1" drives) XKEII




#define FDC_PART_ID         0x11    // Part Id Command
#define READ                0x46    // read data + MFM + do not Skip del. data + not Multi-track
#define WRITE               0x45    // write data + MFM + not Multi-track
#define WRTDEL              0x49    // write deleted data + MFM + not Multi-track
#define FDC_SNS_INT         0x08    // FDC sense interrupt status
#define FDC_VERSION         0x10    // report FDC version
#define NSC_VERSION         0x18    // report National 8477 version
#define FDC_CONFIG          0x13    // configure 82077 FDC
#define FDC_SPECIFY         0x03    // specify setup parameters
#define FDC_SAVE            0x4e    // 82078 64 pin save state command
#define FDC_DRIVE_SPECIFY   0x8e    // 82078 64 pin drive specification command
#define PERP_MODE_COMMAND   0x12    // Perpendicular Mode Command Byte
#define FDC_P_MODE2         0x00    // second byte of perpendicular mode cmd
#define FDC_INVALID_CMD     0x80    // invalid cmd sent to FDC returns this value
#define RTIMES              3       // times to retry on a read of a sector (retry mode)
#define NTIMES              2       // times to retry on a read of a sector (normaly)
#define WTIMES              10      // times to retry on a write of a sector
#define VTIMES              0       // times to retry on verify
#define ANTIMES             0
#define ARTIMES             6
#define DRIVE_SPEC_SAVE     2       // sizeof the drive spec save command

#define FIND_RETRIES        2
#define REPORT_RPT          6       // Number of times to attempt drive communication when
                                    //  an ESD induced error is suspected.
#define FDC_MSR_RETRIES     50      // Number of times to read the FDC Main
                                    //  Status Register before reporting a NECFlt

#define DRIVEA              0
#define DRIVEB              1
#define DRIVEC              2
#define DRIVED              3
#define DRIVEU              4
#define DRIVEUB             5

#define DISABLE_PRECOMP     1       // Value used by the 82078's Drive Spec
                                    // command to disable Precomp

#define FDC_BOOT_MASK     0x06      // Mask used to isolate the Boot Select
                                    // Bits in the TDR Register

#define MAX_SEEK_COUNT      10

#define WRITE_REF_RPT       2

#define _DISK_RESET         0

#define WRITE_PROTECT_MASK  0x20    // bit from byte from port 2 of the jumbo B processor that indicates write protect

// Constants for sense_speed algorithm
// These ranges are based on 1.5 sec @ 250kb.  The units are 54.95ms (1 IBM PC
// timer tick (18.2 times a second)) and are +-1 tick from nominal due to time
// base fluctuation (in FDC and IBM PC TIMER).
// The threshold for the 750kb transfer rate is < 11 ticks due to the
// uncertainty of this future transfer rate.
// If a transfer rate of 750kb is needed code MUST be added to verify that
// 750kb does exist

#define sect_cnt            35      // .04285 sec. per sector * 35 = 1.4997 sec.
#define MIN1000             0
#define MAX1000             11
#define MIN500              12
#define MAX500              15
#define MIN250              26
#define MAX250              29

// Array indices and size for the time_out array. The time out array contains the  *
// time outs for the QIC-117 commands.
#define L_SLOW                          0
#define L_FAST              1
#define PHYSICAL                2
#define TIME_OUT_SIZE           3

// Constants for the arrays defined in the S_O_DGetInfo structure
#define OEM_LENGTH          20
#define SERIAL_NUM_LENGTH       4
#define MAN_DATE_LENGTH     2
#define PEGASUS_START_DATE  517

// Constant for the array dimension used in Q117iHighSpeedSeek
#define  FOUR_NIBS                      4

// Constants for identifing bytes in a word array
#define LOW_BYTE            0
#define HI_BYTE                         1

// Strings that are sent to DComFirm
#define DCOMFIRM_READ_RAM_STRING            "\x01\x2C\xFD\x01\x00"
#define DCOMFIRM_SET_RAM_STRING             "\x04\x2A\x00\x2B\x00\xFD\x00"
#define DCOMFIRM_WRITE_RAM_STRING           "\x06\x28\x28\x00\x29\x29\x00\xFD\x00"

// The number of characters in the DComFirm strings
#define DCOMFIRM_NUM_READ_RAM_CHARS         5
#define DCOMFIRM_NUM_SET_RAM_CHARS          7
#define DCOMFIRM_NUM_WRITE_RAM_CHARS        9

// Numbers used as arguments to Q117iGetComFirmStr to indicate the desired
// DComfirm String
#define DCOMFIRM_READ_RAM_REQ               0
#define DCOMFIRM_SET_RAM_REQ                1
#define DCOMFIRM_WRITE_RAM_REQ              2


#define DCOMFIRM_MAX_CHARS  10  // Max number of chars in a DComFirm string

// Various addresses used as arguments in the set ram command for the Sankyo
// motor fix hack
#define DBL_HOLE_CNTER_ADD_UPPER_NIBBLE         5
#define DBL_HOLE_CNTER_ADD_LOWER_NIBBLE         0xD
#define HOLE_FLAG_BYTE_ADD_UPPER_NIBBLE         4
#define HOLE_FLAG_BYTE_ADD_LOWER_NIBBLE         8
#define TAPE_ZONE_BYTE_ADD_UPPER_NIBBLE         6
#define TAPE_ZONE_BTYE_ADD_LOWER_NIBBLE         8

// String indices for key bytes in DComFirm Strings
#define DCOMFIRM_READ_RTRN_BYTE_INDEX       4
#define DCOMFIRM_SETRAM_HI_NIB_INDEX            2
#define DCOMFIRM_SETRAM_LOW_NIB_INDEX           4
#define DCOMFIRM_WRTRAM_HI_NIB_INDEX            3
#define DCOMFIRM_WRTRAM_LOW_NIB_INDEX           6


// Miscellaneous defines used in the Sankyo Motor fix hack
#define REVERSE                             0
#define FORWARD                             1
#define AT_BOT                              0
#define AT_EOT                              1
#define HOLE_INDICATOR_MASK                 0X40
#define EOT_ZONE_COUNTER_UPPER_NIBBLE       2
#define EOT_ZONE_COUNTER_LOWER_NIBBLE       9
#define BOT_ZONE_COUNTER_UPPER_NIBBLE       2
#define BOT_ZONE_COUNTER_LOWER_NIBBLE       3
#define MAX_DECOMFIRM_CHARS                 10
