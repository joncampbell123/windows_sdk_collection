TITLE   HANDLER.ASM -- Sample Windows DLL ISR
;****************************************************************************
;
;   PROGRAM: handler.asm
;
;   PURPOSE: Demonstrates installing an interrupt handler under Windows 3.x
;            This ISR (Interrupt service routine) will simply count the
;            number of interrupts it receives.
;
;   FUNCTIONS:
;       SetISRWindow: Called by the application to pass window handle
;       GetISRCount:  Function to retrieve the current count
;       DoInt:        Performs PostMessage() to app upon interrupt
;       InstallHandler: Installs the interrupt handler
;       DeInstallHandler: Removes the interrupt handler
;
;****************************************************************************   

        .286
memM    EQU 1                   ;Medium memory model

        .xlist
        include windows.inc
        include cmacros.inc
        include handler.inc
        .list


nHookVector   EQU 09h           ;hook vect9, IRQ1, keyboard interrupt
DOS_SetVector EQU 2500h
DOS_GetVector EQU 3500h

sBegin  Data
staticD dOldVector,0
staticW nCount,0
staticW hWndApp,0
sEnd

sBegin  Code
        assumes cs,Code
        assumes ds,Data
;****************************************************************************
;  FUNCTION: LibMain(HANDLE, WORD, WORD, LPSTR)
;
;  PURPOSE:  Is called by LibEntry.  LibEntry is called by Windows when
;            the DLL is loaded.  The LibEntry routine is provided in
;            the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
;            source LIBENTRY.ASM is also provided.)  
;
;            LibEntry initializes the DLL's heap, if a HEAPSIZE value is
;            specified in the DLL's DEF file.  Then LibEntry calls
;            LibMain.  The LibMain function below satisfies that call.
;            
;            The LibMain function should perform additional initialization
;            tasks required by the DLL. LibMain should return a value of 1
;            if the initialization is successful.
;          
;****************************************************************************   
cProc LibMain, <FAR,PUBLIC>, <si,di>
parmW	hModule;
parmW   wDataSeg;
parmW   cbHeapSize;
parmD   lpszCmdLine;
        
cBegin  LibMain
        cCall   InstallHandler,<>
        mov     ax,1
cEnd    LibMain


;****************************************************************************
;   FUNCTION:  WEP(int)
;
;   PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
;             called automatically by Windows when the DLL is unloaded (no
;             remaining tasks still have the DLL loaded). 
;
;******************************************************************************/
cProc WEP, <FAR,PUBLIC>, <si,di>
parmW   bSystemExit

cBegin  WEP
        cCall   DeInstallHandler,<>
        mov     ax,1
cEnd    WEP


;****************************************************************************
;   FUNCTION:  SetISRWindow(HWND)
;
;   PURPOSE:  This routine receives the handle to the window that should
;             receive ISRM_RUPT messages.
;
;****************************************************************************   
cProc SetISRWindow, <FAR,PUBLIC>, <si,di>
parmW   hWnd
cBegin  SetISRWindow
        mov     ax,hWnd
        mov     hWndApp,ax
cEnd    SetISRWindow



;****************************************************************************
;   FUNCTION:  GetISRCount()
;
;   PURPOSE:  This function simply returns the value of nCount to the
;             caller.
;
;****************************************************************************   
cProc GetISRCount, <FAR,PUBLIC>, <si,di>
cBegin  GetISRCount 
        mov     ax,nCount
cEnd    GetISRCount 


;****************************************************************************
;   FUNCTION:  DoInt()
;
;   PURPOSE:  This routine is called by the ISR in the InstallHandler
;             routine. Note that this routine is EXPORTED, so when it
;             is called, DS is loaded with the default data segment of
;             this DLL. Thus, the routine has access to dOldVector, so
;             it can call the next interrupt handler in the chain.
;
;
;****************************************************************************   
cProc DoInt, <FAR,PUBLIC>, <si,di>
cBegin  DoInt

        pushf
        call    DWORD PTR dOldVector ; call previous handler

        inc     nCount
        cmp     hWndApp,0
        jz      doiexit

        mov     bx,ISRM_RUPT
        sub     ax,ax
        cCall   PostMessage,<hWndApp,bx,nCount,ax,ax>

doiexit:

cEnd    DoInt


;****************************************************************************
;   FUNCTION:  InstallHandler()
;
;   PURPOSE:  This routine saves the interrupt vector "nHookVector" in
;             the global variable "dOldVector". Then, it installs a small
;             ISR at that vector which calls the routine "DoInt()" when
;             the interrupt occurs.
;          
;
;****************************************************************************   
cProc InstallHandler, NEAR, <si,di>
cBegin  InstallHandler

        push    bx                      ;Save previous vector
        push    es
        mov     ax,DOS_GetVector + nHookVector
        int     21h
        mov     WORD PTR dOldVector,bx
        mov     WORD PTR dOldVector+2,es
        pop     es
        pop     bx


        push    ds                      ;Install handler
        push    dx                      ;
        push    cs
        pop     ds
        mov     dx,OFFSET MyISR
        mov     ax,DOS_SetVector + nHookVector
        int     21h
        pop     dx      ;
        pop     ds      ;

        jmp     set_exit

        ; ****  Entry point of ISR  ****
MyISR:                                  ;Our ISR
            pusha
            push    ds
            push    es

            cCall   DoInt               ;Do Interrupt Handling

            pop     es
            pop     ds
            popa
            iret                        ;exit MyISR
                
set_exit:                               ;exit InstallHandler                    

cEnd    InstallHandler


;****************************************************************************
;   FUNCTION:  DeInstallHandler()
;
;   PURPOSE:  Restores the interrupt vector "nHookVector" with the address
;             at "dOldVector".
;
;****************************************************************************   
cProc DeInstallHandler, NEAR, <si,di>
cBegin  DeInstallHandler

        push    ds                      ;
        push    dx                      ;
        mov     dx,WORD PTR dOldVector
        mov     ax,WORD PTR dOldVector+2
        cmp     dx,0                    ;were we installed?
        jnz     dih_go
        cmp     ax,0
        jz      dih_skip
dih_go:
        mov     ds,ax
        mov     ax,DOS_SetVector + nHookVector
        int     21h
dih_skip:
        pop     dx      ;
        pop     ds      ;

cEnd    DeInstallHandler

sEnd
        end
