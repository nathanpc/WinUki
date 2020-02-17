/**
 * PageManager.c
 * Helps with the page view and edit windows.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <htmlctrl.h>
#include "PageManager.h"
#include "UkiHelper.h"
#include "Utilities.h"
#include "resource.h"

// Global variables.
HINSTANCE hInst;
LPCTSTR szHTMLControlLibrary = L"htmlview.dll";
HINSTANCE hinstHTML;
HWND hwndPageEdit;
HWND hwndPageView;

/**
 * Initializes the TreeView component.
 *
 * @param  hParentInst Application interface handle.
 * @param  hwndParent  Parent window handle.
 * @param  rcClient    Client rectagle to place the controls.
 * @param  hPageEditID Page edit control resource ID.
 * @param  hPageViewID Page HTML viewer control resource ID.
 * @return             TRUE if the initialization was successful.
 */
BOOL InitializePageView(HINSTANCE hParentInst, HWND hwndParent, RECT rcClient,
						HMENU hPageEditID, HMENU hPageViewID) {
	hInst = hParentInst;

	// Create the Edit page view control.
	hwndPageEdit = CreateWindowEx(0, L"EDIT", NULL,
		WS_CHILD | WS_BORDER | WS_VSCROLL | ES_LEFT | ES_MULTILINE |
		ES_AUTOVSCROLL | ES_NOHIDESEL,
		rcClient.left, rcClient.top, rcClient.right, rcClient.bottom,
		hwndParent, hPageEditID, hInst, NULL);

	// Set editor to the max limit.
	SendMessage(hwndPageEdit, EM_SETLIMITTEXT, 0, 0);

	// Load the HTMLViewer control.
	InitHTMLControl(hInst);
	
	// Create HTMLViewer control.
	hwndPageView = CreateWindow(DISPLAYCLASS, NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE | WS_CLIPSIBLINGS,
		rcClient.left, rcClient.top, rcClient.right, rcClient.bottom,
		hwndParent, hPageViewID, hInst, NULL);

	// Make images fit the HTML viewer.
	PostMessage(hwndPageView, DTM_ENABLESHRINK, 0, (LPARAM)TRUE);

	// Show welcome.
	ShowWelcomePage();
	
	return TRUE;
}

/**
 * Sends a message to the page editor control.
 *
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT SendPageEditMessage(UINT wMsg, WPARAM wParam, LPARAM lParam) {
	return SendMessage(hwndPageEdit, wMsg, wParam, lParam);
}

/**
 * Process the WM_COMMAND message for the page editor.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT PageEditHandleCommand(HWND hWnd, UINT wMsg, WPARAM wParam,
							  LPARAM lParam) {
	switch(HIWORD(wParam)) {
	default:
		return DefWindowProc(hWnd, wMsg, wParam, lParam);
	}

	return 0;
}

/**
 * Populates the page view with an article.
 *
 * @param  nIndex Article index.
 * @return        TRUE if the operation was successful.
 */
BOOL PopulatePageViewArticle(const size_t nIndex) {
	TCHAR szPath[UKI_MAX_PATH];
	LPTSTR szFileContents;
	UKIARTICLE ukiArticle;

	// Get article and file contents.
	GetUkiArticle(&ukiArticle, nIndex);
	GetUkiArticlePath(szPath, ukiArticle);
	ReadFileContents(szPath, &szFileContents);

	// Set contents.
	SendMessage(hwndPageEdit, WM_SETTEXT, 0, (LPARAM)szFileContents);
    SendMessage(hwndPageView, WM_SETTEXT, 0, (LPARAM)L"");
    SendMessage(hwndPageView, DTM_ADDTEXTW, 0, (LPARAM)szFileContents);
    SendMessage(hwndPageView, DTM_ENDOFSOURCE, 0, 0);

	// Free the file contents buffer.
	LocalFree(szFileContents);

	return TRUE;
}

/**
 * Populates the page view with a template.
 *
 * @param  nIndex Template index.
 * @return        TRUE if the operation was successful.
 */
BOOL PopulatePageViewTemplate(const size_t nIndex) {
	TCHAR szPath[UKI_MAX_PATH];
	LPTSTR szFileContents;
	UKITEMPLATE ukiTemplate;

	// Get template and file contents.
	GetUkiTemplate(&ukiTemplate, nIndex);
	GetUkiTemplatePath(szPath, ukiTemplate);
	ReadFileContents(szPath, &szFileContents);

	// Set contents.
	SendMessage(hwndPageEdit, WM_SETTEXT, 0, (LPARAM)szFileContents);
    SendMessage(hwndPageView, WM_SETTEXT, 0, (LPARAM)L"");
    SendMessage(hwndPageView, DTM_ADDTEXTW, 0, (LPARAM)szFileContents);
    SendMessage(hwndPageView, DTM_ENDOFSOURCE, 0, 0);

	// Free the file contents buffer.
	LocalFree(szFileContents);

	return TRUE;
}

/**
 * Shows a nice welcome page in the page viewer.
 *
 * @return TRUE if the operation was successful.
 */
BOOL ShowWelcomePage() {
	HRSRC hrSource;
	HGLOBAL hResource;
	LPCTSTR szHTML;
	
	// Get source information handle.
	hrSource = FindResource(NULL, MAKEINTRESOURCE(IDR_WELCOME), RT_RCDATA);
	if (hrSource == NULL)
		return FALSE;

	// Get resource handle.
	hResource = LoadResource(NULL, hrSource);
	if (hResource == NULL)
		return FALSE;
	
	// Get resource contents.
	szHTML = (LPCTSTR)LockResource(hResource);
	if (szHTML == NULL)
		return FALSE;
	
	return TRUE;
}

/**
 * Shows the HTML viewer control.
 */
void ShowPageViewer() {
	LPTSTR szEditorContents;
	LONG nTextLen;

	ShowWindow(hwndPageEdit, SW_HIDE);
	ShowWindow(hwndPageView, SW_SHOW);

	// TODO: Check for modifications.

	// Allocate buffer and populate it.
	nTextLen = SendMessage(hwndPageEdit, WM_GETTEXTLENGTH, 0, 0) + 1;
	szEditorContents = LocalAlloc(LMEM_FIXED, (nTextLen + 1) * sizeof(TCHAR));
	SendMessage(hwndPageEdit, WM_GETTEXT, (WPARAM)nTextLen,
		(LPARAM)szEditorContents);

	// Set page view contents to page editor.
    SendMessage(hwndPageView, WM_SETTEXT, 0, (LPARAM)L"");
    SendMessage(hwndPageView, DTM_ADDTEXTW, 0, (LPARAM)szEditorContents);
    SendMessage(hwndPageView, DTM_ENDOFSOURCE, 0, 0);

	// Clean up.
	LocalFree(szEditorContents);
}

/**
 * Shows the page editor control.
 */
void ShowPageEditor() {
	ShowWindow(hwndPageView, SW_HIDE);
	ShowWindow(hwndPageEdit, SW_SHOW);
}

/**
 * Toggles between the HTML viewer and editor controls.
 */
void TogglePageView() {
	if (IsPageEditorActive()) {
		ShowPageViewer();
	} else {
		ShowPageEditor();
	}
}

/**
 * Checks if the page editor is currently active.
 *
 * @return TRUE if the user is editing a page.
 */
BOOL IsPageEditorActive() {
	return IsWindowVisible(hwndPageEdit);
}

/**
 * Fetches the page editor window handle.
 *
 * @return The page editor window handle.
 */
HWND GetPageEditHandle() {
	return hwndPageEdit;
}

/**
 * Fetches the page viewer window handle.
 *
 * @return The page viewer window handle.
 */
HWND GetPageViewHandle() {
	return hwndPageView;
}
