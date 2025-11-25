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
	See SampIOS.h for descriptions of these functions

	This file is built into SampIOS.lib, which may be linked into your
	executables to supply the various output functions.
*/

#include <streams.h>
#include <stdio.h>
#include <iostream.h>
#include "SampIOS.h"

ostream & operator <<( ostream & out, LPCWSTR pwStr )
{
    const int WideStrLen = lstrlenW(pwStr);
    char *const buffer = new char[WideStrLen+1];
    const int bytes_used =
        WideCharToMultiByte ( CP_ACP, 0, pwStr, WideStrLen
                            , buffer, WideStrLen+1, 0, 0);
    ASSERT( bytes_used != FALSE );
    buffer[bytes_used] = '\0';
    out << buffer;
    delete [] buffer;
    return out;
}

ostream & operator <<( ostream & out, const GUID & clsid )
{
    OLECHAR sz_clsid[CHARS_IN_GUID] = L"{Unknown}";
    StringFromGUID2( clsid, sz_clsid, sizeof(sz_clsid)/sizeof(sz_clsid[0]) );
    out << sz_clsid;
    return out;
}

ostream & operator <<( ostream & out, const IBaseFilter *const pIFilter )
{
    HRESULT hr;
    {
        FILTER_INFO filter_info;
        filter_info.pGraph = 0;
        hr = const_cast<IBaseFilter *>(pIFilter)->QueryFilterInfo( &filter_info );
        ASSERT( SUCCEEDED(hr) );
        out << '"' << filter_info.achName << '"';
	if (filter_info.pGraph) {
            filter_info.pGraph->Release();
	}
    }
    {
        CLSID clsid;
        hr = const_cast<IBaseFilter *>(pIFilter)->GetClassID( &clsid );
        ASSERT( SUCCEEDED(hr) );
        out << clsid;
    }
    return out;
}

ostream & operator <<( ostream & out, const IEnumFilters *const pIEnumFilters )
{
    IBaseFilter * pIFilter;
    ULONG count;

    while ( SUCCEEDED(const_cast<IEnumFilters*>(pIEnumFilters)->Next( 1, &pIFilter, &count )) && (count > 0))
    {
        out << pIFilter << '\n';
        pIFilter->Release();
    }
    return out;
}

ostream & operator <<( ostream & out, const IPin *const pIPin )
{
    LPWSTR      pin_id;
    PIN_INFO    pin_info;
    const IPin *p_connected_to;
    pin_info.pFilter = 0;

    const_cast<IPin*>(pIPin)->QueryPinInfo( &pin_info );
    const_cast<IPin*>(pIPin)->QueryId( &pin_id );
    out << pin_info.pFilter << ':' << pin_id << '(' << pin_info.achName << ')';
    if (pin_info.pFilter) {
        pin_info.pFilter->Release();
	pin_info.pFilter = 0;
    }
    CoTaskMemFree( pin_id );

    const_cast<IPin*>(pIPin)->ConnectedTo( const_cast<IPin**>(&p_connected_to) );
    if ( p_connected_to )
    {
        out << (pin_info.dir == PINDIR_OUTPUT ? "-->" : "<--");
        const_cast<IPin*>(p_connected_to)->QueryPinInfo( &pin_info );
        const_cast<IPin*>(p_connected_to)->QueryId( &pin_id );
        out << pin_info.pFilter << ':' << pin_id << '(' << pin_info.achName << ')';
        CoTaskMemFree( pin_id );
        if (pin_info.pFilter) {
            pin_info.pFilter->Release();
        }
        const_cast<IPin*>(p_connected_to)->Release();
    }
    return out;
}


ostream & operator <<( ostream & out, const IEnumPins *const pIEnumPins )
{
    IPin * pIPin;
    ULONG count;

    while ( SUCCEEDED(const_cast<IEnumPins*>(pIEnumPins)->Next( 1, &pIPin, &count )) && (count > 0))
    {
        out << pIPin << '\n';
        pIPin->Release();
    }
    return out;
}

ostream & WriteAllPins( ostream & out, const IBaseFilter *const piFilter )
{
    HRESULT hr;
    IEnumPins * pIEnumPins;
    hr = const_cast<IBaseFilter*>(piFilter)->EnumPins( &pIEnumPins );
    ASSERT( SUCCEEDED(hr) );
    out << pIEnumPins << flush;
    pIEnumPins->Release();
    return out;
}

ostream & operator <<( ostream & out, const IFilterGraph *const piFilterGraph )
{
    IEnumFilters * pIEnumFilters;
    HRESULT hr = const_cast<IFilterGraph*>(piFilterGraph)->EnumFilters( &pIEnumFilters );
    ASSERT( SUCCEEDED(hr) );

    IBaseFilter * pIFilter;
    ULONG count;

    while ( SUCCEEDED(const_cast<IEnumFilters*>(pIEnumFilters)->Next( 1, &pIFilter, &count )) && (count > 0))
    {
        out << pIFilter << '\n';
        WriteAllPins( out, pIFilter );
        pIFilter->Release();
    }
    pIEnumFilters->Release();

    return out << endl;
}

extern "C" void DumpFilterInfo( const IBaseFilter * pIFilter )
{
    cout << pIFilter << endl;
}

extern "C" void DumpAllFilters( const IFilterGraph * piFilterGraph )
{
    HRESULT hr;
    IEnumFilters * pIEnumFilters;
    hr = const_cast<IFilterGraph*>(piFilterGraph)->EnumFilters( &pIEnumFilters );
    ASSERT( SUCCEEDED(hr) );
    cout << pIEnumFilters << flush;
    pIEnumFilters->Release();
}

extern "C" void DumpPinInfo( const IPin * pIPin )
{
    cout << pIPin << endl;
}

extern "C" void DumpAllPins( const IBaseFilter * piFilter )
{
    WriteAllPins( cout, piFilter );
}

extern "C" void DumpFilterGraph( const IFilterGraph * piFilterGraph )
{
    cout << piFilterGraph << flush;
}
