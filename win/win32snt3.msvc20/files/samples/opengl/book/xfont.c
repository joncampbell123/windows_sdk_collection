/*
 * (c) Copyright 1993, Silicon Graphics, Inc.
 *               1993, 1994 Microsoft Corporation
 *
 * ALL RIGHTS RESERVED
 *
 * Please refer to OpenGL/readme.txt for additional information
 *
 */

/*
 *  xfont.c
 *  Draws some text in a bitmapped font.  Uses glBitmap() 
 *  and other pixel routines.  Also demonstrates use of 
 *  display lists.
 */
#include "glos.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "aux.h"

void myinit(void);
void makeRasterFont(void);
void printString(char *s);
void CALLBACK display(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);

GLuint base;

void makeRasterFont(void)
{
    XFontStruct *fontInfo;
    Font id;
    unsigned int first, last;
    Display *xdisplay;

    xdisplay = auxXDisplay ();
    fontInfo = XLoadQueryFont(xdisplay, 
    "-adobe-helvetica-medium-r-normal--17-120-100-100-p-88-iso8859-1");
    if (fontInfo == NULL) {
        //printf ("no font found\n");
    exit (0);
    }

    id = fontInfo->fid;
    first = fontInfo->min_char_or_byte2;
    last = fontInfo->max_char_or_byte2;

    base = glGenLists((GLuint) last+1);
    if (base == 0) {
        //printf ("out of display lists\n");
    exit (0);
    }
    glXUseXFont(id, first, last-first+1, base+first);
/*    *height = fontInfo->ascent + fontInfo->descent;
    *width = fontInfo->max_bounds.width;  */
}

void printString(char *s)
{
    glPushAttrib (GL_LIST_BIT);
    glListBase(base);
    glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte *)s);
    glPopAttrib ();
}

void myinit (void) 
{
    makeRasterFont ();
    glShadeModel (GL_FLAT);    
}

void CALLBACK display(void)
{
    GLfloat white[3] = { 1.0, 1.0, 1.0 };
    int i, j;
    char teststring[33];

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3fv(white);
    for (i = 32; i < 127; i += 32) {
    glRasterPos2i(20, 200 - 18*(GLint) i/32);
    for (j = 0; j < 32; j++)
        teststring[j] = (char) (i+j);
    teststring[32] = 0;
    printString(teststring);
    }
    glRasterPos2i(20, 100);
    printString("The quick brown fox jumps");
    glRasterPos2i(20, 82);
    printString("over a lazy dog.");
    glFlush ();
}

void CALLBACK myReshape(GLsizei w, GLsizei h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (0.0, (GLfloat) w, 0.0, (GLfloat) h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
    auxInitDisplayMode (AUX_SINGLE | AUX_RGB);
    auxInitPosition (0, 0, 500, 500);
    auxInitWindow ("Bitmap Font");
    auxReshapeFunc (myReshape);
    myinit ();
    auxMainLoop(display);
}
