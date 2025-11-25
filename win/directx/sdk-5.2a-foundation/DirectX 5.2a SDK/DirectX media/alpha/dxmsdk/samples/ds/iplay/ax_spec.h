/************************************************************************
*                                                                       *
*               INTEL CORPORATION PROPRIETARY INFORMATION               *
*                                                                       *
*    This listing is supplied under the terms of a license agreement    *
*      with INTEL Corporation and may not be copied nor disclosed       *
*        except in accordance with the terms of that agreement.         *
*                                                                       *
*************************************************************************
*                                                                       *
*		Copyright (c) 1994-1997 Intel Corporation.		*
*                         All Rights Reserved.                          *
*                                                                       *
************************************************************************/

/*
 *  Tabs set to 4
 *
 *  ax_spec.h
 *
 *  DESCRIPTION:
 *  The Indeo(R) Video Interactive codec provides access to new features
 *  using the ICM_SETCODECSTATE and ICM_GETCODECSTATE messages for VfW.
 *	This header file defines the data structures used in these messages.
 */

/* $Header:   I:\proj\src\common\vcs\ax_spec.h_v   1.2	 16 Dec 1996 14:54:04	tschwart  $
 */

/*  Custom interface for Indeo(r) codecs
 */

#ifndef __AX_SPEC_H__
#define __AX_SPEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FOURCC_IV41

#	ifndef mmioFOURCC
#	define mmioFOURCC( ch0, ch1, ch2, ch3 )				\
		( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
		( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#	endif

#define FOURCC_IV41 mmioFOURCC('I','V','4','1')
#endif

#include "vfw_spec.h"

/* Temporary Encode parameters used to set up the compression information
 */ 

typedef struct { 
	DWORD dwFrameRate;
	DWORD dwKeyFrameInterval;
	DWORD dwDataRate;
	DWORD dwQuality;
	R4_FLAG fPadding;
} R4_ENC_CMP_DATA, FAR * PTR_R4_ENC_CMP_DATA;

/*	AX_SPEC's GUID's
 *
 *	{CCDA9131-CE8A-11CE-82DD-0800095A5B55}	for Encode
 *	{CCDA9132-CE8A-11CE-82DD-0800095A5B55}  for Decode
 *
 */

#define EC_KEY_FRAME (EC_USER + 1)
#define EC_DECODE_RECT_CHANGED (EC_USER + 2)

DEFINE_GUID(IID_IIndeoEncode,
0xCCDA9131, 0xCE8A, 0x11CE, 0x82, 0xDD, 0x08, 0x00, 0x09, 0x5A, 0x5B, 0x55);

/*	AX_SPEC	interface definition
*/

#undef	INTERFACE
#define	INTERFACE IIndeoEncode

DECLARE_INTERFACE_(IIndeoEncode, IUnknown)
{

	/*
	 *	IUnknown methods
	 */

	STDMETHOD(QueryInterface)
	(
		THIS_
		REFIID riid,
		LPVOID FAR* ppvObj
	) PURE;

	STDMETHOD_(ULONG,AddRef)
	(
		THIS
	) PURE;

	STDMETHOD_(ULONG,Release)
	(
		THIS
	) PURE;

	/*
	 *	Query methods
	 */

    STDMETHOD(query_EncodeSequence)
	(
		THIS_
		DWORD *pdwFlags 			/* [out] */ // get data pointer
	) PURE;

	STDMETHOD(query_ImageDimensions)
	(
		THIS_
		DWORD *pdwWidth,			/* [out] */ // get data pointer
		DWORD *pdwHeight			/* [out] */ // get data pointer
	) PURE;

	/*
	 *	Get methods
	 */

    STDMETHOD(get_EncodeSequence)
	(
		THIS_
		PTR_R4_ENC_SEQ_DATA pData	/* [out] */ // get data pointer
	) PURE;

    STDMETHOD(get_EncodePersistent)
	(
		THIS_
		PTR_R4_ENC_SEQ_DATA pData	/* [out] */ // get data pointer
	) PURE;

    STDMETHOD(get_EncodeCompression)
	(
		THIS_
		PTR_R4_ENC_CMP_DATA pData	/* [out] */ // get data pointer
	) PURE;

    STDMETHOD(get_EncodeCompressionDefault)
	(
		THIS_
		PTR_R4_ENC_CMP_DATA pData	/* [out] */ // get data pointer
	) PURE;


	/*
	 *	Set methods
	 */

    STDMETHOD(set_EncodeSequence)
	(
		THIS_
		PTR_R4_ENC_SEQ_DATA pData	/* [in] */ // set data pointer
	) PURE;

    STDMETHOD(set_EncodePersistent)
	(
		THIS_
		PTR_R4_ENC_SEQ_DATA pData	/* [in] */ // set data pointer
	) PURE;

    STDMETHOD(set_EncodeCompression)
	(
		THIS_
		PTR_R4_ENC_CMP_DATA pData	/* [in] */ // set data pointer
	) PURE;
};


DEFINE_GUID(IID_IIndeoDecode,
0xCCDA9132, 0xCE8A, 0x11CE, 0x82, 0xDD, 0x08, 0x00, 0x09, 0x5A, 0x5B, 0x55);

#undef	INTERFACE
#define	INTERFACE IIndeoDecode

DECLARE_INTERFACE_(IIndeoDecode, IUnknown)
{

	/*
	 *	IUnknown methods
	 */

	STDMETHOD(QueryInterface)
	(
		THIS_
		REFIID riid,
		LPVOID FAR* ppvObj
	) PURE;

	STDMETHOD_(ULONG,AddRef)
	(
		THIS
	) PURE;

	STDMETHOD_(ULONG,Release)
	(
		THIS
	) PURE;

	/*
	 *	Query methods
	 */

    STDMETHOD(query_DecodeSequence)
	(
		THIS_
		DWORD *pdwFlags				/* [out] */ // query data pointer
	) PURE;

    STDMETHOD(query_DecodeFrame)
	(
		THIS_
		DWORD *pdwFlags				/* [out] */ // query data pointer
	) PURE;

	STDMETHOD(query_ImageDimensions)
	(
		THIS_
		DWORD *pdwWidth,			/* [out] */ // get data pointer
		DWORD *pdwHeight			/* [out] */ // get data pointer
	) PURE;

	/*
	 *	Get methods
	 */

    STDMETHOD(get_DecodeSequence)
	(
		THIS_
		PTR_R4_DEC_SEQ_DATA pData 	/* [out] */ // get data pointer
	) PURE;

    STDMETHOD(get_DecodeFrame)
	(
		THIS_
		PTR_R4_DEC_FRAME_DATA pData	/* [out] */ // get data pointer
	) PURE;


	/*
	 *	Set methods
	 */

    STDMETHOD(set_DecodeSequence)
	(
		THIS_
		PTR_R4_DEC_SEQ_DATA pData	/* [in] */ // set data pointer
	) PURE;

    STDMETHOD(set_DecodeFrame)
	(
		THIS_
		PTR_R4_DEC_FRAME_DATA pData	/* [in] */ // set data pointer
	) PURE;

	STDMETHOD(set_NotifyKeyFrame)
	(
		THIS_
		DWORD bState				/* [in] */ // Set notify on/off
	) PURE;
};

#ifdef __cplusplus
}
#endif

#endif /* __AX_SPEC_H__ */
