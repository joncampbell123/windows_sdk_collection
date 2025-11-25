#include <windows.h>

#include "hellowin.h"

ULONG   Filters[6]={0,
                    NDIS_PACKET_TYPE_DIRECTED,
                    NDIS_PACKET_TYPE_MULTICAST,
                    NDIS_PACKET_TYPE_BROADCAST,
                    NDIS_PACKET_TYPE_ALL_MULTICAST,
                    NDIS_PACKET_TYPE_PROMISCUOUS};

static  char     szbuff[40];

char    szChildAppName[]="hexdump";


HFONT   hFont;
HWND    hwndchild;

HINSTANCE  hInst;

ADAPTER Adapter;

UINT    showdump=0;

LRESULT FAR PASCAL WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT FAR PASCAL ChildWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


HFONT GetFont(void);

DWORD GetTextSize(HWND hWnd, HFONT hFont);

LRESULT
HandleCommands(
    HWND     hWnd,
    UINT     uMsg,
    UINT     wParam,
    LONG     lParam
    );

BOOLEAN
CreateAdapter(
    IN PADAPTER  pAdapter
    );

BOOLEAN
CloseAdapter(
    IN PADAPTER  pAdapter
    );


int PASCAL WinMain (HINSTANCE hInstance,
		    HINSTANCE hPrevInstance,
                    LPSTR     lpszCmdParam,
                    int       nCmdShow)




  {
    static    char szAppName[]="HelloWin";
    HWND      hwnd;
    MSG       msg;
    WNDCLASS  wndclass;

    hInst=hInstance;

    if (!hPrevInstance)
       {
         wndclass.style        =  CS_HREDRAW | CS_VREDRAW;
         wndclass.lpfnWndProc  =  WndProc;
         wndclass.cbClsExtra   =  0;
         wndclass.cbWndExtra   =  0;
         wndclass.hInstance    =  hInstance;
         wndclass.hIcon        =  LoadIcon (NULL, IDI_APPLICATION);
         wndclass.hCursor      =  LoadCursor(NULL, IDC_ARROW);
         wndclass.hbrBackground=  GetStockObject(WHITE_BRUSH);
	 wndclass.lpszMenuName =  "GenericMenu";
	 wndclass.lpszClassName=  szAppName;

         RegisterClass(&wndclass);
       }


    if (!hPrevInstance)
       {
         wndclass.style        =  CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS |
                                  CS_BYTEALIGNCLIENT;
         wndclass.lpfnWndProc  =  ChildWndProc;
         wndclass.cbClsExtra   =  0;
         wndclass.cbWndExtra   =  0;
         wndclass.hInstance    =  hInstance;
         wndclass.hIcon        =  LoadIcon (hInstance,MAKEINTRESOURCE(1000));
         wndclass.hCursor      =  LoadCursor(NULL, IDC_ARROW);
         wndclass.hbrBackground=  GetStockObject(WHITE_BRUSH);
	 wndclass.lpszMenuName =  NULL;
	 wndclass.lpszClassName=  szChildAppName;

         RegisterClass(&wndclass);

       }



    hwnd = CreateWindow (szAppName,
                         "the hello world program",
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,
                         NULL,
                         NULL,
                         hInstance,
                         NULL);

    ShowWindow (hwnd,nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage (&msg, NULL, 0,0))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

    return (msg.wParam);
  }

LRESULT FAR PASCAL WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

  {
    HDC       hdc;
    PAINTSTRUCT  ps;
    RECT         rect;


    switch (message)
      {
        case WM_COMMAND:
            HandleCommands(hwnd, message, wParam, lParam);
            return 0;

        case WM_LBUTTONDOWN:
          if (!showdump) {
               hwndchild = CreateWindow (szChildAppName,
                               "Hex Dump",
                               WS_CHILD | WS_CAPTION | WS_SYSMENU | WS_VSCROLL |
                               WS_VISIBLE |  WS_THICKFRAME | WS_CLIPSIBLINGS,
                               0,0,
                               300,100,
                               hwnd,
                               (HMENU) 1,
                               hInst,
                               NULL);
          } else {

              SendMessage(hwndchild,WM_DUMPCHANGE,0,0l);
          }

          showdump=1;
          return 0;



        case WM_CREATE:

            /* Open a handle to the SC Manager database. */

            Adapter.schSCManager = OpenSCManager(
                                         NULL,                   /* local machine           */
                                         NULL,                   /* ServicesActive database */
                                         SC_MANAGER_ALL_ACCESS); /* full access rights      */

            if (Adapter.schSCManager == NULL) {

                MessageBox(NULL,TEXT("Could not open SC"),TEXT("Hellwin"),MB_OK);

            } else {

                Adapter.ServiceHandle=OpenService(Adapter.schSCManager,
                                                  TEXT("Packet"),
                                                  SERVICE_START);

                if (Adapter.ServiceHandle == NULL) {

                    MessageBox(NULL,TEXT("Could not open service"),TEXT("Hellwin"),MB_OK);

                }

                StartService(Adapter.ServiceHandle,
                             0,
                             NULL
                             );

            }


          lstrcpy(Adapter.AdapterName,
                  "\\\\.\\Packet");

          Adapter.BufferSize=1514;

          Adapter.hMem=GlobalAlloc(GMEM_MOVEABLE,1514);

          Adapter.lpMem=GlobalLock(Adapter.hMem);

          Adapter.hMem2=GlobalAlloc(GMEM_MOVEABLE,1514);

          Adapter.lpMem2=GlobalLock(Adapter.hMem2);


          return 0;



        case WM_KEYDOWN:



          return 0;

	case WM_PAINT:
          hdc=BeginPaint(hwnd,&ps);
          GetClientRect (hwnd,&rect);

	  EndPaint(hwnd,&ps);
          return 0;


	case WM_DESTROY:
          PostQuitMessage(0);
          return 0;
      }
    return DefWindowProc(hwnd,message, wParam, lParam);
  }





LRESULT
HandleCommands(
    HWND     hWnd,
    UINT     uMsg,
    UINT     wParam,
    LONG     lParam
    )

{
    ULONG    Filter;
    ULONG    BytesReturned;
    BOOLEAN  Result;


    UINT     i;


    OVERLAPPED   OverLapped;

    switch (wParam) {

        case IDM_OPEN:

            Result=CreateAdapter(&Adapter);

            if (Result) {

                SetWindowText(hWnd,Adapter.AdapterName);

            }


            return TRUE;


        case IDM_CLOSE:

            CloseHandle(Adapter.hFile);

            return TRUE;


        case  IDM_NO_FILTER:

        case  IDM_DIRECTED:

        case  IDM_MULTICAST:

        case  IDM_BROADCAST:

        case  IDM_ALL_MULTICAST:

        case  IDM_PROMISCUOUS:


            Filter=Filters[wParam-IDM_NO_FILTER];

            DeviceIoControl(Adapter.hFile,
                            (DWORD)IOCTL_PROTOCOL_SET_FILTER,
                            &Filter,
                            4,
                            NULL,
                            0,
                            &BytesReturned,
                            NULL);





            return TRUE;

        case IDM_RESET:

            DeviceIoControl(Adapter.hFile,
                            (DWORD)IOCTL_PROTOCOL_RESET,
                            &Filter,
                            4,
                            NULL,
                            0,
                            &BytesReturned,
                            NULL);





            return TRUE;



        case IDM_READ:



          OverLapped.Offset=0;
          OverLapped.OffsetHigh=0;
          OverLapped.hEvent=Adapter.hEvent;


          Result=ReadFile(Adapter.hFile,
                        Adapter.lpMem,
                        1500,
                        &BytesReturned,
                        &OverLapped);


          return TRUE;


        case IDM_SEND:

          OverLapped.Offset=0;
          OverLapped.OffsetHigh=0;
          OverLapped.hEvent=Adapter.hEvent;



          Adapter.lpMem2[0]=(UCHAR)0xff;
          Adapter.lpMem2[1]=(UCHAR)0xff;
          Adapter.lpMem2[2]=(UCHAR)0xff;
          Adapter.lpMem2[3]=(UCHAR)0xff;
          Adapter.lpMem2[4]=(UCHAR)0xff;
          Adapter.lpMem2[5]=(UCHAR)0xff;

          for (i=0;i<5;i++) {

              Result=WriteFile(Adapter.hFile,
                        Adapter.lpMem2,
                        64,
                        &BytesReturned,
                        &OverLapped);
          }


          return TRUE;


        default:

            return 0;

    }


}




BOOLEAN
CreateAdapter(
    IN PADAPTER  pAdapter
    )

{

    pAdapter->hEvent=CreateEvent(NULL,
                       FALSE,
                       FALSE,
                       "Protocol_Event");

    if (pAdapter->hEvent == NULL) {
        return 0;
    }

    pAdapter->hFile=CreateFile(pAdapter->AdapterName,
                    GENERIC_WRITE | GENERIC_READ,
                    0,
                    NULL,
                    CREATE_ALWAYS,
                    FILE_FLAG_OVERLAPPED,
                    0
                    );

    if (pAdapter->hFile == INVALID_HANDLE_VALUE) {
        CloseHandle(pAdapter->hEvent);
        return 0;
    }




    return TRUE;

}



BOOLEAN
CloseAdapter(
    IN PADAPTER  pAdapter
    )

{

    CloseHandle(pAdapter->hFile);

    CloseHandle(pAdapter->hEvent);

    pAdapter->hFile=NULL;

    return TRUE;
}








LRESULT FAR PASCAL ChildWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

  {
    HDC           hdc;
    PAINTSTRUCT   ps;
    RECT          rect;
    UINT          i,j;
    char          szbuff[100],szbuff2[8],szbuff3[17];
    UINT          limit;
    LPSTR         lpMem;
static HFONT  hOldFont;
static UINT       cyclient,cxclient,lines,dumppos,oldpos,paragphs,cychar;
static WORD   start;

    switch (message)
      {
        case WM_CREATE:
          cychar = HIWORD(GetTextSize(hwnd,hFont));


        case WM_DUMPCHANGE:
          oldpos=0;
          dumppos=0;
          wsprintf(szbuff,"Hex Dump--");
          SetWindowText(hwnd,szbuff);

          limit=Adapter.BufferSize;

          paragphs=(short)((limit & 0xffff) >> 4);
          SetScrollRange(hwnd,SB_VERT,0,(WORD)paragphs,0);
          SetScrollPos(hwnd,SB_VERT,(WORD)dumppos,1);

          InvalidateRect(hwnd,NULL,1);
          return 0;

        case WM_SIZE:
          cyclient=HIWORD(lParam);
          cxclient=LOWORD(lParam);
          lines=cyclient/cychar;
          return 0;


	case WM_PAINT:
          hdc=BeginPaint(hwnd,&ps);
          hOldFont=SelectObject(hdc,hFont);

          if (1)
             {
               limit=Adapter.BufferSize;


               limit=limit & 0xffff;

               lpMem=(LPSTR)Adapter.lpMem;

               start=dumppos*cychar;

               for (i=start ;((i<limit) && ((i-start)/cychar<lines));i+=16)
                 {
                   wsprintf(szbuff,"%08lx  ",i);
                   for (j=0;(j<16 && (j+i<limit));j++)
                     {
                       wsprintf(szbuff2,"%02hX ",((WORD)lpMem[i+j] & 0xff));
                       lstrcat(szbuff,szbuff2);
                       if (j==3 || j==7 || j==11)
                          lstrcat(szbuff," ");
                       wsprintf(&szbuff3[j],"%c",lpMem[i+j]);
                       if (szbuff3[j]=='\0')
                          szbuff3[j]='.';
                     }
                   szbuff3[j]='\0';
                   lstrcat(szbuff,szbuff3);
                   TextOut(hdc,0,i-start,szbuff,strlen(szbuff));
                 }
              }


          SelectObject(hdc,hOldFont);
	  EndPaint(hwnd,&ps);
          return 0;

        case WM_VSCROLL:
          oldpos=dumppos;
          switch(wParam)
            {
              case SB_PAGEDOWN:
                dumppos+=lines;
                break;

              case SB_LINEDOWN:
                dumppos+=1;
                break;

              case SB_THUMBPOSITION:
                dumppos=LOWORD(lParam);
                break;

              case SB_LINEUP:
                dumppos-=1;
                break;

              case SB_PAGEUP:
                dumppos-=lines;
                break;

              default:
                return 0;
            }

          limit=Adapter.BufferSize;

          paragphs=(short)((limit & 0xffff) >> 4);

          if (dumppos<0)
             dumppos=0;
           else
              if (dumppos>paragphs)
                 dumppos=paragphs;

          if (((oldpos-dumppos==1) || (dumppos-oldpos==1)))
             {
               rect.left=0;
               rect.top=0;
               rect.right=cxclient;
               rect.bottom=lines*cychar;
               ScrollWindow(hwnd,0,cychar*(oldpos-dumppos),&rect,NULL);
               UpdateWindow(hwnd);
             }
           else
             InvalidateRect(hwnd,NULL,1);

          SetScrollPos(hwnd,SB_VERT,(WORD)dumppos,1);
          return 0;


      }
    return DefWindowProc(hwnd,message, wParam, lParam);
  }


HFONT GetFont(void)
  {
    static LOGFONT logfont;

    logfont.lfHeight=16;
    logfont.lfCharSet=ANSI_CHARSET;
    logfont.lfQuality=PROOF_QUALITY;
    logfont.lfPitchAndFamily=FIXED_PITCH | FF_MODERN;
    lstrcpy((LPSTR)&logfont.lfFaceName,(LPSTR)"Courier");

    return CreateFontIndirect(&logfont);
  }

DWORD GetTextSize(HWND hWnd, HFONT hFont)
  {
    TEXTMETRIC     Metrics;
    HDC            hDC;
    HFONT          hOldFont;

    hDC = GetDC(hWnd);
    hOldFont=SelectObject(hDC,hFont);
    GetTextMetrics(hDC,&Metrics);
    SelectObject(hDC,hOldFont);
    ReleaseDC(hWnd,hDC);

    return MAKELONG(Metrics.tmAveCharWidth,
                    Metrics.tmHeight+Metrics.tmExternalLeading);

  }
