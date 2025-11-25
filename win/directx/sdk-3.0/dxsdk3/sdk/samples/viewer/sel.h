/*==========================================================================
 *
 *  Copyright (C) 1995, 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: sel.h
 *
 ***************************************************************************/

void ShowBoxes(int);
int ToggleBoxes(void);
LPDIRECT3DRMFRAME SelectedFrame(void);
LPDIRECT3DRMMESHBUILDER SelectedVisual(void);
LPDIRECT3DRMLIGHT SelectedLight(void);
void DeselectVisual(void);
void SelectVisual(LPDIRECT3DRMMESHBUILDER visual, LPDIRECT3DRMFRAME frame);
void FindAndSelectVisual(LPDIRECT3DRMVIEWPORT view, int x, int y);
void CutVisual(void);
void CopyVisual(void);
void PasteVisual(LPDIRECT3DRMFRAME scene);
void DeleteVisual(void);
void ClearClipboard(void);
