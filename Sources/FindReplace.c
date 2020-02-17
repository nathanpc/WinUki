/**
 * FindReplace.c
 * Helps with the page view and edit windows.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "FindReplace.h"
#include "PageManager.h"
#include "resource.h"

// Constants.
#define MAX_FIND_STRLEN 100

// Global variables.
HINSTANCE hInst;
HWND hwndParent;
HWND hwndEdit;
TCHAR szNeedle[MAX_FIND_STRLEN + 1];
int fDirection;
BOOL fMatchCase;

// Private methods.
UINT SaveNeedleText(HWND hWnd);
BOOL DlgFindInit(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
BOOL DlgFindCommand(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK FindDialogProc(HWND hWnd, UINT wMsg, WPARAM wParam,
							 LPARAM lParam);

/**
 * Initializes our Find/Replace engine.
 *
 * @param  hParentInst Application interface handle.
 * @param  hwndParent  Parent window handle.
 * @param  hwnEditCtrl Edit control handle to be used.
 * @return             TRUE if the initialization succeeded.
 */
BOOL InitializeFindReplace(HINSTANCE hParentInst, HWND hParentWindow,
						   HWND hwndEditCtrl) {
	// Controls.
	hInst = hParentInst;
	hwndParent = hParentWindow;
	hwndEdit = hwndEditCtrl;

	// Default values.
	szNeedle[0] = L'\0';
	fDirection = IDC_RADIOFINDDOWN;
	fMatchCase = FALSE;

	return TRUE;
}

/**
 * Find dialog procedure.
 *
 * @param  hWnd   Dialog window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        TRUE if we have processed the message.
 */
BOOL CALLBACK FindDialogProc(HWND hWnd, UINT wMsg, WPARAM wParam,
							 LPARAM lParam) {
	switch (wMsg) {
	case WM_INITDIALOG:
		return DlgFindInit(hWnd, wMsg, wParam, lParam);
	case WM_COMMAND:
		return DlgFindCommand(hWnd, wMsg, wParam, lParam);
	case WM_CLOSE:
		SaveNeedleText(hWnd);
		EndDialog(hWnd, 0);
		return FALSE;
	}

	return FALSE;
}

/**
 * Process the WM_INITDIALOG message for the find dialog.
 *
 * @param  hWnd   Dialog window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        TRUE if we have processed the message.
 */
BOOL DlgFindInit(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	// Limit the text input to fit our find buffer and set the text.
	SendDlgItemMessage(hWnd, IDC_FINDEDIT, EM_LIMITTEXT, MAX_FIND_STRLEN, 0);
	SendDlgItemMessage(hWnd, IDC_FINDEDIT, WM_SETTEXT, 0, (LPARAM)szNeedle);

	// Set the selected direction radio button.
	CheckRadioButton(hWnd, IDC_RADIOFINDUP, IDC_RADIOFINDANY, fDirection);

	// Set the match case checkbox.
	if (fMatchCase) {
		SendDlgItemMessage(hWnd, IDC_CHECKMATCHCASE, BM_SETCHECK, BST_CHECKED, 0);
	} else {
		SendDlgItemMessage(hWnd, IDC_CHECKMATCHCASE, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	return TRUE;
}

/**
 * Process the WM_COMMAND message for the find dialog.
 *
 * @param  hWnd   Dialog window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        TRUE if we have processed the message.
 */
BOOL DlgFindCommand(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) {
	switch (LOWORD(wParam)) {
	case IDC_CHECKMATCHCASE:
		// Match Case checkbox.
		fMatchCase = SendDlgItemMessage(hWnd, IDC_CHECKMATCHCASE,
			BM_GETCHECK, 0, 0) == BST_CHECKED;
		break;
	case IDC_RADIOFINDUP:
		// Up direction radio button.
		if (SendDlgItemMessage(hWnd, IDC_RADIOFINDUP, BM_GETCHECK, 0, 0)
			== BST_CHECKED)
			fDirection = IDC_RADIOFINDUP;
		break;
	case IDC_RADIOFINDDOWN:
		// Down direction radio button.
		if (SendDlgItemMessage(hWnd, IDC_RADIOFINDDOWN, BM_GETCHECK, 0, 0)
			== BST_CHECKED)
			fDirection = IDC_RADIOFINDDOWN;
		break;
	case IDC_RADIOFINDANY:
		// Any direction radio button.
		if (SendDlgItemMessage(hWnd, IDC_RADIOFINDANY, BM_GETCHECK, 0, 0)
			== BST_CHECKED)
			fDirection = IDC_RADIOFINDANY;
		break;
	case IDC_FINDNEXT:
		// Find Next button.
		SaveNeedleText(hWnd);
		EndDialog(hWnd, 0);
		break;
	case IDC_FINDCANCEL:
		// Cancel button.
		SaveNeedleText(hWnd);
		EndDialog(hWnd, 0);
		break;
	}

	return TRUE;
}

/**
 * Saves the find edit box contents to the needle buffer.
 *
 * @param  hWnd Dialog window handler.
 * @return      GetDlgItemText return value.
 */
UINT SaveNeedleText(HWND hWnd) {
	return GetDlgItemText(hWnd, IDC_FINDEDIT, szNeedle, MAX_FIND_STRLEN);
}

/**
 * Shows the Find dialog.
 *
 * @return The value of the nResult parameter in the call to the EndDialog.
 */
int ShowFindDialog() {
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_FIND), hwndParent, FindDialogProc);
}
