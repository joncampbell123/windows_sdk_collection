#include "hdrs.h"


ASSERTDATA;

RGBColor	rgbWhite = { 65535, 65535, 65535 },
			rgbBlack = {     0,     0,     0 };


CShape::CShape(short type)
{
	m_cRef      = 0;
	m_Type  	= type;
	m_Next  	= NULL;

	m_RGB.red	= (unsigned short)Random();
	m_RGB.green	= (unsigned short)Random();
	m_RGB.blue	= (unsigned short)Random();
}


CShape::~CShape(void)
{
}


HRESULT STDMETHODCALLTYPE CShape::QueryInterface(REFIID riid, void * * ppv)
{
	*ppv = NULL;

	ENTER_INTERFACE("CShape::QueryInterface");

	// Note: Use IsEqualIID to compare IIDs if you're using C

	if (riid == IID_IUnknown)
	{
		AddRef();
		*ppv = this;
		return ResultFromScode(S_OK);
	}

	return ResultFromScode(E_NOINTERFACE);
}


unsigned long STDMETHODCALLTYPE CShape::AddRef(void)
{
	++m_cRef;

	DUMP_REFCOUNT("CShape::AddRef", m_cRef);

	return m_cRef;
}


unsigned long STDMETHODCALLTYPE CShape::Release(void)
{
	--m_cRef;
	ASSERT(m_cRef >= 0);

	DUMP_REFCOUNT("CShape::Release",m_cRef);

	if (m_cRef == 0)
	{
		delete this;
		return 0;
	}

	return m_cRef;
}


void CShape::Draw(void)
{
	Rect		r;

	r = m_Bounds;

	PenNormal();

	RGBForeColor(&m_RGB);

	switch(m_Type)
	{
		case LINE:
			MoveTo(r.left, r.top);
			LineTo(r.right, r.bottom);
			RGBForeColor(&rgbBlack);
			break;

		case SQUARE:
			PaintRect(&r);
			RGBForeColor(&rgbBlack);
			FrameRect(&r);
			break;

		case CIRCLE:
			PaintOval(&r);
			RGBForeColor(&rgbBlack);
			FrameOval(&r);
			break;
	}
}

void CShape::SetBounds(Rect rect)
{
	m_Bounds = rect;
}


void CShape::TrackShape(Point pt)
{
	Rect 	r;
	short	hCur,
			vCur,
			hOld,
			vOld,
			h1,
			v1;

	hCur = pt.h;
	vCur = pt.v;
	hOld = hCur;
	vOld = vCur;
	h1 	 = hCur;
	v1   = vCur;

	SetRect(&r, hCur, vCur, hCur, vCur);

	PenMode(patXor);
	while (StillDown())
	{
		Point	thePoint;

		GetMouse(&thePoint);

		vCur = thePoint.v;
		hCur = thePoint.h;

		if (hCur != hOld || vCur != vOld)
		{
			switch(m_Type)
			{
				case LINE:
					MoveTo(r.left, r.top);
					LineTo(r.right, r.bottom);
					break;

				case SQUARE:
					FrameRect(&r);
					break;

				case CIRCLE:
					FrameOval(&r);
					break;
			}

			r.top    = m_Type == LINE ? v1   : MIN(vCur, v1);
			r.bottom = m_Type == LINE ? vCur : MAX(vCur, v1);
			r.left   = m_Type == LINE ? h1   : MIN(hCur, h1);
			r.right  = m_Type == LINE ? hCur : MAX(hCur, h1);

			switch(m_Type)
			{
				case LINE:
					MoveTo(r.left, r.top);
					LineTo(r.right, r.bottom);
					break;

				case SQUARE:
					FrameRect(&r);
					break;

				case CIRCLE:
					FrameOval(&r);
					break;
			}
		}

		hOld = hCur;
		vOld = vCur;
	}

	m_Bounds = r;

	PenNormal();

	Draw();
}


