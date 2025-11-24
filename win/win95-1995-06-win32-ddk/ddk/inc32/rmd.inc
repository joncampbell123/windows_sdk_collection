;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

rmd_scsi_unit_t	STRUC
rsu_bus_number	DB	?
rsu_target_id	DB	?
rsu_lun	DB	?
rsu_reserved1	DB	?
rmd_scsi_unit_t	ENDS
?UnionSize = 0
if 4 gt ?UnionSize
	?UnionSize = 4
endif
rai_block_unit_number	equ	<(dword ptr 0)>

if size rmd_scsi_unit_t gt ?UnionSize
	?UnionSize = size rmd_scsi_unit_t
endif
rai_scsi_unit	equ	<(byte ptr 0)>


rmd_api_id_t	STRUC
	DB	?UnionSize dup(?)
rmd_api_id_t	ENDS

rai_scsi_bus_number	EQU	<rai_scsi_unit.rsu_bus_number>
rai_scsi_target_id	EQU	<rai_scsi_unit.rsu_target_id>
rai_scsi_lun	EQU	<rai_scsi_unit.rsu_lun>

rmd_unit_spec_t	STRUC
rus_source_api_id	DB	SIZE rmd_api_id_t DUP (?)
rus_dest_api	DB	?
rus_flags	DB	?
rus_reserved1	DB	2 DUP (?)
rus_dest_api_id	DB	SIZE rmd_api_id_t DUP (?)
rus_inquiry_checksum	DW	?
rus_relative_hba	DB	?
rus_device_type	DB	?
rus_next_unit	DD	?
rmd_unit_spec_t	ENDS

rmd_port_io_range_t	STRUC
rpir_range_start	DD	?
rpir_range_end	DD	?
rpir_in_memory	DB	?
rpir_reserved1	DB	3 DUP (?)
rmd_port_io_range_t	ENDS

rmd_port_adapter_node_t	STRUC
rpan_next_adapter_node	DD	?
rpan_bus_number	DB	?
rpan_host_bus_type	DB	?
rpan_irq_level	DB	?
rpan_reserved1	DB	?
rpan_max_xfer_length	DD	?
rpan_max_sg_elements	DB	?
rpan_dma_channel	DB	?
rpan_dma_width	DB	?
rpan_dma_speed	DB	?
rpan_num_io_ranges	DD	?
rpan_io_ranges	DD	?
rpan_flags	DD	?
rmd_port_adapter_node_t	ENDS
RMD_RPAN_FNEEDS_PHYS_ADDRESSES	EQU	00000001H
RMD_RPAN_FDMA_BUS_MASTER	EQU	00000002H
RMD_RPAN_FCACHES_DATA	EQU	00000004H
RMD_RPAN_FADAPTER_SCANS_DOWN	EQU	00000008H
RMD_RPAN_FDMA_32BIT_ADDRESSES	EQU	00000010H

rmd_header_t	STRUC
rh_rmd_type	DB	?
rh_next_rmd	DD	?
rmd_header_t	ENDS
RMD_TYPE_PORT	EQU	00H
RMD_TYPE_ASPI	EQU	01H
RMD_TYPE_INT13	EQU	02H
RMD_TYPE_CAM	EQU	03H
RMD_TYPE_INT4B	EQU	04H
RMD_TYPE_BLOCK	EQU	05H
RMD_TYPE_SCSIPORT	EQU	06H
RMD_TYPE_IOSYS	EQU	07H

rmd_port_t	STRUC
rp_header	DB	SIZE rmd_header_t DUP (?)
rp_adapter_node	DD	?
rp_miniport_name	DD	?
rp_api_rmd_chain	DD	?
rp_flags	DD	?
rmd_port_t	ENDS
rp_rmd_type	EQU	<rp_header.rh_rmd_type>
rp_next_port_rmd	EQU	<rp_header.rh_next_rmd>

rmd_aspi_t	STRUC
ra_header	DB	SIZE rmd_header_t DUP (?)
ra_units	DD	?
ra_entry	DD	?
ra_chain	DD	?
ra_flags	DD	?
ra_miniport_name	DB	9 DUP (?)
rmd_aspi_t	ENDS
ra_rmd_type	EQU	<ra_header.rh_rmd_type>
ra_next_aspi_rmd	EQU	<ra_header.rh_next_rmd>
RMD_RA_FHAS_DCBS	EQU	00000001H
RMD_ASPI_HOOKED	EQU	00000010H

rmd_block_t	STRUC
rb_header	DB	SIZE rmd_header_t DUP (?)
rb_units	DD	?
rb_entry	DD	?
rmd_block_t	ENDS
rb_rmd_type	EQU	<rb_header.rh_rmd_type>
rb_next_block_rmd	EQU	<rb_header.rh_next_rmd>

rmd_int13_t	STRUC
ri13_header	DB	SIZE rmd_header_t DUP (?)
ri13_units	DD	?
ri13_entry	DD	?
ri13_chain	DD	?
ri13_flags	DD	?
rmd_int13_t	ENDS
ri13_rmd_type	EQU	<ri13_header.rh_rmd_type>
ri13_next_int13_rmd	EQU	<ri13_header.rh_next_rmd>
RMD_RI13_FBIOS_HOOKED	EQU	00000001H

rmd_cam_t	STRUC
rc_header	DB	SIZE rmd_header_t DUP (?)
rc_units	DD	?
rc_entry	DD	?
rc_chain	DD	?
rc_flags	DD	?
rc_miniport_name	DB	9 DUP (?)
rmd_cam_t	ENDS
rc_rmd_type	EQU	<rc_header.rh_rmd_type>
rc_next_int13_rmd	EQU	<rc_header.rh_next_rmd>

rmd_int4B_t	STRUC
ri4B_header	DB	SIZE rmd_header_t DUP (?)
ri4B_units	DD	?
ri4B_entry	DD	?
ri4B_chain	DD	?
ri4B_flags	DD	?
rmd_int4B_t	ENDS
ri4B_rmd_type	EQU	<ri4B_header.rh_rmd_type>
ri4B_next_int4B_rmd	EQU	<ri4B_header.rh_next_rmd>
RMDI_TYPE_DRVR_ADDR	EQU	1
RMDI_TYPE_INT13	EQU	2
RMDI_TYPE_BIOS_INT13	EQU	3
RMDI_TYPE_INT4B	EQU	4
RMDI_TYPE_INT4F	EQU	5
RMDI_TYPE_ASPI_ENTRY	EQU	6
RMDI_TYPE_DRVR_HOOK	EQU	7
RMDI_TYPE_LINENUM	EQU	8
RMDI_TYPE_CONFIGSTAMP	EQU	9
RMDI_TYPE_HWINTHOOK	EQU	10
MAX_RMDI_TYPES	EQU	10
RMDEF_UNSAFE_BIT	EQU	0
RMDEF_UNSAFE	EQU	(1 SHL RMDEF_UNSAFE_BIT)
RMDEF_UMB_PROVIDER_BIT	EQU	1
RMDEF_UMB_PROVIDER	EQU	(1 SHL RMDEF_UMB_PROVIDER_BIT)

rmd_driver_info_t	STRUC
rmdi_Type	DB	?
rmdi_Addr	DD	?
rmd_driver_info_t	ENDS
rmdi_hookmap	EQU	<rmdi_Addr>

rmd_driver_entry_t	STRUC
rmde_DriverName	DB	11 DUP (?)
rmde_Flags	DW	?
rmde_NumUnits	DB	?
rmde_NumInfo	DB	?
rmde_Info	DB	SIZE rmd_driver_info_t * 1 DUP (?)
rmd_driver_entry_t	ENDS

rmd_bios_t	STRUC
rbios_rmdhdr	DB	SIZE rmd_header_t DUP (?)
rbios_NumDriverEntries	DB	?
rbios_DriverEntries	DB	SIZE rmd_driver_entry_t * 1 DUP (?)
rmd_bios_t	ENDS

rmd_max_driver_entry_t	STRUC
rmdme_DriverName	DB	8 DUP (?)
rmdme_DriverExt	DB	3 DUP (?)
rmdme_Flags	DW	?
rmdme_NumUnits	DB	?
rmdme_Info	DB	SIZE rmd_driver_info_t * MAX_RMDI_TYPES DUP (?)
rmdme_DrvrAddrs	DD	26 DUP (?)
rmd_max_driver_entry_t	ENDS
