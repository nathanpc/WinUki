/**
 * PageManager.h
 * Helps with the page view and edit windows.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _PAGEMANAGER_H
#define _PAGEMANAGER_H

#include <windows.h>

// State check.
BOOL IsArticleLoaded();
BOOL IsTemplateLoaded();

// Initialization.
void ClearPageToDefaults();
BOOL InitializePageView(HINSTANCE hParentInst, HWND hwndParent, RECT rcClient,
						HMENU hPageEditID, HMENU hPageViewID);

// Messaging.
LRESULT SendPageEditMessage(UINT wMsg, WPARAM wParam, LPARAM lParam);
LRESULT PageEditHandleCommand(HWND hWnd, UINT wMsg, WPARAM wParam,
							  LPARAM lParam);

// Population.
BOOL PopulatePageViewArticle(const size_t nIndex);
BOOL PopulatePageViewTemplate(const size_t nIndex);

// Visibility.
BOOL IsPageEditorActive();
void ShowPageViewer();
void ShowPageEditor();
void TogglePageView();

// Saving.
LRESULT SaveCurrentPage();
LRESULT CreateNewPage(BOOL fIsArticle);
LRESULT SavePageAs();

// Handle getters.
HWND GetPageEditHandle();
HWND GetPageViewHandle();

#endif  // _PAGEMANAGER_H