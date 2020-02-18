/**
 * TreeViewManager.h
 * A simple TreeView manager helper module.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _TREEVIEWMANAGER_H
#define _TREEVIEWMANAGER_H

#include <windows.h>
#include <commctrl.h>
#include "resource.h"

// Initialization.
HWND InitializeTreeView(HINSTANCE hInst, HWND hwndParent,
						RECT rcClient, HMENU hTreeViewID,
						HIMAGELIST hIml);
BOOL TreeViewClear();

// Operations.
HTREEITEM TreeViewAddItem(HTREEITEM hParent, LPTSTR szText,
						  HTREEITEM hInsAfter, int iImage,
						  LPARAM lParam);
BOOL TreeViewGetItem(TVITEM *tvItem);
BOOL TreeViewExpandNode(HTREEITEM hNode);

#endif  // _TREEVIEWMANAGER_H