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

class CBall
{
public:

    CBall(int iImageWidth = 320, int iImageHeight = 240, int iBallSize = 10);

    // Plots the square ball in the image buffer, at the current location.
    // Use BallPixel[] as pixel value for the ball.
    // Plots zero in all 'background' image locations.
    // iPixelSize - the number of bytes in a pixel (size of BallPixel[])
    void PlotBall(BYTE pFrame[], BYTE BallPixel[], int iPixelSize);

    // Moves the ball 1 pixel in each of the x and y directions
    void MoveBall(CRefTime rt);

private:

    enum xdir { LEFT = -1, RIGHT = 1 };
    enum ydir { UP = 1, DOWN = -1 };

    // The dimensions we can plot in, allowing for the width of the ball
    int m_iAvailableHeight, m_iAvailableWidth;

    int m_iImageHeight;         // The image height
    int m_iImageWidth;		// The image width
    int m_iBallSize;		// The diameter of the ball
    int m_iRandX, m_iRandY;     // For a bit of randomness
    xdir m_xDir;	        // Direction the ball
    ydir m_yDir;	        // Likewise vertically

    // The x position, in pixels, of the ball in the frame
    // (0 < x < m_iAvailableWidth)
    int m_x;

    // the y position, in pixels, of the ball in the frame
    // (0 < y < m_iAvailableHeight)
    int m_y;	

    // Return the 1-dimensional position of the ball at time t millisecs
    int BallPosition(int iPixelTime, int iLength, int time, int iOffset);

    /// tests a given pixel to see if it should be plotted
    BOOL WithinCircle(int x, int y);

}; // CBall

