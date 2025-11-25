#ifndef _MSC_VER
#define __STRING__
#define __STDIO__

#ifndef __size_t__
#define __size_t__
typedef unsigned long size_t;
#endif


#ifdef __cplusplus
extern "C" {
#endif

// declarations from stdio.h
int sprintf(char *s, const char *format, ...);

// declarations from string.h
char *strcpy (char *s1, const char *s2);
size_t strlen (const char *s);


#ifdef __cplusplus
}
#endif


#endif  // _MSC_VER


#ifdef _MSC_VER
#include <macos\Windows.h>
#else
#include <Windows.h>
#endif

#include <AppleEvents.h>
#include <Controls.h>
#include <Desk.h>
#include <Dialogs.h>
#include <DiskInit.h>
#include <Errors.h>
#include <Events.h>
#include <Files.h>
#include <Fonts.h>
#include <GestaltEqu.h>
#include <Lists.h>
#include <LowMem.h>
#include <Memory.h>
#include <Menus.h>
#include <OSUtils.h>
#include <Printing.h>
#include <Processes.h>
#include <Quickdraw.h>
#include <Resources.h>
#include <Scrap.h>
#include <StandardFile.h>
#include <StdIO.h>
#include <StdLib.h>
#include <String.h>
#include <TextEdit.h>
#include <ToolUtils.h>
#include <Traps.h>
#include <Types.h>
