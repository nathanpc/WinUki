/**
 * CommonDlgManager.h
 * A helper module to handle common dialogs.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _COMMONDLGMANAGER_H
#define _COMMONDLGMANAGER_H

#include <windows.h>

// Initialization.
BOOL InitializeCommonDialogs(HINSTANCE hParentInst, HWND hParentWindow);

// Open dialogs.
BOOL OpenWorkspace(LPTSTR szWikiRoot);

#endif  // _COMMONDLGMANAGER_H