#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include "app.h"

#include "grid.hxx"

//+--------------------------------------------------------
// Class:       CFontSelect
//
// Purpose:     Select/Deselect a font
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

// pure inline

//+--------------------------------------------------------
// Class:       CGridIt
//
// Purpose:     Iterate over a grid
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

// pure inline

//+--------------------------------------------------------
// Class:       CLineGrid
//
// Purpose:     Create an n x m Grid of lines
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

CLineGrid::CLineGrid(UINT cCol, UINT cRow, SIZE size ) 
{
    _size = size;
    _cCol = cCol;
    _cRow = cRow;
    SetStyle();
    SetWeight();
}

void CLineGrid::SetStyle(int iStyle)
{
    _iStyle = iStyle;
}

void CLineGrid::SetWeight(int nWeight)
{
    _nWeight = nWeight*20;  // internal twips, API is points
}

void CLineGrid::Paint(CCanvas& canvas, RECT rc, POINT pt)
{
    int cx, cy;
    UINT i;

    // set up pen
    CBlackPen pen(canvas, _iStyle, _nWeight);

    // Draw the grid
    for(cx = pt.x, i=0; i<=_cCol; i++, cx+=_size.cx )
    {
        if( cx >= rc.left && cx <= rc.right )
        {
            canvas.Line(cx, pt.y,
                        cx, pt.y+_cRow*_size.cy);
        }
    }
    for(cy = pt.y, i=0; i<=_cRow; i++, cy+=_size.cy )
    {
        if( cy >= rc.top && cy <= rc.bottom )
        {
            canvas.Line(pt.x,                cy,
                        pt.x+_cCol*_size.cx, cy);
        }
    }
}

//+--------------------------------------------------------
// Class:       CTextGrid
//
// Purpose:     Create an n x m grid of textual elements
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

void CTextGrid::SetFont(HFONT hfont)
{
    _font = hfont;
}

void CTextGrid :: Paint(CCanvas& canvas, RECT rc, POINT pt)
{
    // Choose text alignment
    SetTextAlign(HDC(canvas), TA_BASELINE|TA_CENTER);

    CFontSelect fs(canvas, _font);

    for( CGridIt It( _cCol, _cRow, _size, pt ); !It.Done(); ++It)
    {
        if( It.Cx()+_size.cx > rc.left && It.Cx() < rc.right &&
                It.Cy()+_size.cy > rc.top && It.Cy() < rc.bottom )
        {
            DrawElement(canvas, It.Cx()+_ptOrg.x, It.Cy()+_ptOrg.y, 
                                It.Col(), It.Row());
        }
    }
}

UINT CTextGrid::Hittest(POINT pt, POINT ptTest)
{
    for( CGridIt It( _cCol, _cRow, _size, pt ); !It.Done(); ++It)
    {
        if( It.Cx() <= ptTest.x && It.Cy() <= ptTest.y  &&
            It.Cx()+_size.cx > ptTest.x && It.Cy() +_size.cy > ptTest.y)
            {
                return   _cRow* It.Col()+ It.Row() + _iEltOffset;
            }
    }
    return 0xFFFF;
}

//+--------------------------------------------------------
// Class:       CCharGrid
//
// Purpose:     Create an n x m Grid of single characters
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

CCharGrid :: CCharGrid(UINT cCol, UINT cRow, SIZE size, UINT iEltOffset) 
{
    _size  = size;
    _cCol  = cCol;
    _cRow  = cRow;
    _iEltOffset = iEltOffset;
    SetTextOrg();
}

void CCharGrid::DrawElement(CCanvas& canvas, COORD x, COORD y, UINT i, UINT j)
{
    canvas.Char(x, y, (_cRow*i+j +_iEltOffset));
}

//+--------------------------------------------------------
// Class:       CCodeGrid
//
// Purpose:     Create an n x m Grid, numbered in sequence
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------
CCodeGrid :: CCodeGrid(UINT cCol, UINT cRow, SIZE size, UINT iEltOffset) 
{
    _size  = size;
    _cCol  = cCol;
    _cRow  = cRow;
    _iEltOffset = iEltOffset;
    SetTextOrg();
    SetFormat();
}

void CCodeGrid :: SetFormat(UINT fuFormat, UINT cDigits)
{
    _cDigits = cDigits;

    _szFormat[0]='%';
    if( fuFormat == DECIMAL )
    {
        _cDigits = 4;       // Constraint:
                            // This format for Decimal
        _szFormat[1]='3';   // really only works with _cDigits == 4
        _szFormat[2]='d';   // 
        _szFormat[3]=' ';   // (blank padding for better positioning)
    } 
    else // 
    {
        _szFormat[1]='0';
        _szFormat[2]=_cDigits%10+'0';
        _szFormat[3]='X';
    }
    _szFormat[4]='\0';
}

void CCodeGrid::DrawElement(CCanvas &canvas, COORD x, COORD y, UINT i, UINT j)
{
     TCHAR sz[10];
     wsprintf( sz,_szFormat, (_cRow*i+j+_iEltOffset));
     canvas.Text(x, y, sz, _cDigits);
}


//+--------------------------------------------------------
// Class:       CCharBlock
//
// Purpose:     Create an n x m lined block of characters and codes
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

CCharBlock::CCharBlock(UINT cCol, UINT cRow, UINT iBlockOffset, const CBlockFormat &bf) :
     _Line(cCol, cRow, bf._size),
     _Char(cCol, cRow, bf._size, iBlockOffset),
     _Code(cCol, cRow, bf._size, iBlockOffset)
{
     // LINE GRID
     _Line.SetStyle(PS_SOLID);

     // LARGE CHARACTER each cell
     _Char.SetFont(bf._fontChar);       

     // CODE POINT label each cell
     _Code.SetTextOrg(bf._size.cx/2, 9*bf._size.cy/10);
     _Code.SetFont(bf._fontCode);
     _Code.SetFormat(HEXADECIMAL,4);
}

void CCharBlock::Paint(CCanvas& canvas, RECT rc, POINT pt)
{
    _Line.Paint(canvas, rc, pt);        
    _Char.Paint(canvas, rc, pt);
    _Code.Paint(canvas, rc, pt);
}

UINT CCharBlock::Hittest(POINT pt, POINT ptTest)
{
    return _Code.Hittest(pt, ptTest);
}

void CCharBlock :: SetFormat(UINT fuFormat)
{
    _Code.SetFormat(fuFormat&DECIMAL,4);
}

void CCharBlock::SetFont(HFONT font)
{
    _Char.SetFont(font);        
}

//+--------------------------------------------------------
// Class:       CBlockFrame
//
// Purpose:     Create an n x m frame around a block of characters
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

CBlockFrame::CBlockFrame(UINT cCol, UINT cRow, POINT pt, UINT iBlockOffset, 
                                TCHAR * szHeader, const CFrameFormat &ff):
     CCharBlock(cCol, cRow, iBlockOffset, ff ),
     _size(ff._size),
     _Cols(cCol,    1, ff._size, iBlockOffset/16),
     _cRow(cRow),
     _cCol(cCol)
{

     // COLUMN label above first row
     _Cols.SetFormat(HEXADECIMAL,3);
     _Cols.SetFont(ff._fontLabel);      


     // if not first block on page, suppress row labels
     if( iBlockOffset % 0x100 )
     {
        _pRows = NULL;
     }
     else
     {
        // ROW label to left of first column
        _pRows = new CCodeGrid(1, cRow,  ff._size);
        _pRows->SetFont(ff._fontLabel); 
     }

     // BLOCK HEADER
     _szHeader = new TCHAR[lstrlen(szHeader)+sizeof(TCHAR)];
     lstrcpy(_szHeader, szHeader);
     _fontHeader = ff._fontHeader;

}

CBlockFrame::~CBlockFrame()
{
    delete _pRows;
    delete _szHeader;
}


void CBlockFrame::Paint(CCanvas& canvas, RECT rc, POINT pt)
{
    Draw(canvas, pt);
    {
        POINT ptCols = {pt.x, pt.y-_size.cy};
        _Cols.Paint(canvas, rc, ptCols);  
    } 
    CCharBlock::Paint(canvas, rc, pt);  
    
    // these two are optional. Test first, then draw
    if(_pRows )
    {   
        POINT ptRows = {pt.x-_size.cx, pt.y};
        _pRows->Paint(canvas, rc, ptRows);
    }   
}

void CBlockFrame::Draw(CCanvas& canvas, POINT pt)
{
    UINT dy = 3*_size.cy/2;     // height of short uprights
    UINT cx = _cCol*_size.cx;   
    UINT cy = _cRow*_size.cy;
        
    // set up pen
    CBlackPen pen(canvas, PS_SOLID, 40);    // Solid Black 40/20 points

    //Draw block divider lines
    
    canvas.Line(pt.x,    pt.y-dy, pt.x,    pt.y+cy);  // left edge
    canvas.Line(pt.x+cx, pt.y-dy, pt.x+cx, pt.y+cy);  // right edge
    canvas.Line(pt.x,    pt.y,    pt.x+cx, pt.y);     // top edge
    canvas.Line(pt.x,    pt.y+cy, pt.x+cx, pt.y+cy);  // bottom
    
    CFontSelect fs(canvas, _fontHeader);

    SetTextAlign(canvas, TA_BASELINE|TA_LEFT);
    canvas.Text(pt.x + _size.cx/6, 
                pt.y - 6*_size.cy/5, 
                _szHeader, 
                lstrlen(_szHeader));
}

// The following Array contains the widhts of the Unicode blocks in
// columns. Each Block has a corresponding entry in the stringtable
// giving its block header. "Unassigned" blocks can span page boun-
// daries. 

static UINT aBlockWidth[]=
{
    2,6,2,6,   // 0000
    8,8,       // 0100
    5,6,5,     // 0200
    8,8,       // 0300
    16,        // 0400
    3,6,7,     // 0500
    16,        // 0600
    32,        // 0700 - 0800 
    8,8,       // 0900
    8,8,       // 0A00
    8,8,       // 0B00
    8,8,       // 0C00
    8,8,       // 0D00
    8,8,       // 0E00
    16,        // 0F00
    6,4,6,     // 1100
    240,       // 1200 - 1F00
    7,3,3,3,   // 2000
    5,4,7,     // 2100
    16,        // 2200
    16,        // 2300
    4,2,10,    // 2400
    8,2,6,     // 2500
    16,        // 2600
    12,        // 2700
    132,       // 2800 - 2F00 
    4,6,6,     // 3000
    3,6,7,     // 3100
    16,        // 3200
    8,8,       // 3300
    144,       // 3400 - 3C00
    3,13,      // 3D00
    256,       // 3E00 - 4D00
    82*16,     // 4E00 - 9FF0
    64*16,     // A000 - E000
    25*16,     // E000 - F800
    32,        // F900 - FA00
    48,        // FB00 - FD00 
    3,2,2,9,   // FE00
    15,1,      // FF00
    200        // Sentinel
};

//+--------------------------------------------------------
// Class:       CBlockFormat
//
// Purpose:     Block formatting
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

CBlockFormat::CBlockFormat() :
    _fontCode    (TEXT("Arial Narrow"), -6),
#ifdef UNICODE
    _fontChar    (TEXT("Lucida Sans Unicode"), -16, TRUE)
#else
    _fontChar    (TEXT("Lucida Sans"), -16, TRUE)
#endif
{
};

//+--------------------------------------------------------
// Class:       CFrameFormat
//
// Purpose:     Frame formatting
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------

CFrameFormat::CFrameFormat() :
    _fontHeader  (TEXT("Arial"), -12, TRUE),
    _fontLabel   (TEXT("Arial"), -10, TRUE)
{
}

//+--------------------------------------------------------
// Class:       CPageFormat
//
// Purpose:     Page formatting
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------
CPageFormat::CPageFormat(UINT fuFormat) :
    _fontPageNum (TEXT("Times New Roman"), -10, FALSE)
{
    _size.cx =  4*INCH2/5;
    _size.cy =  INCH2;

    SetFormat(fuFormat);
}

void CPageFormat::SetFormat(UINT fuFormat)
{
    _fuFormat = fuFormat;

    if( fuFormat & PAGEPRINT )
    {
        _pt.x=     INCH2+(INCH2*7)/10;
        _pt.y=     INCH1+(INCH1*7)/10;
    } 
    else
    {
        _pt.x=     INCH2;
        _pt.y=     INCH1;
    }

    // locations of headers / footers
    _ptPE[0].x = _pt.x-_size.cx; 
    _ptPE[0].y = _size.cy/2;
    _ptPE[1].x = _pt.x+_size.cx*16; 
    _ptPE[1].y = _size.cy/2;
    _ptPE[2].x = _pt.x+(_size.cx*15)/2;
    _ptPE[2].y = _pt.y+(_size.cy*33)/2;
}

//+--------------------------------------------------------
// Class:       CPage
//
// Purpose:     One or more blocks
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------
CPage::CPage(HINSTANCE hInst, CPageFormat &pf, UINT nPage) :
    _cBlock(0),
    _pf(pf),
    _hInst(hInst),
    _PageHeadL (1, 1, pf._size,nPage*256      ),
    _PageHeadR (1, 1, pf._size,(nPage+1)*256-1),
    _PageNums  (1, 1, pf._size,nPage+1        )        
                                // page numbers are 1 based on output
{
    // Set up page elementss

    _PageHeadL.SetFont(pf._fontLabel);
    _PageHeadR.SetFont(pf._fontLabel);
    _PageNums.SetFont(pf._fontPageNum);

    InitPage( nPage);
    SetFormat(_pf._fuFormat);
}

CPage::~CPage()
{
    while( _cBlock )
    {
        delete _apBlock[--_cBlock];
    }   
}



void CPage::SetFormat(UINT fuFormat)
{
    // set it
    _pf.SetFormat(fuFormat);

    // apply it
    for( UINT i = 0; i < _cBlock ; ++i )
    {
        _apBlock[i]->SetFormat(fuFormat&MASKROOT);
    }

    _PageHeadL.SetFormat(HEXADECIMAL, 4);
    _PageHeadR.SetFormat(HEXADECIMAL, 4);
    _PageNums.SetFormat(DECIMAL);
}


void CPage::Paint(CCanvas& canvas, RECT rc)
{
    for( UINT i = 0; i < _cBlock; ++i )
    {
        _apBlock[i]->Paint(canvas, rc, _aptBlock[i]);
    }

    if( _pf._fuFormat & PAGEELEMS )
    {
        _PageHeadL.Paint(canvas, rc, _pf._ptPE[0]);
        _PageHeadR.Paint(canvas, rc, _pf._ptPE[1]);
        _PageNums.Paint (canvas, rc, _pf._ptPE[2]);
    }   
}

UINT CPage::Hittest(POINT ptTest)
{
    UINT uHit = 0xFFFF;

    for( UINT i = 0; i < _cBlock && uHit ==0xFFFF ; ++i )
    {
        uHit = _apBlock[i]->Hittest(_aptBlock[i], ptTest);
    }
    return uHit;
}

//-- protected member functions...

UINT CPage::InitPage(UINT nPage)
{
    UINT iEnd = 0;
    TCHAR szBlockHeader[40];
    UINT i;
    POINT ptBlock=_pf._pt;

    // Set up blocks

    for( i=0; i < sizeof(aBlockWidth)/sizeof(UINT); i++ )
    {
        if( nPage*16 < (iEnd+=aBlockWidth[i]) )
        {
            break;
        }
    }
    UINT iStart = max(nPage*16, iEnd-aBlockWidth[i]);

    do
    {
        LoadString(_hInst, i, szBlockHeader, 40);
        _apBlock[_cBlock]= new CBlockFrame(
                  min( aBlockWidth[i],      // grid width in columns
                       (nPage+1)*16-iStart),// (but at most to end of page)
                  16,                       // cRow always 16
                  ptBlock,                  // grid origin
                  iStart*16,                // first char offset
                  szBlockHeader,            // header string
                  _pf);                     // common formatting

        _aptBlock[_cBlock] = ptBlock;
        _cBlock++;

        ptBlock.x+=_pf._size.cx*aBlockWidth[i];
        iStart=iEnd;
    } 
    while(
            (i++ < sizeof(aBlockWidth)/sizeof(UINT))
                &&
            ((nPage+1)*16 >= (iEnd+=aBlockWidth[i]))
                &&
            (_cBlock < 4)  
         );


    return _cBlock;
}


void CPage::SetFont(HFONT hfont)
{
    for( UINT i = 0; i < _cBlock ; ++i )
    {
        _apBlock[i]->SetFont(hfont);
    }
}

//+--------------------------------------------------------
// Class:       CModel  
//
// Purpose:     Iterator over pages
//
// History:     22-Jan-1993     asmusf  created
//----------------------------------------------------------
CModel::CModel(HINSTANCE hInst, UINT fuFormat) :
    _iPage(0),
#ifdef UNICODE
    _macPage(0x100),
#else
    _macPage(1),
#endif
    _pf(fuFormat),
    _hInst(hInst)
{
    _pPage = new CPage(hInst, _pf, _iPage);     
}

CModel::~CModel()
{
    delete _pPage;      
}

void CModel::NextPage()
{
    SetPage(_iPage+=(_iPage+1<_macPage? 1 : 0));
}

void CModel::PrevPage()
{
    SetPage(_iPage-=(_iPage? 1 : 0));
}

void CModel::NextSection()
{
    SetPage(_iPage+=(_iPage+16<_macPage? 16 : 0));
}

void CModel::PrevSection()
{
    SetPage(_iPage-=(_iPage >= 16 ? 16 : 0));
}

void CModel::SetPage(UINT nPage)
{
    delete _pPage;
    _pPage = new CPage(_hInst, _pf, nPage);     
}

void CModel::GetFormat(UINT &fuFormat) 
{
    fuFormat=_pf._fuFormat; 
}

void CModel::SetFormat(UINT fuFormat) 
{
    _pPage->SetFormat(fuFormat);
}

HFONT CModel::GetFont()
{
    return _pf._fontChar;
}

BOOL CModel::CreateFont(LOGFONT &lf)
{
    if(_pf._fontChar.Create(lf))
    {
         _pPage->SetFont(_pf._fontChar);
         return TRUE;
    }
    return FALSE;
}

BOOL CModel::ChooseFont(HWND hwnd)
{
    if(_pf._fontChar.Choose(hwnd))
    {
         _pPage->SetFont(_pf._fontChar);
         return TRUE;
    }
    return FALSE;
}

UINT CModel::Hittest( POINT pt)
{
     return _pPage->Hittest( pt);
}
