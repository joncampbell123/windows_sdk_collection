#ifndef _CSHAPE_H_
#define _CSHAPE_H_

#define		NO_SHAPE	-1

class CShape : public IUnknown
{
	public:

	unsigned long		m_cRef;
	short				m_Type;
	Rect				m_Bounds;
	CShape *			m_Next;
	RGBColor 			m_RGB;

	CShape(short type);
	~CShape(void);

    // *** IUnknown methods ***
    virtual HRESULT STDMETHODCALLTYPE	QueryInterface(REFIID riid, void * * ppvObj);
    virtual unsigned long STDMETHODCALLTYPE		AddRef(void);
    virtual unsigned long STDMETHODCALLTYPE		Release(void);

	void	Draw			(void);
	void	SetBounds		(Rect rect);
	void 	TrackShape		(Point pt);
};

#endif // _CSHAPE_H_