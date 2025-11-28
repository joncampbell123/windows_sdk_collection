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

/*
	This header file provides function prototypes for the functions
	exported from AM_ios.lib.  These functions are designed to enable
	developers to get information in text format regarding objects in
	the filter graph.

	The 'C' functions, DumpXXX, require "cout" to be valid in the environment
	in which you are working.
*/

#ifndef _SampIOS_h
#define _SampIOS_h

#ifdef __cplusplus

    // Just so you can write:   cout << piFilterGraph << endl;  etc.

    // Converts wide string to ANSI string and outputs it
    ostream & operator <<( ostream & out, LPCWSTR pwStr );
    // Builds and outputs a textual version of a GUID
    ostream & operator <<( ostream & out, const GUID & clsid );
    // Output filter information
    ostream & operator <<( ostream & out, const IBaseFilter *const pIFilter );
    // Output filter informations for all filters left in the enumerator
    ostream & operator <<( ostream & out, const IEnumFilters *const pIEnumFilters );
    // Output filter and connection details
    ostream & operator <<( ostream & out, const IFilterGraph *const piFilterGraph );
    // Output pin details (including connection info)
    ostream & operator <<( ostream & out, const IPin *const pIPin );
    // As above, for all pins left in the enumerator
    ostream & operator <<( ostream & out, const IEnumPins *const pIEnumPins );
    // Output the pin info for all pins on supplied filter
    ostream & WriteAllPins( ostream & out, const IBaseFilter *const pIFilter );

    extern "C" {
#endif

void DumpFilterInfo( const IBaseFilter * pIFilter );
void DumpPinInfo( const IPin * pIPin );
void DumpAllPins( const IBaseFilter * piFilter );
void DumpAllFilters( const IFilterGraph * piFilterGraph );
void DumpFilterGraph( const IFilterGraph * piFilterGraph );

#ifdef __cplusplus
    }
#endif

#endif /* _SampIOS_h */
