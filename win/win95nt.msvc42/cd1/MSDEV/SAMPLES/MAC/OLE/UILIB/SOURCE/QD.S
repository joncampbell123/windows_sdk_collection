CurrentA5 equ 0x904
thePort equ 0xca		;; offset of qd.thePort

	import _qd
	export _SetLpqdFromA5
_SetLpqdFromA5:
	lwz r4, CurrentA5(r0)  ;; r4 contains A5
	movei r6, thePort
	lwz r5, 0(r4)  ;; r5 should now contain a pointer to the qd.thePort
	lwz r3, _qd(rtoc)
	subf r5, r6, r5
	stw r5, _qd(rtoc)
	ret

	export _RestoreLpqd
_RestoreLpqd:
	stw r3, _qd(rtoc)
	ret
