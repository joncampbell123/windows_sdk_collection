#include <windows.h>
#include <windowsx.h>
#include "app.h"
#include "box.hxx"

CTextField::CTextField(TCHAR *sz)
{
    _sz = sz;
}

void CTextField::Paint(CCanvas &canvas, int x, int y)
{
    SetTextAlign(canvas, TA_LEFT | TA_BASELINE );
    CFontSelect fs(canvas, _font);
    canvas.Text(x, y, _sz, lstrlen(_sz) );
}

void CTextField::GetExtent(CCanvas &canvas, SIZE *pSize)
{
	CFontSelect fs(canvas, _font);
    GetTextExtentPoint(canvas, _sz, lstrlen(_sz), pSize);
}

void CTextField::SetFont(HFONT hfont)
{
    _font = hfont;      
}

//-----------------------------------------------
CBoxFormat::CBoxFormat(SIZE sizeChar) :
        _fontAlias(TEXT("Arial Narrow"), -10, FALSE),
        _fontCtype(TEXT("Arial"), 12, TRUE, FALSE, TRUE)
{
        _size = sizeChar;
}

//-----------------------------------------------
CBox::CBox( CBoxFormat &bxf, UINT iChar, HFONT hfont ) :
        _fontBlock( hfont ),
        _Block(1, 1, iChar, bxf ),
        _bxf(bxf),
        _Alias(TEXT("Character Type Bits:"))
{
    
    _fontBlock.Update( -40, TRUE);
    _Block.SetFont(_fontBlock); 

    _sizeBox.cx = bxf._size.cx+100; // shadow 100/20 poitns
    _sizeBox.cy = bxf._size.cy+100;

#ifdef UNICODE
    _sizeBox.cx += INCH4;
    _sizeBox.cy += INCH4;
#endif


    _Alias.SetFont(bxf._fontAlias);
    _iChar = iChar;
}
        

void CBox::Paint(CCanvas &canvas, POINT pt, RECT rc)
{
    int x = pt.x;
    int y = pt.y;

#ifdef UNICODE
    // adjust to length of header

    SIZE size;
    _Alias.GetExtent(canvas, &size);
    _sizeBox.cx += size.cx;
#endif
    // make rc large enough
    rc.left = pt.x;
    rc.top = pt.y;
    rc.right = pt.x+_sizeBox.cx-100;
    rc.bottom = pt.y+_sizeBox.cy-100;

    OffsetRect( &rc, 100, 100 );
    FillRect(canvas, &rc, GetStockBrush(GRAY_BRUSH));
    OffsetRect( &rc, -100, -100 );
    FillRect(canvas, &rc, GetStockBrush(WHITE_BRUSH));

#ifdef UNICODE
    CBlackPen pen(canvas, PS_SOLID, 20);
    Rectangle(canvas, rc.left, rc.top, rc.right, rc.bottom);

    pt.x += INCH8;
    pt.y += INCH8;
#endif

    _Block.Paint(canvas, rc, pt);

#ifdef UNICODE
    _Alias.Paint(canvas, x+=(INCH1-INCH10), y+= INCH4);
    _Alias.GetExtent(canvas, &size);

    pt.x = x;
    pt.y = y+INCH10;

    USHORT uTemp[2]={_iChar, 0};
    USHORT uType[2];

    SetTextColor(canvas, RGB(0,128,0));
    for( int i = 0; i < 3; i++ )
    {
        pt.y += size.cy;

        GetStringTypeW(1<<i, uTemp, 2, uType );
        CCodeGrid Ctype(1,1, size, uType[0]);
        Ctype.SetFont(_bxf._fontCtype);
        Ctype.SetFormat(HEXADECIMAL,4);
        Ctype.Paint(canvas, rc, pt);
    }
    SetTextColor(canvas, RGB(0,0,0));
#endif
}

UINT CBox::Hittest(CCanvas &canvas, POINT pt)
{
     return 0x00C5;
}
