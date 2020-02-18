/**
 * CommonDlgManager.c
 * A helper module to handle common dialogs.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <windows.h>
#include <commdlg.h>
#include "CommonDlgManager.h"
#include "UkiHelper.h"
#include "resource.h"

// Global variables.
HINSTANCE hInst;
HWND hwndParent;

/**
 * Initializes the common dialog manager.
 *
 * @param  hParentInst   Application interface handle.
 * @param  hParentWindow Parent window handle.
 * @return               TRUE if the initialization was successful.
 */
BOOL InitializeCommonDialogs(HINSTANCE hParentInst, HWND hParentWindow) {
	hInst = hParentInst;
	hwndParent = hParentWindow;

	return TRUE;
}

/**
 * Opens a Uki wiki root.
 *
 * @param  szWikiRoot Pre-allocated string to store the wiki root path.
 * @return            TRUE if we actually opened something.
 */
BOOL OpenWorkspace(LPTSTR szWikiRoot) {
	OPENFILENAME ofn = {0};
    TCHAR szManifestPath[MAX_PATH] = L"";

	// Populate the structure.
    ofn.lStructSize = sizeof(ofn);
	ofn.lpstrTitle = L"Open Uki Workspace";
    ofn.hwndOwner = hwndParent;
    ofn.lpstrFilter = L"Uki Manifest (MANIFEST.uki)\0MANIFEST.uki\0"
		L"Uki Files (*.uki)\0*.uki\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szManifestPath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = L"uki";

	// Open the file dialog.
	if (!GetOpenFileName(&ofn))
		return FALSE;

	// Get the wiki root and return
	return GetUkiRootFromManifest(szWikiRoot, szManifestPath);
}