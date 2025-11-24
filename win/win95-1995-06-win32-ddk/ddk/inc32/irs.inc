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

ifndef	_IRS_H
_IRS_H	EQU	1
IRS_GET_VRP	EQU	0
IRS_REPLACE_VRP	EQU	1
IRS_GET_DRVINFO	EQU	2
IRS_MEDIA_CHECK_RESET	EQU	3
IRS_SET_CUR_FLP_UNIT	EQU	4
IRS_REAL_MODE_HANDOFF	EQU	5
IRS_QUERY_VOLUME_LOCK	EQU	6
IRS_GET_LOGICAL_DRVS	EQU	7
IRS_MOUNT_NOTIFY	EQU	8
IRS_FIRSTNXT_CHAR_DEV	EQU	9
IRS_IS_DRVCOMPRESSED	EQU	10

IRS_header	STRUC
IRS_func	DB	?
IRS_result	DB	?
IRS_header	ENDS
IRS_SUCCESS	EQU	00H

IRS_vrp_get	STRUC
filler1	DB	?
filler2	DB	?
IRS_v_g_dev_type	DB	?
IRS_v_g_designtr	DD	?
IRS_v_g_handle	DD	?
IRS_v_g_vrp	DD	?
IRS_alignchar1	DB	?
IRS_vrp_get	ENDS
IRS_V_G_CHAR	EQU	01H
IRS_V_G_SUCCESS	EQU	IRS_SUCCESS
IRS_V_G_NO_DEVICE	EQU	0A0H
IRS_V_G_UNCERTAIN_MEDIA	EQU	0A2H

IRS_vrp_replace	STRUC
filler3	DB	?
filler4	DB	?
IRS_v_r_designtr	DB	?
IRS_v_r_flags	DB	?
IRS_v_r_old_vrp	DD	?
IRS_v_r_new_vrp	DD	?
IRS_vrp_replace	ENDS
IRS_V_R_CHAR	EQU	IRS_V_G_CHAR
IRS_V_R_DEF	EQU	02H

IRS_drv_get	STRUC
IRS_filler_5	DB	?
IRS_filler_6	DB	?
IRS_DrvLetter	DB	?
IRS_DrvFlags	DD	?
IRS_alignchar2	DB	?
IRS_drv_get	ENDS
IRS_DRV_SINGLE_FLOPPY	EQU	00800000H
IRS_DRV_PAGEABLE	EQU	00400000H
IRS_DRV_RMM	EQU	00002000H
IRS_DRV_REMOTE	EQU	00000008H
IRS_DRV_REMOVABLE	EQU	00000004H
IRS_DRV_INT13_DRIVE	EQU	00000001H

IRS_media_chk_reset	STRUC
filler7	DB	?
filler8	DB	?
IRS_m_c_r_designtr	DB	?
IRS_m_c_r_vrp	DD	?
IRS_alignchar3	DB	?
IRS_media_chk_reset	ENDS
IRS_M_C_R_SUCCESS	EQU	IRS_SUCCESS
IRS_M_C_R_UNCERTAIN_MEDIA	EQU	0A2H

IRS_cur_flp_unit_set	STRUC
filler9	DB	?
filler10	DB	?
IRS_s_c_f_unit	DB	?
IRS_alignchar4	DB	?
IRS_cur_flp_unit_set	ENDS

IRS_rm_handoff	STRUC
IRS_r_m_h_hdr	DB	SIZE IRS_header DUP (?)
IRS_r_m_h_pad	DB	2 DUP (?)
IRS_alignchar5	DB	?
IRS_rm_handoff	ENDS

IRS_query_vollock	STRUC
IRS_q_v_l_hdr	DB	SIZE IRS_header DUP (?)
IRS_q_v_l_designtr	DB	?
IRS_q_v_l_drivemap	DD	?
IRS_q_v_l_pad	DB	?
IRS_query_vollock	ENDS

IRS_get_logical_drives	STRUC
IRS_g_l_d_hdr	DB	SIZE IRS_header DUP (?)
IRS_g_l_d_designtr	DB	?
IRS_g_l_d_drivemap	DD	?
IRS_g_l_d_pad	DB	?
IRS_get_logical_drives	ENDS

IRS_mnt_notify	STRUC
IRS_m_n_hdr	DB	SIZE IRS_header DUP (?)
IRS_m_n_drive	DW	?
IRS_m_n_pvrp	DD	?
IRS_m_n_drivemap	DD	?
IRS_m_n_effective_drive	DD	?
IRS_m_n_actual_drive	DD	?
IRS_mnt_notify	ENDS
IRS_M_N_SUCCESS	EQU	IRS_SUCCESS
IRS_M_N_REMOUNT	EQU	(IRS_SUCCESS+1)

IRS_frstnxt_char_dev	STRUC
IRS_g_f_n_hdr	DB	SIZE IRS_header DUP (?)
IRS_g_f_n_hndl	DD	?
IRS_g_f_n_ptr	DD	?
IRS_alignshort1	DW	?
IRS_frstnxt_char_dev	ENDS

IRS_drv_comp	STRUC
IRS_i_d_c_hdr	DB	SIZE IRS_header DUP (?)
IRS_i_d_c_drive	DB	?
IRS_i_d_c_flags	DB	?
IRS_drv_comp	ENDS
IRS_I_D_C_COMP	EQU	01H
IRS_I_D_C_PM_COMP	EQU	02H
endif
