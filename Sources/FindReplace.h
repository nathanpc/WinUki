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

// Dialog.
int ShowFindDialog();

#endif  // _FINDREPLACE_H