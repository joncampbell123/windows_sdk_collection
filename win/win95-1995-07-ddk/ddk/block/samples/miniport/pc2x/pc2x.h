/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*************************************************************************
*
* Copyright (c) 1993 Iomega Corporation
*
* Module Name:
*
*    PC2x.h
*
* Abstract:
*
*    Definitions for the Iomega PC2x 8-bit SCSI adapter card.
*
*/

#include "scsi.h"

#define BASE_PORT      0x340
#define NUMBR_PORTS    4
#define BASE_WIDTH     0x10

//
// Offsets from Base Port Address.
//

#define SWITCH         0   
#define STATUS         1
#define CONTROL        2
#define DATA           3

//
// Macros for accessing the control memory of the PC2X.
//
#define PC2X_SET_CONTROL(BASE, VALUE) \
           ScsiPortWritePortUchar((BASE + CONTROL), (VALUE))

#define PC2X_READ_STATUS(BASE) \
           ScsiPortReadPortUchar(BASE + STATUS)

#define PC2X_WRITE_DATA(BASE, VALUE) \
           ScsiPortWritePortUchar((BASE + DATA), (UCHAR) (VALUE))

#define PC2X_READ_DATA(BASE) \
           ScsiPortReadPortUchar(BASE + DATA)

#define PC2X_READ_SWITCHES(BASE) \
           ScsiPortReadPortUchar(BASE + SWITCH)

//
// Control port output codes.
//
#define C_IDLE         0x00    
#define C_RESET        0x01    
#define C_SELECT       0x02    
#define C_DMA_ENABLE   0x04    
#define C_INT_ENABLE   0x08    

//
// Input status bit definitions.
//
#define S_REQUEST      0x01    // Request line from SCSI bus
#define S_IO           0x02    // Input/Output line from SCSI bus
#define S_CD           0x04    // Command/Data line from SCSI bus
#define S_MESSAGE      0x08    // Message line from SCSI bus
#define S_BUSY         0x10    // Busy line from SCSI bus
#define S_PARITY       0x20    // Parity error status
#define S_ACK          0x40    // Acknowlege line from SCSI bus
#define S_DMA          0x80    // DMA Status and BUS_FREE with DMA disabled

//
// Useful status combinations.
//
#define BP_BUS_FREE    0
#define BP_COMMAND     ( S_BUSY | S_CD | S_REQUEST )
#define BP_MESSAGE_IN  ( S_BUSY | S_MESSAGE | S_IO | S_CD | S_REQUEST )
#define BP_MESSAGE_OUT ( S_BUSY | S_MESSAGE | S_CD | S_REQUEST )
#define BP_DATA_IN     ( S_BUSY | S_IO | S_REQUEST )
#define BP_DATA_OUT    ( S_BUSY | S_REQUEST )
#define BP_STATUS      ( S_BUSY | S_IO | S_CD | S_REQUEST )
#define BP_RESELECT    ( S_SELECT | S_IO )

//
// Status mask for bus phase.  This is everything except Parity, ACK, and
// DMA.
//
#define S_PHASE_MASK   ((UCHAR) 0x1F)

#define PC2X_READ_PHASE(BASE)  ((UCHAR) (PC2X_READ_STATUS(BASE) & S_PHASE_MASK))

//
// SCSI source identifier for host system.
//
#define SCSI_INITIATOR_ID  7
#define PC2X_IDT_VECTOR    5

//
// Various timeout values (in microseconds).
//
#define REQUEST_SPIN_WAIT   1000000 // Wait for target to assert REQUEST
#define RESET_HOLD_TIME     25      // Time to hold RESET line to reset bus
#define SELECTION_DELAY     100000  // Wait for target to assert BUSY
#define BUS_FREE_DELAY      100000  // Wait for BUS_FREE condition

#define PC2X_TIMER_VALUE    40000   // Time for timer to fire

//
// Various limits...
//

//
// Number of adapters this driver will support.
//
#define MAX_ADAPTERS       1

//
// Limit transfer length to 64 Kbytes
// 
#define MAX_TRANSFER_LENGTH ( 64 * 1024 )



