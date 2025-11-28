// File: chull.cpp
// 
// Desc: Convex-hull code for the D3D shadow demo. 
//       This has been modified from Clarkson's original.

/*
 * Ken Clarkson wrote this.  Copyright (c) 1996 by AT&T..
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 */


/*
 * two-dimensional convex hull
 * read points from stdin,
 *      one point per line, as two numbers separated by whitespace
 * on stdout, points on convex hull in order around hull, given
 *      by their numbers in input order
 * the results should be "robust", and not return a wildly wrong hull,
 *	despite using floating point
 * works in O(n log n); I think a bit faster than Graham scan;
 * 	somewhat like Procedure 8.2 in Edelsbrunner's "Algorithms in Combinatorial
 *	Geometry".
 */

#include <stdlib.h>

#include "shadow.h"

int ccw(COLORVERTEX *P[], int i, int j, int k) {
	double	a = P[i]->p.x - P[j]->p.x,
		b = P[i]->p.y - P[j]->p.y,
		c = P[k]->p.x - P[j]->p.x,
		d = P[k]->p.y - P[j]->p.y;
	return a*d - b*c <= 0;	   /* true if points i, j, k counterclockwise */
}

int cmpl(const void *a, const void *b) {
	float v;
        COLORVERTEX **av,**bv;

        av=(COLORVERTEX **)a;
        bv=(COLORVERTEX **)b;

	v = (*av)->p.x - (*bv)->p.x;

	if( v>0 ) return 1;
	if( v<0 ) return -1;

	v = (*bv)->p.y - (*av)->p.y;

	if( v>0 ) return 1;
	if( v<0 ) return -1;

	return 0;
}

int cmph(const void *a, const void *b) {return cmpl(b,a);}

int make_chain(COLORVERTEX *V[], int n, int (*cmp)(const void*, const void*)) 
{
	int i, j, s = 1;
	COLORVERTEX *t;

	qsort(V, n, sizeof(COLORVERTEX*), cmp);
	for( i=2; i<n; i++ ) 
	{
		for( j=s; j>=1 && ccw(V, i, j, j-1); j-- )
		{}
		s = j+1;
		t = V[s]; V[s] = V[i]; V[i] = t;
	}
	return s;
}

int ch2d(COLORVERTEX *P[], int n)  
{
	int u = make_chain(P, n, cmpl);		/* make lower hull */
	if( !n ) return 0;
	P[n] = P[0];
	return u+make_chain(P+u, n-u+1, cmph);	/* make upper hull */
}

void Find2DConvexHull(DWORD nverts,COLORVERTEX *pntptr,DWORD *cNumOutIdxs,WORD **OutHullIdxs)
{
   COLORVERTEX **PntPtrs;
   DWORD i;

   *cNumOutIdxs=0;               //max space needed is n+1 indices
   *OutHullIdxs = (WORD *)malloc((nverts+1)*(sizeof(DWORD)+sizeof(COLORVERTEX *)));

   PntPtrs = (COLORVERTEX**) &(*OutHullIdxs)[nverts+1];

   // alg requires array of ptrs to verts (for qsort) instead of array of verts, so do the conversion
   for(i=0;i<nverts;i++) {
      PntPtrs[i] = &pntptr[i];
   }

   *cNumOutIdxs=ch2d(PntPtrs,nverts);

   // convert back to array of idxs
   for(i=0;i<*cNumOutIdxs;i++) {
      (*OutHullIdxs)[i]= (WORD) (PntPtrs[i]-&pntptr[0]);
   }

   // caller will free returned array
}

