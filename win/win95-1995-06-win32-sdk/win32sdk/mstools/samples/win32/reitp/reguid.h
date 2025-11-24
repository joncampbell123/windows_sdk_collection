#ifdef WIN32
# ifndef __based
#  define __based(a)
# endif
#endif

#define DECLARE_REGUID(name, b) \
	EXTERN_C const GUID CDECL FAR name


DECLARE_REGUID(IID_IRichEditOleCallback,		0x03);
