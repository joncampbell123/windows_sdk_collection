/****************************************************************************
 *
 *   capparms.cpp: Controls a dialog for setting capture parameters
 * 
 *   Microsoft Video for Windows Capture Class Sample Program
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include "stdafx.h"
#include <windowsx.h>
#include <compddk.h>
#include <msacm.h>

#include "captest.h"
#include "captedoc.h"
#include "captevw.h"
#include "mainfrm.h"
#include "capparms.h"

CCapParms::CCapParms(
CWnd* pParent /*=NULL*/)
	: CDialog(CCapParms::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCapParms)
	m_FrameRate = 0.F;
	m_EnableAudio = FALSE;
	m_DosBuffers = FALSE;
	m_VideoBuffers = 0;
	m_DisableSmartDrv = FALSE;
	//}}AFX_DATA_INIT
}

void CCapParms::DoDataExchange(
CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCapParms)
	DDX_Control(pDX, IDC_AUDIOFORMATTEXT, m_AudioFormatText);
	DDX_Text(pDX, IDC_FRAME_RATE, m_FrameRate);
	DDV_MinMaxFloat(pDX, m_FrameRate, (float)1.7e-002, (float)100.);
	DDX_Check(pDX, IDC_AUDIO, m_EnableAudio);
	DDX_Check(pDX, IDC_DOS_BUFFERS, m_DosBuffers);
	DDX_Text(pDX, IDC_VIDEO_BUFFERS, m_VideoBuffers);
	DDV_MinMaxInt(pDX, m_VideoBuffers, 1, 1000);
	DDX_Check(pDX, IDC_SMARTDRV, m_DisableSmartDrv);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCapParms, CDialog)
	//{{AFX_MSG_MAP(CCapParms)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SETAUDIOFORMAT, OnClickedSetaudioformat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCapParms message handlers

int CCapParms::OnCreate(
LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	return 0;
}


BOOL CCapParms::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SetWaveFormatText (hwndCap, &m_AudioFormatText);
	
	return TRUE; // return TRUE unless you set the focus to a control
}

void CCapParms::OnOK()
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CCapParms::SetWaveFormatText(
HWND hwndCap, 
CStatic *pcStatic) 
{
	LPWAVEFORMATEX lpwfex;
	DWORD dwSize;
	ACMFORMATDETAILS acmfd;

	pcStatic->SetWindowText("");
	
	// Get the current audio format
	if (dwSize = capGetAudioFormatSize (hwndCap)) {
		if (lpwfex = (LPWAVEFORMATEX) GlobalAllocPtr(GHND, dwSize)) {
			if (capGetAudioFormat(hwndCap, lpwfex, dwSize)) {

				_fmemset (&acmfd, 0, sizeof (ACMFORMATDETAILS));
				acmfd.cbStruct = sizeof (ACMFORMATDETAILS);
				acmfd.pwfx = lpwfex;
				if (lpwfex->wFormatTag == WAVE_FORMAT_PCM) 
					dwSize = sizeof (PCMWAVEFORMAT);
				else
					dwSize = sizeof (WAVEFORMATEX) + lpwfex -> cbSize;

				acmfd.cbwfx = dwSize;
				acmfd.dwFormatTag = lpwfex->wFormatTag;
				acmFormatDetails (NULL, &acmfd, ACM_FORMATDETAILSF_FORMAT);

				pcStatic->SetWindowText(acmfd.szFormat);
			}
			GlobalFreePtr(lpwfex) ;
		}
	}
} 


void CCapParms::OnClickedSetaudioformat()
{
	ACMFORMATCHOOSE cfmt;
	LPWAVEFORMATEX lpwfex;
	DWORD dwSize;
	
	// Ask the ACM what the largest wave format is.....
	acmMetrics(NULL,
		ACM_METRIC_MAX_SIZE_FORMAT,
		&dwSize);

	// Get the current audio format
	dwSize = max (dwSize, capGetAudioFormatSize (hwndCap));
	lpwfex = (LPWAVEFORMATEX) GlobalAllocPtr(GHND, dwSize) ;
	capGetAudioFormat(hwndCap, lpwfex, dwSize) ;

	_fmemset (&cfmt, 0, sizeof (ACMFORMATCHOOSE));
	cfmt.cbStruct = sizeof (ACMFORMATCHOOSE);
	cfmt.fdwStyle = ACMFORMATCHOOSE_STYLEF_INITTOWFXSTRUCT;
	// Only allow PCM and hardware compressed audio formats 
	cfmt.fdwEnum = ACM_FORMATENUMF_HARDWARE |
							ACM_FORMATENUMF_INPUT;
	cfmt.hwndOwner = GetParent()->GetSafeHwnd();
	cfmt.pwfx = lpwfex;
	cfmt.cbwfx = dwSize;
	
	if (!acmFormatChoose(&cfmt)) {
		capSetAudioFormat(hwndCap, lpwfex, dwSize) ;
		SetWaveFormatText (hwndCap, &m_AudioFormatText);
	}
	
	GlobalFreePtr(lpwfex) ;
	
}
