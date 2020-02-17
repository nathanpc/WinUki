/**
 * FindReplace.h
 * Helps with the page view and edit windows.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _FINDREPLACE_H
#define _FINDREPLACE_H

#include <windows.h>

// Initialization.
BOOL InitializeFindReplace(HINSTANCE hParentInst, HWND hParentWindow,
						   HWND hwndEditCtrl);

// Finding and replacing.
BOOL PageEditCanFindNext();
BOOL PageEditFindNext(BOOL fShowMsg);
BOOL PageEditReplaceNext(BOOL fShowMsg);
BOOL PageEditReplaceAll();

// Dialog.
int ShowFindDialog();
int ShowReplaceDialog();

#endif  // _FINDREPLACE_H