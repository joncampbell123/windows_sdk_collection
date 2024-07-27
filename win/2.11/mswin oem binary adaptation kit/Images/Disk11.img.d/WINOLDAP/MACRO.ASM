title ~~ Macro Management routines for SPUD (WINDOWS 2.00)

        .xlist
        include cmacros.inc
        include macro.inc 
        include tty.inc
        include error.inc
        .list
 
sBegin  data
        
externW         MacroSeg       
externW         hMacroSeg      
externW         MacroPara
externW         pifBehavior
externW         OldApState              ;The current state of the old app
externW         MacroMemUsed            ;Macro Memory Used up
externW         MacroSegSize            ;macro memory in bytes
GlobalW         MacroFirst,0            ;First Macro Offset
GlobalW         MacroLast,0             ;Last Macro Offset      
GlobalW         FirstMacAddr,0          ;First Macro Address
GlobalW         FromWithIn,1            ;Is AddMacro from DDE or Change?
GlobalW         TotalMacros,0           ;number of macros 
sEnd    data

sBegin  code

        assumes cs,code
        assumes ds,data

externFP        <GlobalLock, GlobalUnlock,GlobalFree>

PUBLIC          UpdateMacro,AddMacro,DeleteMacro,ChangeMacro
PUBLIC          VerifyString,CheckOutMacroString
PUBLIC          CheckInMacroString,GetMacroInfo,GetMacroList
PUBLIC          SortMacroInfo
PUBLIC          LockMacroSeg,UnlockMacroSeg     ;added 5/8/87

;-------UpdateMacro----------------------------------------
;
;       DESCRIPTION
;               This routines recieves pointer the the MacroInfo
;               structure (MacroID,MacroOpcode,AsciizString)
;               
;       ENTRY
;               parameters in C convntion on the stack
;               in the following order:
;                       Offset to the structure,
;                       Segment of the structure
;       EXIT      
;               if successful
;                       AX = 0
;               else 
;                       AX = ErrorCode as defined in Error.inc
;    
;       USES
;
;               AX,BX,CX,DX 
;       
;       HISTORY
;               Changed the calling convention. Now (9/10/87) 
;               SHELL supplies the MacroID. So the MACRO MANAGER
;               does not have to create macroIDs anymore. Changes
;               effect the following modules:  
;               1. AddMacro
;               2. VerifyString
;----------------------------------------------------------

cProc   UpdateMacro,<NEAR,PUBLIC>,<DS,ES,SI,DI>
        parmD   MacroOp  

cBegin  UpdateMacro

        cCall   LockMacroSeg            ;AX has macroseg
        mov     DX,AX
        or      AX,AX                   ;see if there is any macroseg
        jnz     UMC1
        mov     AX,ERR_NO_MEMORY        ;no memory
	cCall	UnlockMacroSeg		;unlock a segment which is locked
        jmp     UMC107                  ;return

UMC1:
        mov     BX, SEG_MacroOp         ;Segment of the C parameter 
        mov     ES,BX
        mov     SI, OFF_MacroOp         ;Offset in SI
        push    SI
        push    ES
        mov     AX,ES:[SI]              ;macro id in AX 
	cmp	AX,LowMacroID		;make sure MacroID >1024
	jb	UM_ERRID		;not in limit
	cmp	AX,HighMacroID		;is macroID <2047 ?
	jbe	UM_OKAY			;within limits
UM_ERRID:
	mov	AX,ERR_INVALID_ID	;not in limits
	cCall	UnlockMacroSeg		;unlock locked segment
	jmp	UMC107			
UM_OKAY:	
        mov     DX,ES:[SI+2]            ;macro opcode
        cmp     DX,LowOp                ;see if opcode is within limits
        jb      UM_ERROR                ;not in limits exit with err code
        cmp     DX,HighOp               ;upper limit is 2
        ja      UM_ERROR                ;not in limits exit with err code

UMC2:
        cmp     DX,HighOp               ;is it change macro opcode
        jnz     UMC3                    ;no then see other 2 opcodes
        call    VerifyString            ;verify the string 
        jc      UM_ERROR                ;if correct CX=size,ES:[SI+4]->string
        call    ChangeMacro             
        jmp     UM_DONE                 ;go to end
        
UMC3:
        cmp     DX,LowOp                ;See if it is DeleteMacro
        jz      UMC4                    ;no it is AddMacro
        call    DeleteMacro             
        jmp     UM_DONE                 ;go to end
UMC4:
        call    VerifyString            ;ES:[SI] -> to the data strucure
        jc      UM_ERROR   
        call    AddMacro
        jmp     UM_DONE   

UM_ERROR:
        mov     AX,ERR_INVALID_OBJECT   ;from verify string
                                
UM_DONE:
        call    SortMacroInfo           ;sort macro list in ascending 
        cCall   UnlockMacroSeg          ;now call to unlock

UMC105:           
        pop     ES
        pop     SI
UMC107:
cEnd    UpdateMacro
        
;--------ChangeMacro---------------------------------------
;
;       DESCRIPTION
;               Replaces the current macro text with the    
;               given text
;
;       ENTRY
;               ES:SI   points to the text replacing the
;                       current text
;               AX      MacroID
;               CX      Size of the String      
;
;       EXIT          
;               if successful
;                       AX=0    
;               else
;                       AX=ErrorCode  
;               
;       USES
;              BX,CX,DX
;
;----------------------------------------------------------
 
ChangeMacro     PROC    NEAR

        push    ES
        push    DS
        push    SI
        push    DI

        push    ES        
        mov     BX,MacroSeg             ;Setup the DS and DI to point
        mov     ES,BX                   ;the macro memory
        mov     DI,MacroFirst           
CMC2:
        cmp     ES:[DI].MacroID,AX      ;Is this the macro 
        jz      CMC5                    ;yes we got it 
        sub     DI,MacroInfoSize        ;get the next pointer
        cmp     DI,MacroLast
        jnz     CMC2                    ;no check again
        mov     AX,ERR_INVALID_ID       ;Invalid Macro number
        pop     ES                      ;clean up
        jmp     CMC99                   
CMC5:
        cmp     ES:[DI].MacroSize,CX       ;is it the same size
        jz      CMC7                    ;yes sir just replace
        pop     ES
        push    AX                      ;save the macroID
        push    CX                      ;and its size
        call    DeleteMacro             ;Delete current macro
        pop     CX
        pop     AX
        mov     FromWithIn,0            ;Make sure the supplied MacroID
                                        ;is used instead of allocating
        call    AddMacro                ;now add the current macro
        jnc     CMC99           
        mov     AX,ERR_NO_MEMORY          
        jmp     CMC99
        
CMC7:  
        pop     DS                      ;move ES into DS 
        mov     DI,ES:[DI].MacroAddress
        rep     movsb
CMC99:
        pop     DI
        pop     SI
        pop     DS
        pop     ES
        ret
ChangeMacro     ENDP
        
;--------AddMacro-------------------------------------------
;
;       DESCRIPTION
;               Adds a macro to the macro library
;                       
;       ENTRY
;               ES:SI pointing to the macro to be added
;               CX = StringLength
;               AX = MacroID 
;       EXIT
;               if successful             
;                       AX = 0 
;			reset carry
;               else
;                       AX = Error Code 
;			set carry 
;       USES
;               BX,CX
;----------------------------------------------------------

AddMacro        PROC    NEAR

        push    ES
        push    DS
        push    SI
        push    DI
        
        mov     BX,MacroSegSize         ;get macro memory in BX
        mov     DX,MacroMemUsed         ;get macromemory used up
        add     DX,CX                   ;add current request
        add     DX,MacroInfoSize        ;and the macro pointer size
        cmp     DX,BX                   ;compare required vs available
        jbe     ADM1                    ;memory available
        mov     AX,ERR_NO_MEMORY        ;no memory ?
        stc                             ;error condition
        jmp     ADM100
ADM1:
        cmp     FromWithIn,1            ;is it from ChangeMacro ?
        jnz     ADM2                    
        call    VerifyMacroID           ;see if this macro is already present?
        jnc     ADM2                    ;macro id is ok
        mov     AX,ERR_INVALID_ID       ;macro already allocated 
        stc                             ;set carry
        jmp     ADM100                  
ADM2:             
        mov     FromWithin,1            ;outside calls to be processed
        mov     DX,AX                   ;MacroId is required in DX
        push    DS
        push    ES                      ;save this
        mov     ES,MacroSeg
        mov     DI,MacroLast            
        cmp     DI,MacroFirst
        jnz     ADM3                    
        mov     BX,FirstMacAddr         ;first macro initialize the address
        mov     ES:[DI].MacroAddress,BX
ADM3:
        mov     ES:[DI].MacroID,AX              ;macro id in ID field
        mov     ES:[DI].MacroSize,CX            ;size field filled in
        mov     ES:[DI].MacroDirty,Clean        ;not check out
        mov     DI,MacroLast                    ;prepare for rep movsb
        pop     DS                              ;get string segment ES

        mov     DI,ES:[DI].MacroAddress
        rep     movsb
        pop     DS                              ;original DS value
        mov     BX,MacroLast
        sub     MacroLast,MacroInfoSize         ;update macrolast
        mov     SI,MacroLast                    ;update next address
        mov     ES:[SI].MacroAddress,DI         ;next macro address
        add     MacroMemUsed,MacroInfoSize      ;update used memory
        mov     CX,ES:[BX].MacroSize
        add     MacroMemUsed,CX
        inc     TotalMacros
        xor     AX,AX                           ;clear for success
        clc                                     ;clear carry
ADM100:
        pop     DI
        pop     SI
        pop     DS
        pop     ES
        ret        
AddMacro        ENDP

;--------VerifyMacroID-----------------------------------
;
;        DESCRIPTION
;               Verifies if macroID is already allocated
;
;        ENTRY
;               AX = MacroID
;
;        EXIT
;              if not present in database
;                       carry reset 
;              else
;                       carry set 
;
;        USES
;              DI
;----------------------------------------------------------
        
VerifyMacroID PROC    NEAR

	clc                      	;optimestically say "not in DB"
        push    ES
        mov     ES,MacroSeg  
        mov     DI,MacroFirst
VM_REPEAT:
	cmp	AX,ES:[DI]		;see if present
	jz	VM_YES			;present
	cmp	DI,MacroLast		;are we at the end of data ?
	jz	VM_NO			;if it is then this ID is OK	
	sub	DI,MacroInfoSize	;next entry in database
	jmp	VM_REPEAT
VM_YES:
	stc  
VM_NO:
        pop     ES
        ret

VerifyMacroID ENDP

;--------DeleteMacro---------------------------------------
;
;        DESCRIPTION
;               Deletes the macro with given MacroID from     
;               the macro library
;
;        ENTRY
;               AX:     MacroID
;
;        EXIT
;               if successful
;                       AX = 0
;		else
;                       AX = Error Code
;
;        USES
;               BX,DX,CX,DI,SI
;
;----------------------------------------------------------

DeleteMacro     PROC    NEAR        

        push    ES
        push    DS
        push    SI
        push    DI

        push    AX                      ;save macroID
        mov     BX,MacroSeg
        mov     ES,BX                   
        mov     SI,MacroFirst
DMC1:
        cmp     SI,MacroLast            ;macro last
        jnz     DMC0                    ;relative jump out of range
        jmp     DMC98                   ;macro not in memory 
DMC0:
        cmp     ES:[SI].MacroID,AX
        jz      DMC3                    ;got it!
        sub     SI,MacroInfoSize        ;go to next one
        jmp     DMC1

DMC3:      
        mov     DX,SI                           ;save this pointer
        mov     AX,MacroFirst
        sub     AX,MacroInfoSize
        cmp     AX,MacroLast                    ;is this only macro
        jz      DMC6                            ;yes, no need to move macro data
        mov     CX,ES:[SI].MacroSize
        mov     DI,ES:[SI].MacroAddress
        mov     SI,DI
        add     SI,CX
        push    SI
        push    DI        

        mov     SI,DX                   ;current macro to be deleted
        xor     CX,CX                   ;text to be moved
        mov     DI,MacroFirst           ;which macros to be moved
        mov     AX,ES:[SI].MacroAddress
        mov     BX,ES:[SI].MacroSize
DMC4:
        cmp     DI,MacroLast                    ;correct way
        jz      DMC5
        cmp     AX,ES:[DI].MacroAddress
        jae     DMC10
        add     CX,ES:[DI].MacroSize       
        sub     ES:[DI].MacroAddress,BX
DMC10:
        sub     DI,MacroInfoSize
        jmp     DMC4            


DMC5: 
        mov     DI,MacroLast
        sub     ES:[DI].MacroAddress,BX         ;adjust the macrolast addr
        pop     DI
        pop     SI              
        push    DS
        mov     BX,ES
        mov     DS,BX
        rep     movsb
        pop     DS           
        
;Next it is time to move the macro info 
DMC6:        
        
        xor     CX,CX
        mov     DI,DX
        mov     BX,ES:[DI].MacroSize    ;get the size of the macro deleted
        mov     SI,DI
DMC7:
        sub     SI,MacroInfoSize                        
        mov     AX,MacroLast            ;see if this is last one
        sub     AX,MacroInfoSize
        cmp     SI,AX
        jz      DMC8
        add     CX,MacroInfoSize
        jmp     DMC7
DMC8:
        add     di,MacroInfoSize - 1
        mov     SI,DI
        sub     SI,MacroInfoSize                                  
        std
        push    DS
        mov     AX,ES
        mov     DS,AX
        rep     movsb
        pop     DS
        cld
        add     MacroLast,MacroInfoSize         ;Got space for one more 
        sub     MacroMemUsed,BX
        sub     MacroMemUsed,MacroInfoSize
        dec     TotalMacros
        xor     AX,AX
        jmp     DMC99
        
DMC98:  
        mov     AX,ERR_INVALID_ID               ;Check this

DMC99:
        pop     DX                      ;retrieve macroID
        pop     DI
        pop     SI
        pop     DS
        pop     ES
        ret
DeleteMacro     ENDP                                       

;--------SortMacroInfo---------------------------------------
;
;        DESCRIPTION
;               Sorts Macro Info When a macro is deleted or 
;               added.                     
;
;        ENTRY     
;               ES:MacroSeg
;
;        EXIT
;               Sorted Macro Info
;
;        USES
;               BX,DX
;----------------------------------------------------------

SortMacroInfo   PROC    NEAR 

        push    ES
        push    SI
        push    DI
        push    CX
        push    AX

        mov     BX,MacroSeg
        mov     ES,BX
        
        mov     SI,MacroFirst
        mov     DI,SI
        cmp     SI,MacroLast
        jz      SMI99
        sub     DI,MacroInfoSize
        cmp     DI,MacroLast
        jz      SMI99                   ;only one macro no sorting!
        mov     CX,TotalMacros
        add     SI,MacroInfoSize        ;for the loop to work 
SMI10:
        sub     SI,MacroInfoSize
        mov     DI,SI
        mov     AX,ES:[SI].MacroID
SMI15:
        sub     DI,MacroInfoSize
        cmp     DI,MacroLast
        jz      SMI20
        cmp     AX,ES:[DI].MacroID
        jbe     SMI15
        call    MacroInfoXchg
SMI20:  loop    SMI10
SMI99:            
        pop     AX   
        pop     CX
        pop     DI
        pop     SI
        pop     ES
        ret
              
SortMacroInfo   ENDP

;--------MacroInfoXchg-------------------------------------
;
;        DESCRIPTION
;               Exchanges two macro info pointers pointed
;               by SI,DI
;                     
;        ENTRY          
;               ES:SI -> First Macro Info
;               ES:DI -> Second Macro Info
;
;        EXIT
;               ES:SI -> Second Macro Info
;               ES:DI -> First Macro Info 
;                                        
;        USES
;               AX
;----------------------------------------------------------

MacroInfoXchg   PROC    NEAR
        
        mov     AX,ES:[SI].MacroID
        xchg    AX,ES:[DI].MacroID
        mov     ES:[SI].MacroID,AX

        mov     AX,ES:[SI].MacroSize
        xchg    AX,ES:[DI].MacroSize
        mov     ES:[SI].MacroSize,AX

        mov     AX,ES:[SI].MacroDirty
        xchg    AX,ES:[DI].MacroDirty
        mov     ES:[SI].MacroDirty,AX

        mov     AX,ES:[SI].MacroAddress
        xchg    AX,ES:[DI].MacroAddress
        mov     ES:[SI].MacroAddress,AX
        ret
MacroInfoXchg   ENDP


;--------CheckOutMacroString-------------------------------
;
;        DESCRIPTION
;               Given the macroID, the address of macro text
;               will be returned with the macro marked "dirty"
;       
;        ENTRY
;               MacroID in C convention on stack
;
;        EXIT
;               if successful
;                       DX:AX -> to the macro text (asciiz) 
;               else
;                       DX:AX = 0:0     (null)       
;        USES
;               BX
;
;----------------------------------------------------------

cProc   CheckOutMacroString,<NEAR, PUBLIC>,<DS,ES,SI,DI>
        parmW   MacID           

cBegin  CheckOutMacroString

        cCall   LockMacroSeg
        or      AX,AX                   ;see if the macroseg exists
        jz      COM98                   ;no it does not 

COM1:   
        mov     AX,MacID
        mov     DI,MacroFirst           
        mov     ES,MacroSeg
COM2:   
        cmp     DI,MacroLast            ;is it the last macro?
        jz      COM98                   ;then error
        cmp     AX,ES:[DI].MacroID
        jz      COM3                    ;got it!
        sub     DI,MacroInfoSize        ;get next macroinfo pointer     
        jmp     COM2                    ;try again
COM3:
        cmp     ES:[DI].MacroDirty,Dirty        
        jz      COM98                           ;already checked out
        mov     ES:[DI].MacroDirty,Dirty        ;mark this as checked out
        mov     DX,MacroSeg                     ;get Macro Segment in DX
        mov     AX,ES:[DI].MacroAddress         ;macro offset in AX
        mov     BX,AX                           ;save AX 
        mov     AX,BX                           ;retrieve AX
        jmp     COM99

COM98:
        xor     AX,AX                    ;Invalid data or already checked out
        xor     DX,DX
COM99:
        cCall   UnlockMacroSeg 

cEnd    CheckOutMacroString

;--------CheckInMacroString--------------------------------
;
;        DESCRIPTION
;               Given the macroID, make it available for someone
;               else to use it.
;               
;        ENTRY
;               Macro ID on stack
;
;        EXIT
;               if successful
;                       AX = MacroID
;               else
;                       AX = 0 (check this out)
;
;        USES
;               
;----------------------------------------------------------

cProc   CheckInMacroString,<NEAR, PUBLIC>,<DS,ES,SI,DI>
        parmW   MacID

cBegin  CheckInMacroString

        cCall   LockMacroSeg
        or      AX,AX
        jnz     CIM1
        mov     AX,ERR_NO_MEMORY
        jmp     CIM00
        
CIM1:
        mov     ES,MacroSeg
        mov     AX,MacID
        mov     DI,MacroFirst
CIM2:      
        cmp     DI,MacroLast
        jz      CIM98                   ;not in the library             
        cmp     ES:[DI].MacroID,AX
        jz      CIM3			;yes exists
        sub     DI,MacroInfoSize
        jmp     CIM2
CIM98:
        mov     AX,ERR_INVALID_ID       ;not in the library
        jmp	CIM00			;exit 
CIM3:
        mov     ES:[DI].MacroDirty,Clean
CIM00:
        cCall   UnlockMacroSeg

             
cEnd    CheckInMacroString

;---------GetMacroInfo-------------------------------------
;
;        DESCRIPTION
;               Given MacroID, the information about the Macro
;               is copied at the address passed by C parameter  
;                     
;        ENTRY
;               MacroID and a Long Pointer to data area for macro
;               info to be copied
;
;        EXIT                   
;               if successful
;                       if lpMacro = Null then return only Size
;                                       and don't copy data
;                       else copy macro text at lpMacro
;               else 
;                       AX = 0
;			DX = Error Code
;                       
;        USES
;               CX,DX     
;               
;----------------------------------------------------------

cProc   GetMacroInfo,<NEAR, PUBLIC>,<ES,DS,DI,SI>
        parmW   MacID
        parmD   lpMacro

cBegin  GetMacroInfo

        cCall   LockMacroSeg
        or      AX,AX
        jnz     GMI2
        mov     DX,ERR_NO_MEMORY
        jmp     GMI99
        
GMI2:
        mov     ES,MacroSeg
        mov     DI,MacroFirst
        mov     AX,MacID
GMI4:
        cmp     AX,ES:[DI].MacroID
        jz      GMI3
        cmp     DI,MacroLast            ;last macro
        jz      GMI98                   ;no more macro to test wrong ID
        sub     DI,MacroInfoSize
        jmp     GMI4
GMI3:
        mov     DX,DI                   ;save the pointer to macroinfo
        mov     AX,SEG_lpMacro          ;see if it null
        or      AX,OFF_lpMacro          ;
        jz      GMI8                    ;yes return only size
        push    ES
        mov     SI,ES:[DI].MacroAddress
        mov     CX,ES:[DI].MacroSize
        mov     ES,SEG_lpMacro
        mov     DI,OFF_lpMacro
        mov     AX,MacID
        mov     ES:[DI],AX              ;load the macroID
        add     DI,SIZE MacroID         ;we get DI to ID and
        add     DI,SIZE MacroSize       ;size and text          
        push    DS
        mov     DS,MacroSeg
        rep     movsb                   ;copy macro text at lpMacro
        pop     DS
        pop     ES

GMI8:                  
        mov     DI,DX
        mov     AX,ES:[DI].MacroSize    ;get the Macro size in AX
        add     AX,Size MacroID         ;we have to return MacroSize+4bytes
        add     AX,Size MacroSize       
        jmp     GMI99
GMI98:
        xor     AX,AX                   ;show this is error
        mov     DX,ERR_INVALID_ID       ;error code in DX
        
GMI99:
        cCall   UnlockMacroSeg
        
cEnd    GetMacroInfo

;--------GetMacroList--------------------------------------
;
;        DESCRIPTION
;               Given the pointer to a data area, list of macroIDs
;               are to be copied in ascending order.    
;
;        ENTRY            
;               dummy           on stack
;               lpMacroList     on stack
;                       
;        EXIT    
;               if lpMacroList = Null 
;                       AX = (number of macros * 2 + 4) bytes
;               else MacroIDs are copied at lpMacroList
;                       AX = (number of macros * 2 + 4) bytes
;
;        USES
;               BX,CX,DX
;----------------------------------------------------------

cProc   GetMacroList,<NEAR, PUBLIC>,<ES,DS,SI,DI>
        parmW   dummy
        parmD   lpMacroList

cBegin  GetMacroList

        cCall   LockMacroSeg
        or      AX,AX
        jnz     GML2
        mov     AX,ERR_NO_MEMORY
        jmp     GML99

GML2:
        mov     ES,MacroSeg
        mov     SI,MacroFirst
        mov     DX,SEG_lpMacroList      ;see if lpMacroList is null
        mov     CX,OFF_lpMacroList
        or      CX,DX           
        jz      GML5                    ;yes only size in AX

        mov     DI,OFF_lpMacroList      
        push    ES                      ;save macroseg
        mov     ES,DX                   ;load with lpMacroList
        mov     AX,TotalMacros
        mov     ES:[DI],AX              ;number of macros at top
        pop     ES
        add     DI,SIZE MacroSize       ;next word of SEG_lpMacroList
        mov     AX,SIZE MacroID         ;set up various values in
        mov     BX,MacroLast            ;registers so that DS is loaded
        mov     CX,MacroInfoSize        ;with SEG_lpMacroList
        push    DS                      ;save DS
        mov     DS,DX                   ;load SEG_lpMacroList              

GML3:
        cmp     SI,BX                   ;is it MacroLast?
        jz      GML4                    ;now get size in AX
        mov     DX,ES:[SI].MacroID      
        mov     DS:[DI],DX              ;move MacroId to OFF_lpMacroList 
        sub     SI,CX                   ;next macro in macroseg
        add     DI,AX                   ;next word in SEG_lpMacroList
        jmp     GML3
GML4:
        pop     DS                      ;retrieve
GML5:
        xor     AX,AX                   ;init for size
        mov     SI,MacroFirst
GML6:
        cmp     SI,MacroLast            ;is it last macro ?
        jz      GML8                    
        add     AX,SIZE MacroSize       
        sub     SI,MacroInfoSize        ;next macro
        jmp     GML6
GML8:
        add     AX,SIZE MacroID         ;add the number macro size also

        cCall   UnlockMacroSeg
        
GML99:
           
cEnd    GetMacroList
           
;--------LockMacroSeg--------------------------------------
;       
;        DESCRIPTION
;               Locks macro segment during the execution of
;               macro if it is a good app.           
;       
;        ENTRY   
;               Nothing
;
;        EXIT          
;               AX = MacroSeg if bad app
;               AX = Address of the locked hMacroSeg
;
;        USES                                       
;               None
;
;----------------------------------------------------------
cProc   LockMacroSeg,<NEAR,PUBLIC>,<ES,DI,DS,SI>

cBegin  LockMacroSeg
        
	mov	AX,hMacroSeg			
	or	AX,AX				;is hMacro valid ?
	jz	LMS99				; No, use MacroSeg
        cmp     [OldApState],SF_NOT_STARTED
        je      LMS1
        test    [pifBehavior],BADBITS
        jnz     LMS99
LMS1:             
        push    BX
        push    CX
        push    DX
        cCall   GlobalLock, <hMacroSeg>
        mov     MacroSeg,DX
        pop     DX
        pop     CX
        pop     BX
LMS99:
	cmp	MacroPara, 0
	je	LMS100
        mov     AX,MacroSeg
LMS100:

cEnd    LockMacroSeg

;--------UnLockMacroSeg------------------------------------
;       
;        DESCRIPTION
;               Unlocks macro segment during the execution of
;               macro if it is a good app.           
;       
;        ENTRY   
;               Nothing
;
;        EXIT          
;               Nothing
;
;        USES                                       
;               None
;
;----------------------------------------------------------
cProc   UnlockMacroSeg,<NEAR,PUBLIC>,<ES,DI,DS,SI>

cBegin  UnlockMacroSeg
        
	cmp	hMacroSeg, 0			
	jz	UNL100				;don't do anything
        cmp     [OldApState],SF_NOT_STARTED
        je      UNL1
        test    [pifBehavior],BADBITS
        jnz     UNL100
UNL1:
	Save	<AX,BX,CX,DX>
        cCall   GlobalUnlock, <hMacroSeg>
UNL100: 

cEnd    UnlockMacroSeg

;--------VerifyString--------------------------------------
;
;        DESCRIPTION 
;               Verifies if the given string is of proper
;               length and is null terminated
;
;        ENTRY  
;               ES:SI  points to the data structure of the 
;               following form:
;               ES:SI   -> wMacroID
;               ES:SI+2 -> wAction
;               ES:SI+4 -> szMacro
;
;        EXIT
;               AX = MacroID 
;               CX = Size of the String in bytes including null
;               ES:SI -> points to the structure
;               
;        USES                                   
;               BX,DI
;
;----------------------------------------------------------
VerifyString    PROC    NEAR

        mov     DI,SI
        mov     AX,ES:[DI]      
        add     DI,4            ;increment to point to the string
        add     SI,4            ;make it point to the string
        xor     CX,CX
VRS1:
        mov     BL, ES:[DI]
        cmp     BL,0            ;is it null        
        jz      VRS2            ;yes, done
        inc     CX               
        inc     DI              ;next char of macro
        jmp     VRS1
VRS2:
        inc     CX              ;include the null also
        ret

VerifyString    ENDP

sEnd    code
        END


