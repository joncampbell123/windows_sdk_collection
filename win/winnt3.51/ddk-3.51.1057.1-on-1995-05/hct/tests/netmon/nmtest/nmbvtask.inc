'******************************************************************************
'*                                                                            *
'* WARNING: Do not modify this file with any editor other than MTUIEDIT.EXE.  *
'*                                                                            *
'******************************************************************************
const CaptureNet =                102
const PasswordList =              103
const Transmit =                  105
const TransmitNet =               106
const RemoteName =                108
const Remote_Connect =            112
const Buffer_Size =               104
const Frame_Size =                110
const RemoteCaptureNet =          111
const NM_Path =                   114
const Description =               116
const Local_Ident =               119
const Remote_Indent =             121
const Rmt_Ident_X =               122
const Multi_Pass =                127
const Suspend_Reps =              131
const Output_Path =               133
const Total_Reps =                134
const File_IO =                   135
const Sngl_Frame_Rep =            137
const Same_Frame_Rep =            141
const Lcl_Ident_Reps =            142
const Big_Frame_Rep =             143
const Discon_Reps =               144
const Viewport_IO =               145
const Infinite_Pass =             146
const NMBVTIO2 =                  200
const Do_Lcl_Cap =                203
type _DLG_NMBVTIO2
	Txt_TransmitNet as string
	Txt_RemoteName as string
	Cmb_PasswordList as string
	Chk_Transmit as short
	Chk_Remote_Connect as short
	Txt_Buffer_Size as string
	Lbl_107 as string
	Lbl_109 as string
	Txt_Frame_Size as string
	Lbl_113 as string
	Txt_RemoteCaptureNet as string
	Txt_CaptureNet as string
	Txt_NM_Path as string
	Txt_Description as string
	Lbl_101 as string
	Lbl_115 as string
	Lbl_117 as string
	Txt_Sngl_Frame_Rep as string
	Chk_Local_Ident as short
	Txt_Lcl_Ident_Reps as string
	Chk_Remote_Indent as short
	Txt_Rmt_Ident_X as string
	Opt_Infinite_Pass as short
	Opt_Multi_Pass as short
	Opt_Viewport_IO as short
	Opt_129 as short
	Lbl_123 as string
	Lbl_124 as string
	Txt_Discon_Reps as string
	Txt_Suspend_Reps as string
	Lbl_132 as string
	Txt_Output_Path as string
	Txt_Total_Reps as string
	Opt_File_IO as short
	Lbl_138 as string
	Txt_Big_Frame_Rep as string
	Lbl_140 as string
	Txt_Same_Frame_Rep as string
	Lbl_201 as string
	Chk_Do_Lcl_Cap as short
	Lbl_202 as string
end type
global Dlg_NMBVTIO2 as _DLG_NMBVTIO2
