
;
; HISTORY
;	1.01	092387	jhc	Changed some incorrect comments on multimode
;				INPUT_STATUS flags.
;


		ifdef	CGA
;
; Define CGA register ports
;
CRTC_ADDR	=	003D4h
CRTC_DATA	=	003D5h
MODE_CONTROL	=	003D8h
COLOR_SELECT	=	003D9h
INPUT_STATUS	=	003DAh
LPEN_CLEAR	=	003DBh
LPEN_SET	=	003DCh


;
; Define bitmapped codes for MODE_CONTROL
;
MC_TEXT_RES	=	00000001b		;0 = 40x25, 1 = 80x25
MC_GRPH 	=	00000010b		;0 = text, 1 = graphics
MC_BLACKWHITE	=	00000100b		;0 = color, 1 = bw
MC_ENABLE_DSP	=	00001000b		;0 = no disp, 1 = enable display
MC_GRPH_RES	=	00010000b		;0 = 320x200, 1 = 640x200
MC_ENABLE_BLINK =	00100000b		;0 = no blink, 1 = enable blink


;
; Define bitmapped codes for COLOR_SELECT
;
CS_BLUE 	=	00000001b
CS_GREEN	=	00000010b
CS_RED		=	00000100b
CS_INTENSE0	=	00001000b
CS_INTENSE1	=	00010000b
CS_PALETTE	=	00100000b


;
; Define bitmapped codes for INPUT_STATUS
;
IS_H_RETRACE	=	00000001b		;1 = hrz retrace in progress
IS_LPEN_TRIGGER =	00000010b		;1 = light pen triggered
IS_LPEN_SWITCH	=	00000100b		;1 = light pen switch closed
IS_V_RETRACE	=	00001000b		;1 = vrt retrace in progress
		endif


		ifdef	HERCULES
;
; Define Hercules register ports
;
CRTC_ADDR	=	003B4h
CRTC_DATA	=	003B5h
MODE_CONTROL	=	003B8h
LPEN_SET	=	003B9h
INPUT_STATUS	=	003BAh
LPEN_CLEAR	=	003BBh
EX_MODE_CONTROL =	003BFh


;
; Define bitmapped codes for MODE_CONTROL
;
MC_RESERVED_0	=	00000001b
MC_GRPH 	=	00000010b		;0 = text, 1 = graphics
MC_RESERVED_2	=	00000100b
MC_ENABLE_DSP	=	00001000b		;0 = no disp, 1 = enable display
MC_RESERVED_4	=	00010000b
MC_ENABLE_BLINK =	00100000b		;0 = no blink, 1 = enable blink
MC_RESERVED_6	=	01000000b
MC_PAGE_SELECT	=	10000000b		;0 = page 0, 1 = page 1


;
; Define bitmapped codes for INPUT_STATUS
;
IS_H_RETRACE	=	00000001b		;1 = hrz retrace in progress
IS_RESERVED_1	=	00000010b
IS_RESERVED_2	=	00000100b
IS_DOTS 	=	00001000b		;1 = dot stream on
IS_VER_ID_4	=	00010000b
IS_VER_ID_5	=	00100000b
IS_RESERVED_6	=	01000000b
IS_V_RETRACE	=	10000000b		;0 = vrt retrace in progress


;
; Define bitmapped codes for EX_MODE_CONTROL
;
EMC_ALLOW_GRPH	=	00000001b		;1 = allow graphics mode
EMC_ALLOW_PAGE1 =	00000010b		;1 = allow page 1 usage
		endif


		ifdef	MULTIMODE
;
; Define MULTIMODE register ports
;
CRTC_ADDR	=	003D4h
CRTC_DATA	=	003D5h
MODE_CONTROL	=	003D8h
COLOR_SELECT	=	003D9h
INPUT_STATUS	=	003DAh
LPEN_CLEAR	=	003DBh
LPEN_SET	=	003DCh
EX_MODE_CONTROL =	003DDh
ADAPTER_ID	=	003DFh


;
; Define bitmapped codes for MODE_CONTROL
;
MC_TEXT_RES	=	00000001b		;0 = 40x25, 1 = 80x25
MC_GRPH 	=	00000010b		;0 = text, 1 = graphics
MC_BLACKWHITE	=	00000100b		;0 = color, 1 = bw
MC_ENABLE_DSP	=	00001000b		;0 = no disp, 1 = enable display
MC_GRPH_RES	=	00010000b		;0 = 320x200, 1 = 640x200
MC_ENABLE_BLINK =	00100000b		;0 = no blink, 1 = enable blink


;
; Define bitmapped codes for COLOR_SELECT
;
CS_BLUE 	=	00000001b
CS_GREEN	=	00000010b
CS_RED		=	00000100b
CS_INTENSE	=	00001000b
CS_INTENSE_ALT	=	00010000b
CS_PALETTE	=	00100000b


;
; Define bitmapped codes for INPUT_STATUS
;
IS_H_RETRACE	=	00000001b		;1 = hrz retrace in progress
IS_LPEN_TRIGGER =	00000010b		;1 = light pen triggered
IS_LPEN_SWITCH	=	00000100b		;1 = light pen switch closed
IS_V_RETRACE	=	00001000b		;1 = vrt retrace in progress
IS_MONITOR_FREQ =	00010000b		;1 = 15kHz monitor present
IS_ANALOG_ADAPT =	00100000b		;1 = no analog adapter present
IS_DIAG_6	=	01000000b
IS_DIAG_7	=	10000000b


;
; Define bitmapped codes for EX_MODE_CONTROL
;
EMC_VRT_RES	=	00000001b		;0 = 200 lines, 1 = 400 lines
EMC_ENABLE_UL	=	00000010b		;0 = normal, 1 = enable underline
EMC_FONT_SELECT =	00000100b		;0 = PC8, 1 = RMN8
EMC_PAGE_SELECT =	00000100b		;0 = page 0, 1 = page 1 displayable
EMC_DISABLE_MEM =	00001000b		;0 = normal, 1 = disable memory access
EMC_16K_32K_MEM =	00010000b		;0 = 16K, 1 = 32K addressable
EMC_PAGE_ACCESS =	00100000b		;0 = page 0, 1 = page 1 addressable
		endif


;
; Define CRTC register indices
;
C_HRZ_TOT	=	000h
C_HRZ_DSP	=	001h
C_HRZ_SYN_POS	=	002h
C_HRZ_SYN_WID	=	003h
C_VRT_TOT	=	004h
C_VRT_ADJ	=	005h
C_VRT_DSP	=	006h
C_VRT_SYN_POS	=	007h
C_INTERLACE	=	008h
C_MAX_SCAN_LN	=	009h
C_CRSR_START	=	00Ah
C_CRSR_END	=	00Bh
C_STRT_HGH	=	00Ch
C_STRT_LOW	=	00Dh
C_CRSR_LOC_HGH	=	00Eh
C_CRSR_LOC_LOW	=	00Fh
C_LGHT_PEN_HGH	=	010h
C_LGHT_PEN_LOW	=	011h


;
; Define CRTC register structure
;
VideoParms	struc
  vpHrzTot	db	?
  vpHrzDsp	db	?
  vpHrzSynPos	db	?
  vpHrzSynWid	db	?
  vpVrtTot	db	?
  vpVrtAdj	db	?
  vpVrtDsp	db	?
  vpVrtSynPos	db	?
  vpInterlace	db	?
  vpMaxScanLn	db	?
  vpCrsrStart	db	?
  vpCrsrEnd	db	?
  vpStartHgh	db	?
  vpStartLow	db	?
  vpCrsrLocHgh	db	?
  vpCrsrLocLow	db	?
VideoParms	ends

