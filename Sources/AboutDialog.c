/**
 * AboutDialog.c
 * A simple about dialog.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "AboutDialog.h"
#include "resource.h"

// Private methods.
BOOL CALLBACK AboutDialogProc(HWND hWnd, UINT wMsg, WPARAM wParam,
							  LPARAM lParam);

/**
 * Shows a nice About dialog.
 *
 * @param  hParentInst Parent instance handle.
 * @param  hwndParent  Parent window handle.
 * @return             DialogBox return value.
 */
int ShowAboutDialog(HINSTANCE hParentInst, HWND hwndParent) {
	return DialogBox(hParentInst, MAKEINTRESOURCE(IDD_ABOUT), hwndParent,
		AboutDialogProc);
}

/**
 * Replace dialog procedure.
 *
 * @param  hWnd   Dialog window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        TRUE if we have processed the message.
 */
BOOL CALLBACK AboutDialogProc(HWND hWnd, UINT wMsg, WPARAM wParam,
							  LPARAM lParam) {
	switch (wMsg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return FALSE;
	}

	return FALSE;
}