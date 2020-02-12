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

// Operations.
HTREEITEM TreeViewAddItem(HTREEITEM hParent, LPCTSTR szText,
						  HTREEITEM hInsAfter, int iImage);
BOOL TreeViewExpandNode(HTREEITEM hNode);

#endif  // _TREEVIEWMANAGER_H