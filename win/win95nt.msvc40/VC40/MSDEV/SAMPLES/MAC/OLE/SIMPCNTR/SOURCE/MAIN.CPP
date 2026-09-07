#include "hdrs.h"

pascal long growProc(long dw);

void main(void)
{
	gApp = new CApplication;
	if (gApp == NULL)
		FailAlert("\pCApplication object could not be created.");

	gApp->InitApplication();
	gApp->EventLoop();
	gApp->DisposeApplication();
}
