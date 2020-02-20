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

// Dialogs.
BOOL OpenWorkspace(LPTSTR szWikiRoot);
BOOL SaveNewPage(LPTSTR szFilePath, LPCTSTR szDialogTitle);

#endif  // _COMMONDLGMANAGER_H