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

/****************************************************************************\
*
* RMD (Real-Mode Driver) Data Structure
*
\****************************************************************************/

typedef struct rmd_scsi_unit_t { /* */
    UCHAR                       rsu_bus_number;
    UCHAR                       rsu_target_id;
    UCHAR                       rsu_lun;
    UCHAR                       rsu_reserved1;
} rmd_scsi_unit_t, *rmd_scsi_unitp_t;


typedef union rmd_api_id_t { /* */
    ULONG                       rai_block_unit_number;
    rmd_scsi_unit_t             rai_scsi_unit;
} rmd_api_id_t, *rmd_api_idp_t;

#define rai_scsi_bus_number     rai_scsi_unit.rsu_bus_number
#define rai_scsi_target_id      rai_scsi_unit.rsu_target_id
#define rai_scsi_lun            rai_scsi_unit.rsu_lun


typedef struct rmd_unit_spec_t { /* */
    rmd_api_id_t                rus_source_api_id;
    UCHAR                       rus_dest_api;       // RMD_TYPE_xxx
    UCHAR                       rus_flags;
    UCHAR                       rus_reserved1[2];
    rmd_api_id_t                rus_dest_api_id;
    USHORT                      rus_inquiry_checksum;
    UCHAR                       rus_relative_hba;
    UCHAR                       rus_device_type;
    struct rmd_unit_spec_t *    rus_next_unit;
} rmd_unit_spec_t, *rmd_unit_specp_t;


typedef struct rmd_port_io_range_t { /* */
    ULONG                       rpir_range_start;
    ULONG                       rpir_range_end;
    UCHAR                       rpir_in_memory;     // TRUE=mem; FALSE=IOport
    UCHAR                       rpir_reserved1[3];
} rmd_port_io_range_t, *rmd_port_io_rangep_t;


typedef struct rmd_port_adapter_node_t { /* */
    struct rmd_port_adapter_node_t *    rpan_next_adapter_node;
    UCHAR                               rpan_bus_number;
    UCHAR                               rpan_host_bus_type;
    UCHAR                               rpan_irq_level;
    UCHAR                               rpan_reserved1;
    ULONG                               rpan_max_xfer_length;
    UCHAR                               rpan_max_sg_elements;
    UCHAR                               rpan_dma_channel;
    UCHAR                               rpan_dma_width;
    UCHAR                               rpan_dma_speed;
    ULONG                               rpan_num_io_ranges;
    rmd_port_io_range_t *               rpan_io_ranges;
    ULONG                               rpan_flags;
} rmd_port_adapter_node_t, *rmd_port_adapter_nodep_t;

#define RMD_RPAN_FNEEDS_PHYS_ADDRESSES  0x00000001
#define RMD_RPAN_FDMA_BUS_MASTER        0x00000002
#define RMD_RPAN_FCACHES_DATA           0x00000004
#define RMD_RPAN_FADAPTER_SCANS_DOWN    0x00000008
#define RMD_RPAN_FDMA_32BIT_ADDRESSES   0x00000010


typedef struct rmd_header_t { /* */
    UCHAR                       rh_rmd_type;
    struct rmd_header_t *       rh_next_rmd;
} rmd_header_t, *rmd_headerp_t;

#define RMD_TYPE_PORT           0x00
#define RMD_TYPE_ASPI           0x01
#define RMD_TYPE_INT13          0x02
#define RMD_TYPE_CAM            0x03
#define RMD_TYPE_INT4B          0x04
#define RMD_TYPE_BLOCK          0x05
#define RMD_TYPE_SCSIPORT       0x06
#define RMD_TYPE_IOSYS          0x07


typedef struct rmd_port_t { /* */
    rmd_header_t                rp_header;
    rmd_port_adapter_node_t *   rp_adapter_node;
    PVOID                       rp_miniport_name;
    PVOID                       rp_api_rmd_chain;
    ULONG                       rp_flags;
} rmd_port_t, *rmd_portp_t;

#define rp_rmd_type             rp_header.rh_rmd_type
#define rp_next_port_rmd        rp_header.rh_next_rmd


typedef struct rmd_aspi_t { /* */
    rmd_header_t                ra_header;
    rmd_unit_spec_t *           ra_units;
    PVOID                       ra_entry;
    PVOID                       ra_chain;
    ULONG                       ra_flags;
    UCHAR                       ra_miniport_name[9];
} rmd_aspi_t, *rmd_aspip_t;

#define ra_rmd_type             ra_header.rh_rmd_type
#define ra_next_aspi_rmd        ra_header.rh_next_rmd

#define RMD_RA_FHAS_DCBS        0x00000001
#define RMD_ASPI_HOOKED         0x00000010


typedef struct rmd_block_t { /* */
    rmd_header_t                rb_header;
    rmd_unit_spec_t *           rb_units;
    PVOID                       rb_entry;
} rmd_block_t, *rmd_blockp_t;

#define rb_rmd_type             rb_header.rh_rmd_type
#define rb_next_block_rmd       rb_header.rh_next_rmd


typedef struct rmd_int13_t { /* */
    rmd_header_t                ri13_header;
    rmd_unit_spec_t *           ri13_units;
    PVOID                       ri13_entry;
    PVOID                       ri13_chain;
    ULONG                       ri13_flags;
} rmd_int13_t, *rmd_int13p_t;

#define ri13_rmd_type           ri13_header.rh_rmd_type
#define ri13_next_int13_rmd     ri13_header.rh_next_rmd

#define RMD_RI13_FBIOS_HOOKED   0x00000001


typedef struct rmd_cam_t { /* */
    rmd_header_t                rc_header;
    rmd_unit_spec_t *           rc_units;
    PVOID                       rc_entry;
    PVOID                       rc_chain;
    ULONG                       rc_flags;
    UCHAR                       rc_miniport_name[9];
} rmd_cam_t, *rmd_camp_t;

#define rc_rmd_type             rc_header.rh_rmd_type
#define rc_next_int13_rmd       rc_header.rh_next_rmd


typedef struct rmd_int4B_t { /* */
    rmd_header_t                ri4B_header;
    rmd_unit_spec_t *           ri4B_units;
    PVOID                       ri4B_entry;
    PVOID                       ri4B_chain;
    ULONG                       ri4B_flags;
} rmd_int4B_t, *rmd_int4Bp_t;

#define ri4B_rmd_type           ri4B_header.rh_rmd_type
#define ri4B_next_int4B_rmd     ri4B_header.rh_next_rmd


#define RMDI_TYPE_DRVR_ADDR     1
#define RMDI_TYPE_INT13         2
#define RMDI_TYPE_BIOS_INT13    3
#define RMDI_TYPE_INT4B         4
#define RMDI_TYPE_INT4F         5
#define RMDI_TYPE_ASPI_ENTRY    6
#define RMDI_TYPE_DRVR_HOOK	 7
#define RMDI_TYPE_LINENUM	 8
#define RMDI_TYPE_CONFIGSTAMP	 9
#define RMDI_TYPE_HWINTHOOK	 10

#define MAX_RMDI_TYPES          10

/* rmde_Flags definitions */

#define RMDEF_UNSAFE_BIT	0
#define RMDEF_UNSAFE		(1 << RMDEF_UNSAFE_BIT)
#define RMDEF_UMB_PROVIDER_BIT	1
#define RMDEF_UMB_PROVIDER	(1 << RMDEF_UMB_PROVIDER_BIT)

typedef struct rmd_driver_info_t {              // struct for storing different
                                                // type of addresses
   UCHAR        rmdi_Type ;                     // Type of Addr RMDI_TYPE_*
   ULONG        rmdi_Addr ;                     // SEG:OFF
} rmd_driver_info_t, *rmd_driver_infop_t ;

#define	rmdi_hookmap	rmdi_Addr

typedef struct rmd_driver_entry_t {
   UCHAR        rmde_DriverName[11] ;           // blank padded driver name
   WORD	 rmde_Flags ;			 // Flags field
   UCHAR        rmde_NumUnits ;                 // 0 for char devices
   UCHAR        rmde_NumInfo ;                  // no. of address packets
   rmd_driver_info_t    rmde_Info[1] ;          // Array of addr packets
} rmd_driver_entry_t, *rmd_driver_entryp_t ;

typedef struct rmd_bios_t {
    rmd_header_t                rbios_rmdhdr ;
    UCHAR                       rbios_NumDriverEntries;
    rmd_driver_entry_t  rbios_DriverEntries[1] ;
} rmd_bios_t, *rmd_biosp_t;


// used by io.sys to save current values of all information
typedef struct rmd_max_driver_entry_t {
   UCHAR        rmdme_DriverName[8] ;           // blank padded driver name
   UCHAR        rmdme_DriverExt[3] ;
   WORD	 rmdme_Flags ;			 // Flags field
   UCHAR        rmdme_NumUnits ;                // 0 for char devices
   rmd_driver_info_t    rmdme_Info[MAX_RMDI_TYPES] ;    // Array of addr packets
   ULONG	 rmdme_DrvrAddrs[26] ;		// Save area for driver address
} rmd_max_driver_entry_t, *rmd_max_driver_entryp_t ;


