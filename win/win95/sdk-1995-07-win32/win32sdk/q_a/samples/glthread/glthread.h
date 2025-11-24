
#include "glos.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include "resource.h"

// Function proto-types
GLvoid initialize(HWND);
GLvoid draw_scene(HWND);
GLvoid resize(HWND);
BOOL bSetupPixelFormat(HDC);
GLvoid ThreadFunc1(void);
GLvoid ThreadFunc2(void);
GLvoid DrawWave(GLfloat fHeight, GLfloat fWidth, GLfloat fHorizon);
GLfloat solve(GLfloat x, GLfloat z, GLfloat h, GLfloat w);
GLvoid CalculateVectorNormal(GLfloat fVert1[], GLfloat fVert2[], GLfloat fVert3[], 
                             GLfloat *fNormalX, GLfloat *fNormalY, GLfloat *fNormalZ);
