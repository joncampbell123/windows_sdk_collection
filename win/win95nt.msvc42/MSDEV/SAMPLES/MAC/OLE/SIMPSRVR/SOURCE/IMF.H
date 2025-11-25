#ifndef _MSC_VER
#pragma once
#endif

#ifndef __OLEMESSAGEFILTER_H__
#define __OLEMESSAGEFILTER_H__

#include "App.h"

void OleMessageFilterRegister(CApplication * pOleApp);
void OleMessageFilterRevoke(CApplication* pOleApp);

extern "C" Boolean OleMessageFilterMessagePending(long userData);

#endif
