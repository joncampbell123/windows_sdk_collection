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

VRP	STRUC
VRP_demand_flags	DD	?
VRP_event_flags	DD	?
VRP_max_sgd	DW	?
VRP_max_req_size	DW	?
VRP_delta_to_ior	DD	?
VRP_block_size	DD	?
VRP_fsd_hvol	DD	?
VRP_fsd_entry	DD	?
VRP_device_handle	DD	?
VRP_partition_offset	DD	?
VRP_next_vrp	DD	?
VRP_logical_handle	DD	?
VRP_reserved	DD	?
VRP	ENDS
VRP_dmd_small_memory	EQU	0010H
VRP_dmd_word_align	EQU	0200H
VRP_dmd_dword_align	EQU	0400H
VRP_dmd_phys_sgd	EQU	0800H
VRP_dmd_phys_sgd_ptr	EQU	1000H
VRP_dmd_do_a_b_toggling	EQU	2000H
VRP_dmd_removable_supp	EQU	4000H
VRP_dmd_lock_unlock_media	EQU	00010000H
VRP_dmd_load_eject_media	EQU	00020000H
VRP_dmd_clear_media_chg	EQU	00040000H
VRP_dmd_no_xcl_required	EQU	00200000H
VRP_dmd_pageability	EQU	00400000H
VRP_ef_media_changed	EQU	0001H
VRP_ef_media_uncertain	EQU	0002H
VRP_ef_prompting	EQU	0004H
VRP_ef_input_share	EQU	0008H
VRP_ef_output_share	EQU	0010H
VRP_ef_user_canceled	EQU	0020H
VRP_ef_write_protected	EQU	0040H
VRP_ef_real_mode_mapped	EQU	0080H
VRP_ef_ios_locked	EQU	8000H
