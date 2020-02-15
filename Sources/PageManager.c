/**
 * PageManager.c
 * Helps with the page view and edit windows.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "PageManager.h"
#include "UkiHelper.h"
#include "Utilities.h"

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

	// Free the file contents buffer.
	LocalFree(szFileContents);

	return TRUE;
}