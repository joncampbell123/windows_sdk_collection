// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "afx.h"
#ifdef _WINDOWS
#include "afxwin.h"
#endif
#pragma hdrstop

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#pragma optimize("qgel", off)
#pragma warning(disable:4100)

extern "C" BOOL FAR PASCAL
AfxIsValidAddress(const void FAR* lp, UINT nBytes, BOOL bReadWrite /* = TRUE */)
{
#ifdef _NTWIN
	// simple version using Win-32 APIs for pointer validation.
	return (lp != NULL && !IsBadReadPtr(lp, nBytes) &&
		(!bReadWrite || !IsBadWritePtr((LPVOID)lp, nBytes)));
#else
	BOOL bValid = FALSE;
	
#ifdef _WINDOWS
	BOOL b286 = (WF_CPU286 & GetWinFlags()) ? TRUE : FALSE;
	
if ( !b286 )
{
		
	_asm {
			_asm _emit  0x66
			xor     bx, bx                  ; clear out ebx
			mov     bx, WORD PTR lp + 2
			or      bx,bx
			jz      done                    ; zero segments are always bad
			_asm _emit 0x0F                 ; lar cx, bx
			_asm _emit 0x02
			_asm _emit 0xCB                     
			jnz     done

			test    ch,128                  ; if (!present(bx))
			jz      skip_range_test         ;   return fTrue;

			cmp     bReadWrite, 0
			je      skip_write_test

			_asm _emit 0x0F                 ; verw bx
			_asm _emit 0x00
			_asm _emit 0xEB
											; if (!writeable(bx))
			jnz     done

	skip_write_test:

			_asm _emit  0x66
			_asm _emit  0x0F                ; lsl, edx, bx
			_asm _emit  0x03
			_asm _emit  0xD3
			jnz     done
											;* dx = last valid address

			mov     bx, WORD PTR lp
			mov     cx, nBytes
			jcxz    skip_range_test         ; if length == 0, ignore offset part

			;* make sure start address is valid
			_asm _emit  0x66
			cmp     bx,dx                   ;  if (offsetof(lp) > dx)
			ja      done

			;* make sure last address is also valid
			dec     cx                      ; bias for last byte
			add     bx,cx
			jc      done                    ; overflow => no good
			_asm _emit  0x66
			cmp     dx,bx                   ;  if (offsetof(lp) + nBytes <= dx)
			jb      done
	skip_range_test:
			inc     bValid
	done:
	}
}
else
#endif  // !_WINDOWS
	_asm {
			mov     bx, WORD PTR lp + 2
			or      bx,bx
			jz      done2                   ; zero segments are always bad
#ifndef _DOS
			_asm _emit 0x0F                 ; lar cx, bx
			_asm _emit 0x02
			_asm _emit 0xCB                     
			jnz     done2

			test    ch,128                  ; if (!present(bx))
			jz      skip_range_test2        ;   return fTrue;

			cmp     bReadWrite, 0
			je      skip_write_test2

			_asm _emit 0x0F                 ; verw bx
			_asm _emit 0x00
			_asm _emit 0xEB
											; if (!writeable(bx))
			jnz     done2

	skip_write_test2:

			_asm _emit  0x0F                ; lsl, dx, bx
			_asm _emit  0x03
			_asm _emit  0xD3
			jnz     done2
											;* dx = last valid address

			mov     bx, WORD PTR lp
			mov     cx, nBytes
			jcxz    skip_range_test2            ; if length == 0, ignore offset part

			;* make sure start address is valid
			cmp     bx,dx                   ; if (offsetof(lp) > dx)
			ja      done2

			;* make sure last address is also valid
			dec     cx                      ; bias for last byte
			add     bx,cx
			jc      done2                   ; overflow => no good
			cmp     dx,bx                   ; if (offsetof(lp) + nBytes <= dx)
			jb      done2
	skip_range_test2:

#endif //!_DOS
			inc     bValid
	done2:
	}
	return bValid;
#endif // _NTWIN
}

#pragma warning(default:4100)
#pragma optimize("", on)
