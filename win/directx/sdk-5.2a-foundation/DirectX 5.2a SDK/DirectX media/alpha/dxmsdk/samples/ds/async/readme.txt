#----------------------------------------------------------------------------
#
#  ActiveMovie readme.txt
#
#  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#  PURPOSE.
#
#  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
#
#----------------------------------------------------------------------------

The async sample code comprises 3 directories:


INCLUDE
-------

Defines some helper classes to create filters which conform to the
IAsyncReader interface.


Four classes are defined:

    CAsyncReader
    ------------
    To create a filter.  Defines the filter and pin.

    CAsyncOutputPin
    ---------------
    The filter's output pin

    CAsyncIo
    --------
    Methods to manage a set of async requests

    CAsyncStream
    ------------
    An abstract class that represents the source of the data


BASE
----

    Impelments

    -- CAsyncReader

    -- CAsyncOutputPin

    -- CAsyncIo

    CAsyncReader is passed a CAsyncStream object which the filter uses
    as its data source


MEMFILE
-------

    Consists of

    1.  A definition of

        --  CMemStream - a non-abstract class derived from CAsyncStream that
            wraps a memory segment as the IAsyncReader source data

        --  CMemReader - a source filter derived from CAsyncReader.  It
            just overrides the constructor of CAsyncReader to set the
            output pin's media type dependent on the file type

    1.  A main() routine that

        Opens a file

        Guesses the file's media type dependent on the file extension.  Note
        that RenderFile actually looks at the check bytes in the

            HKEY_CLASSES_ROOT\Media Type

        key of the registation data base to try to determine the media type
        and media subtype of the file.

        Creates a filter graph

        Creates a source filter of type CMemReader and adds it to the filter
        graph

        Renders the output pin of this filter

        Plays the graph through once
