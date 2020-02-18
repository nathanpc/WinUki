/**
 * TreeViewManager.c
 * A simple TreeView manager helper module.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "TreeViewManager.h"

// Global variables.
HWND hwndTreeView;

/**
 * Initializes the TreeView component.
 *
 * @param  hInst       Application interface handle.
 * @param  hwndParent  Parent window handle.
 * @param  rcClient    Client rectagle to place the TreeView.
 * @param  hTreeViewID TreeView resource ID.
 * @return             TreeView handle.
 */
HWND InitializeTreeView(HINSTANCE hInst, HWND hwndParent,
						RECT rcClient, HMENU hTreeViewID,
						HIMAGELIST hIml) {
    // Ensure that the common control DLL is loaded. 
    InitCommonControls(); 
	
	// Create TreeView window.
	hwndTreeView = CreateWindowEx(0, WC_TREEVIEW, L"Tree View",
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT, 
		rcClient.left, rcClient.top, rcClient.right, rcClient.bottom,
		hwndParent, hTreeViewID, hInst, NULL);

	// Associate an ImageList with the control.
	TreeView_SetImageList(hwndTreeView, hIml, TVSIL_NORMAL);

	return hwndTreeView;
}

/**
 * Adds an item to the TreeView.
 *
 * @param  hParent   Parent TreeView item handle.
 * @param  szText    Item caption.
 * @param  hInsAfter Insert after this TreeView item handle.
 * @param  iImage    Index to the item icon in the ImageList.
 * @param  lParam    Item lParam.
 * @return           Added item handle.
 */
HTREEITEM TreeViewAddItem(HTREEITEM hParent, LPTSTR szText,
						  HTREEITEM hInsAfter, int iImage,
						  LPARAM lParam) {
	HTREEITEM hItem;
	TV_ITEM tvItem;
	TV_INSERTSTRUCT tvInsert;
	
	// Fill out the item structure.
	tvItem.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvItem.pszText = szText;
	tvItem.cchTextMax = wcslen(szText);
	tvItem.iImage = iImage;
	tvItem.iSelectedImage = iImage;
	tvItem.lParam = lParam;
	
	// Fill out the insert structure.
	tvInsert.item = tvItem;
	tvInsert.hInsertAfter = hInsAfter;
	tvInsert.hParent = hParent;
	
	// Insert the item into the tree.
	hItem = (HTREEITEM)SendMessage(hwndTreeView, TVM_INSERTITEM, 0, 
		(LPARAM)(LPTV_INSERTSTRUCT)&tvInsert);
	
	return hItem;
}

/**
 * Gets a TreeView item.
 *
 * @param  tvItem TVITEM structure with the information to get.
 * @return        TRUE if the operation was successful.
 */
BOOL TreeViewGetItem(TVITEM *tvItem) {
	return TreeView_GetItem(hwndTreeView, tvItem);
}

/**
 * Expands a node in the TreeView.
 *
 * @param  hNode Handle to the node to expand.
 * @param        TRUE if the operation was successful.
 */
BOOL TreeViewExpandNode(HTREEITEM hNode) {
	return TreeView_Expand(hwndTreeView, hNode, TVE_EXPAND);
}

/**
 * Clears the entire TreeView contents.
 *
 * @return TRUE if the operation was successful.
 */
BOOL TreeViewClear() {
	return TreeView_DeleteAllItems(hwndTreeView);
}

