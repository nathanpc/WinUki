/**
 * PageManager.c
 * Helps with the page view and edit windows.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <htmlctrl.h>
#include "PageManager.h"
#include "CommonDlgManager.h"
#include "UkiHelper.h"
#include "Utilities.h"
#include "resource.h"

// Global variables.
HINSTANCE hInst;
HINSTANCE hinstHTML;
HWND hwndPageEdit;
HWND hwndPageView;
UKIARTICLE ukiOpenArticle;
UKITEMPLATE ukiOpenTemplate;

// Private methods.
void ClearUkiState();
BOOL ShowWelcomePage();

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

	// Reset all the controls to the defaults.
	ClearPageToDefaults();
	
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

	// Clear our Uki state.
	ClearUkiState();

	// Get article and file contents.
	GetUkiArticle(&ukiOpenArticle, nIndex);
	GetUkiArticlePath(szPath, ukiOpenArticle);
	ReadFileContents(szPath, &szFileContents);

	// Set contents.
	SendMessage(hwndPageEdit, WM_SETTEXT, 0, (LPARAM)szFileContents);
    SendMessage(hwndPageView, WM_SETTEXT, 0, (LPARAM)L"");
    SendMessage(hwndPageView, DTM_ADDTEXTW, 0, (LPARAM)szFileContents);
    SendMessage(hwndPageView, DTM_ENDOFSOURCE, 0, 0);

	// Clear the modification flag of the edit control.
	SendMessage(hwndPageEdit, EM_SETMODIFY, (WPARAM)FALSE, 0);

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

	// Clear our Uki state.
	ClearUkiState();

	// Get template and file contents.
	GetUkiTemplate(&ukiOpenTemplate, nIndex);
	GetUkiTemplatePath(szPath, ukiOpenTemplate);
	ReadFileContents(szPath, &szFileContents);

	// Set contents.
	SendMessage(hwndPageEdit, WM_SETTEXT, 0, (LPARAM)szFileContents);
    SendMessage(hwndPageView, WM_SETTEXT, 0, (LPARAM)L"");
    SendMessage(hwndPageView, DTM_ADDTEXTW, 0, (LPARAM)szFileContents);
    SendMessage(hwndPageView, DTM_ENDOFSOURCE, 0, 0);

	// Clear the modification flag of the edit control.
	SendMessage(hwndPageEdit, EM_SETMODIFY, (WPARAM)FALSE, 0);

	// Free the file contents buffer.
	LocalFree(szFileContents);

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
 * Saves the currently open page.
 *
 * @return 0 if the operation was successful.
 */
LRESULT SaveCurrentPage() {
	LPTSTR szContents;
	LONG nTextLen;
	BOOL bSuccess = FALSE;
	
	// Allocate memory and load contents from the page editor control.
	nTextLen = SendMessage(hwndPageEdit, WM_GETTEXTLENGTH, 0, 0) + 1;
	szContents = (LPTSTR)LocalAlloc(LMEM_FIXED, (nTextLen + 1) * sizeof(TCHAR));
	SendMessage(hwndPageEdit, WM_GETTEXT, (WPARAM)nTextLen, (LPARAM)szContents);

	// Save article or template.
	if (IsArticleLoaded()) {
		bSuccess = SaveUkiArticle(ukiOpenArticle, szContents);
	} else if (IsTemplateLoaded()) {
		bSuccess = SaveUkiTemplate(ukiOpenTemplate, szContents);
	}

	// Clear the modification flag of the edit control.
	SendMessage(hwndPageEdit, EM_SETMODIFY, (WPARAM)FALSE, 0);

	LocalFree(szContents);
	return (LRESULT)(!bSuccess);
}

/**
 * Creates a new page with the contents empty.
 * @remark Remember to refresh the TreeView after this.
 *
 * @param  fIsArticle Is this new page an article?
 * @return            0 if the operation was successful.
 */
LRESULT CreateNewPage(BOOL fIsArticle) {
	TCHAR szPath[MAX_PATH];
	LONG nIndex;

	// Get the file name.
	if (fIsArticle) {
		if (!SaveNewPage(szPath, L"New Article", fIsArticle))
			return 1;
	} else {
		if (!SaveNewPage(szPath, L"New Template", fIsArticle))
			return 1;
	}

	// Add the file to the engine.
	if (fIsArticle) {
		// Add article.
		nIndex = AddUkiArticle(szPath);
		if (nIndex < 0L)
			return 1;

		// Set current open article.
		ClearUkiState();
		GetUkiArticle(&ukiOpenArticle, nIndex);
	} else {
		// Add template.
		nIndex = AddUkiTemplate(szPath);
		if (nIndex < 0L)
			return 1;

		// Set current open template.
		ClearUkiState();
		GetUkiTemplate(&ukiOpenTemplate, nIndex);
	}

	// Clear the editor contents.
	SendMessage(hwndPageEdit, WM_SETTEXT, 0, (LPARAM)L" ");

	// Save the new page.
	if (SaveCurrentPage())
		return 1;

	return 0;
}

/**
 * Saves a new page with the contents of the current opened one in a new file.
 * @remark Remember to refresh the TreeView after this.
 *
 * @return 0 if the operation was successful.
 */
LRESULT SavePageAs() {
	TCHAR szPath[MAX_PATH];
	LONG nIndex;

	// Get the file name.
	if (IsArticleLoaded()) {
		if (!SaveNewPage(szPath, L"Save Article As", IsArticleLoaded()))
			return 1;
	} else if (IsTemplateLoaded()) {
		if (!SaveNewPage(szPath, L"Save Template As", IsArticleLoaded()))
			return 1;
	} else {
		return 1;
	}

	// Add the file to the engine.
	if (IsArticleLoaded()) {
		// Add article.
		nIndex = AddUkiArticle(szPath);
		if (nIndex < 0L)
			return 1;

		// Set current open article.
		ClearUkiState();
		GetUkiArticle(&ukiOpenArticle, nIndex);
	} else if (IsTemplateLoaded()) {
		// Add template.
		nIndex = AddUkiTemplate(szPath);
		if (nIndex < 0L)
			return 1;

		// Set current open template.
		ClearUkiState();
		GetUkiTemplate(&ukiOpenTemplate, nIndex);
	}

	// Save the page as another one.
	if (SaveCurrentPage())
		return 1;

	return 0;
}

/**
 * Shows a nice welcome page in the page viewer.
 *
 * @return TRUE if the operation was successful.
 */
BOOL ShowWelcomePage() {
	// TODO: Load a static folder with the assets from the program root.
    SendMessage(hwndPageView, WM_SETTEXT, 0, (LPARAM)L"");

	return TRUE;
}

/**
 * Clears the internal Uki state of the module.
 */
void ClearUkiState() {
	// Clear article.
	ukiOpenArticle.path = NULL;
	ukiOpenArticle.name = NULL;
	ukiOpenArticle.parent = NULL;
	ukiOpenArticle.deepness = 0;

	// Clear template.
	ukiOpenTemplate.path = NULL;
	ukiOpenTemplate.name = NULL;
	ukiOpenTemplate.parent = NULL;
	ukiOpenTemplate.deepness = 0;
}

/**
 * Clears the page viewer and editor to its defaults.
 */
void ClearPageToDefaults() {
	// Clear controls.
	ShowWelcomePage();
	SendMessage(hwndPageEdit, WM_SETTEXT, 0, (LPARAM)L"");

	// Clear internal state.
	ClearUkiState();
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

/**
 * Checks if there's currently an article loaded on the screen.
 *
 * @return TRUE if there is.
 */
BOOL IsArticleLoaded() {
	return ukiOpenArticle.name != NULL;
}

/**
 * Checks if there's currently a template loaded on the screen.
 *
 * @return TRUE if there is.
 */
BOOL IsTemplateLoaded() {
	return ukiOpenTemplate.name != NULL;
}

/**
 * Checks if the page text was modified and haven't been saved yet.
 *
 * @return TRUE if the text state is dirty.
 */
BOOL IsPageDirty() {
	return SendMessage(hwndPageEdit, EM_GETMODIFY, 0, 0);
}