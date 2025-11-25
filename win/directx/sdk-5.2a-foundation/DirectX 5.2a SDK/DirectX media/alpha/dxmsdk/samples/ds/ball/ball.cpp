//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

#include <streams.h>
#include "ball.h"

//
//
// What this samples illustrates
//
// A simple source filter that produces decompressed images showing a ball
// bouncing around. Each movement of the ball is done by generating a new
// image. We use the CSource and CSourceStream base classes to manage a
// source filter - we are a live source and so do not support any seeking.
//
//
// Summary
//
// This is a sample source filter - we produce a never ending stream of images
// that show a coloured ball bouncing around the window. Depending on the bit
// depth of the current display device we colour the ball differently. As we
// are effectively a live video stream we do not offer any seeking interfaces.
// We can supply 32,24,16 (555 and 565) as well as eight bit palettised types
//
//
// Implementation
//
// We use the CSource and CSourceStream base classes from the SDK which look
// after some of the grunge associated with source filters, in particular the
// starting the stopping of workers threads as we're activated and stopped.
// The worker thread sits in a loop asking for buffers and then calling the
// PURE virtual FillBuffer method when it has a buffer for us to fill up.
//
// For an example we also have a simple quality management implementation in
// this filter, quality management of everyone except renderers (who are the
// ones normally to initiate it) is controlled through IQualityControl. This
// is called on each frame to say how we are getting on, because this filter
// is pretty straightforward we can control the spacing of samples we send
// downstream so that we always run flat out with whatever CPU is available
//
//
// Demonstration instructions
//
// Start GRAPHEDT available in the ActiveMovie SDK tools. Click on the Graph
// menu and select "Insert Filters". From the dialog box double click on
// "Bouncing ball" and then dismiss the dialog. Go to the right hand pin of
// the filter box and right click, select Render. A video renderer will be
// inserted and connected up (on some displays there may be a colour space
// convertor put between them to get the pictures into a suitable format).
// Then click run on GRAPHEDT and see the ball bounce around the window...
//
//
// Files
//
// ball.cpp         Looks after drawing a moving bouncing ball
// ball.def         What APIs the DLL imports and exports
// ball.h           Class definition for the ball drawing object
// ball.rc          Version and title information resources
// ball.reg         What goes in the registry to make us work
// balluids.h       The CLSIDs for the bouncing ball filter
// fball.cpp        The real filter class implementation
// fball.h          Class definition for the main filter object
// makefile         How to build it...
// resource.h       A couple of identifiers for our resources
//
//
// Base classes used
//
// CSource          Base class for a generic source filter
// CSourceStream    A base class for a source filters stream
//
//


//
// Constructor
//
// The default arguments provide a reasonable image and ball size
//
CBall::CBall(int iImageWidth, int iImageHeight, int iBallSize) :
    m_iImageWidth(iImageWidth),
    m_iImageHeight(iImageHeight),
    m_iBallSize(iBallSize),
    m_iAvailableWidth(iImageWidth - iBallSize),
    m_iAvailableHeight(iImageHeight - iBallSize),
    m_x(0),
    m_y(0),
    m_xDir(RIGHT),
    m_yDir(UP)
{
    // Check we have some (arbitrary) space to bounce in.
    ASSERT(iImageWidth > 2*iBallSize);
    ASSERT(iImageHeight > 2*iBallSize);

    // Random position for showing off a video mixer
    m_iRandX = rand();
    m_iRandY = rand();

} // (Constructor)


//
// PlotBall
//
// Assumes the image buffer is arranged row 1,row 2,...,row n
//      in memory and that the data is contiguous.
//
void CBall::PlotBall(BYTE pFrame[], BYTE BallPixel[], int iPixelSize)
{
    ASSERT(m_x >= 0);
    ASSERT(m_x <= m_iAvailableWidth);
    ASSERT(m_y >= 0);
    ASSERT(m_y <= m_iAvailableHeight);

    // The current byte of interest in the frame
    BYTE *pBack =   pFrame;		

    // Plot the ball into the correct location
    BYTE *pBall = pFrame + ( m_y * m_iImageWidth * iPixelSize) + m_x * iPixelSize;

    for (int row = 0; row < m_iBallSize; row++) {

        for (int col = 0; col < m_iBallSize; col++) {

            // For each byte fill its value from BallPixel[]
            for (int i = 0; i < iPixelSize; i++) {	
                if (WithinCircle(col, row)) {
                    *pBall = BallPixel[i];
                }
                pBall++;
	    }
	}
	pBall += m_iAvailableWidth * iPixelSize;
    }

} // PlotBall


//
// BallPosition
//
// Return the 1-dimensional position of the ball at time t millisecs
//      (note that millisecs runs out after about a month!)
//
int CBall::BallPosition(int iPixelTime, // Millisecs per pixel
                        int iLength,    // Distance between the bounce points
                        int time,       // Time in millisecs
                        int iOffset)    // For a bit of randomness
{
    // Calculate the position of an unconstrained ball (no walls)
    // then fold it back and forth to calculate the actual position

    int x = time / iPixelTime;
    x += iOffset;
    x %= 2*iLength;

    // check it is still in bounds
    if (x>iLength) {	
        x = 2*iLength - x;
    }
    return x;

} // BallPosition


//
// MoveBall
//
// Set (m_x, m_y) to the new position of the ball.  move diagonally
// with speed m_v in each of x and y directions.
// Guarantees to keep the ball in valid areas of the frame.
// When it hits an edge the ball bounces in the traditional manner!.
// The boundaries are (0..m_iAvailableWidth, 0..m_iAvailableHeight)
//
void CBall::MoveBall(CRefTime rt)
{
    m_x = BallPosition( 10, m_iAvailableWidth, rt.Millisecs(), m_iRandX );
    m_y = BallPosition( 10, m_iAvailableHeight, rt.Millisecs(), m_iRandY );

} // MoveBall


//
// WithinCircle
//
// Return TRUE if (x,y) is within a circle radius S/2, centre (S/2, S/2)
//      where S is m_iBallSize else return FALSE
//
inline BOOL CBall::WithinCircle(int x, int y)
{
    unsigned int r = m_iBallSize / 2;

    if ( (x-r)*(x-r) + (y-r)*(y-r)  < r*r) {
        return TRUE;
    } else {
        return FALSE;
    }

} // WithinCircle

