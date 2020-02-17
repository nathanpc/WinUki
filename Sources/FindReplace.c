/**
 * FindReplace.c
 * Helps with the page view and edit windows.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <windowsx.h>
#include <ctype.h>
#include "FindReplace.h"
#include "PageManager.h"
#include "resource.h"

// Constants.
#define MAX_FIND_STRLEN 100

// Global variables.
HINSTANCE hInst;
HWND hwndParent;
HWND hwndEdit;
HWND hwndDialog;
TCHAR szNeedle[MAX_FIND_STRLEN + 1];
int fDirection;
BOOL fMatchCase;
BOOL fCanFindNext;

// Private methods.
LONG FindNext(LPTSTR szHaystack, LONG nCursorPos);
BOOL SetFindNextState(HWND hWnd);
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
	fCanFindNext = FALSE;

	return TRUE;
}

/**
 * Performs the "Find Next" operation and selects the needle found in the
 * haystack of the edit box.
 *
 * @return TRUE if we found something.
 */
BOOL PageEditFindNext() {
	LPTSTR szHaystack;
	LONG nTextLen;
	LONG nCursorPos;
	size_t nNeedleLen;
	int fCachedDirection = fDirection;

	// Allocate memory for the haystack and get the text.
	nTextLen = SendDlgItemMessage(hwndParent, IDC_EDITPAGE,
		WM_GETTEXTLENGTH, 0, 0) + 1;
	szHaystack = LocalAlloc(LMEM_FIXED, (nTextLen + 1) * sizeof(TCHAR));
	GetDlgItemText(hwndParent, IDC_EDITPAGE, szHaystack, nTextLen);

	// Get current cursor position and needle length.
	SendDlgItemMessage(hwndParent, IDC_EDITPAGE, EM_GETSEL, (WPARAM)NULL,
		(LPARAM)&nCursorPos);
	nNeedleLen = wcslen(szNeedle);

	// Respect the direction chosen.
	if (fDirection == IDC_RADIOFINDANY) {
		nCursorPos = 0;
		fDirection = IDC_RADIOFINDDOWN;
	}

	// Find occurence.
	nCursorPos = FindNext(szHaystack, nCursorPos);
	if (nCursorPos >= 0L) {
		// Select the text.
		ShowPageEditor();
		SendDlgItemMessage(hwndParent, IDC_EDITPAGE, EM_SETSEL,
			(WPARAM)nCursorPos, (LPARAM)(nCursorPos + nNeedleLen));
	} else {
		// Show not found message.
		TCHAR szMsg[MAX_FIND_STRLEN + 21];
		wsprintf(szMsg, L"Cannot find \"%s\".", szNeedle);
		MessageBox(NULL, szMsg, L"Not Found", MB_OK | MB_ICONEXCLAMATION);

		// Reset the direction flag in case it was in Any before.
		fDirection = fCachedDirection;
	}

	// Clean up and return.
	LocalFree(szHaystack);
	return FALSE;
}

/**
 * Finds the next occurence of a needle in a haystack after the cursor position.
 *
 * @param  szHaystack Haystack to find the needle in.
 * @param  nCursorPos Starting cursor position.
 * @return            Cursor position of the next occurence or -1 in case of
 *                    failure.
 */
LONG FindNext(LPTSTR szHaystack, LONG nCursorPos) {
	LPTSTR szLocalNeedle;
	LPTSTR lpLocalNeedle;
	LPTSTR lpHaystack = szHaystack;
	LPTSTR lpFound;
	LONG nPos;

	// Convert everything to uppercase if we want a case insensitive search.
	if (!fMatchCase) {
		// Allocate space for string.
		szLocalNeedle = (LPTSTR)LocalAlloc(LMEM_FIXED, (wcslen(szNeedle) + 1) *
			sizeof(TCHAR));
		lpLocalNeedle = szLocalNeedle;

		// Copy needle string and convert to uppercase.
		wcscpy(szLocalNeedle, szNeedle);
		for ( ; *lpLocalNeedle; lpLocalNeedle++)
			*lpLocalNeedle = towupper(*lpLocalNeedle);

		// Convert haystack string to uppercase.
		for ( ; *lpHaystack; lpHaystack++)
			*lpHaystack = towupper(*lpHaystack);
	} else {
		szLocalNeedle = szNeedle;
	}

	// Find occurence.
	lpLocalNeedle = szLocalNeedle;
	lpHaystack = szHaystack + nCursorPos;
	lpFound = wcsstr(lpHaystack, szLocalNeedle);
	if (lpFound == NULL)
		return -1L;

	// Cleanup.
	if (!fMatchCase) {
		LocalFree(szLocalNeedle);
	}

	// Calculate the cursor position and return.
	nPos = lpFound - szHaystack;
	return nPos;
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

	// Make sure the text is selected for easily replacing and set focus.
	SendDlgItemMessage(hWnd, IDC_FINDEDIT, EM_SETSEL, 0, -1);
	SendMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hWnd, IDC_FINDEDIT),
		TRUE);

	// Set the selected direction radio button.
	CheckRadioButton(hWnd, IDC_RADIOFINDUP, IDC_RADIOFINDANY, fDirection);

	// Set the match case checkbox.
	if (fMatchCase) {
		SendDlgItemMessage(hWnd, IDC_CHECKMATCHCASE, BM_SETCHECK,
			BST_CHECKED, 0);
	} else {
		SendDlgItemMessage(hWnd, IDC_CHECKMATCHCASE, BM_SETCHECK,
			BST_UNCHECKED, 0);
	}

	// Enable/disable the Find Next button if there's something in the edit box.
	SetFindNextState(hWnd);

	// Store our dialog handle.
	hwndDialog = hWnd;

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
		PageEditFindNext();
		break;
	case IDC_FINDCANCEL:
		// Cancel button.
		SaveNeedleText(hWnd);
		EndDialog(hWnd, 0);
		break;
	case IDC_FINDEDIT:
		// Find edit box.
		switch (HIWORD(wParam)) {
		case EN_CHANGE:
			// Change event.
			SetFindNextState(hWnd);
		}
		break;
	}

	return TRUE;
}

/**
 * Sets the "Find Next" button and flag state.
 *
 * @param  hWnd This dialog handle.
 * @return      TRUE if we can find next.
 */
BOOL SetFindNextState(HWND hWnd) {
	if (SendDlgItemMessage(hWnd, IDC_FINDEDIT, WM_GETTEXTLENGTH, 0, 0) > 0) {
		EnableWindow(GetDlgItem(hWnd, IDC_FINDNEXT), TRUE);
		fCanFindNext = TRUE;
	} else {
		EnableWindow(GetDlgItem(hWnd, IDC_FINDNEXT), FALSE);
		fCanFindNext = FALSE;
	}

	return fCanFindNext;
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

/**
 * Checks if we can perform a "Find Next" operation.
 *
 * @return TRUE if we can.
 */
BOOL PageEditCanFindNext() {
	return fCanFindNext;
}