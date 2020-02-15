/**
 * PageManager.h
 * Helps with the page view and edit windows.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _PAGEMANAGER_H
#define _PAGEMANAGER_H

#include <windows.h>

// Initialization.
BOOL InitializePageView(HINSTANCE hInst, HWND hwndParent, RECT rcClient,
						HMENU hPageEditID, HMENU hPageViewID);

// Population.
BOOL PopulatePageViewArticle(const size_t nIndex);
BOOL PopulatePageViewTemplate(const size_t nIndex);

// Visibility.
void TogglePageView();

#endif  // _PAGEMANAGER_H