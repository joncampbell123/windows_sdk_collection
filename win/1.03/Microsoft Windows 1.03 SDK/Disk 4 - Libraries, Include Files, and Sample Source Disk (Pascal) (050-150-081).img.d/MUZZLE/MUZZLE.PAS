{$windows+}
{$stackseg+ $debug- $symtab-}

INTERFACE;
    UNIT paslibw (
        {$INCLUDE : 'muzzle.inc'}
    );
{$INCLUDE:'windows.inc'}                 { Get WINDOWS definitions. }

MODULE muzzle[];
    USES paslibw (
        {$INCLUDE : 'muzzle.inc'}
    );

CONST
    FILE_AND_HEAP = 1;
    DUMMY = 0;

{To test the file system set FILE_AND_HEAP to be 1.
 Use 0 for the normal case.}

{$IF FILE_AND_HEAP $THEN}

{The stuff in the next sections is test code to show that files and
 the heap work.}

TYPE
    pMyRec = ^MyRec;
    MyRec = RECORD
        i, j : INTEGERC;
    END;

VAR
    MyFile : TEXT;
    MyInteger : INTEGERC;
    v1MyRec, v2MyRec : pMyRec;

{$END}

CONST
    STIR           =   20;      { flag for menu event }
    IDM_ABOUT      =    1;      { addition to system menu message }
    ABOUTBOX       =    1;      { aboutbox dialog resource id }
    XCOUNT         =    4;      { number of boxes in puzzle }
    YCOUNT         =    4;


{ Global variables }

VAR
    xExt        : INT;              { size of puzzle boxes }
    yExt        : INT;
    Bx          : INT;              { origin of puzzle }
    By          : INT;
    xscr,yscr   : INT;              { physical x,y dimension of screen }
    xdepth      : INT;              { depth of x 3-d effects }
    depth       : INT;              { depth of 3-d effects }
    xc,yc       : INT;              { the lower right corner of puzzle}
    xLineExt    : INT;              { length of lines across the puzzle }
    yLineExt    : INT;
    MTi         : INT;              { index of empty square}
    MTj         : INT;
    CharWidth   : INT;              { dimensions of characters }
    CharHeight  : INT;
    CharAscent  : INT;
    Aspect      : INT;              { inverse aspect ratio }
    Color       : BOOL;             { indicates if color can be used}
    A           : ARRAY[0..XCOUNT-1] OF ARRAY [0..YCOUNT-1] OF INT;
    seed        : LONG;             { variable used for random numbers }
    OrderCheck  : BOOL;             { flag to check order of puzzle }
    hbrGray     : HBRUSH;           { define shading used }
    hbrBlack    : HBRUSH;
    hbrWhite    : HBRUSH;
    hbrLtGray   : HBRUSH;
    hbrBlue     : HBRUSH;
    hbrGreen    : HBRUSH;
    hbrBack     : HBRUSH;
    penWhite    : HPEN;
    penBlack    : HPEN;
    StirOrder   : ARRAY[0..35] OF BYTE;
    lpprocAbout : FARPROC;          { Aboutbox procedure }
    hInst       : HANDLE;           { Instance handle for dialogbox }
    strBuf      : ARRAY[0..3] OF CHAR;


PROCEDURE MovePiece( hWindow : HWND; Si, Sj : INT ); FORWARD;
FUNCTION TICS : WORD; EXTERN;
FUNCTION lstrlen( s : LPSTR ) : INT; EXTERN;
PROCEDURE begxqq; EXTERN;
PROCEDURE endxqq; EXTERN;
PROCEDURE _exit( i : WORD ); EXTERN;
PROCEDURE ENTGQQ [ PUBLIC ];
    BEGIN
    { a dummy routine to pacify link4 }
    END;


PROCEDURE Myitoa( val : INT );

    { A kludge to convert integers 1 to 15.  _itoa in LLIBC doesn't work }

    BEGIN

    IF ( val < 10 ) THEN
        BEGIN
        strBuf[0] := CHR( val + 48 );
        strBuf[1] := CHR( 0 );
        END
    ELSE
        BEGIN
        strBuf[0] := CHR( 49 );
        strBuf[1] := CHR( val-10 + 48 );
        strBuf[2] := CHR( 0 );
        END;

    END; { Myitoa }


FUNCTION CheckOrder : BOOL;

    { checks to see if puzzle is in order }

    VAR i, j : INT;

    BEGIN

    CheckOrder := FALSE_;

    FOR j := 0 TO YCOUNT-1 DO
        FOR i := 0 TO XCOUNT-1 DO
            IF ( A[i][j] <> (1 + j * XCOUNT + i) ) THEN RETURN;

    CheckOrder := TRUE_;

    END; { CheckOrder }


PROCEDURE AutoMove( hWindow : HWND;
                    k       : INT ) [ PUBLIC ];

    { moves one of the puzzle pieces surrounding the empty
      square determined by the argument k
    }

    VAR Si, Sj : INT;

    BEGIN

    { determine piece to be moved }

    CASE k OF
        1 { bottom }: BEGIN Si := 0;  Sj :=-1; END;
        2 { left   }: BEGIN Si :=-1;  Sj := 0; END;
        3 { right  }: BEGIN Si := 0;  Sj := 1; END;
        4 { top    }: BEGIN Si := 1;  Sj := 0; END;
        END;

    { move the piece }

    Si := Si + MTi;
    Sj := Sj + MTj;
    MovePiece( hWindow, Si, Sj );

    END; { AutoMove }


PROCEDURE seedrnd;

    { seed the random number generator }

    BEGIN

    seed := MAKELONG( TICS, TICS );

    END; { seedrnd }


FUNCTION random : UNSIGNED;

    { produce a 16 bit random number }

    BEGIN

    seed := (3431 * seed + 13367) MOD 65280;

    { remove least significant digits since they are not
      random for modulo operations
    }

    random := RETYPE(UNSIGNED, LOWORD(seed DIV 100) );

    END; { random }


FUNCTION FindDirec( first, second : INT ) : INT;

    VAR offset : INT;

    BEGIN

    offset := first - second;

    IF ( offset < 0 ) THEN FindDirec :=-1
    ELSE
    IF ( offset > 0 ) THEN FindDirec := 1
    ELSE                   FindDirec := 0;

    END; { FindDirec }


PROCEDURE PutNum( hDC_       : HDC;
                  xBox, yBox : INT;     { index of puzzle box }
                  val        : INT );   { number to be written in box }

    { this procedure places the number value in the puzzle box indicated
      by the index xBox,yBox.
    }

    VAR
        len    : INT;       { length of string }
        offset : INT;       { offset from center where character should be put }

    BEGIN

    { Convert number to string }

    Myitoa( val );
    len := lstrlen( RETYPE(LPSTR, ADS strBuf) );

    { Center number in box and write out }

    offset := CharWidth * len DIV 2;
    EVAL( TextOut( hDC_,
                   Bx + xBox*xExt + xExt DIV 2 - offset,
                   By + yBox*yExt + yExt DIV 2 - CharAscent DIV 2,
                   RETYPE(LPSTR, ADS strBuf),
                   len ) );

    END; { PutNum }


PROCEDURE FillSqu( hDC_       : HDC;
                   xBox, yBox : INT );

    { fills the puzzle piece index indicated by xBox,yBox with the
      most recently selected brush
    }

    BEGIN

    EVAL( PatBlt( hDC_,
                  Bx + xBox * xExt + 1,
                  By + yBox * yExt + 1,
                  xExt - 1,
                  yExt - 1,
                  PATCOPY ) );

    END; { FillSqu }


PROCEDURE DrawEmpty( hDC_       : HDC;
                     xBox, yBox : INT );

    { draws the empty box space}

    VAR i, j : INT;

    BEGIN

    i := Bx + xBox * xExt;
    j := By + yBox * yExt;

    { draw in 3-d edges }

    EVAL( SelectObject( hDC_, penBlack ) );
    EVAL( MoveTo( hDC_, i+depth*Aspect, j+yExt  ) );
    EVAL( LineTo( hDC_, i+depth*Aspect, j+depth ) );
    EVAL( LineTo( hDC_, i+xExt,         j+depth ) );
    EVAL( MoveTo( hDC_, i,              j       ) );
    EVAL( LineTo( hDC_, i+depth*Aspect, j+depth ) );

    { draw in bottom of empty square }

    IF ( Color = TRUE_ )
        THEN EVAL( SelectObject( hDC_, hbrGreen ) )
        ELSE EVAL( SelectObject( hDC_, hbrLtGray ) );

    EVAL( PatBlt( hDC_,
                  i + depth * Aspect + 1,
                  j + depth + 1,
                  xExt - depth * Aspect - 1,
                  yExt - depth - 1,
                  PATCOPY ) );

    END; { DrawEmpty }


PROCEDURE MovePiece;

    { This procedure attempts to move the puzzle piece indicated by Si,Sj.
      Nothing is returned if the move was not successful.  The piece will
      be moved into the empty space
    }

    VAR hDC_      : HDC;
        i         : INT;
        direction : INT;    { direction of cursor relative to empty square }

    BEGIN

    { insure that click is in a valid area }

    IF ( (Si>=0) AND (Si<XCOUNT) AND (Sj>=0) AND (Sj<YCOUNT) ) THEN
        BEGIN

        { make sure that the cursor isn't on the empty box}

        IF ( (MTj = Sj) AND (MTi = Si) ) THEN RETURN;

        { make sure that empty box is close by }

        IF ( (MTj = Sj) OR (MTi = Si) ) THEN
            BEGIN
            hDC_ := GetDC( hWindow );
            EVAL( SetBkMode( hDC_, OPAQUE ) );
            EVAL( SelectObject( hDC_, hbrWhite ) );
            EVAL( SelectObject( hDC_, GetStockObject( ANSI_FIXED_FONT ) ) );

            { check for empty square in vertical direction }

            IF ( MTj = Sj ) THEN
                BEGIN
                direction := FindDirec( Si, MTi );

                { move all puzzle pieces in direction of empty box }

                i := MTi;
                WHILE ( i <> Si ) DO
                    BEGIN
                    A[i][Sj] := A[i + direction][Sj];

                    { redraw moved puzzle piece}

                    FillSqu( hDC_, i, Sj );
                    PutNum( hDC_, i, Sj, A[i][Sj] );

                    i := i + direction;
                    END;
                END;

            { check for empty square in horizontal direction }

            IF ( MTi = Si ) THEN
                BEGIN
                direction := FindDirec( Sj, MTj );
                i := MTj;
                WHILE ( i <> Sj ) DO
                    BEGIN
                    A[Si][i] := A[Si][i + direction];
                    FillSqu( hDC_, Si, i );
                    PutNum( hDC_, Si, i, A[Si][i] );
                    i := i + direction;
                    END;
                END;

            { reset where the empty slot is }

            MTi := Si;
            MTj := Sj;

            { define empty slot to be 16 for checking purposes }

            A[Si][Sj] := 16;
            DrawEmpty( hDC_, MTi, MTj );

            { check if puzzle is in order }

            IF ( OrderCheck = TRUE_ ) THEN
                BEGIN
                OrderCheck := FALSE_;

                { display message if puzzle solved }

                IF ( CheckOrder = TRUE_ ) THEN
                    EVAL( MessageBox( hWindow,
                                      RETYPE(LPSTR, ADS 'Congratulations!' * CHR(0)),
                                      RETYPE(LPSTR, ADS 'Muzzle' * CHR(0)),
                                      MB_OK ) );
                END;

            EVAL( ReleaseDC( hWindow, hDC_ ) );
            END;
        END;

    END; { MovePiece }


FUNCTION About ( hDlg    : HWND;
                 message : UNSIGNED;
                 wParam  : WORD;
                 lParam  : LONG ) : BOOL [ PUBLIC, WINDOWS ];
    BEGIN

    IF ( message = WM_COMMAND ) THEN
        BEGIN
        EndDialog( hDlg, TRUE_ );
        About := TRUE_;
        RETURN;
        END
    ELSE IF ( message = WM_INITDIALOG ) THEN
        BEGIN
        About := TRUE_;
        RETURN;
        END
    ELSE
        BEGIN
        About := FALSE_;
        RETURN;
        END;

    END; { About }


PROCEDURE MuzzleCreate( hWindow : HWND ) [ PUBLIC ];

    VAR i,j      : INT;
        hDC_     : HDC;
        charsize : TEXTMETRIC;  { characteristics of the characters }
        str      : ARRAY[1..11] OF CHAR;

    BEGIN

    { Initialize location of empty box }

    MTi := XCOUNT - 1;
    MTj := YCOUNT - 1;

    { Get device-dependent info }

    hDC_ := GetDC( hWindow );
    EVAL( SelectObject( hDC_, GetStockObject( ANSI_FIXED_FONT ) ) );
    EVAL( GetTextMetrics( hDC_, RETYPE(LPTEXTMETRIC, ADS charsize) ) );
    EVAL( ReleaseDC( hWindow, hDC_ ) );

    { determine the inverse aspect ratio }

    IF ( GetDeviceCaps( hDC_, VERTRES ) = 200 )
        THEN Aspect := 2
        ELSE Aspect := 1;

    { determine if a color device is present }

    IF ( GetDeviceCaps( hDC_, NUMPENS ) < 3 ) THEN
        BEGIN
        Color := FALSE_;
        hbrBack := hbrGray;
        END
    ELSE
        BEGIN
        Color := TRUE_;
        hbrBack := hbrBlue;
        END;

    { determine dimensions of characters
      these need to be scaled to user coords if transformation routines
      are used
    }

    CharHeight := charsize.tmHeight + charsize.tmExternalLeading;
    CharWidth  := charsize.tmAveCharWidth;
    CharAscent := charsize.tmAscent;

    { initialize puzzle to be in order }

    FOR j := 0 TO YCOUNT-1 DO
        FOR i := 0 TO XCOUNT-1 DO
            A[i][j] := 1 + j * XCOUNT + i;

    { set up mix-up order }

    StirOrder[0] := 2;  StirOrder[1] := 2;  StirOrder[2] := 2;
    StirOrder[3] := 1;  StirOrder[4] := 1;  StirOrder[5] := 1;
    StirOrder[6] := 4;  StirOrder[7] := 4;  StirOrder[8] := 3;
    StirOrder[9] := 3;  StirOrder[10]:= 2;  StirOrder[11]:= 2;
    StirOrder[12]:= 1;  StirOrder[13]:= 4;  StirOrder[14]:= 3;
    StirOrder[15]:= 3;  StirOrder[16]:= 2;  StirOrder[17]:= 1;
    StirOrder[18]:= 4;  StirOrder[19]:= 4;  StirOrder[20]:= 4;
    StirOrder[21]:= 3;  StirOrder[22]:= 2;  StirOrder[23]:= 2;
    StirOrder[24]:= 1;  StirOrder[25]:= 1;  StirOrder[26]:= 4;
    StirOrder[27]:= 4;  StirOrder[28]:= 3;  StirOrder[29]:= 2;
    StirOrder[30]:= 1;  StirOrder[31]:= 1;  StirOrder[32]:= 4;
    StirOrder[33]:= 3;  StirOrder[34]:= 3;  StirOrder[35]:= 3;

    { StirOrder is used to determine the sequence of moves made when
      the menu item Mix-Up is invoked.  The above sequence moves the
      empty square through all the boxes in the puzzle.
    }

    {$IF FILE_AND_HEAP $THEN BEGIN}

    SetWindowText( hWindow, RETYPE(LPSTR, ADS 'Muzzle' * CHR(0)) );
    str[1] := 'T';
    str[2] := 'e';
    str[3] := 's';
    str[4] := 't';
    str[5] := ' ';

    REWRITE( MyFile );
    WRITE( MyFile, 1, 2, 3 );
    RESET( MyFile );

    READ( MyFile, MyInteger );
    IF ( MyInteger = 1 )
        THEN str[6] := 'G'
        ELSE str[6] := 'B';

    READ( MyFile, MyInteger );
    IF ( MyInteger = 2 )
        THEN str[7] := 'G'
        ELSE str[7] := 'B';

    READ( MyFile, MyInteger );
    IF ( MyInteger = 3 )
        THEN str[8] := 'G'
        ELSE str[8] := 'B';

    NEW( v1MyRec );
    v1MyRec^.i := 2;
    v1MyRec^.j := 3;
    v2MyRec := v1MyRec;
    IF ( v2MyRec^.j = 3 )
        THEN str[9] := 'G'
        ELSE str[9] := 'B';

    IF ( v2MyRec^.j = v1MyRec^.j )
        THEN str[10] := 'G'
        ELSE str[10] := 'B';

    str[11] := CHR(0);
    DISPOSE( v2MyRec );

    SetWindowText( hWindow, RETYPE(LPSTR, ADS str) );

    {$END}

    END; { MuzzleCreate }


PROCEDURE MuzzleSize ( hWindow : HWND ) [ PUBLIC ];

    VAR
        Tx, Ty       : INT;         { Total size of available window }
        TyMin, TxMin : INT;         { minimum size of puzzle }
        rcSize       : RECT;        { size of window }

    BEGIN

    { determine size of window }

    GetClientRect( hWindow, RETYPE(LPRECT, ADS rcSize) );
    xscr := rcSize.right - rcSize.left;
    yscr := rcSize.bottom - rcSize.top;
    Ty := yscr;
    Tx := Ty * Aspect;

    { set minimum size }

    TyMin := (CharHeight * 2 + 1) * (YCOUNT + 1) + 1;
    TxMin := (CharWidth  * 2 + 1) * (XCOUNT + 1) + 1;

    { make sure that puzzle will fit in x dimension}

    IF ( Tx > xscr ) THEN
        BEGIN
        Ty := xscr DIV Aspect;
        Tx := xscr;
        END;

    { Don't go below minimum size }

    IF ( Tx < TxMin ) THEN
        BEGIN
        Tx := TxMin;
        Ty := TxMin DIV Aspect;
        END;

    IF ( Ty < TyMin ) THEN
        BEGIN
        Ty := TyMin;
        Tx := TyMin * Aspect;
        END;

    { define constants }

    xExt := Tx DIV (XCOUNT + 1);    { height-width of number box }
    yExt := Ty DIV (YCOUNT + 1);
    xLineExt := XCOUNT * xExt;      { height-width of puzzle}
    yLineExt := YCOUNT * yExt;

    { define origin of puzzle }

    depth := yLineExt DIV 30;

    Bx := (xscr - xLineExt) DIV 2;  { upper-left corner of puzzle}
    IF ( Bx < 0 ) THEN Bx := 0;     { minimum origin is 0,0 }

    By := (yscr - yLineExt) DIV 2;
    IF ( By < 0 ) THEN By := 0;

    { define origin of 3-d backround  }

    xc := Bx + xLineExt;            { right side of puzzle }
    yc := By + yLineExt;            { bottom of puzzle }
    xdepth := Aspect * depth;

    END; { MuzzleSize }


PROCEDURE MuzzlePaint ( hWindow : HWND;
                        hDC_    : HDC ) [ PUBLIC ];

    VAR i, j : INT;

    BEGIN

    { draw black shadow behind puzzle }

    EVAL( SelectObject( hDC_, hbrBlack ) );
    EVAL( Rectangle( hDC_, Bx+xdepth, By+depth, xc+3*xdepth, yc+3*depth ) );

    { draw front face of puzzle }

    EVAL( SelectObject( hDC_, hbrWhite ) );
    EVAL( Rectangle( hDC_, Bx-xdepth, By-depth, xc+xdepth, yc+depth ) );

    { draw vertical lines of grid }

    EVAL( SelectObject( hDC_, penBlack ) );
    i := Bx;
    WHILE ( i <= Bx + xLineExt ) DO
        BEGIN
        EVAL( MoveTo( hDC_, i, By ) );
        EVAL( LineTo( hDC_, i, By+yLineExt ) );
        i := i + xExt;
        END;

    { draw horizontal lines of grid}

    i := By;
    WHILE ( i <= By + yLineExt ) DO
        BEGIN
        EVAL( MoveTo( hDC_, Bx, i ) );
        EVAL( LineTo( hDC_, Bx+xLineExt, i ) );
        i := i + yExt;
        END;

    { fill in right edge of puzzle }

    EVAL( SelectObject( hDC_, penWhite ) );
    FOR i := 0 TO xdepth-1 DO
        BEGIN
        EVAL( MoveTo( hDC_, xc+xdepth+i, By-depth+i DIV Aspect ) );
        EVAL( LineTo( hDC_, xc+xdepth+i, yc+depth+i DIV Aspect ) );
        END;

    { fill in bottom edge of puzzle }

    FOR  i := 0 TO depth-1 DO
        BEGIN
        EVAL( MoveTo( hDC_, xc+xdepth+i*Aspect, yc+depth+i ) );
        EVAL( LineTo( hDC_, Bx-xdepth+i*Aspect, yc+depth+i ) );
        END;

    { draw in 3-d edges of puzzle }

    IF ( (By > 0) AND (Bx > 0) ) THEN
        BEGIN
        EVAL( SelectObject( hDC_, penBlack ) );
        EVAL( MoveTo( hDC_, xc+xdepth,   By-depth   ) );
        EVAL( LineTo( hDC_, xc+2*xdepth, By         ) );
        EVAL( LineTo( hDC_, xc+2*xdepth, yc+2*depth ) );
        EVAL( LineTo( hDC_, Bx,          yc+2*depth ) );
        EVAL( LineTo( hDC_, Bx-xdepth,   yc+depth   ) );
        EVAL( MoveTo( hDC_, xc+2*xdepth, yc+2*depth ) );
        EVAL( LineTo( hDC_, xc+xdepth-1, yc+depth-1 ) );
        END;

    { put in the numbers }

    FOR j := 0 TO YCOUNT-1 DO
        FOR i := 0 TO XCOUNT-1 DO
            PutNum( hDC_, i, j, A[i][j] );

    { Draw in the empty slot }

    DrawEmpty( hDC_, MTi, MTj );

    END; { MuzzlePaint }


PROCEDURE MuzzleMouse ( hWindow : HWND;
                        pt      : POINT ) [ PUBLIC ];

    VAR Si, Sj : INT;           { cell index of event }

    BEGIN

    { reduce coordinates to a puzzle box }

    Si := (pt.x - Bx) DIV xExt;
    Sj := (pt.y - By) DIV yExt;

    { move piece in location Si,Sj }

    MovePiece( hWindow, Si, Sj );

    END; { MuzzleMouse }


PROCEDURE MuzzleKeyboard( hWindow : HWND;
                          key     : WORD ) [ PUBLIC ];

    VAR move : INT;             { direction to move }

    BEGIN

    CASE key OF
        VK_DOWN:  move := 1;
        VK_LEFT:  move := 4;
        VK_RIGHT: move := 2;
        VK_UP:    move := 3;
        OTHERWISE
            RETURN;
        END;

    AutoMove( hWindow, move );

    END; { MuzzleKeyboard }


PROCEDURE MuzzleCommand( hWindow : HWND;
                         cmd     : WORD ) [ PUBLIC ];

    VAR k : BYTE;
        i : int;

    BEGIN

    { check for Mix-up command }

    IF ( cmd = STIR ) THEN
        BEGIN

        { stir the puzzle in a fixed manner
          this insures that every piece in the puzzle gets moved
        }

        FOR i := 0 TO 35 DO
            BEGIN
            k := StirOrder[i];
            AutoMove( hWindow, RETYPE(int,k) );
            END;

        { plant the seed for random number generator }

        seedrnd;

        { stir the puzzle in a random manner }

        FOR i := 0 TO 14 DO
            BEGIN
            k := (random MOD 4) + 1;
            AutoMove( hWindow, RETYPE(int,k) );
            END;

        { Set flag to check if the puzzle is solved }

        OrderCheck := TRUE_;
        END;

    END; { MuzzleCommand }


FUNCTION MuzzleWndProc( hWindow : HWND;
                        message : UNSIGNED;
                        wParam  : WORD;
                        lParam  : LONG ) : LONG [ PUBLIC, WINDOWS ];

    { This routine dispatches all messages sent to this window }

    LABEL defproc;

    VAR ps : PAINTSTRUCT;

    BEGIN

    CASE message OF

        WM_SYSCOMMAND:
            IF ( wParam = IDM_ABOUT ) THEN
                EVAL( DialogBox( hInst,
                                 MAKEINTRESOURCE(ABOUTBOX),
                                 hWindow,
                                 lpprocAbout ) )
            ELSE GOTO defproc;

        WM_CREATE:

            { lParam contains lParam field of CreateWindow }

            BEGIN
            MuzzleCreate( hWindow );
            UpdateWindow( hWindow );
            END;

        WM_DESTROY:
            PostQuitMessage( 0 );

        WM_SIZE:
            MuzzleSize( hWindow );

        WM_PAINT:
            BEGIN
            EVAL( BeginPaint( hWindow, RETYPE(LPPAINTSTRUCT, ADS ps) ) );
            EVAL( SetBkMode( ps.hdc_, OPAQUE ) );
            EVAL( SelectObject( ps.hdc_, GetStockObject( ANSI_FIXED_FONT ) ) );
            MuzzlePaint( hWindow, ps.hdc_ );
            EndPaint( hWindow, RETYPE(LPPAINTSTRUCT, ADS ps) );
            END;

        WM_COMMAND:
            MuzzleCommand( hWindow, wParam );

        WM_LBUTTONDOWN,
        WM_RBUTTONDOWN:
            MuzzleMouse( hWindow, MAKEPOINT(lParam) );

        WM_KEYUP:
            MuzzleKeyboard( hWindow, wParam );

    OTHERWISE
    defproc:

        { Let Windows handle all the other messages }

        BEGIN
        MuzzleWndProc := DefWindowProc( hWindow, message, wParam, lParam );
        RETURN;
        END;

    END; { CASE }

    { Tell Windows not to process the message further }

    MuzzleWndProc := RETYPE(LONG, BYLONG(0,0));

    END; { MuzzleWndProc }


FUNCTION MuzzleInit( hInstance : HANDLE ) : BOOL;

    VAR
        MuzzleClass : WNDCLASS;

    BEGIN

    { Pascal requires that the user file system and heap be initialized.     }
    { This procedure must be called before anything else is done in the      }
    { initilization procedure. No files may be declared in this procedure.   }
    { Keep this procedure as simple as possible to avoid unexpected          }
    { initilization code which could be generated before the call to begxqq. }

    begxqq;

    {$IF FILE_AND_HEAP $THEN BEGIN}
      ASSIGN( MyFile, CHR(0) );
    {$END}

    hbrWhite    := GetStockObject( WHITE_BRUSH );
    hbrGray     := GetStockObject( GRAY_BRUSH );
    hbrBlack    := GetStockObject( BLACK_BRUSH );
    hbrLtGray   := GetStockObject( LTGRAY_BRUSH );
    hbrBlue     := CreateSolidBrush( RETYPE(DWORD, BYLONG(0,255)) );
    hbrGreen    := CreateSolidBrush( RETYPE(DWORD, BYLONG(0,65280)) );
    penWhite    := GetStockObject( WHITE_PEN );
    penBlack    := GetStockObject( BLACK_PEN );
    hbrBack     := hbrBlue;

    MuzzleClass.lpszClassName := RETYPE(LPSTR, ADS 'Muzzle' * CHR(0));
    MuzzleClass.hIcon_        := LoadIcon( hInstance, RETYPE(LPSTR, ADS 'muzzle' * CHR(0)) );
    MuzzleClass.hCursor_      := LoadCursor( RETYPE(HWND, NULL_), IDC_ARROW );
    MuzzleClass.lpszMenuName  := RETYPE(LPSTR, ADS 'muzzle' * CHR(0));
    MuzzleClass.hbrBackground := hbrBack;

    { The class chooses to reformat if the window becomes wider and not
      to reformat ie. have its contents saved if it merely becomes taller.
    }

    MuzzleClass.lpfnWndProc   := ADS MuzzleWndProc;
    MuzzleClass.hInstance     := hInstance;
    MuzzleClass.style         := CS_VREDRAW OR CS_HREDRAW;
    MuzzleClass.cbClsExtra    := 0;
    MuzzleClass.cbWndExtra    := 0;

    { register this new class with WINDOWS }

    IF ( RegisterClass( RETYPE(LPWNDCLASS, ADS MuzzleClass) ) <> TRUE_ ) THEN
        BEGIN
        MuzzleInit := FALSE_;
        RETURN;
        END;

    MuzzleInit := TRUE_;

    END; { MuzzleInit }


FUNCTION WinMain( hInstance     : HANDLE;
                  hPrevInstance : HANDLE;
                  lpszCmdline   : LPSTR;
                  cmdShow       : INT ) : BOOL [ PUBLIC ];

    VAR
        hWindow : HWND;
        msg_    : MSG;
        hMenu_  : HMENU;

    { This routine is called once by Windows as the application is
      instantiated.

      Every application must include this function!
    }

    BEGIN


{$IF FILE_AND_HEAP $THEN}
    Muzzle;
{$END}

    IF ( hPrevInstance = 0 ) THEN

        { Call initialization routine }

        BEGIN
        IF ( MuzzleInit( hInstance ) = FALSE_ ) THEN

            { Initialization failed.  Windows will deallocate all
              resources for this app.
            }

            BEGIN
            WinMain := FALSE_;
            RETURN;
            END;
        END
    ELSE

        { Copy static data from previous instantiation }

        BEGIN
        EVAL( GetInstanceData( hPrevInstance,
                               RETYPE(PSTR, ADR hbrWhite),
                               ORD( SIZEOF( hbrWhite ) ) ) );
        EVAL( GetInstanceData( hPrevInstance,
                               RETYPE(PSTR, ADR hbrGray),
                               ORD( SIZEOF( hbrGray ) ) ) );
        EVAL( GetInstanceData( hPrevInstance,
                               RETYPE(PSTR, ADR hbrBlack),
                               ORD( SIZEOF( hbrBlack ) ) ) );
        EVAL( GetInstanceData( hPrevInstance,
                               RETYPE(PSTR, ADR hbrLtGray),
                               ORD( SIZEOF( hbrLtGray ) ) ) );
        EVAL( GetInstanceData( hPrevInstance,
                               RETYPE(PSTR, ADR hbrBlue),
                               ORD( SIZEOF( hbrBlue ) ) ) );
        EVAL( GetInstanceData( hPrevInstance,
                               RETYPE(PSTR, ADR hbrGreen),
                               ORD( SIZEOF( hbrGreen ) ) ) );
        EVAL( GetInstanceData( hPrevInstance,
                               RETYPE(PSTR, ADR penWhite),
                               ORD( SIZEOF( penWhite ) ) ) );
        EVAL( GetInstanceData( hPrevInstance,
                               RETYPE(PSTR, ADR penBlack),
                               ORD( SIZEOF( penBlack ) ) ) );
        EVAL( GetInstanceData( hPrevInstance,
                               RETYPE(PSTR, ADR hbrBack),
                               ORD( SIZEOF( hbrBack ) ) ) );
        END;

    { Create a window instance of class 'Muzzle' }

    hWindow := CreateWindow(
        RETYPE(LPSTR, ADS 'Muzzle' * CHR(0)),   { Class name            }
        RETYPE(LPSTR, ADS 'Muzzle' * CHR(0)),   { Instance name         }
        WS_TILEDWINDOW,                         { Window style          }
        0,                                      { Desired column        }
        0,                                      { cy: not used          }
        0,                                      { cx: not used          }
        100,                                    { Desired height: pixel }
        RETYPE(HWND, NULL_),                    { No parent window      }
        RETYPE(HMENU, NULL_),                   { Use default menu      }
        hInstance,                              { Instance handle       }
        RETYPE(LPSTR, BYLONG(0,NULL_)) );       { No additional parameter }

    { Save instance handle for dialog box }

    hInst := hInstance;

    { Insert aboutbox in system menu }

    lpprocAbout := MakeProcInstance( RETYPE(FARPROC, ADS About), hInstance );
    hMenu_ := GetSystemMenu( hWindow, FALSE_ );
    EVAL( ChangeMenu( hMenu_, 0,
                      RETYPE(LPSTR, BYLONG(0,NULL_)),
                      999, MF_APPEND OR MF_SEPARATOR ) );
    EVAL( ChangeMenu( hMenu_, 0,
                      RETYPE(LPSTR, ADS 'About...' * CHR(0)),
                      IDM_ABOUT, MF_APPEND OR MF_STRING ) );

    { Send message to update window }

    EVAL( ShowWindow( hWindow, cmdShow ) );
    UpdateWindow( hWindow );

    { Main polling loop }

    WHILE ( GetMessage( RETYPE(LPMSG, ADS msg_), NULL_, 0, 0) <> 0 ) DO
        BEGIN
        EVAL( TranslateMessage( RETYPE(LPMSG, ADS msg_) ) );
        EVAL( DispatchMessage( RETYPE(LPMSG, ADS msg_) ) );
        END;

    { The last thing that a Pascal Windows Proc Ap must do is   }
    { terminate the Pascal stuff. Call endxqq just before exit. }

    endxqq;

    _exit( msg_.wParam );

    END; { WinMain }

END.
