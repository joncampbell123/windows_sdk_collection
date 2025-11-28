#ifndef __AUDIO_H__
#define __AUDIO_H__


BOOL InitPrimarySoundBuffer( void );
BOOL InitSoundDevices( BOOL );
BOOL ReOpenSoundDevices( BOOL );
BOOL EnumDSDevices( void );
BOOL EnumWaveDevices( void );
BOOL FormatCodeToText( DWORD, LPSTR, int );
BOOL FormatCodeToWFX( DWORD, PWAVEFORMATEX );
void DisableFormatCode( PFORMATDATA, DWORD );
BOOL WriteSilenceToOutput( DWORD, DWORD );

void FillInputDeviceCombo( HWND );
void FillOutputDeviceCombo( HWND );

void ClearDSDeviceList( void );
void ClearWaveDeviceList( void );

void ScanAvailableDSFormats( void );
void ScanAvailableWaveFormats( void );
void CloseWaveDevice( void );
void CloseDSDevice( void );

BOOL InitBuffers( void );
BOOL DestroyBuffers( void );
BOOL StartBuffers( void );
BOOL StopBuffers( void );

BOOL CALLBACK DSEnumProc( LPGUID, LPSTR, LPSTR, LPVOID );
void CALLBACK WaveInCallback( HWAVEIN, UINT, DWORD, DWORD, DWORD );

#endif


