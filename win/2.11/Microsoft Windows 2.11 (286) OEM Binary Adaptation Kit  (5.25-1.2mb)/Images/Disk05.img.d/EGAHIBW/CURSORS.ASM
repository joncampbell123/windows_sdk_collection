        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:	CURSORS.ASM
;
; This module contains the routines which are required to manage
; the cursor image.  The actual cursor drawing primitive reside
; elsewhere.
;
; Create   15-Feb-1987
; Author:  **** ***** [*****]
;
; Copyright (c) 1987  Microsoft Corporation
;
; Exported Functions:	SetCursor
;			CheckCursor
;			MoveCursor
;
; Public Functions:	exclude
;			unexclude
;			exclude_far		(allows far call to exclude)
;			unexclude_far		( "" to unexclude)
;
; Public Data:		screen_busy
;
; General Description:
;
;   All display drivers must support a "cursor" for the pointing
;   device.  The cursor is a small graphics image which is allowed
;   to move around the screen independantly of all other operations
;   to the screen, and is normally bound to the location of the
;   pointing device.  The cursor is non-destructive in nature, i.e.
;   the bits underneath the cursor image are not destroyed by the
;   presence of the cursor image.
;
;   Logically, the cursor image isn't part of the physical display
;   surface.  When a drawing operation coincides with the cursor
;   image, the result is the same as if the cursor image wasn't
;   there.  In reality, if the cursor image is part of the display
;   surface it must be removed from memory before the drawing
;   operation may occur, and redrawn at a later time.
;
;   This exclusion of the cursor image is the responsibility of
;   the display driver.  If the cursor image is part of physical
;   display memory, then all output operations must perform a hit
;   test to determine if the cursor must be removed from display
;   memory, and set a protection rectangle wherein the cursor must
;   not be displayed.  The actual cursor image drawing routine
;   must honor this protection rectangle by never drawing the
;   cursor image within its boundary.
;
;
; Restrictions:
;
;   The Window Manager is responsible for mapping generic cursors
;   into the correct size for the driver, as specified in the
;   display driver's resources.
;
;   The segments containing these routines and their data must be fixed
;   in memory since they are called at interrupt time.
;
;   The exclusion rectangle is a serially reusabel resource.  Only
;   one may be defined at any given moment.  There is no check for
;   this.  It is assumed that there will never be contention for it
;   since Windows is non-preemptive.
;
;-----------------------------------------------------------------------;


	.xlist
	include cmacros.inc
	include windefs.inc
	.list

	??_out	Cursors

	externNP cursor_off		;Remove cursor from the screen
	externNP move_cursors		;Move cursor data structure
	externNP draw_cursor		;Draw the cursor
	externA  CUR_HEIGHT		;Height of a cursor/icon
	externA  CUR_ROUND_LEFT 	;Used to round left  exclude X down
	externA  CUR_ROUND_RIGHT	;Used to round right exclude X up


sBegin	Data

	externB cur_cursor		;Internal cursor data structure
	externW real_width		;Real width of current cursor (16|32)



;	(x_cell,y_cell) is the location of the cursor on the screen.
;	These locations are only updated whenever a cursor is drawn.
;
;	(real_x,real_y) is the location of the cursor as specified
;	by the user.  These locations are always kept current.
;
;	These cells may not be the same if the cursor drawing takes
;	a lot of time and the mouse is moving quickly.	Therefore,
;	after a cursor has been drawn, a check must be made to see
;	if the cursor has moved, and if so the cursor must be drawn
;	again.


	externW x_cell			;x_cell of last drawn cursor
	externW y_cell			;y_cell of last drawn cursor
	externA INIT_CURSOR_X		;initial x position
	externA INIT_CURSOR_Y		;initial y position

real_x	dw	INIT_CURSOR_X		;Real x location of cursor
real_y	dw	INIT_CURSOR_Y		;Real x location of cursor



;	hot_x and hot_y contain the hot spot adjustment for the cursor.
;
;	These locations should be zeroed whenever a cursor is changing
;	or has been turned off, and should be set once a cursor has
;	been defined.  When the cursor is turned off, the hot spot
;	adjustment should be added back to the real cursor coordinates
;	(real_x, real_y).  When a cursor is set, they should be
;	subtracted off.  This will keep the cursor based at the hot
;	spot during a change instead of the upper left corner.


hot_x	 dw	 0			 ;X hot spot adjustment
hot_y	 dw	 0			 ;Y hot spot adjustment



;	screen_busy is a flag used for critical section code to
;	indicate that the screen is busy.  Since cursor operations
;	take a very long time (e.g. drawing a cursor), the screen_busy
;	flag is set to 0 to show that the screen is busy, and then
;	interrupts are enabled to allow other interrupts.  The basic
;	operation for the semephore is:
;
;		xor	cx,cx
;		xchg	screen_busy,cx
;		jcxz	operation_in_progress


	public	screen_busy
screen_busy	db	NOT_BUSY	;Show screen not busy
IS_BUSY 	equ	0		;  Cursor operation in progress
NOT_BUSY	equ	1		;  No cursor operation in progress



;	cur_flags contains control flags indicating the cursor status.
;	Flags are defined for the cursor being off, and the cursor being
;	excluded.

cur_flags	db	CUR_OFF 	;Cursor status, initially hidden
CUR_OFF 	equ	10000000b	;  Null cursor has been specified
CUR_EXCLUDED	equ	01000000b	;  Cursor has been excluded




;	The following words contain the bounding rectangle wherein the
;	cursor is not allowed to be displayed.	The values for left and
;	right will always be rounded to contain the entire byte (or
;	word or dword if working to those boundaries).
;
;	These values will only be valid if exclude_count is non-zero.
;
;	NOTE: Only one rectangle at a time may be set.

		even
exclude_left	dw	0		;left side  of exclusion rectangle
exclude_top	dw	0		;top	    of exclusion rectangle
exclude_right	dw	0		;right side of exclusion rectangle
exclude_bottom	dw	0		;bottom     of exclusion rectangle

exclude_count	db	0		;Set non-zero if rectangle is valid
RECT_PRES	equ	1		;  Exclusion rectangle set
RECT_NOT_PRES	equ	0		;  No exclusion rectangle set

sEnd	Data


sBegin	Code
assumes cs,Code
page
;---------------------------Public-Routine------------------------------;
;
; Exclude - Set Cursor Exclusion Rectangle
;
;   The cursor exclusion area is set to the passed value.  If the
;   cursor image is currently within the excluded area, it will be
;   removed from the screen.
;
;   The given X coordinates will be rounded to the word size being
;   used (byte, word, dword).  This removes the adjustment for word
;   size out of the actual hit test code into the code that creates
;   the exclusion rectangle.  The left side will be rounded down and
;   the right side will be rounded up).
;
;   As an example, for bytes:
;
;	   top,left  ---------------------
;		    |			  |
;		    |			  |
;		    |			  |
;		    |	    SCREEN	  |
;		    |			  |
;		    |			  |
;		    |			  |
;		     ---------------------  right,bottom
;
;
;	   |L		   |		   |		  R|
;	 |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
;
;	    left will always be at D7
;	    right will always be at D0
;
;
;   Exclude is not called by the interrupt cursor drawing code.
;
; Entry:
;	  (cx) = left
;	  (dx) = top
;	  (si) = right	(inclusive)
;	  (di) = bottom (inclusive)
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	BP
; Registers Destroyed:
;	AX,BX,CX,DX,SI,DI,DS,ES,FLAGS
; Calls:
;	cursor_off
;	exclude_test
; History:
;	Sun 15-Feb-1987 14:59:29 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; void exclude (left,top,right,bottom);
; SWORD left;				// Left   pixel of exclusion rect
; SWORD top;				// Top	  pixel of exclusion rect
; SWORD right;				// Right  pixel of exclusion rect
; SWORD bottom; 			// Bottom pixel of exclusion rect
; {
;   WORD    old_state;
;
;   round x coordinates to a boundary;	// round x boundaries
;   save exclusion rectangle;		// save left, top, right, bottom
;   enter_crit();			// start of critical setcion
;
;   if (cursor within exclusion rectangle)
;   {
;	old_state = screen_busy 	// We'll restore this later
;	screen_busy = IS_BUSY;		// Lock out all other routines
;	cur_flags = CUR_EXCLUDED;	// Show cursor as excluded
;
;	// We now have exclusive control of the screen.  We have the
;	// semephore so CheckCursor won't try to bring back the cursor,
;	// and we have told MoveCursor that the cursor is excluded, so
;	// it won't try to move it.
;
;	enable_interrupts;		// Allow other things to happen
;	cursor_off();
;	screen_buys = old_state;
;   }
;   exit_crit();			// End of critical section
;
; }
;-----------------------------------------------------------------------;

cProc 	exclude_far,<FAR,PUBLIC>
cBegin	<nogen>
	call 	exclude		; make near call to the "exclude" routine
	ret
cEnd	<nogen>

page
cProc	exclude,<NEAR,PUBLIC>

cBegin

	mov	ds,cs:_cstods		;Need access to our data segment
	assumes ds,Data
	assumes es,nothing		;Have no idea what's in it

	and	cx,CUR_ROUND_LEFT	;Round left coordinate down
	or	si,CUR_ROUND_RIGHT	;Round right coordinate up


;	Since OEM calls are atomic operations (they may not be preempted,
;	only interrupted by the cursor code itself), setting of the exclude
;	rectangle need not be treated as a critical section.


	mov	exclude_left,cx 	;Set up exclusion rectangle
	mov	exclude_top,dx
	mov	exclude_right,si
	mov	exclude_bottom,di
	mov	exclude_count,RECT_PRES
	mov	cl,CUR_OFF+CUR_EXCLUDED ;Set test mask for exclude_test

	EnterCrit			;Hit test is critical code
	mov	ax,x_cell		;Get current cursor x,y location
	mov	bx,y_cell
	call	exclude_test		;Go see if excluded
	jnc	no_exclude_needed	;Hidden or already excluded
	jz	no_exclude_needed	;Don't need to exclude


;	The cursor needs to be removed from the screen.  Show that
;	the screen is busy and then take the cursor down.  Since this
;	is a cursor drawing operation which will take some time, do
;	it with interrupts enabled.  If the screen_busy flag is set,
;	then somebody has called an output routine at interrupt time
;	which is a no-no!  There is no real way to recover from this.
;	Just go ahead and take down the cursor


	xor	cx,cx
	xchg	screen_busy,cl		;Set screen busy and save old state
	errnz	IS_BUSY
	push	cx
	mov	cur_flags,CUR_EXCLUDED	;Show cursor excluded
	sti				;Allow ints. (EnterCrit disabled them)
	call	cursor_off		;Remove cursor
	mov	ds,cs:_cstods		;Restore our data segment
	assumes ds,Data

	pop	ax			;Restore screen busy state
	mov	screen_busy,al

no_exclude_needed:
	LeaveCrit			;Restore interrupts to caller's state

cEnd
page
;--------------------------Private-Routine------------------------------;
;
; exclude_test - Cursor Exclusion Test
;
;   The cursor is checked for its current status (excluded, hidden),
;   and a hit test possibly made against the exclusion rectangle.
;
;   The cur_flags are tested against the passed test mask, and if
;   any of the test mask bits are set, then the hit test is skipped.
;   This allows drawing code to check for either exclusion or the
;   user turning off the cursor before a hit test is performed.  It
;   also allow the timer code to just check for the user turning off
;   the cursor, and if it hasn't been turned off, to go ahead and
;   perform the hit test.
;
; Entry:
;	(ax) = x_cell to hit against
;	(bx) = y_cell to hit against
;	(cl) = cur_flags test mask
; Returns:
;	'C' clear if any bits in test mask are set in [cur_flags]
;	'C' set if no bits in test mask are set in [cur_flags]
;	  'Z' clear if cursor is excluded
;	  'Z' set if cursor isn't excluded
; Error Returns:
;	None
; Registers Preserved:
;	DX,SI,DI,BP,ES
; Registers Destroyed:
;	AX,BX,CX,DS,FLAGS
; Calls:
;	None
; History:
;	Sun 15-Feb-1987 14:59:29 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; flags exclude_test(x_cell,y_cell,cur_flags_mask);
; SWORD x_cell; 			    // X location of cursor
; SWORD x_cell; 			    // Y location of cursor
; WORD	cur_flags_mask; 		    // Mask to test curs_flags with
; {
;   if (cur_flags & cur_flags_mask)	    // Skip hit test?
;	return(NOCARRY) 		    // Yes, show test skipped
;
;   if (exclude_count)			    // If no exclusion rectangle
;	return (CARRY,ZERO)		    //	 show not excluded
;
;   if (x_cell > exclude_right) 	    // If start is > right
;	return(CARRY,ZERO)		    //	 isn't exclude
;
;   if (x_cell+real_width < exclude_left)   // If right of cursor < left
;	return(CARRY,ZERO)		    //	 isn't exclude
;
;   if (y_cell > exclude_bottom)	    // If top is > bottom
;	return(CARRY,ZERO)		    //	 isn't exclude
;
;   if (y_cell+CUR_HEIGHT-1<exclude_bottom) // If bottom of cursor < top
;	return(CARRY,ZERO)		    //	 isn't exclude
;
;   return (CARRY,NONZERO)		    // SHow excluded
; }
;-----------------------------------------------------------------------;


cProc	exclude_test,<NEAR>

cBegin

	mov	ds,cs:_cstods
	assumes ds,Data
	assumes es,nothing		;Have no idea what's in it

	test	cur_flags,cl		;Do the hit test? (Clears 'C')
	jnz	exclude_test_20 	;Skip hit test
	xor	cx,cx			;Show cursor not excluded (need a 0)
	cmp	exclude_count,cl	;Is there an exclusion area
	je	exclude_test_10 	;  No, show not excluded
	errnz	RECT_NOT_PRES

	cmp	ax,exclude_right	;Is left of cursor > right of exclude?
	jg	exclude_test_10 	;  Yes, not excluded
	add	ax,real_width		;Add in width of cursor or icon
	cmp	ax,exclude_left 	;Is right of cursor < left of exclude?
	jl	exclude_test_10 	;  Yes, not excluded

	cmp	bx,exclude_bottom	;Is top of cursor > bottom of exclude
	jg	exclude_test_10 	;  Yes, not excluded
	add	bx,CUR_HEIGHT-1 	;Add in height of cursor/icon
	cmp	bx,exclude_top		;Is bottom of cursor < top of exclude
	jl	exclude_test_10 	;  Yes, not excluded
	inc	cx			;Show cursor is excluded

exclude_test_10:
	or	cx,cx			;Clear 'Z' if cursor is excluded
	stc				;Show some bits in mask were set

exclude_test_20:

cEnd
page
;---------------------------Public-Routine------------------------------;
;
; UnExclude - Remove Exclusion Area
;
;   The exclusion rectangle is removed.  The cursor will not be redrawn
;   since it might have to be taken down for the next call.  Redrawing
;   the cursor will be left to the ChechCursor routine.
;
; Entry:
;	None
; Returns:
;	DS = Data segment
; Error Returns:
;	None
; Registers Preserved:
;	AX,BX,CX,DX,Si,DI,BP
; Registers Destroyed:
;	DS
; Calls:
;	None
; History:
;	Sun 15-Feb-1987 14:59:29 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; void unexclude();
; {
;   exclude_count = RECT_NOT_PRES;
;   return();
; }
;-----------------------------------------------------------------------;

cProc 	unexclude_far,<FAR,PUBLIC>
cBegin 	<nogen>
	call 	unexclude	; make near call to the "unexclude" routine
	ret
cEnd	<nogen>

page
cProc	unexclude,<NEAR,PUBLIC>

cBegin

	mov	ds,cs:_cstods
	assumes ds,Data
	assumes es,nothing		;Have no idea what's in it

	mov	exclude_count,RECT_NOT_PRES

cEnd

page
;--------------------------Exported-Routine-----------------------------;
; SetCursor - Set Current Cursor Shape
;
;   This is a private entry point within the display driver for the
;   Window Manager.
;
;   The given cursor shape is saved in local storage to be used as
;   the current cursor shape.  If the pointer to the cursor shape is
;   NULL, then no cursor is to be drawn.
;
;   The given cursor shape will have been converted from generic form
;   into the dimensions requested by this driver.
;
;   Any cursor on the screen will be removed before the new cursor
;   shape is set.  The drawing of the new cursor will be delayed
;   until the check_cursor routine is called (check_cursor is the
;   only routine that can make the cursor become visible).
;
;   If this routine is reentered, the request will be ignored.
;
; Entry:
;	None
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	cursor_off
;	move_cursor
; History:
;	Sun 15-Feb-1987 16:47:15 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; void SetCursor(lp_cursor)
; CURSOR far *lp_cursor
; {
;   WORD    old_busy;
;
;   old_busy = IS_BUSY; 		// Try for screen semephore
;   if (swap(screen_busy, old_busy) == IS_BUSY)
;	return();
;
;   // We control the vertical.  We control the horizontal.
;   // We control the screen semephore.  Since we do, we can
;   // enable and disable interrupts at our will without
;   // worry that somebody will do something to the cursor image.
;
;   disable_interrupts; 		// Treat as a critical section
;   cur_flags = CUR_OFF;		// Assume a null cursor;
;   real_x += hot_x;			// Remove hot spot adjustment
;   real_y += hot_y;			//   from real (X,Y) position
;   hot_x = hot_y = 0;			// Don't want hot spot adjustments
;   enable_interrupts;			// Interrupt can play with real x & y
;   cursor_off();			// Remove old cursor from screen
;   if (lp_cursor)			// If there is a new cursor shape
;   {
;	copy(cur_cursor,lp_cursor);	// Copy cursor header information
;	move_cursors(); 		// Move the patterns, adj. hot spot
;	disable_interrupts;		// Treat as a critical section
;	hot_x = cur_cursor.csHotX	// Save X hot spot adjustment
;	hot_y = cur_cursor.csHotY	// Save Y hot spot adjustment
;	real_x -= hot_x;		// Adjust real (X,Y) for the
;	real_y -= hot_y;		//   hot spot
;	cur_flags = CUR_EXCLUDED;	// Show excluded, but not hidden
;	enable_interrupts;
;   }
;   screen_busy = NOT_BUSY;		// Others can have the screen now
; }
;-----------------------------------------------------------------------;



;	Define _cstods.  This location will contain our data segment value.
;	Since our data segment is a single fixed data segment, this will
;	work.  Interrupt code and some other routines will need access
;	to the data segment.


	org	$+1			;The data segment value will
_cstods label	word			;  be stuffed here and kept
	org	$-1			;  current by the kernel
	public	_cstods


cProc	SetCursor,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmD	lp_cursor		;Far pointer to new cursor shape

cBegin
	assumes ds,Data 		;Set up by prologue
	assumes es,nothing		;Have no idea what's in it

	xor	cx,cx			;Set screen busy
	xchg	screen_busy,cl		;  and get current status
	jcxz	set_cursor_20		;Screen is busy, error, ignore call
	errnz	IS_BUSY
	mov	cur_flags,CUR_OFF	;Show null cursor, not excluded


;	Remove any old cursor.	Since this is a cursor drawing operation
;	which will take some time, do it with interrupts enabled.
;
;	real_x and real_y are kept as the upper left corner of the
;	where the cursor is to be drawn.  Since the hot spot might
;	change with the cursor shape, the hot spot must be added
;	back into these locations.  After the new cursor has been
;	set, the hot spot can be subtracted out for it.
;
;	Also, since the moving of the cursor shapes takes a little
;	time itself, leave interrupts enabled until after the shape
;	has been moved (Its safe to do this since the screen_busy
;	flag has been set).  After the shape has been set, restore
;	the interrupts to the user state and free the screen.


	pushf				;Save user interrupt state
	cli				;Don't want real_x, real_y changing
	xor	ax,ax			;Zero old hot spot and add it back
	xchg	ax,hot_x		;  to real [X,Y] coordinate
	add	real_x,ax
	xor	ax,ax
	xchg	ax,hot_y
	add	real_y,ax
	sti				;Allow interrupts

	call	cursor_off		;Remove old cursor from screen
	cld				;Just in case

	mov	es,_cstods		;Destination is in our data segment
	lds	si,lp_cursor		;--> cursor shape
	assumes es,Data
	assumes ds,nothing

	mov	ax,ds			;If the cursor pointer is null,
	or	ax,si			;  then no cursor is to be drawn
	jz	set_cursor_10		;They don't want a cursor

	mov	di,DataOFFSET cur_cursor;Save cursor information
	mov	cx,(size cursorShape)/2
	errnz	<cursorShape and 1>	;  (must be even byte count)

	rep	movsw			;Copy cursor definition
	call	move_cursors		;Copy AND/XOR masks to static,
					;  aligned storage

	mov	ax,es			;Restore data segment pointer
	mov	ds,ax			;  ES was left set by move_cursors
	assumes ds,Data 		;  so we can just take it from there
	assumes es,Data


;	Since the cursor must be drawn again (it wasn't a null cursor),
;	we have to draw it.  To do this, the cursor excluded flag will
;	be set in cur_flags.  Only the timer code can bring the cursor
;	back once it has been excluded, and will do it when the next
;	timer interrupt occurs.  This minimizes the code that has to
;	be written or duplicated in SetCursor!
;
;	Also set up the new hot spot adjustments and adjust the cursor
;	coordinate for it.


	cli				;Don't want real_x, real_y changing
	mov	ax,cur_cursor.csHotX	;Set X hot spot adjustment
	mov	hot_x,ax
	sub	real_x,ax		;Adjust real_x for hotspot
	mov	ax,cur_cursor.csHotY	;Ditto for Y
	mov	hot_y,ax
	sub	real_y,ax

	mov	cur_flags,CUR_EXCLUDED	;Show excluded, but not hidden



;	Whatever had to be done has been.  Restore the interrupt state
;	to whatever was set by the user and free the screen and make the
;	screen available.

set_cursor_10:
	assumes es,Data 		;Can only guarantee that ES has
	assumes ds,nothing		;  the data segment selector

	POPFF				;Restore interrupts to user state
	mov	screen_busy,NOT_BUSY

	assumes es,nothing

set_cursor_20:

cEnd
page
;--------------------------Exported-Routine-----------------------------;
; MoveCursor - Move Cursor To Given Coordinate
;
;   This is a private entry point within the display driver for the
;   Window Manager.
;
;   The current cursor location is set to the given coordinates.
;   If the cursor is visible, it will be moved to the new location.
;   If the cursor is off (a NULL cursor), or if the cursor has been
;   excluded, then just the cursor location will be updated.
;
;   If the cursor is on and the new cursor position will cause the
;   cursor to be excluded, it will be removed from the screen.
;
; Entry:
;	None
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	SI,DI,DS,BP
; Registers Destroyed:
;	AX,BX,CX,DX,ES,FLAGS
; Calls:
;	exclude_test
;	cursor_off
;	draw_cursor
; History:
;	Sun 15-Feb-1987 16:47:15 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; void MoveCursor(abs_x,abs_y)
; SWORD abs_x;				// x coordinate of cursor
; SWORD abs_y;				// y coordinate of cursor
; {
;   WORD    old_busy;
;
;   enter_crit();			// Updating the real X,Y is
;   real_x = abs_x - hot_x;		//   a critical section
;   real_y = abs_y - hot_y;
;   old_busy = IS_BUSY; 		// Try for screen semephore
;   swap(screen_busy,old_busy);
;   leave_crit();
;
;   if (old_busy == NOT_BUSY)
;   {
;	while(cursor positions disagree)
;	{
;	    if (cursor hidden || already excluded)
;	    {
;		screen_busy = NOT_BUSY;
;		return();
;	    }
;	    if (newly excluded)
;	    {
;		cur_flags = CUR_EXCLUDED;
;		cursor_off();
;		screen_busy = NOT_BUSY;
;		return();
;	    }
;	    draw_cursor();		// can actually draw cursor
;	}
;	screen_busy = NOT_BUSY; 	// others can have the screen now
;   }
;   return();
; }
;-----------------------------------------------------------------------;


cProc	MoveCursor,<FAR,PUBLIC,WIN,PASCAL>,<si,di>

	parmW	abs_x			;x coordinate of cursor
	parmW	abs_y			;y coordinate of cursor

cBegin

	assumes ds,Data 		;Set by prologue
	assumes es,nothing		;Have no idea what's in it

	EnterCrit			;Moving is a critical section

	mov	ax,abs_x		;Get the new cursor position,
	sub	ax,hot_x		;  relative to its hot spot
	mov	real_x,ax
	mov	bx,abs_y
	sub	bx,hot_y
	mov	real_y,bx
	xor	cx,cx			;Show that the screen is busy
	xchg	screen_busy,cl		;  and get the old status
	errnz	IS_BUSY

move_cursor_10:

	LeaveCrit			;Can allow interrupts now


;	The real cursor (x,y) has been updated and the screen_busy
;	flag has been set, and the old value of screen_busy is
;	ready for testing.
;
;	If the screen previously wasn't busy, then if the cursor is
;	hidden or excluded, no action will take place.	If the
;	cursor isn't hidden or excluded, then the cursor will be
;	drawn at it's new location if the hit test of the exclusion
;	area succeeds.	If the hit test of the exclusion area fails,
;	then the cursor will be taken down and left down.
;
;	If the screen was busy, the real cursor (x,y) will have
;	been updated so that when the cursor is drawn next, it
;	will have the current (x,y) to draw at.  The actions
;	which could cause the screen to be busy are the following:
;
;	    Cursor is being taken down by the SetCursor routine:
;
;		If this is the case, then the cursor is to remain
;		off until a new cursor is selected, at which time
;		the timer code will try to bring the cursor back.
;
;
;	    Cursor is being taken down by exclusion from an output
;	    function:
;
;		If this is the case, then the cursor will remain
;		excluded until either one of two events occur:
;
;		    a new cursor is selected, at which time the
;		    timer code will try to bring the cursor back.
;
;		    The timer interrupt occured and determined that
;		    the cursor needed to be unexcluded ([screen_busy]
;		    must be false for the timer to redraw the cursor).
;
;	    Cursor is being drawn by MoveCursor:
;
;		Hey, thats us!	If this is the case, after the cursor
;		has been drawn, the real cursor location will be checked
;		to see if the cursor has moved, and if it has, it will
;		be drawn again at the correct spot (unless it has become
;		excluded).
;
;

	jcxz	move_cursor_40		;Screen is busy, just update x,y
	errnz	IS_BUSY

	mov	x_cell,ax		;Set x,y where drawing will occur
	mov	y_cell,bx


;	The following call to exclude_test need not be made from here
;	with interrupts disabled since screen_busy has been set, and no
;	code will modify the screen while screen_busy is set (except the
;	normal drawing routines which cannot be called at interrupt time,
;	who just happen to set up the exclusion rectangle, which should
;	never be able to be called while screen_busy is set, ...)


	mov	cl,CUR_OFF+CUR_EXCLUDED ;Set flag mask for exclude test
	call	exclude_test		;Go see if excluded
	jnc	move_cursor_20		;Hidden or already excluded
	jz	move_cursor_30		;Can draw the cursor


;	The hit test was positive.  The cursor must be excluded.  Since
;	this is a cursor drawing operation which will take some time,
;	do it with interrupts enabled.

	or	cur_flags,CUR_EXCLUDED	;Show cursor excluded
	pushf				;Save user interrupt state
	sti				;Allow interrupts
	call	cursor_off		;Remove cursor
	POPFF				;Restore interrupts to user state
	mov	ds,_cstods		;Restore our data segment
	assumes ds,Data
	assumes es,nothing

move_cursor_20:
	mov	screen_busy,NOT_BUSY	;Show screen no longer busy
	jmp	short move_cursor_40	;Exit


;	The hit test was negative and the cursor was on and not
;	excluded.  Draw the cursor at the passed location.  Since
;	this is a cursor drawing operation which will take some time,
;	do it with interrupts enabled.


move_cursor_30:

	pushf				;Save user interrupt state
	sti				;Allow interrupts
	call	draw_cursor		;Draw new cursor
	POPFF				;Restore interrupts to user state
	mov	ds,_cstods		;Restore our DS
	assumes ds,Data
	assumes es,nothing


;	The cursor has been drawn.  Check to see if the cursor
;	has moved again while being drawn.  If it has, then draw
;	the cursor at the new location.  Otherwise make the screen
;	available.


	EnterCrit			;This is a critical section!
	mov	cl,NOT_BUSY		;Show screen available to move_cursor_10
	mov	ax,real_x		;Get the real X location
	mov	bx,real_y
	cmp	ax,x_cell		;If real X is different then cursor's
	jne	move_cursor_10		;  X, then go draw the cursor
	cmp	bx,y_cell		;If real Y is different then cursor's
	jne	move_cursor_10		;  Y, then go draw the cursor
	mov	screen_busy,cl		;Show screen available
	LeaveCrit			;Done with this critical section

move_cursor_40:

cEnd
page
;--------------------------Exported-Routine-----------------------------;
; CheckCursor - Check On The Cursor
;
;   This is a private entry point within the display driver for the
;   Window Manager.
;
;   The cursor is checked to see if it can be unexcluded, and if so
;   it is unexcluded.  This is the only routine which can cause the
;   cursor to become visible once it has become invisible (excluded
;   by a drawing operation or turned off because of a new shape being
;   set).
;
;   It is expected that this routine be called at a rate close to once
;   every quarter of a second.	This allows for a lazy redraw of the
;   cursor whenever it has become excluded.
;
;   If the screen is busy (due to a current cursor operation), the
;   cursor is turned off, or it isn't excluded, then there is nothing
;   for this routine to do.  If the cursor is excluded, then a hit test
;   will be performed against the current exclusion rectangle (if there
;   is one), and if the cursor is now visible, it will be drawn.
;
;   This routine is intended to be interrupt code.  The state of the
;   interrupts will be maintained over the entire call, but interrupts
;   will be enabled whenever the cursor is drawn.
;
;   For devices with hardware support for cursors, this routine could
;   be a nop.
;
; Entry:
;	None
; Returns:
;	None
; Error Returns:
;	None
; Registers Preserved:
;	SI,DI,DS,BP,ES
; Registers Destroyed:
;	AX,BX,CX,DX,FLAGS
; Calls:
;	exclude_test
;	cursor_off
;	draw_cursor
; History:
;	Sun 15-Feb-1987 16:47:15 -by-  **** ***** [*****]
;	Created.
;-----------------------------------------------------------------------;


;------------------------------Pseudo-Code------------------------------;
; void CheckCursor();
; {
;   WORD    old_busy;
;
;   if (swap(screen_busy,old_busy) == screen_busy)
;	return();			// cannot access the screen
;
;   if (cursor is off || cursor not excluded)
;   {					// nothing to do
;	screen_busy = NOT_BUSY;
;	return();
;   }
;
;   // The cursor is currently excluded.  If it is now unexcluded,
;   // it must be drawn.
;
; test_if_unexcluded:
;
;   enter_crit();
;   if (cursor unexcluded)
;   {
;	leave_crit();
;	draw_cursor();			// draw cursor at new location
;	cur_flags = 0;			// show cursor is on and unexcluded
;	enter_crit();
;	if (cursor positions disagree)
;	    goto test_if_unexcluded;	// moved while we were drawing it.
;	screen_busy = NOT_BUSY;
;	leave_crit();
;	return();
;   }
;   leave_crit();
;
;   // Must test to see if the cursor became excluded after we
;   // just brought it back.
;
;   if (cursor is excluded)
;   {
;	cursor_off();
;	cur_flags = CUR_EXCLUDED;
;   }
;
;   screen_busy = NOT_BUSY;		// others can have the screen now
;   return();
; }
;-----------------------------------------------------------------------;


cProc	CheckCursor,<FAR,PUBLIC,WIN,PASCAL>,<si,di,es>

cBegin
	xor	cx,cx			;Set screen busy
	xchg	screen_busy,cl		;  and get current status
	jcxz	check_cursor_20 	 ;Screen is busy, skip check
	errnz	IS_BUSY

	mov	al,cur_flags		;See if the cursor is on and excluded
	add	al,al
	jc	check_cursor_40 	;Cursor is off
	jns	check_cursor_40 	;Cursor is on, but not excluded
	errnz	CUR_OFF-10000000b	;Must be this bit
	errnz	CUR_EXCLUDED-01000000b	;Must be this bit


;	The cursor is on and is currently excluded.  Perform a hit
;	test against the current (real_x, real_y) cursor location,
;	and if the cursor is no longer excluded, show the cursor.
;
;	After draing the cursor, the exclusion test must be repeated.
;	If the cursor is excluded, the mouse moved while drawing it.
;	If this is the case, take it down, then wait for the next
;	timer interrupt to try and put it back up.


	EnterCrit			;Dont want an old X and a new Y!
	mov	ax,real_x		;Get the X,Y for the test
	mov	bx,real_y

check_cursor_10:
	LeaveCrit

	mov	si,ax			;Save these over the exclude test
	mov	di,bx			;  (exclude_test alters ax & bx)
	xor	cx,cx			;Don't test any flags
	call	exclude_test		;Is cursor excluded?
	jnz	check_cursor_30 	;  Yes (might have to remove it now)


;	The cursor is no longer excluded, so draw the new cursor.  Since
;	this is a cursor drawing operation which will take some time, do
;	it with interrupts enabled.


	mov	x_cell,si		;Save (x,y) where cursor will be drawn
	mov	y_cell,di

	pushf				;Save user interrupt state
	sti				;Allow interrupts
	call	draw_cursor		;Draw cursor
	POPFF				;Restore interrupts to user state
	mov	ds,_cstods		;Restore our data segment
	assumes ds,Data
	assumes es,nothing

	mov	cur_flags,0		;Cursor is on and not excluded


;	The cursor has been drawn.  Check to see if the cursor
;	has moved again while being drawn.  If it has, then draw
;	the cursor at the new location.  Otherwise make the screen
;	available.


	EnterCrit			;This is a critical section!
	mov	ax,real_x		;Get the real cursor (X,Y) location
	mov	bx,real_y
	cmp	ax,x_cell		;If real X is different than cursor's
	jne	check_cursor_10 	;  X, then go draw the cursor
	cmp	bx,y_cell		;If real Y is different than cursor's
	jne	check_cursor_10 	;  Y, then go draw the cursor
	mov	screen_busy,NOT_BUSY	;Show available before enabling
					;  ints so MoveCursor can update
	LeaveCrit			;Done with this critical section

check_cursor_20:
	jmp	short check_cursor_50


;	The cursor is excluded.  If it became excluded after the cursor
;	was brought up (while interrupts were enabled), it must be
;	taken down again.  If it is taken down, then no attempt will
;	be made to draw it again until the next timer interrupt.

check_cursor_30:
	test	cur_flags,CUR_EXCLUDED	;Is cursor excluded?
	jnz	check_cursor_40 	;  Yes, don't need to take it down

	pushf				;Save user interrupt state
	sti				;Allow interrupts
	call	cursor_off		;Remove the cursor
	POPFF				;Restore interrupts to user state
	mov	ds,_cstods		;Restore our data segment
	assumes ds,Data
	assumes es,nothing

	mov	cur_flags,CUR_EXCLUDED	;Cursor is now excluded



;	Whatever was needed has been done.  Free the screen and exit

check_cursor_40:
	mov	screen_busy,NOT_BUSY

check_cursor_50:


cEnd

	ifdef	PUBDEFS
	include cursors.pub
	endif


sEnd	code
end
