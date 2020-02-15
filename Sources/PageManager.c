/**
 * PageManager.c
 * Helps with the page view and edit windows.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "PageManager.h"

// Global variables.
HWND hwndPageEdit;


/**
 * Initializes the TreeView component.
 *
 * @param  hInst       Application interface handle.
 * @param  hwndParent  Parent window handle.
 * @param  rcClient    Client rectagle to place the controls.
 * @param  hPageEditID Page edit control resource ID.
 * @return             TRUE if the initialization was successful.
 */
BOOL InitializePageView(HINSTANCE hInst, HWND hwndParent,
						RECT rcClient, HMENU hPageEditID) {
	// Create the Edit page view control.
	hwndPageEdit = CreateWindowEx(0, L"EDIT", NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE |
		ES_AUTOVSCROLL,
		rcClient.left, rcClient.top, rcClient.right, rcClient.bottom,
		hwndParent, hPageEditID, hInst, NULL);

	return TRUE;
}