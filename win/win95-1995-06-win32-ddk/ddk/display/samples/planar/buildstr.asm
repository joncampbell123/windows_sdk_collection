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

;-----------------------------Module-Header-----------------------------;
; Module Name: BUILDSTR.ASM                                             ;
; This module contains the strblt functions which build                 ;
; up data on the stack for the actual output routines                   ;
;                                                                       ;
;-----------------------------------------------------------------------;
        .xlist
        include cmacros.inc
        include mflags.inc
incFont         = 1                     ;Include control for gdidefs.inc
        include gdidefs.inc
        include display.inc
        include macros.inc
        include strblt.inc
        include fontseg.inc
        .list

NLABEL  macro n
public n&_386
n&_386:
endm

PLABEL  macro n
n:
public n&_386
n&_386:
endm


        externNP        comp_byte_interval_386
        externNP        worst_case_ext_386
        externNP        output_o_rect_386
        externNP        DrawString_386
        externA         stack_top       ;Stack probe location

;----------------------------------------------------------------------------;
createSeg _PROTECT,pCode,word,public,CODE
sBegin  pCode
        .386p
        assumes cs,pCode

;----------------------------------------------------------------------------;
; The following macro is a used as a part of the code to add a word to ebx   ;
; the word is added to bx and this macro adds 10000h to ebx if a carry is    ;
; generated.                                                                 ;
;----------------------------------------------------------------------------;

updc_ebx  macro
        local   no_carry_from_bx

        jnc     short no_carry_from_bx
        add     ebx,10000h
no_carry_from_bx:
        endm

;---------------------------Public-Routine------------------------------;
; build_string
;
;   build_string builds up data on the stack consisting of character
;   offsets and widths, then invokes the routine which processes
;   this data.
;
; Entry:
;       stack frame as per strblt
; Returns:
;       none
; Error Returns:
;       none
; Registers Preserved:
;       BP
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Calls:
;
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,nothing

define_frame strblt_dummy1_386          ;Define strblt's frame
cBegin  <nogen>
cEnd    <nogen>

public  build_string_386
build_string_386        proc    near
        mov     ax,ss:stack_top         ;Compute minimum allowable SP
        add     ax,STACK_SLOP
        mov     min_stack,ax

        xor     ax,ax
        mov     nBackups,ax             ;Initially, no backup tokens on stack.

        mov     ax,text_bbox.left       ;Set up text bbox to be a null interval
        mov     text_bbox.right,ax      ;  We'll grow it as we progress
        mov     current_lhs,ax          ;Need to init this for interval calc
        mov     al,accel                ;Save the actual state of the
        mov     bl,al                   ;  IS_OPAQUE flag since the
        and     al,IS_OPAQUE            ;  worst case code may alter
        mov     wc_flags,al             ;  it
        mov     ah,excel                ;Only set text visible once a string
        and     ah,not TEXT_VISIBLE     ; is actually displayed
        mov     excel,ah

        mov     cx,offset non_justified_text
        test    bl,WEIRD_SPACING
        jz      short build_have_proc

        mov     cx,offset justified_text
        test    bl,NEG_SPACING+HAVE_WIDTH_VECT
        jz      short build_have_proc

PLABEL build_worst_ok
        mov     cx,offset worst_case
        mov     bl,accel
        mov     ah,excel

PLABEL build_have_proc
        mov     build_proc,cx           ;Save build routine address

PLABEL build_restart
        mov     clear_stack,sp          ;Used to clean the stack on exit
        mov     ax,sp                   ;Save buffer start pointer
        dec     ax
        dec     ax
        mov     buffer,ax

;-----------------------------------------------------------------------;
;       The routines which push text onto the stack expect the
;       following registers to be set on entry:
;
;               DS:SI --> current character in the string
;               ES:    =  font segment
;               AL     =  excel flags
;               AH     =  accel flags
;               CX     =  number of characters in the string
;               DI     =  current X position (and therefore starting x)
;-----------------------------------------------------------------------;
        lds     si,lp_string
        assumes ds,nothing

        mov     es,seg_lp_font
        assumes es,FontSeg

        mov     al,excel
        mov     ah,accel
        mov     cx,count
        mov     di,x
        jmp     build_proc

PLABEL build_ret_addr                   ;build routines return here
        mov     count,cx                ;Save count
        xchg    x,ax                    ;Save next char's start
        mov     xOrigin,ax              ;Save off original starting x.
        mov     off_lp_string,si        ;Save next character
        call    pad_right_hand_side     ;Fill to byte boundary if possible
        mov     current_rhs,di          ;Save rhs
        mov     bx,di                   ;Compute the interval data
        mov     dx,current_lhs
        call    comp_byte_interval_386
        jc      short build_all_done            ;If nothing shows, we're done
        mov     left_clip_mask,al
        mov     right_clip_mask,ah
        mov     inner_byte_count,si
        mov     scan_start,di
        push    0ffffh                  ;Push end sentinel.
        call    DrawString_386

PLABEL build_clr_stack
        mov     sp,clear_stack
        or      excel,TEXT_VISIBLE      ;Show something was displayed

;-----------------------------------------------------------------------;
;
;       If there is an opaquing rectangle, we must update the text
;       bounding box so that it won't overwrite the text we just
;       displayed when the rectangle is output.  IN transparent mode,
;       OPAQUE_RECT would have been cleared after the rectangle was
;       drawn and before we were called.
;
;-----------------------------------------------------------------------;
        test    excel,OPAQUE_RECT
        jz      short build_no_o_rect
        mov     ax,current_lhs
        mov     bx,text_bbox.left
        min_ax  bx
        mov     text_bbox.left,ax
        mov     ax,current_rhs
        mov     bx,text_bbox.right
        max_ax  bx
        mov     text_bbox.right,ax

PLABEL build_no_o_rect
        mov     cx,count                ;If no more characters
        jcxz    build_restore_opaque    ;  go home, have dinner, sleep


;-----------------------------------------------------------------------;
;
;       Prepare for the next string.  If in opaque mode, the
;       CLIPPED_LEFT flag will have to be set so that we don't
;       try padding the lhs.  If in transparent mode, because
;       of stepping backwards, we might actually be clipped
;       anyway, so we'll have to test for this also.
;
;       FIRST_IN_PREV must be cleared.  It will be set if the
;       clipping code determines it needs to be.
;
;-----------------------------------------------------------------------;

        mov     bl,excel                ;Will be changing flags in here
        and     bl,not (CLIPPED_LEFT+FIRST_IN_PREV)
        mov     di,x
        mov     ax,di                   ;Assume start will be current_lhs
        test    accel,IS_OPAQUE         ;When opaque, must clip on
        jnz     short build_clip_next_time  ;  a restart
        cmp     di,clip.left
        jge     short build_no_clip_next_time

PLABEL build_clip_next_time
        or      bl,CLIPPED_LEFT         ;Clipping is required
        mov     ax,clip.left            ;  and clip.left for clipping check
        max_ax  di                      ;  (but only if start X > old clip
        mov     clip.left,ax            ;   lhs)

PLABEL build_no_clip_next_time
        mov     excel,bl
        mov     current_lhs,ax          ;Need to update lhs for interval calc
        jmp     build_restart           ;Try next part of the string

PLABEL build_all_done
        mov     sp,clear_stack

PLABEL build_restore_opaque
        mov     al,wc_flags
        test    al,WC_SET_LR            ;If we stepped backwards, we'll
        jz      short build_really_done     ;  have to restore the real lhs
        mov     bx,wc_opaque_lhs        ;  and rhs incase we have an
        mov     text_bbox.left,bx       ;  opaque rectangle
        mov     bx,wc_opaque_rhs
        mov     text_bbox.right,bx

PLABEL build_really_done
        and     al,IS_OPAQUE            ;Restore IS_OPAQUE incase worst_case
        or      accel,al                ;  code cleared it, else the opaque
        ret                             ;  rectangle may overwrite our text

build_string_386 endp

;---------------------------Public-Routine------------------------------;
; non_justified_text
;
;   This is the simple case for proportional text.  No justification,
;   no width vector.  Just run the string.  If we run out of stack
;   space, then that portion of the string which fits will be displayed,
;   and we'll restart again after that.
;
;   spcl - simple, proportional, clip lhs
;   sfcl - simple, fixed pitch,  clip lhs
;   sc   - simple, clip rhs
;
; Entry:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       AL     =  excel flags
;       AH     =  accel flags
;       CX     =  number of characters in the string
;       DI     =  current X position (and therefore starting x)
;       stack frame as per strblt
; Returns:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       CX     =  number of characters left in string
;       DI     =  string rhs
;       AX     =  next character's X
; Error Returns:
;       none
; Registers Preserved:
;       BP
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Calls:
;       None
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,FontSeg

non_justified_text proc near

        test    al,CLIPPED_LEFT
        jz      sc_no_left_clipping     ;No clipping needed on lhs
        mov     dx,clip.left            ;Characters become visible here
        test    ah,FIXED_PITCH
        jz      short spcl_next_char    ;Proportional font

;-----------------------------------------------------------------------;
;
;       Fixed pitch, no justification, left hand clipping
;
;-----------------------------------------------------------------------;
        mov     bx,lfd.font_width       ;Fixed pitch font.

sfcl_next_char:
        add     di,bx                   ;Does this character become visible?
        cmp     dx,di                   ;DX is clip.left
        jl      short sfcl_current_is_visible ;This char is visible
        inc     si
        loop    sfcl_next_char          ;See if next character

sfcl_no_chars_visible:
        jmp     build_ret_addr          ;Return to caller

sfcl_current_is_visible:
        sub     di,bx                   ;Restore staring address of character
                                        ;  and just fall into the proportional
                                        ;  code which will handle everything
;-----------------------------------------------------------------------;
;
;       Proportional, no justification, left hand clipping
;
;-----------------------------------------------------------------------;

spcl_next_char:
        lodsb
        sub     al,lfd.first_char
        cmp     al,lfd.last_char
        jbe     short spcl_good_character
        mov     al,lfd.default_char     ;Character was out of range

spcl_good_character:
        xor     ah,ah

;----------------------------------------------------------------------------;
; For version 2 fonts, the header has 2 byte pointer and entry size is       ;
; 4 per char.  For version 3 fonts, pointers are 4 byte and size of entry    ;
; is 6 bytes.                                                                ;
;----------------------------------------------------------------------------;
        shl     ax,1
        mov     bx,ax
        shl     ax,1
        add     bx,ax
        mov     ax,wptr fsCharOffset[bx][PROP_WIDTH]
        add     di,ax                   ;0 width chars won't change x position
        cmp     dx,di                   ;DX is clip.left
        jl      short spcl_current_is_visible ;This char is visible

spcl_see_if_next:
        loop    spcl_next_char          ;See if next character
        jmp     build_ret_addr          ;Return to caller

spcl_current_is_visible:
        sub     di,ax                   ;Restore starting x of character
        mov     ebx,dword ptr fsCharOffset[bx][PROP_OFFSET]
        add     bx,amt_clipped_on_top   ;Adjust pointer for any clipping

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx

;-----------------------------------------------------------------------;
;
;       Instead of incrementing the current position by 8 and
;       having to recover the real current position, we just
;       slide the clip region left.  It has the same effect.
;
;-----------------------------------------------------------------------;

        sub     dx,di                   ;Compute bits until we're visible
        je      short spcl_save_first ;Starts on clip edge
        sub     dx,8                    ;Is current byte visible?
        jl      short spcl_have_vis_start; Yes

spcl_step_clipped_char:
        sub     ax,8                    ;Shorten the width of the character
        add     di,8                    ;Move current position right
        add     bx,lfd.font_height      ;Move to next column of character

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx

        sub     dx,8                    ;Is current byte visible?
        jge     short spcl_step_clipped_char    ;  No


;-----------------------------------------------------------------------;
;
;       If the lhs of the clip region and the starting X of the
;       character are in different bytes, then the FIRST_IN_PREV
;       flag must be set.  Only a clipped character can set this
;       flag.
;
;-----------------------------------------------------------------------;

spcl_have_vis_start:
        mov     dx,clip.left
        xor     dx,di
        and     dx,not 7
        jz      short spcl_save_first ;In same byte
        or      excel,FIRST_IN_PREV


;-----------------------------------------------------------------------;
;
;       We have the start of the first character which is visible
;       Determine which loop (clipped/non-clipped) will process it.
;       We let the routine we're about to call push the character
;       since it will handle both right clipping (if needed) and
;       fat characters.
;
;-----------------------------------------------------------------------;

spcl_save_first:
        jmp     short scc_clip_enters_here


;-----------------------------------------------------------------------;
;
;       There was no left hand clipping.  Whenever this is the case,
;       we want to try and pad the lhs out to a byte boundary so that
;       full byte code can be used.
;
;-----------------------------------------------------------------------;

sc_no_left_clipping:
        call    pad_left_hand_side      ;Might be able to pad lhs
        jmp     short scc_next_char


;-----------------------------------------------------------------------;
;
;       scc - simple case, rhs clipping.
;
;       This loop is used when it is possible for the character
;       to be clipped on the rhs.  lhs clipping has already
;       been performed.  There is no justification.
;
;       Currently:
;               DS:SI --> current character in the string
;               ES:    =  font segment
;               DI     =  current X position
;               CX     =  number of bytes left in the string
;
;-----------------------------------------------------------------------;


scc_bad_char:
        mov     al,lfd.default_char     ;Character was out of range,
        jmp     short scc_good_char

scc_next_char:
        lodsb
        sub     al,lfd.first_char
        cmp     al,lfd.last_char
        ja      short scc_bad_char

scc_good_char:
        xor     ah,ah

;----------------------------------------------------------------------------;
; For version 2 fonts, the header has 2 byte pointer and entry size is       ;
; 4 per char.  For version 3 fonts, pointers are 4 byte and size of entry    ;
; is 6 bytes.                                                                ;
;----------------------------------------------------------------------------;
        shl     ax,1
        mov     bx,ax
        shl     ax,1
        add     bx,ax
        mov     ax,wptr fsCharOffset[bx][PROP_WIDTH]
        mov     ebx,dword ptr fsCharOffset[bx][PROP_OFFSET]
        or      ax,ax                   ;If width is 0, ignore character
        jz      short scc_see_if_next
        add     bx,amt_clipped_on_top   ;Adjust pointer for any clipping

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx

scc_clip_enters_here:
        mov     dx,di                   ;Compute phase
        and     dl,7
        add     di,ax                   ;DI = next char's X position
        cmp     di,clip.right
        jge     short scc_char_is_clipped       ;Clipped (or first pixel of
                                                ;next is)
scc_been_clipped:
        mov     dh,dl                   ;Need phase in DH
        cmp     ax,8                    ;If character is less than 8 bits
        jbe     short scc_width_ok      ;  wide, push it's data
        mov     dl,8                    ;Need 8 for size in DL

scc_still_wide:
        push    dx                      ;Push data showing phase,
        push    ebx                     ;  character is 8 wide, then
        sub     ax,8                    ;  create another character
        add     bx,lfd.font_height      ;  of the remaining width

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx
        cmp     ax,8
        ja      short scc_still_wide

scc_width_ok:
        mov     ah,dh
        push    ax                      ;Push data showing phase,
        push    ebx                     ;Save offset to bits
        cmp     sp,min_stack            ;Stack compare must be unsigned
        jb      short scc_restart       ;Not enough stack for another character

scc_see_if_next:
        loop    scc_next_char           ;Until all characters pushed
        mov     ax,di                   ;Next character starts here
        jmp     build_ret_addr


;-----------------------------------------------------------------------;
;
;       This character is either clipped, or it's last pixel is
;       the last pixel which will fit within the clipping rhs.
;       Adjust it's width so it fits, set the remaining character
;       count to 1 so the loop will terminate, and reenter the
;       code where we came from.
;
;-----------------------------------------------------------------------;

scc_char_is_clipped:
        mov     cx,clip.right           ;Compute number of pixels
        sub     di,cx                   ;  which have to be clipped
        sub     ax,di                   ;Set new character width
        mov     di,cx                   ;Set new rhs
        mov     cx,1                    ;Show this as last character
        jmp     scc_been_clipped        ;Finish here


;-----------------------------------------------------------------------;
;
;       These is no more space on the stack to build characters.
;       If this was the last character, then don't bother with the
;       restart.
;
;-----------------------------------------------------------------------;

scc_restart:
        dec     cx                      ;Adjust count for char just pushed
        mov     ax,di                   ;Next character starts here
        jmp     build_ret_addr

non_justified_text endp

;---------------------------Public-Routine------------------------------;
;
; justified_text
;
;   This is the justification case for text, when positive character
;   extra and/or a positive DDA are present.  If we run out of stack
;   space, then that portion of the string which fits will be displayed,
;   and we'll restart again after that.
;
;   jc  - justify clipped
;   jcl - justify clip left
;
; Entry:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       AL     =  excel flags
;       AH     =  accel flags
;       CX     =  number of characters in the string
;       DI     =  current X position (and therefore starting x)
;       stack frame as per strblt
; Returns:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       CX     =  number of characters left in string
;       DI     =  string rhs
;       AX     =  next character's X
; Error Returns:
;       none
; Registers Preserved:
;       BP
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Calls:
;
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,FontSeg

justified_text  proc    near

        test    excel,CLIPPED_LEFT
        jnz     short jcl_next_char     ;Clipping needed
        call    pad_left_hand_side      ;Might be able to pad lhs
        jmp     jc_next_char


jcl_bad_character:
        mov     al,lfd.default_char     ;Character was out of range
        jmp     short jcl_good_character


;-----------------------------------------------------------------------;
;
;       This is the code which runs the DDA to intersperse pixels
;       into the string
;
;       Compute the amount of white space that will be introduced by
;       this character.  This will be the sum of any character extra,
;       any break extra (if a break character), and dda interspersed
;       pixels (if a break character)
;
;-----------------------------------------------------------------------;

jcl_have_break_char:
        mov     bl,accel
        and     bl,DDA_NEEDED+HAVE_BRK_EXTRA
        jz      short jcl_have_tot_extra;Must have only been char extra
        add     dx,brk_extra            ;Extra every break (0 if none)
        test    bl,DDA_NEEDED
        jz      short jcl_have_tot_extra
        mov     bx,brk_err              ;The dda is required for this char
        sub     bx,brk_rem              ;  Run it and add in an extra pixel
        jg      short jcl_dont_distribute;  if needed.
        add     bx,brk_count            ;Add one pixel for the dda
        inc     dx

jcl_dont_distribute:
        mov     brk_err,bx              ;Save rem for next time
        jmp     short jcl_have_tot_extra

;-----------------------------------------------------------------------;
;
;       This is the code which computes the number of DDA interspersed
;       pixels to be added to the string
;
;       If all the extra pixels will fit on the end of this character,
;       just adjust it's width, otherwise a null character should be
;       created for the extra.
;
;-----------------------------------------------------------------------;
jcl_extra_pixels:
        mov     ax,wptr fsCharOffset[bx][PROP_WIDTH]
        neg     ax
        and     ax,7                    ;AX = # extra pixels which will fit
        jz      short jcl_have_entire_width ;None will fit
        cmp     ax,dx
        jl      short jcl_have_what_fits    ;Some extra pixels will not fit
        mov     ax,dx                   ;All pixels will fit, make DX = 0

jcl_have_what_fits:
        sub     dx,ax                   ;DX = extra for the dummy character

jcl_have_entire_width:
        add     ax,wptr fsCharOffset[bx][PROP_WIDTH]
        jmp     short jcl_have_width


;-----------------------------------------------------------------------;
;
;       This is the start of the real loop for left hand clipping.
;
;-----------------------------------------------------------------------;

jcl_next_char:
        lodsb
        sub     al,lfd.first_char
        cmp     al,lfd.last_char
        ja      short jcl_bad_character

jcl_good_character:
        mov     dx,char_xtra            ;Base amount of extra pixels needed
        cmp     cx,1                    ;Last character?
        jnz     short @f
        add     dx,fontweight           ;Add in final character overhang.
@@:
        cmp     al,lfd.break_char
        je      short jcl_have_break_char ;Go compute dda added pixels

jcl_have_tot_extra:
        xor     ah,ah

;----------------------------------------------------------------------------;
; For version 2 fonts, the header has 2 byte pointer and entry size is       ;
; 4 per char.  For version 3 fonts, pointers are 4 byte and size of entry    ;
; is 6 bytes.                                                                ;
;----------------------------------------------------------------------------;
        shl     ax,1
        mov     bx,ax
        shl     ax,1
        add     bx,ax
        or      dx,dx
        jnz     jcl_extra_pixels
        mov     ax,wptr fsCharOffset[bx][PROP_WIDTH]

jcl_have_width:
        add     di,ax                   ;DI = next chars starting X
        cmp     clip.left,di
        jl      short jcl_current_is_visible    ;This char is visible
        add     di,dx
        cmp     clip.left,di
        jl      short jcl_dummy_is_visible  ;Dummy is first visible character

jcl_see_if_next:
        loop    jcl_next_char           ;See if next character
        jmp     build_ret_addr          ;Return to caller

;-----------------------------------------------------------------------;
;
;       The dummy character is the first character which became
;       visible.  Just set the starting X to clip.left, and shorten
;       the width of the dummy character appropriately.
;
;-----------------------------------------------------------------------;

jcl_dummy_is_visible:
        mov     dx,di
        mov     di,clip.left            ;Starting X is clip.left
        sub     dx,di                   ;DX is # pixels in dummy
        xor     ax,ax                   ;Show no real character
        mov     ebx, null_char_offset   ;don't get fucked with invalid ebx

        jmp     short jcl_all_done


;-----------------------------------------------------------------------;
;
;       We just encountered the first character which will be visible
;       Clip it on the lhs as needed.
;
;-----------------------------------------------------------------------;

jcl_current_is_visible:
        sub     di,ax                   ;Restore starting x of character
        mov     ebx,dword ptr fsCharOffset[bx][PROP_OFFSET]
        add     bx,amt_clipped_on_top   ;Adjust pointer for any clipping

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx


;-----------------------------------------------------------------------;
;
;       Instead of incrementing the current position by 8 and
;       having to recover the real current position, we just
;       slide the clip region left.  It has the same effect.
;
;-----------------------------------------------------------------------;

        push    dx                      ;Save extra pixels
        mov     dx,clip.left
        sub     dx,di                   ;Compute bits until we're visible
        je      short jcl_save_first    ;Starts on clip edge
        sub     dx,fontweight           ;If bold, 9 pixels could be visible.
        sub     dx,8                    ;Is current byte visible?
        jl      short jcl_have_vis_start    ;  Yes

jcl_step_clipped_char:
        sub     ax,8                    ;Shorten the width of the character
        add     di,8                    ;Move current position right
        add     bx,lfd.font_height      ;Move to next column of character

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx

        sub     dx,8                    ;Is current byte visible?
        jge     short jcl_step_clipped_char ;  No

;-----------------------------------------------------------------------;
;
;       If the lhs of the clip region and the starting X of the
;       character are in different bytes, then the FIRST_IN_PREV
;       flag must be set.  Only a clipped character can set this
;       flag.
;
;-----------------------------------------------------------------------;

jcl_have_vis_start:
        mov     dx,clip.left
        xor     dx,di
        and     dx,not 7
        jz      short jcl_save_first    ;In same byte
        or      excel,FIRST_IN_PREV

;-----------------------------------------------------------------------;
;
;       We have the start of the first character which is visible
;       We let the routine we're about to call push the character
;       since it will handle both right clipping (if needed) and
;       fat characters.
;
;-----------------------------------------------------------------------;


jcl_save_first:
        pop     dx                      ;Restore extra pixels

jcl_all_done:
        jmp     short jc_clip_enters_here

;-----------------------------------------------------------------------;
;
;       jc - justified with clipping
;
;       This loop is used for justified text.  It will perform
;       rhs clipping.  lhs clipping has already been performed.
;
;       Currently:
;               DS:SI --> current character in the string
;               ES:    =  font segment
;               DI     =  current X position
;               CX     =  number of bytes left in the string
;
;-----------------------------------------------------------------------;

jc_bad_char:
        mov     al,lfd.default_char     ;Character was out of range,
        jmp     short jc_good_character

;-----------------------------------------------------------------------;
;
;       This is the code which runs the DDA to intersperse pixels
;       into the string
;
;       Compute the amount of white space that will be introduced by
;       this character.  This will be the sum of any character extra,
;       any break extra (if a break character), and dda interspersed
;       pixels (if a break character)
;
;-----------------------------------------------------------------------;
jc_have_break_char:
        mov     bl,accel
        and     bl,DDA_NEEDED+HAVE_BRK_EXTRA
        jz      short jc_have_tot_extra   ;Must have only been char extra
        add     dx,brk_extra            ;Extra every break (0 if none)
        test    bl,DDA_NEEDED
        jz      short jc_have_tot_extra
        mov     bx,brk_err              ;The dda is required for this char
        sub     bx,brk_rem              ;  Run it and add in an extra pixel
        jg      short jc_dont_distribute  ;  if needed.
        add     bx,brk_count            ;Add one pixel for the dda
        inc     dx

jc_dont_distribute:
        mov     brk_err,bx              ;Save rem for next time
        jmp     short jc_have_tot_extra

;-----------------------------------------------------------------------;
;
;       If all the extra pixels will fit on the end of this character,
;       just adjust it's width, otherwise a null character should be
;       created for the extra.
;
;-----------------------------------------------------------------------;

jc_extra_pixels:
        neg     ax
        and     ax,7                    ;AX = # extra pixels which will fit
        jz      short jc_have_entire_width  ;None will fit
        cmp     ax,dx
        jl      short jc_have_what_fits     ;Some extra pixels will not fit
        mov     ax,dx                   ;All pixels will fit, make DX = 0

jc_have_what_fits:
        sub     dx,ax                   ;DX = extra for the dummy character

jc_have_entire_width:
        add     ax,wptr fsCharOffset[bx][PROP_WIDTH]
        jmp     short jc_have_width


;-----------------------------------------------------------------------;
;
;       This is the start of the real loop
;
;-----------------------------------------------------------------------;

jc_next_char:
        lodsb
        sub     al,lfd.first_char
        cmp     al,lfd.last_char
        ja      short jc_bad_char

jc_good_character:
        mov     dx,char_xtra            ;Base amount of extra pixels needed
        cmp     cx,1                    ;Last character?
        jnz     short @f
        add     dx,fontweight           ;Add in final character overhang.
@@:
        cmp     al,lfd.break_char
        je      short jc_have_break_char    ;Go compute dda added pixels

jc_have_tot_extra:
        xor     ah,ah

;----------------------------------------------------------------------------;
; For version 2 fonts, the header has 2 byte pointer and entry size is       ;
; 4 per char.  For version 3 fonts, pointers are 4 byte and size of entry    ;
; is 6 bytes.                                                                ;
;----------------------------------------------------------------------------;
        shl     ax,1
        mov     bx,ax
        shl     ax,1
        add     bx,ax
        mov     ax,wptr fsCharOffset[bx][PROP_WIDTH]
        or      dx,dx
        jnz     short jc_extra_pixels ;Extra pixels required

jc_have_width:
        or      ax,ax
        jz      short jc_check_dummy    ;If width is 0, might still have dummy
        mov     ebx,dword ptr fsCharOffset[bx][PROP_OFFSET]
        add     bx,amt_clipped_on_top   ;Adjust pointer for any clipping

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx

jc_clip_enters_here:
        mov     num_null_pixels,dx      ;Save # null pixels
        mov     dx,di                   ;Compute phase
        and     dl,7
        add     di,ax                   ;DI = next char's X position
        cmp     di,clip.right
        jge     short jc_char_is_clipped  ;Clipped (or first pixel of next is)

jc_been_clipped:
        mov     dh,dl                   ;Need phase in DH
        mov     dl,8                    ;Need 8 for size in DL if fat
        cmp     ax,8                    ;If character is less than 8 bits
        jbe     short jc_width_ok       ;  wide, push it's data

jc_still_wide:
        push    dx                      ;Push data showing phase,
        push    ebx                     ;  character is 8 wide, then
        sub     ax,8                    ;  create another character
        add     bx,lfd.font_height      ;  of the remaining width

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx

        cmp     ax,8
        ja      short jc_still_wide

jc_width_ok:
        mov     ah,dh
        push    ax                      ;Push data showing phase,
        push    ebx                     ;Save offset to bits
        mov     dx,num_null_pixels

jc_check_dummy:
        or      dx,dx
        jz      short jc_see_if_next    ;No pixels for justification
        xchg    ax,dx                   ;Set ax = number of pixels to fill
        mov     ebx,null_char_offset
        mov     dx,di                   ;Compute phase
        and     dl,7
        add     di,ax                   ;DI = next char's X position
        cmp     di,clip.right
        jge     short jc_dummy_is_clipped ;Clipped (or first pixel of next is)

jc_dummys_been_clipped:
        mov     dh,dl                   ;Need phase in DH
        mov     dl,8                    ;Need 8 for size in DL if fat
        cmp     ax,8                    ;If dummy is less than 8 bits
        jbe     short jc_dummy_width_ok   ;  wide, push it's data

jc_dummy_still_wide:
        push    dx                      ;Push data showing phase,
        push    ebx                     ;  character is 8 wide, then
        sub     ax,8                    ;  create another character
        cmp     ax,8
        ja      short jc_dummy_still_wide

jc_dummy_width_ok:
        mov     ah,dh
        push    ax                      ;Push data showing phase,
        push    ebx                     ;Save offset to bits

jc_see_if_next:
        cmp     sp,min_stack            ;Stack compare must be unsigned
        jb      short jc_restart        ;Not enough stack for another character
        dec     cx
        jle     short jc_all_done
        jmp     jc_next_char            ;Until all characters pushed

jc_all_done:
        mov     ax,di                   ;Next character starts here
        jmp     build_ret_addr


;-----------------------------------------------------------------------;
;
;       This character is either clipped, or it's last pixel is
;       the last pixel which will fit within the clipping rhs.
;       Adjust it's width so it fits, set the remaining character
;       count to 1 so the loop will terminate, and reenter the
;       code where we came from.
;
;       Might as well set num_null_pixels to zero to skip that code.
;
;-----------------------------------------------------------------------;

jc_char_is_clipped:
        mov     cx,clip.right           ;Compute number of pixels
        sub     di,cx                   ;  which have to be clipped
        sub     ax,di                   ;Set new character width
        mov     di,cx                   ;Set new rhs
        xor     cx,cx                   ;Dont't want any extra pixels
        mov     num_null_pixels,cx
        inc     cx                      ;Show this as last character
        jmp     jc_been_clipped         ;Finish here


;-----------------------------------------------------------------------;
;
;       The dummy is either clipped, or it's last pixel is
;       the last pixel which will fit within the clipping rhs.
;       Adjust it's width so it fits, set the remaining character
;       count to 1 so the loop will terminate, and reenter the
;       code where we came from.
;
;-----------------------------------------------------------------------;

jc_dummy_is_clipped:
        mov     cx,clip.right           ;Compute number of pixels
        sub     di,cx                   ;  which have to be clipped
        sub     ax,di                   ;Set new character width
        mov     di,cx                   ;Set new rhs
        mov     cx,1                    ;Show this as last character
        jmp     jc_dummys_been_clipped  ;Finish here


;-----------------------------------------------------------------------;
;
;       These is no more space on the stack to build characters.
;       If this was the last character, then don't bother with the
;       restart.
;
;-----------------------------------------------------------------------;

jc_restart:
        dec     cx                      ;Adjust count for char just pushed
        mov     ax,di                   ;Next character starts here
        jmp     build_ret_addr

justified_text  endp

        page
;---------------------------Public-Routine------------------------------;
;
; worst_case
;
;   This is the worst case text code, when there is some combination
;   of the width vector, negative character extra, and negative dda.
;   If we run out of stack space, then whatever has been built up on
;   the stack will be displayed, and we'll restart again after that.
;
;   wcc - worse case clipped
;   wccl - worse case clip left
;
; Entry:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       AL     =  excel flags
;       AH     =  accel flags
;       CX     =  number of characters in the string
;       DI     =  current X position (and therefore starting x)
;       stack frame as per strblt
; Returns:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       CX     =  number of characters left in string
;       DI     =  string rhs
;       AX     =  next character's X
; Error Returns:
;       none
; Registers Preserved:
;       BP
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Calls:
;
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,FontSeg

worst_case      proc near
NLABEL  worst_case
        mov     RHSInk,di
        test    excel,CLIPPED_LEFT
        jnz     short wccl_next_char    ;Clipping needed
        call    pad_left_hand_side      ;Might be able to pad lhs
        mov     FirstStackEntryX,di
        jmp     wcc_next_char

;-----------------------------------------------------------------------;
;
;       Set current character to the default character
;
;-----------------------------------------------------------------------;
PLABEL wccl_bad_character
        mov     al,lfd.default_char
        jmp     short wccl_good_character


;-----------------------------------------------------------------------;
;
;       This runs the DDA to intersperse pixels into the string
;
;       Compute the adjustment to the character's width.  This will
;       be the sum of any character extra, any break extra (if a
;       break character), and dda interspersed pixels (if a break
;       character)
;
;       The dda must be capable of handling positive and negative
;       justification.  Character extra may be negative.
;
;-----------------------------------------------------------------------;
PLABEL wccl_have_break_char
        mov     bl,accel
        and     bl,DDA_NEEDED+HAVE_BRK_EXTRA
        jz      short wccl_have_tot_extra ;Must have only been char extra
        add     dx,brk_extra            ;Extra every break (0 if none)
        test    bl,DDA_NEEDED
        jz      short wccl_have_tot_extra

        mov     bx,brk_rem              ;If the dda is stepping left instead
        or      bx,bx                   ;  of stepping right, then brk_rem
        jl      short wccl_neg_dda      ;  will be negative
        sub     brk_err,bx              ;Run dda and add in an extra pixel
        jg      short wccl_have_tot_extra ;  if needed.
        mov     bx,brk_count
        add     brk_err,bx
        inc     dx                      ;Add one pixel for the dda
        jmp     short wccl_have_tot_extra

PLABEL wccl_neg_dda
        sub     brk_err,bx              ;Run dda and subtract an extra pixel
        jl      short wccl_have_tot_extra ;  if needed.
        mov     bx,brk_count
        sub     brk_err,bx              ;Subtract one pixel for the dda
        dec     dx
        jmp     short wccl_have_tot_extra

;-----------------------------------------------------------------------;
;
;       This is the start of the real loop for left hand clipping.
;
;-----------------------------------------------------------------------;
PLABEL wccl_next_char
        xor     eax,eax
        test    byte ptr es:[fsType],PF_GLYPH_INDEX
        jz      wccl_oldstylefont
        lodsw
        mov     dx,char_xtra
        cmp     cx, 1
        jnz     short @f
        add     dx,fontweight
@@:     jmp     short wccl_gifont_have_extra

PLABEL wccl_oldstylefont
        lodsb
        sub     al,lfd.first_char
        cmp     al,lfd.last_char
        ja      short wccl_bad_character

PLABEL wccl_good_character
        mov     dx,char_xtra            ;Base amount of extra pixels needed
        cmp     cx,1                    ;Last character?
        jnz     short @f
        add     dx,fontweight           ;Add in final character overhang.
@@:
        cmp     al,lfd.break_char
        je      short wccl_have_break_char  ;Go compute dda added pixels

PLABEL wccl_have_tot_extra
        xor     ah,ah

PLABEL wccl_gifont_have_extra
;----------------------------------------------------------------------------;
; For version 2 fonts, the header has 2 byte pointer and entry size is       ;
; 4 per char.  For version 3 fonts, pointers are 4 byte and size of entry    ;
; is 6 bytes.                                                                ;
;----------------------------------------------------------------------------;
        shl     eax,1
        mov     ebx,eax
        shl     eax,1
        add     ebx,eax
        mov     ax,wptr fsCharOffset[ebx][PROP_WIDTH]
        push    ebx                      ;Must save character index
        test    accel,HAVE_WIDTH_VECT
        jz      short wccl_have_width

PLABEL wccl_get_user_width
        cmp     cx,1
        je      short wccl_have_width

        les     bx,lp_dx
        assumes es,nothing
        add     dx,wptr es:[bx]         ;DX is delta to next char's start
        inc     bx
        inc     bx
        mov     off_lp_dx,bx
        mov     es,seg_lp_font
        assumes es,FontSeg
        sub     dx,ax                   ;Compute adjustment to width

PLABEL wccl_have_width
        pop     ebx                     ;Restore char index
        add     ax,di                   ;AX = rhs of character
        cmp     clip.left,ax
        jl      short wccl_current_is_visible ;Some part of char is visible

;-----------------------------------------------------------------------;
;
;       If the adjustment for the character width is greater than the
;       actual width of the character, then it is possible that the
;       dummy pixels could become visible.  If the adjustment for the
;       character width is less than the actual character width, then
;       the dummy pixel (negative dummy pixels?) cannot become visible.
;
;-----------------------------------------------------------------------;
        add     ax,dx                   ;Next char starts at AX
        mov     di,clip.left
        cmp     di,ax
        jl      short wccl_dummy_is_visible ;part of dummy char became visible
        xchg    di,ax                   ;Set start of next character

PLABEL wccl_see_if_next
        loop    wccl_next_char          ;See if next character
        jmp     build_ret_addr          ;Return to caller

;-----------------------------------------------------------------------;
;
;       The dummy character is the first character which became
;       visible.  Just set the starting X to clip.left, and shorten
;       the width of the dummy character appropriately.
;
;-----------------------------------------------------------------------;
PLABEL wccl_dummy_is_visible
        xchg    ax,dx                   ;Set DX = # pixels in dummy
        sub     dx,di
        xor     ax,ax                   ;Show no real character
        jmp     short wccl_all_done

;-----------------------------------------------------------------------;
;
;       So here we are, we have a character which will become visible,
;       and possibly have some adjustment to the character.
;
;       Our registers currently contain:
;
;               AX = rhs of character
;               BX = index into offset/width table
;               CX = # of characters left in the string
;               DX = # of extra pixels (zero or positive)
;               DI = starting X offset
;               ES = FontSeg
;               DS:SI --> string
;
;-----------------------------------------------------------------------;
PLABEL wccl_current_is_visible
        sub     ax,di                   ;Restore width of the character
        mov     ebx,dword ptr fsCharOffset[ebx][PROP_OFFSET]
        add     bx,amt_clipped_on_top   ;Adjust pointer for any clipping

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx

;-----------------------------------------------------------------------;
;
;       Instead of incrementing the current position by 8 and
;       having to recover the real current position, we just
;       slide the clip region left.  It has the same effect.
;
;-----------------------------------------------------------------------;
        push    dx                      ;Save extra pixels to be added
        mov     dx,clip.left
        sub     dx,di                   ;Compute bits until we're visible
        je      short wccl_save_first   ;Starts on clip edge
        sub     dx,fontweight           ;If bold, 9 pixels could be visible.
        sub     dx,8                    ;Is current byte visible?
        jl      short wccl_have_vis_start; Yes

PLABEL wccl_step_clipped_char
        sub     ax,8                    ;Shorten the width of the character
        add     di,8                    ;Move current position right
        add     bx,lfd.font_height      ;Move to next column of character

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx

        sub     dx,8                    ;Is current byte visible?
        jge     short wccl_step_clipped_char    ; No


;-----------------------------------------------------------------------;
;
;       If the lhs of the clip region and the starting X of the
;       character are in different bytes, then the FIRST_IN_PREV
;       flag must be set.  Only a clipped character can set this
;       flag.
;
;-----------------------------------------------------------------------;
PLABEL wccl_have_vis_start
        mov     dx,clip.left
        xor     dx,di
        and     dx,not 7
        jz      short wccl_save_first ;In same byte
        or      excel,FIRST_IN_PREV

;-----------------------------------------------------------------------;
;
;       We have the start of the first character which is visible.
;       We let the routine we're about to call push the character
;       since it will handle both right clipping (if needed) and
;       fat characters.
;
;-----------------------------------------------------------------------;
PLABEL wccl_save_first
        pop     dx                      ;Restore extra pixels

PLABEL wccl_all_done
        mov     FirstStackEntryX,di
        jmp     wcc_clip_enters_here


;-----------------------------------------------------------------------;
;
;       wcc - worse case, with rhs clipping
;
;       Currently:
;               DS:SI --> current character in the string
;               ES:    =  font segment
;               DI     =  current X position
;               CX     =  number of bytes left in the string
;
;-----------------------------------------------------------------------;
PLABEL wcc_bad_char
        mov     al,lfd.default_char     ;Character was out of range,
        jmp     short wcc_good_character


;-----------------------------------------------------------------------;
;
;       This is the code which runs the DDA to intersperse pixels
;       into the string
;
;       Compute the adjustment to the character's width.  This will
;       be the sum of any character extra, any break extra (if a
;       break character), and dda interspersed pixels (if a break
;       character)
;
;       The dda must be capable of handling positive and negative
;       justification.  Character extra may be negative.
;
;-----------------------------------------------------------------------;
PLABEL wcc_have_break_char
        mov     bl,accel
        and     bl,DDA_NEEDED+HAVE_BRK_EXTRA
        jz      short wcc_have_tot_extra  ;Must have only been char extra
        add     dx,brk_extra            ;Extra every break (0 if none)
        test    bl,DDA_NEEDED
        jz      short wcc_have_tot_extra

        mov     bx,brk_rem              ;If the dda is stepping left instead
        or      bx,bx                   ;  of stepping right, then brk_rem
        jl      short wcc_neg_dda       ;  will be negative
        sub     brk_err,bx              ;Run dda and add in an extra pixel
        jg      short wcc_have_tot_extra;  if needed.
        mov     bx,brk_count
        add     brk_err,bx
        inc     dx                      ;Add one pixel for the dda
        jmp     short wcc_have_tot_extra

PLABEL wcc_neg_dda
        sub     brk_err,bx              ;Run dda and subtract an extra pixel
        jl      short wcc_have_tot_extra;  if needed.
        mov     bx,brk_count
        sub     brk_err,bx
        dec     dx                      ;Subtract one pixel for the dda
        jmp     short wcc_have_tot_extra


;-----------------------------------------------------------------------;
;
;       This is the start of the real loop for right hand clipping.
;
;-----------------------------------------------------------------------;
PLABEL wcc_next_char
        xor     eax,eax
        test    byte ptr es:[fsType],PF_GLYPH_INDEX
        jz      wcc_oldstylefont
        lodsw
        mov     dx,char_xtra
        cmp     cx,1
        jnz     short @f
        add     dx,fontweight
@@:     jmp     short wcc_gifont_have_extra

PLABEL wcc_oldstylefont
        lodsb
        sub     al,lfd.first_char
        cmp     al,lfd.last_char
        ja      short wcc_bad_char

PLABEL wcc_good_character
        mov     dx,char_xtra            ;Base amount of extra pixels needed
        cmp     cx,1                    ;Last character?
        jnz     short @f
        add     dx,fontweight           ;Add in final character overhang.
@@:
        cmp     al,lfd.break_char
        je      short wcc_have_break_char ;Go compute dda added pixels

PLABEL wcc_have_tot_extra
        xor     ah,ah

PLABEL wcc_gifont_have_extra
;----------------------------------------------------------------------------;
; For version 2 fonts, the header has 2 byte pointer and entry size is       ;
; 4 per char.  For version 3 fonts, pointers are 4 byte and size of entry    ;
; is 6 bytes.                                                                ;
;----------------------------------------------------------------------------;
        shl     eax,1
        mov     ebx,eax
        shl     eax,1
        add     ebx,eax
        mov     ax,wptr fsCharOffset[ebx][PROP_WIDTH]
        mov     ebx,dword ptr fsCharOffset[ebx][PROP_OFFSET]
        add     bx,amt_clipped_on_top   ;Adjust pointer for any clipping

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx
        test    accel,HAVE_WIDTH_VECT
        jz      short wcc_have_width

PLABEL wcc_get_users_width
        cmp     cx,1
        je      short wcc_have_width

        push    bx
        les     bx,lp_dx
        assumes es,nothing
        add     dx,wptr es:[bx]         ;DX is delta to next char's start
        inc     bx
        inc     bx
        mov     off_lp_dx,bx
        mov     es,seg_lp_font
        assumes es,FontSeg
        sub     dx,ax                   ;Compute adjustment to width
        pop     bx

PLABEL wcc_have_width
PLABEL wcc_clip_enters_here
        or      ax,ax                   ;If character width is 0,
        jz      short wcc_check_dummy   ;  might still have dummy char
        add     di,ax
        cmp     RHSInk,di
        ja      short @f
        mov     RHSInk,di
@@:     sub     di,ax
        or      dx,dx                   ;Any adjustment to the width?
        jle     short wcc_have_adj_width;No extra pixels to add
        push    bx
        mov     bx,ax                   ;  into the empty space of the
        neg     ax                      ;  character
        and     ax,7                    ;AX = # extra pixels which will fit
        jz      short wcc_have_entire_width ;None will fit
        cmp     ax,dx
        jl      short wcc_have_what_fits  ;Some extra pixels will not fit
        mov     ax,dx                   ;All pixels will fit, make DX = 0

PLABEL wcc_have_what_fits
        sub     dx,ax                   ;DX = extra for the dummy character

PLABEL wcc_have_entire_width
        add     ax,bx                   ;Set number of pixels to use in char
        pop     bx

PLABEL wcc_have_adj_width
        mov     num_null_pixels,dx      ;Save number of dummy pixels
        mov     dx,di                   ;Compute phase
        and     dl,7
        add     di,ax                   ;DI = next char's X position
        cmp     di,clip.right
        jge     wcc_char_is_clipped     ;Clipped (or first pixel of next is)

PLABEL wcc_been_clipped
        mov     dh,dl                   ;Need phase in DH
        mov     dl,8                    ;Need 8 for size in DL if fat
        cmp     ax,8                    ;If character is less than 8 bits
        jbe     short wcc_width_ok      ;  wide, push it's data

PLABEL wcc_still_wide
        push    dx                      ;Push data showing phase,
        push    ebx                     ;  character is 8 wide, then
        sub     ax,8                    ;  create another character
        add     bx,lfd.font_height      ;  of the remaining width

; the following macro will add 10000h to EBX if carry is set above
        updc_ebx
        cmp     ax,8
        ja      short wcc_still_wide

PLABEL wcc_width_ok
        mov     ah,dh
        push    ax                      ;Push data showing phase,

        push    ebx                     ;Save offset to bits
        mov     dx,num_null_pixels

PLABEL wcc_check_dummy
        xchg    ax,dx                   ;Just incase we go backwards
        or      ax,ax
        jz      short wcc_see_if_next   ;No pixels for justification
        jl      wcc_going_back
        mov     ebx,null_char_offset
        mov     dx,di                   ;Compute phase
        and     dl,7
        add     di,ax                   ;DI = next char's X position
        cmp     di,clip.right
        jge     short wcc_dummy_is_clipped  ;Clipped (or first pixel of next is)

PLABEL wcc_dummys_been_clipped
        mov     dh,dl                   ;Need phase in DH
        mov     dl,8                    ;Need 8 for size in DL if fat
        cmp     ax,8                    ;If dummy is less than 8 bits
        jbe     short wcc_dummy_width_ok;  wide, push it's data

PLABEL wcc_dummy_still_wide
        push    dx                      ;Push data showing phase,
        push    ebx                     ;  character is 8 wide, then
        sub     ax,8                    ;  create another character
        cmp     ax,8
        ja      short wcc_dummy_still_wide

PLABEL wcc_dummy_width_ok
        mov     ah,dh
        push    ax                      ;Save width and phase
        push    ebx                     ;Save offset to bits

PLABEL wcc_see_if_next
        cmp     sp,min_stack            ;Stack compare must be unsigned
        jb      wcc_restart_10          ;Not enough stack for another character
        dec     cx
        jle     short wcc_all_done
        jmp     wcc_next_char           ;Until all characters pushed

;-----------------------------------------------------------------------
; Some heinous jump vectors.....
;-----------------------------------------------------------------------
wcc_restart_10:
        jmp     wcc_restart

PLABEL wcc_all_done
        mov     ax,RHSInk               ;clip the RHSInk to the clip rect.
        mov     bx,clip.right
        sub     bx,ax
        jg      short @f
        add     ax,bx
@@:     sub     ax,di                   ;If x pos < RHSInk, then
        jg      short @f                ; go pad with null pixels.
        mov     ax,di                   ;Next character starts here
        jmp     build_ret_addr
@@:     jmp     wcc_pad_up_to_rhs

;-----------------------------------------------------------------------;
;       The dummy is either clipped, or it's last pixel is
;       the last pixel which will fit within the clipping rhs.
;       Adjust it's width so it fits, set the remaining character
;       count to 1 so the loop will terminate, and reenter the
;       code where we came from.
;-----------------------------------------------------------------------;
PLABEL wcc_dummy_is_clipped
        mov     cx,clip.right           ;Compute number of pixels
        sub     di,cx                   ;  which have to be clipped
        sub     ax,di                   ;Set new character width
        mov     di,cx                   ;Set new rhs
        mov     cx,1                    ;Show this as last character
        jmp     wcc_dummys_been_clipped ;Finish here

;-----------------------------------------------------------------------;
;
;       This character is either clipped, or it's last pixel is
;       the last pixel which will fit within the clipping rhs.
;
;       If there is a  negative adjustment to the width of the
;       character, it is possible that the next character could
;       be partially visible.  We'll have to go through the backup
;       in this is the case.
;
;       If this is the last character of the string, then there
;       is no problem.
;
;       If no negative adjustment, adjust it's width so it fits,
;       set the remaining character count to 1 so the loop will
;       terminate, and reenter the code where we came from.
;
;-----------------------------------------------------------------------;
PLABEL wcc_char_is_clipped
        push    dx                      ;If num_null_pixels < 0, then
        mov     dx,num_null_pixels      ;  a restart might be possible
        or      dx,dx
        jl      short wcc_might_backup  ; Might need to backup.

PLABEL wcc_clipped_no_backup
        mov     cx,clip.right           ;Compute number of pixels
        sub     di,cx                   ;  which have to be clipped
        sub     ax,di                   ;Set new character width
        mov     di,cx                   ;Set new rhs
        xor     cx,cx                   ;Don't want any extra pixels
        mov     num_null_pixels,cx
        inc     cx                      ;Show this as last character
        pop     dx
        jmp     wcc_been_clipped        ;Finish here


;-----------------------------------------------------------------------;
;
;       Compute where the next character would start, and if it is
;       to the left of clip.right, then a backup is necessary.
;
;-----------------------------------------------------------------------;

PLABEL wcc_might_backup
        cmp     cx,1                    ;If last character
        jle     short wcc_clipped_no_backup ;  then no restart needed
        add     dx,di                   ;Compute next starting x
        sub     dx,clip.right
        jge     short wcc_clipped_no_backup ;Rest of string is clipped

;-----------------------------------------------------------------------;
;
;       Will have to go through the backup code.  We
;       can do this by computing the number of pixels between
;       where the next character starts and clip.right.  This negative
;       number will be stuffed into num_null_pixels, so when we reenter
;       the main loop, we'll force a backup after pushing the character
;       data.
;
;-----------------------------------------------------------------------;
        mov     num_null_pixels,dx
        mov     dx,clip.right           ;Compute number of pixels
        sub     di,dx                   ;  which have to be clipped
        sub     ax,di                   ;Set new character width
        mov     di,dx                   ;Set new rhs
        pop     dx
        jmp     wcc_been_clipped        ;Finish here


;-----------------------------------------------------------------------;
;       Limit all backward jumps to >= text_bbox.left (i.e., no
;       characters will be allowed to appear to the right of the
;       x coordinate).  This rule is preserved from the 3.0 version of
;       the driver.
;
;       If there is a backward jump that puts us to the left of the
;       left clipping boundary, then we will restart like the 3.0
;       driver.
;-----------------------------------------------------------------------;
PLABEL wcc_going_back
        cmp     cx,1                    ;If last character
        je      wcc_see_if_next         ; don't backup.
        mov     dx,di                   ;
        add     di,ax                   ;DI = next char's X position
PLABEL wcc_check_string_origin
        cmp     di,FirstStackEntryX     ;Don't allow backward jmp
        jl      short wcc_must_restart  ;before the 1st stack entry.
        and     dx,7                    ;Compute phase
        or      dx,8000h                ;Make it into backup sentinel.
        push    dx                      ;80h | phase
        push    ax                      ;# of pixels to backup.
        inc     nBackups                ;Show that backup tokens exist
                                        ;on the stack.
        jmp     wcc_see_if_next         ;Continue processing string.

PLABEL wcc_must_restart
        dec     cx                      ;dec string count.
        mov     bx,di
        sub     di,ax                   ;di=rhs string.
        mov     ax,bx                   ;ax=next char's start position.
        test    accel,IS_OPAQUE         ;If we're TRANSPARENT, then
        jz      short @f                ;restart normally.
        pusha
        call    prepare_for_overlap
        popa
        and     accel,not IS_OPAQUE     ;We are now transparent.
@@:     jmp     build_ret_addr

;-----------------------------------------------------------------------;
;
;       Out of stack space.  Set up for a restart.
;
;-----------------------------------------------------------------------;
PLABEL wcc_restart
        mov     ax,di
        dec     cx                      ;Adjust count for char just pushed
        jmp     build_ret_addr

;-----------------------------------------------------------------------;
; If the last character in the string totally preceeds the rhs of the
; string, then it is necessary to pad out to the rhs with blanks.  A
; good example of this is an italic 'f' followed by a blank character.
; The 'f' has a very small b-space and negative c-space, which means that
; the blank will start within the 'f' character.  Since the
; blank character is 1 pel wide, it won't extend beyond the black box
; rhs of the 'f'.  If we don't pad with null pixels, then the output code
; will think that the string ends at the rhs of the blank and part of
; the 'f' will not be drawn.
;-----------------------------------------------------------------------;
PLABEL  wcc_pad_up_to_rhs
        mov     ebx,null_char_offset
;-----------------------------------------------------------------------;
; There is no need to add amt_clipped_on_top to the ptr of the null
; character since the null character is always blank.
;-----------------------------------------------------------------------;
        mov     dx,di                   ;Compute phase
        and     dl,7
        add     di,ax                   ;DI = next char's X position
        mov     dh,dl                   ;Need phase in DH
        mov     dl,8                    ;Need 8 for size in DL if fat
        cmp     ax,8                    ;If padding is less than 8 bits
        jbe     short wcc_pad_width_ok;  wide, push it's data

PLABEL wcc_pad_still_wide
        push    dx                      ;Push data showing phase,
        push    ebx                     ;  character is 8 wide, then
        sub     ax,8                    ;  create another character
        cmp     ax,8
        ja      short wcc_pad_still_wide

PLABEL wcc_pad_width_ok
        mov     ah,dh
        push    ax                      ;Save width and phase
        push    ebx                     ;Save offset to bits
        mov     ax,di                   ;Next character starts here
        jmp     build_ret_addr


worst_case      endp


;--------------------------Private-Routine------------------------------;
; pad_left_hand_side
;
;   This routine is called when the text string isn't clipped on the
;   left side.  It attempts to pad the character with 0's if at all
;   possible.
;
;   If we can pad to the left of the first character with 0's, then
;   we can use the full byte code, which is many times faster than
;   the partial byte code.
;
;   If we do pad, we must update both current_lhs and the starting
;   X coordinate which will be used by the main loop code.
;
; Entry:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       CX     =  number of characters in the string
;       DI     =  current X position (and therefore starting x)
; Returns:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       CX     =  number of characters in the string
;       DI     =  current X position
; Error Returns:
;       None
; Registers Preserved:
;       CX,SI,DI,DS,ES,FLAGS
; Registers Destroyed:
;       AX,BX,DX
; Calls:
;       None
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,FontSeg

pad_left_hand_side      proc near
        mov     ax,di                   ;Get starting x coordinate
        and     ax,7                    ;Address MOD 8 is amount to pad
        jz      short plhs_all_done     ;At byte boundary, don't need padding


;       If we're in transparent mode, we can always create a dummy
;       character of 0's for the left side to avoid any left clipping

        test    accel,IS_OPAQUE         ;If in transparent mode, we can
        jz      short plhs_add_padding  ;  always add the padding


;       In opaque mode.  If there is an opaquing rectangle, try and pad
;       the left side up to the byte boundary. If we cannot make it to
;       the byte boundary, go as far as we can so that the opaquing code
;       can skip the byte.
;
;       If there isn't an opaque rectangle, we cannot do any padding.

        mov     bl,excel
        not     bl
        test    bl,OPAQUE_RECT+BOUNDED_IN_Y
        jnz     short plhs_all_done     ;Cannot add any padding
        mov     bx,di
        sub     bx,o_rect.left
        jle     short plhs_all_done     ;Cannot add any.  Darn
        min_ax  bx                      ;Set AX = number of bits to pad


;       Some number of 0's can be added to the left of the character.
;       Add them, then move the lhs left by that amount.

plhs_add_padding:
        mov     dx,di                   ;DI = start x = text_bbox.left
        sub     dx,ax                   ;Set new lhs of text bounding box
        mov     current_lhs,dx
        mov     ah,dl                   ;Set phase (x coordinate) of char
        and     ah,7
        pop     dx
        push    ax                      ;Width and phase of null character
        push    null_char_offset        ;Offset in font of null character
        jmp     dx

plhs_all_done:
        ret

pad_left_hand_side endp

        page
;--------------------------Private-Routine------------------------------;
; pad_right_hand_side
;
;   This routine is called once the text string has been pushed onto
;   the stack.  It pads the character on the right up to a byte boundary
;   if possible.
;
;   We always pad out to a byte boundary with 0 since it makes the
;   stopping condition in the driving loop simpler since it never
;   has to check to see if it has any more bytes of data left, it
;   knows it does.  It just checks after each destination column
;   has been output to see if anything else exists.
;
;   The clipping mask will be computed to the last pixel we can
;   alter.  In opaque mode, this depends on the opaquing rectangle.
;
; Entry:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       DI     =  X position where next char would have gone
; Returns:
;       DS:SI --> current character in the string
;       ES:    =  font segment
;       DI     =  rhs of the string, padded to boundary if possible
; Error Returns:
;       'C' set if no interval to output
; Registers Preserved:
;       AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Registers Destroyed:
;       AX,BX,DX
; Calls:
;       comp_byte_interval
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,FontSeg

pad_right_hand_side     proc near
        mov     ax,di                   ;If next char would start at bit 0,
        and     al,7                    ;  then no padding is required.
        jz      short prhs_have_rhs     ;No padding needed
        mov     ah,al                   ;Save phase
        neg     al                      ;Set width needed
        and     al,7
        pop     bx                      ;save return address in BX
        push    ax                      ;Width and phase of null character
        push    null_char_offset        ;Offset in font of null character
        push    bx                      ;put return address back on stack
        xor     ah,ah
        test    accel,IS_OPAQUE
        jz      short prhs_have_rhs     ;Can't pad for transparent text.

prhs_in_opaque_mode:
        mov     bl,excel
        not     bl
        test    bl,OPAQUE_RECT+BOUNDED_IN_Y
        jnz     short prhs_have_rhs     ;Cannot alter actual rhs
        mov     bx,o_rect.right         ;Compute distance from where I am to
        sub     bx,di                   ;  where opaque rectangle ends
        jle     short prhs_have_rhs     ;Opaque rect is left of text end
        min_ax  bx                      ;Compute amount to move rhs
prhs_new_rhs:
        add     di,ax                   ;Slide rhs right

prhs_have_rhs:
        ret

pad_right_hand_side     endp

;---------------------------Public-Routine------------------------------;
;
; prepare_for_overlap
;
;   Possible negative justification and/or width vector.  If
;   opaque mode, then compute the extents of the string so that
;   the bounding box can be output if we step backwards.  If we
;   will step backwards, opaque the area where the string will
;   go and set transparent mode for the actual text output routine.
;
; Entry:
;       bl = accel
; Returns:
;       None
; Error Returns:
;       to build_all_done if nothing will show
; Registers Destroyed:
;       AX,BX,CX,DX,SI,DI,DS,ES
; Registers Preserved:
;       None
; Calls:
;       worst_case_ext
;       output_o_rect
;-----------------------------------------------------------------------;
assumes ds,nothing
assumes es,nothing

prepare_for_overlap proc near
        push    brk_err                 ;Must not destroy the justification
        push    brk_count               ;  DDA parameters while we find out
        push    brk_rem                 ;  how long the string is and if
        push    off_lp_dx               ;  we stepped backwards
        mov     cx,count                ;  we stepped backwards
        call    worst_case_ext_386
        pop     off_lp_dx
        pop     brk_rem
        pop     brk_count
        pop     brk_err

;-----------------------------------------------------------------------;
;
;       Opaque the area where the string will go.  This area will
;       have to be clipped.
;
;-----------------------------------------------------------------------;
        mov     ax,clip.right
        mov     cx,x                    ;CX = lhs
        add     bx,cx                   ;BX = rhs
        jo      short pfo_have_rhs      ;Use clip.right for right side
        min_ax  bx

pfo_have_rhs:
        xchg    ax,bx                   ;Need rhs in BX
        mov     ax,clip.left
        max_ax  cx
        cmp     bx,ax
        stc                             ;JGE is SF = OF, doesn't use carry!
        jle     build_all_done		;Null or negative interval

;-----------------------------------------------------------------------;
;
;       The interval will show.  Dummy this up as a call to the
;       opaque rectangle code to output the bounding box.  Since
;       TEXT_VISIBLE was clear bu build_string, the opaque code
;       will not perform an intersection of text_bbox and o_rect.
;
;-----------------------------------------------------------------------;
        mov     wc_opaque_lhs,ax        ;Save lhs/rhs incase we have an
        mov     wc_opaque_rhs,bx        ;  opaquing rectangle
        or      wc_flags,WC_SET_LR      ;Set left/right into text bbox
        push    o_rect.left             ;Save real o_rect bbox
        push    o_rect.right
        push    o_rect.top
        push    o_rect.bottom
        mov     cx,text_bbox.top
        mov     dx,text_bbox.bottom
        mov     o_rect.left,ax          ;Set text bbox as area to opaque
        mov     o_rect.right,bx
        mov     o_rect.top,cx
        mov     o_rect.bottom,dx
        mov     bl,excel
        call    output_o_rect_386
        pop     o_rect.bottom
        pop     o_rect.top
        pop     o_rect.right
        pop     o_rect.left

        and     accel,not IS_OPAQUE     ;Will output text in transparent mode

pfo_exit:
        ret

prepare_for_overlap endp

if      0
        public  real_build_string
        public  build_worst_ok
        public  build_have_proc
        public  build_restart
        public  build_ret_addr
        public  build_no_o_rect
        public  build_clip_next_time
        public  build_no_clip_next_time
        public  build_all_done
        public  build_restore_opaque
        public  build_really_done
        public  non_justified_text
        public  sfcl_next_char
        public  sfcl_no_chars_visible
        public  sfcl_current_is_visible
        public  spcl_next_char
        public  spcl_good_character
        public  spcl_see_if_next
        public  spcl_current_is_visible
        public  spcl_step_clipped_char
        public  spcl_have_vis_start
        public  spcl_save_first
        public  sc_no_left_clipping
        public  scc_bad_char
        public  scc_next_char
        public  scc_good_char
        public  scc_clip_enters_here
        public  scc_been_clipped
        public  scc_still_wide
        public  scc_width_ok
        public  scc_see_if_next
        public  scc_char_is_clipped
        public  scc_restart
        public  justified_text
        public  jcl_bad_character
        public  jcl_have_break_char
        public  jcl_dont_distribute
        public  jcl_extra_pixels
        public  jcl_have_what_fits
        public  jcl_have_entire_width
        public  jcl_next_char
        public  jcl_good_character
        public  jcl_have_tot_extra
        public  jcl_have_width
        public  jcl_see_if_next
        public  jcl_dummy_is_visible
        public  jcl_current_is_visible
        public  jcl_step_clipped_char
        public  jcl_have_vis_start
        public  jcl_save_first
        public  jcl_all_done
        public  jc_bad_char
        public  jc_have_break_char
        public  jc_dont_distribute
        public  jc_extra_pixels
        public  jc_have_what_fits
        public  jc_have_entire_width
        public  jc_next_char
        public  jc_good_character
        public  jc_have_tot_extra
        public  jc_have_width
        public  jc_clip_enters_here
        public  jc_been_clipped
        public  jc_still_wide
        public  jc_width_ok
        public  jc_check_dummy
        public  jc_dummys_been_clipped
        public  jc_dummy_still_wide
        public  jc_dummy_width_ok
        public  jc_see_if_next
        public  jc_all_done
        public  jc_char_is_clipped
        public  jc_dummy_is_clipped
        public  jc_restart
        public  worst_case
        public  wccl_bad_character
        public  wccl_have_break_char
        public  wccl_neg_dda
        public  wccl_next_char
        public  wccl_good_character
        public  wccl_have_tot_extra
        public  wccl_get_user_width
        public  wccl_have_width
        public  wccl_adjustment_ok
        public  wccl_see_if_next
        public  wccl_dummy_is_visible
        public  wccl_current_is_visible
        public  wccl_step_clipped_char
        public  wccl_have_vis_start
        public  wccl_save_first
        public  wccl_all_done
        public  wcc_bad_char
        public  wcc_have_break_char
        public  wcc_neg_dda
        public  wcc_next_char
        public  wcc_good_character
        public  wcc_have_tot_extra
        public  wcc_get_users_width
        public  wcc_have_width
        public  wcc_adj_is_ok
        public  wcc_clip_enters_here
        public  wcc_have_what_fits
        public  wcc_have_entire_width
        public  wcc_have_adj_width
        public  wcc_been_clipped
        public  wcc_still_wide
        public  wcc_width_ok
        public  wcc_check_dummy
        public  wcc_dummys_been_clipped
        public  wcc_dummy_still_wide
        public  wcc_dummy_width_ok
        public  wcc_see_if_next
        public  wcc_all_done
        public  wcc_dummy_is_clipped
        public  wcc_char_is_clipped
        public  wcc_clipped_no_restart
        public  wcc_might_need_restart
        public  wcc_going_back
        public  wcc_restart
        public  true_type_text
        public  pad_left_hand_side
        public  plhs_add_padding
        public  plhs_all_done
        public  pad_right_hand_side
        public  prhs_have_rhs
        public  process_stack_data
        public  psd_pre_proc
        public  psd_pp_have_first
        public  psd_pp_ega_trans
        public  psd_pp_done
        public  psd_not_clipped
        public  psd_collect_chars
        public  psd_have_exact_fit
        public  psd_have_more_than_enough
        public  psd_unlikely_cases
        public  psd_see_about_last
        public  psd_exit
        public  prepare_for_overlap
        public  pfo_have_rhs
        public  pfo_exit
        public  pfo_exit_nothing_shows
endif
sEnd    pCode
        end
