/**
 * WinUkiCE.c
 * A Uki wiki application for Windows CE.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "WinUkiCE.h"
#include "Utilities.h"
#include "ImgListManager.h"
#include "TreeViewManager.h"
#include "PageManager.h"
#include "FindReplace.h"
#include "CommonDlgManager.h"
#include "AboutDialog.h"

// Definitions.
#define LBL_MAX_LEN 100

// Global variables.
HINSTANCE hInst;
int uki_error;
BOOL fWorkspaceOpen;

/**
 * Application's main entry point.
 *
 * @param  hInstance     Program instance.
 * @param  hPrevInstance Ignored: Leftover from Win16.
 * @param  lpCmdLine     String with command line text.
 * @param  nShowCmd      Initial state of the program's main window.
 * @return               wParam of the WM_QUIT message.
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPWSTR lpCmdLine, int nShowCmd) {
	MSG msg;
	HWND hwndMain;
	HACCEL hAccel;
	int rc;

	// Set flags.
	fWorkspaceOpen = FALSE;

	// Initialize the application.
	rc = InitializeApplication(hInstance);
	if (rc)
		return 0;

	// Initialize this single instance.
	hwndMain = InitializeInstance(hInstance, lpCmdLine, nShowCmd);
	if (hwndMain == 0)
		return 0x10;

	// Load accelerators.
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL));

	// Application message loop.
	while (GetMessage(&msg, NULL, 0, 0)) {
		// Translate accelerators.
		if (!TranslateAccelerator(hwndMain, hAccel, &msg)) {
			// Translate message.
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Clean up.
	return TerminateInstance(hInstance, msg.wParam);
}

/**
 * Closes a Uki workspace.
 *
 * @return 0 if a workspace was closed.
 */
LRESULT CloseWorkspace() {
	// Set all controls to its defaults.
	TreeViewClear();
	ClearPageToDefaults();

	// Close Uki.
	CloseUki();

	fWorkspaceOpen = FALSE;
	return 0;
}

/**
 * Loads a Uki workspace.
 *
 * @param  fReload Is this a reload operation?
 * @return         0 if a workspace was loaded.
 */
LRESULT LoadWorkspace(BOOL fReload) {
	if (fReload) {
		// Reload workspace.
		CloseWorkspace();
		if (!ReloadUki()) {
			return 1;
		}
	} else {
		// Open a new workspace.
		TCHAR szWikiPath[MAX_PATH];
		
		// Get the workspace folder.
		if (!OpenWorkspace(szWikiPath))
			return 1;
		
		// Close the current workspace.
		CloseWorkspace();
		
		// Initialize Uki.
		if (!InitializeUki(szWikiPath)) {
			fWorkspaceOpen = FALSE;
			return 1;
		}
	}

	// Populate the TreeView with stuff.
	PopulateTreeView();

	fWorkspaceOpen = TRUE;
	return 0;
}

/**
 * Checks for unsaved changes and displays a message box if there are any.
 *
 * @return TRUE if we should abort the current operation.
 */
BOOL CheckForUnsavedChanges() {
	int fSelection;

	// Check for page dirtiness.
	if (IsPageDirty()) {
		// Show the message box and get the user selection.
		fSelection = MessageBox(NULL, L"You have unsaved changes. Do you want "
			L"to save your changes?", L"Unsaved Changes",
			MB_YESNOCANCEL | MB_ICONWARNING);

		// Check which button the user clicked.
		switch (fSelection) {
		case IDYES:
			SaveCurrentPage();
			return FALSE;
		case IDNO:
			return FALSE;
		case IDCANCEL:
			return TRUE;
		}
	}

	return FALSE;
}

/**
 * Populates the Articles node in the TreeView.
 *
 * @param  htiParent     Parent TreeView node handle.
 * @return               Last article index processed.
 */
LONG PopulateArticles(HTREEITEM htiParent) {
	HTREEITEM htiLastItem;
	HTREEITEM htiFolder;
	WCHAR szCaption[LBL_MAX_LEN];
	char szaLastParent[MAX_PATH];
	UKIARTICLE ukiArticle;
	LONG iArticle;

	// Go through articles.
	htiLastItem = (HTREEITEM)NULL;
	htiFolder = htiParent;
	for (iArticle = 0; iArticle < GetUkiArticlesAvailable(); iArticle++) {
		// Get article.
		GetUkiArticle(&ukiArticle, iArticle);
		
		// Check if we have a parent.
		if (ukiArticle.parent != NULL) {
			// Check if we should add a new folder.
			if (strcmp(szaLastParent, ukiArticle.parent) != 0) {
				// Set the new last parent.
				strcpy(szaLastParent, ukiArticle.parent);

				// Add folder.
				if (ConvertStringAtoW(szCaption, szaLastParent)) {
					htiFolder = TreeViewAddItem(htiParent, szCaption,
						htiLastItem, ImageListIconIndex(IDB_FOLDER), (LPARAM)0);
				} else {
					MessageBox(NULL, L"Failed to convert article folder name "
						L"from ASCII to Unicode.", L"Article Population Failed",
						MB_OK);
				}
			}
		} else {
			// Go back to the root.
			htiFolder = htiParent;
		}

		// Convert name to Unicode.
		if (ConvertStringAtoW(szCaption, ukiArticle.name)) {
			// Append to the TreeView.
			htiLastItem = TreeViewAddItem(htiFolder, szCaption,
				htiLastItem, ImageListIconIndex(IDB_ARTICLE),
				(LPARAM)iArticle);
		} else {
			MessageBox(NULL, L"Failed to convert article name from ASCII to "
				L"Unicode.", L"Article Population Failed", MB_OK);
		}
	}

	return iArticle - 1;
}

/**
 * Populates the Templates node in the TreeView.
 *
 * @param  htiParent      Parent TreeView node handle.
 * @return                Last template index processed.
 */
LONG PopulateTemplates(HTREEITEM htiParent) {
	HTREEITEM htiLastItem;
	WCHAR szCaption[LBL_MAX_LEN];
	UKITEMPLATE ukiTemplate;
	LONG iTemplate;
	
	// Go through templates.
	htiLastItem = (HTREEITEM)NULL;
	for (iTemplate = 0; iTemplate < GetUkiTemplatesAvailable(); iTemplate++) {
		// Get template.
		GetUkiTemplate(&ukiTemplate, iTemplate);
		
		// Convert name to Unicode.
		if (ConvertStringAtoW(szCaption, ukiTemplate.name)) {
			// Append to the TreeView.
			htiLastItem = TreeViewAddItem(htiParent, szCaption,
				htiLastItem, ImageListIconIndex(IDB_TEMPLATE),
				(LPARAM)iTemplate);
		} else {
			MessageBox(NULL, L"Failed to convert template name from ASCII to "
				L"Unicode.", L"Template Population Failed", MB_OK);
		}
	}

	return iTemplate - 1;
}

/**
 * Populates the TreeView component with stuff.
 *
 * @return 0 if everything went OK.
 */
LRESULT PopulateTreeView() {
	HTREEITEM htiArticles;
	HTREEITEM htiTemplates;
	WCHAR szCaption[LBL_MAX_LEN];

	// Clear the TreeView as a precaution.
	TreeViewClear();

	// Add article library root item.
	LoadString(hInst, IDS_ARTICLE_LIBRARY, szCaption, LBL_MAX_LEN);
	htiArticles = TreeViewAddItem((HTREEITEM)NULL, szCaption,
		(HTREEITEM)TVI_ROOT, ImageListIconIndex(IDB_LIBRARY), (LPARAM)0);

	// Populate the articles.
	PopulateArticles(htiArticles);

	// Add template library root item.
	LoadString(hInst, IDS_TEMPLATE_LIBRARY, szCaption, LBL_MAX_LEN);
	htiTemplates = TreeViewAddItem((HTREEITEM)NULL, szCaption,
		(HTREEITEM)TVI_ROOT, ImageListIconIndex(IDB_TEMPLATELIBRARY), (LPARAM)0);

	// Populate the templates.
	PopulateTemplates(htiTemplates);

	// Expand the view.
	TreeViewExpandNode(htiArticles);
	TreeViewExpandNode(htiTemplates);

	return 0;
}

/**
 * Initializes the application and registers the application class.
 *
 * @param  hInstance Application instance.
 * @return           TRUE if the class was registered.
 */
int InitializeApplication(HINSTANCE hInstance) {
	WNDCLASS wc;
	TCHAR szAppName[LBL_MAX_LEN + 1];

	// Load the application name.
	LoadString(hInst, IDS_APPNAME, szAppName, LBL_MAX_LEN);

	// Register the application's main window class.
	wc.style = 0;					  // Window style.
	wc.lpfnWndProc = MainWindowProc;  // Main window procedure.
	wc.cbClsExtra = 0;				  // Extra class data.
	wc.cbWndExtra = 0;				  // Extra window data.
	wc.hInstance = hInstance;		  // Owner handle.
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
	wc.hCursor = NULL;				  // Default cursor. (Always NULL)
	wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_STATIC);
	wc.lpszMenuName = NULL;           // Menu name. (Always NULL)
	wc.lpszClassName = szAppName;	  // Window class name.

	// Check if the class registration worked.
	if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error",
			MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

	return 0;
}

/**
 * Initializes the instance and creates the window.
 *
 * @param  hInstance     Program instance.
 * @param  lpCmdLine     String with command line text.
 * @param  nShowCmd      Initial state of the program's main window.
 * @return               Window handler.
 */
HWND InitializeInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;
	TCHAR szAppName[LBL_MAX_LEN + 1];
	hInst = hInstance;

	// Load the application name.
	LoadString(hInst, IDS_APPNAME, szAppName, LBL_MAX_LEN);

	// Create the main window.
	hWnd = CreateWindow(szAppName,      // Window class.
						L"WinUki",      // Window title.
						WS_VISIBLE,		// Style flags.
						CW_USEDEFAULT,  // X position.
						CW_USEDEFAULT,  // Y position.
						CW_USEDEFAULT,  // Initial width,
						CW_USEDEFAULT,  // Initial height.
						NULL,			// Parent window.
						NULL,			// Menu class. (Always NULL)
						hInstance,		// Application instance.
						NULL);			// Pointer to create parameters.


	// Check if the window creation worked.
	if (!IsWindow(hWnd)) {
		MessageBox(NULL, L"Window Creation Failed!", L"Error",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
	}

	// Show and update the window.
	ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hWnd);

	return hWnd;
}

/**
 * Terminates the application instance.
 *
 * @param  hInstance Application instance.
 * @param  nDefRC    Return code.
 * @return           Previous return code.
 */
int TerminateInstance(HINSTANCE hInstance, int nDefRC) {
	return nDefRC;
}

/**
 * Main window procedure.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT wMsg, WPARAM wParam,
								LPARAM lParam) {
	switch (wMsg) {
	case WM_CREATE:
		return WndMainCreate(hWnd, wMsg, wParam, lParam);
	case WM_COMMAND:
		return WndMainCommand(hWnd, wMsg, wParam, lParam);
	case WM_INITMENUPOPUP:
		return WndMainInitMenuPopUp(hWnd, wMsg, wParam, lParam);
//	case WM_HIBERNATE:
//		return WndMainHibernate(hWnd, wMsg, wParam, lParam);
//	case WM_ACTIVATE:
//		return WndMainActivate(hWnd, wMsg, wParam, lParam);
	case WM_NOTIFY:
		return WndMainNotify(hWnd, wMsg, wParam, lParam);
	case WM_CLOSE:
		return WndMainClose(hWnd, wMsg, wParam, lParam);
	case WM_DESTROY:
		return WndMainDestroy(hWnd, wMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, wMsg, wParam, lParam);
}

/**
 * Process the TVN_SELCHANGED message for the TreeView.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT TreeViewSelectionChanged(HWND hWnd, UINT wMsg, WPARAM wParam,
								 LPARAM lParam) {
	TVITEM tvItem;
	NMTREEVIEW* pnmTreeView = (LPNMTREEVIEW)lParam;
	size_t nIndex;

	// Get item information.
	tvItem.hItem = pnmTreeView->itemNew.hItem;
	tvItem.mask = TVIF_PARAM | TVIF_IMAGE;
	TreeViewGetItem(&tvItem);
	
	// Get article/template index from parameter.
	nIndex = (size_t)tvItem.lParam;
	
	// Check if an article or template was selected.
	if (tvItem.iImage == ImageListIconIndex(IDB_ARTICLE)) {
		if (CheckForUnsavedChanges())
			return 1;

		PopulatePageViewArticle(nIndex);
	} else if (tvItem.iImage == ImageListIconIndex(IDB_TEMPLATE)) {
		if (CheckForUnsavedChanges())
			return 1;

		PopulatePageViewTemplate(nIndex);
	}

	return 0;
}

/**
 * Process the WM_CREATE message for the window.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT WndMainCreate(HWND hWnd, UINT wMsg, WPARAM wParam,
					  LPARAM lParam) {
	HWND hwndCB;
	HWND hwndTV;
	HIMAGELIST hIml;
	RECT rcTreeView;
	RECT rcPageView;

	// Ensure that the common control DLL is loaded. 
    InitCommonControls();

	// Initialize the Image List.
	hIml = InitializeImageList(hInst);

	// Create CommandBar and add exit button.
	hwndCB = CommandBar_Create(hInst, hWnd, IDC_CMDBAR);
	CommandBar_InsertMenubar(hwndCB, hInst, IDR_MAINMENU, 0);
	CommandBar_AddAdornments(hwndCB, 0, 0);

	// Calculate the TreeView control size and position.
	GetClientRect(hWnd, &rcTreeView);
	rcTreeView.top += CommandBar_Height(hwndCB);
	rcTreeView.bottom -= rcTreeView.top;
	rcTreeView.right = (LONG)(rcTreeView.right / 3.5);

	// Create the TreeView control.
	hwndTV = InitializeTreeView(hInst, hWnd, rcTreeView,
		(HMENU)IDC_TREEVIEW, hIml);

	// Calculate the page view controls size and position.
	GetClientRect(hWnd, &rcPageView);
	rcPageView.top = rcTreeView.top;
	rcPageView.bottom = rcTreeView.bottom;
	rcPageView.left = rcTreeView.right + 5;
	rcPageView.right -= rcPageView.left;

	// Create the page controls.
	InitializePageView(hInst, hWnd, rcPageView, (HMENU)IDC_EDITPAGE,
		(HMENU)IDC_VIEWPAGE);

	// Initialize the find and replace engine.
	InitializeFindReplace(hInst, hWnd, GetPageEditHandle());

	return 0;
}

/**
 * Process the WM_INITMENUPUP message for the window.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT WndMainInitMenuPopUp(HWND hWnd, UINT wMsg, WPARAM wParam,
							 LPARAM lParam) {
	HMENU hMenu = CommandBar_GetMenu(GetDlgItem(hWnd, IDC_CMDBAR), 0);

	// Check if we can undo and enable/disable the menu item accordingly.
	if (SendPageEditMessage(EM_CANUNDO, 0, 0)) {
		EnableMenuItem(hMenu, IDM_EDIT_UNDO, MF_BYCOMMAND | MF_ENABLED);
	} else {
		EnableMenuItem(hMenu, IDM_EDIT_UNDO, MF_BYCOMMAND | MF_GRAYED);
	}

	// Check if editing or viewing a page and change the menu radio group.
	if (IsPageEditorActive()) {
		CheckMenuRadioItem(hMenu, IDM_VIEW_PAGEVIEW, IDM_VIEW_PAGEEDIT,
			IDM_VIEW_PAGEEDIT, MF_BYCOMMAND);
	} else {
		CheckMenuRadioItem(hMenu, IDM_VIEW_PAGEVIEW, IDM_VIEW_PAGEEDIT,
			IDM_VIEW_PAGEVIEW, MF_BYCOMMAND);
	}

	// Enable/disable the Find Next button if there's something in the edit box.
	if (PageEditCanFindNext()) {
		EnableMenuItem(hMenu, IDM_EDIT_FINDNEXT, MF_BYCOMMAND | MF_ENABLED);
	} else {
		EnableMenuItem(hMenu, IDM_EDIT_FINDNEXT, MF_BYCOMMAND | MF_GRAYED);
	}

	// Enable and disable workspace related items.
	if (fWorkspaceOpen) {
		EnableMenuItem(hMenu, IDM_FILE_NEWARTICLE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hMenu, IDM_FILE_NEWTEMPLATE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hMenu, IDM_FILE_REFRESHWS, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hMenu, IDM_FILE_CLOSEWS, MF_BYCOMMAND | MF_ENABLED);
	} else {
		EnableMenuItem(hMenu, IDM_FILE_NEWARTICLE, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_FILE_NEWTEMPLATE, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_FILE_REFRESHWS, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_FILE_CLOSEWS, MF_BYCOMMAND | MF_GRAYED);
	}

	// Enable/disable article related items.
	if (IsArticleLoaded() || IsTemplateLoaded()) {
		EnableMenuItem(hMenu, IDM_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hMenu, IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_ENABLED);
	} else {
		EnableMenuItem(hMenu, IDM_FILE_SAVE, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_FILE_SAVEAS, MF_BYCOMMAND | MF_GRAYED);
	}

	return 0;
}

/**
 * Process the WM_COMMAND message for the window.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT WndMainCommand(HWND hWnd, UINT wMsg, WPARAM wParam,
					   LPARAM lParam) {
	switch (GET_WM_COMMAND_ID(wParam, lParam)) {
	case IDC_EDITPAGE:
		// Page Editor.
		return PageEditHandleCommand(hWnd, wMsg, wParam, lParam);
	case IDM_FILE_NEWARTICLE:
		// New Article.
		if (CheckForUnsavedChanges())
			return 1;

		if (CreateNewPage(TRUE))
			return 1;
		return PopulateTreeView();
	case IDM_FILE_NEWTEMPLATE:
		// New Template.
		if (CheckForUnsavedChanges())
			return 1;

		if (CreateNewPage(FALSE))
			return 1;
		return PopulateTreeView();
	case IDM_FILE_OPENWS:
		// Open Workspace.
		if (CheckForUnsavedChanges())
			return 1;

		return LoadWorkspace(FALSE);
	case IDM_FILE_REFRESHWS:
		// Refresh Workspace.
		if (CheckForUnsavedChanges())
			return 1;

		return LoadWorkspace(TRUE);
	case IDM_FILE_CLOSEWS:
		// Close Workspace.
		if (CheckForUnsavedChanges())
			return 1;

		return CloseWorkspace();
	case IDM_FILE_SAVE:
		// Save.
		return SaveCurrentPage();
	case IDM_FILE_SAVEAS:
		// Save As.
		if (SavePageAs())
			return 1;
		return PopulateTreeView();
	case IDM_FILE_CLOSE:
		// Close.
		if (CheckForUnsavedChanges())
			return 1;

		return SendMessage(hWnd, WM_CLOSE, 0, 0);
	case IDM_EDIT_UNDO:
		// Undo.
		if (SendPageEditMessage(EM_CANUNDO, 0, 0)) 
			return SendPageEditMessage(WM_UNDO, 0, 0);
		break;
	case IDM_EDIT_CUT:
		// Cut.
		return SendPageEditMessage(WM_CUT, 0, 0);
	case IDM_EDIT_COPY:
		// Copy.
		return SendPageEditMessage(WM_COPY, 0, 0);
	case IDM_EDIT_PASTE:
		// Paste.
		return SendPageEditMessage(WM_PASTE, 0, 0);
	case IDM_EDIT_CLEAR:
		// Clear.
		return SendPageEditMessage(WM_CLEAR, 0, 0);
	case IDM_EDIT_SELECTALL:
		// Select All.
		return SendPageEditMessage(EM_SETSEL, 0, -1);
	case IDM_EDIT_FIND:
		// Show Find dialog.
		ShowFindDialog();
		break;
	case IDM_EDIT_FINDNEXT:
		// Find Next.
		PageEditFindNext(TRUE);
		break;
	case IDM_EDIT_REPLACE:
		// Show Replace dialog.
		ShowReplaceDialog();
		break;
	case IDM_VIEW_PAGEVIEW:
		// Show Page Viwer.
		ShowPageViewer();
		break;
	case IDM_VIEW_PAGEEDIT:
		// Show Page Editor.
		ShowPageEditor();
		break;
	case IDM_VIEW_TOGGLEPAGE:
		// Toggle Page View.
		TogglePageView();
		break;
	case IDM_HELP_ABOUT:
		// About.
		ShowAboutDialog(hInst, hWnd);
		break;
	default:
		return DefWindowProc(hWnd, wMsg, wParam, lParam);
	}

	return 0;
}

/**
 * Process the WM_NOTIFY message for the window.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT WndMainNotify(HWND hWnd, UINT wMsg, WPARAM wParam,
					  LPARAM lParam) {
	switch (((LPNMHDR)lParam)->code) {
	case TVN_SELCHANGED:
		return TreeViewSelectionChanged(hWnd, wMsg, wParam, lParam);
	}

	return 0;
}

/**
 * Process the WM_HIBERNATE message for the window.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT WndMainHibernate(HWND hWnd, UINT wMsg, WPARAM wParam,
						 LPARAM lParam) {
	// If we are not the active window, then free the CommandBar to
	// save memory.
	if (GetActiveWindow() != hWnd) {
		CommandBar_Destroy(GetDlgItem(hWnd, IDC_CMDBAR));
	}

	return 0;
}


/**
 * Process the WM_ACTIVATE message for the window.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT WndMainActivate(HWND hWnd, UINT wMsg, WPARAM wParam,
						LPARAM lParam) {
	// If we are activating and there's no CommandBar, then create it.
	if ((LOWORD(wParam) != WA_INACTIVE) &&
		(GetDlgItem(hWnd, IDC_CMDBAR) == 0)) {
		// Create CommandBar and add exit button.
		HWND hwndCB = CommandBar_Create(hInst, hWnd, IDC_CMDBAR);
		CommandBar_AddAdornments(hwndCB, 0, 0);
	}

	return 0;
}

/**
 * Process the WM_CLOSE message for the window.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT WndMainClose(HWND hWnd, UINT wMsg, WPARAM wParam,
					 LPARAM lParam) {
	// Clean up.
	CloseWorkspace();

	// Send window destruction message.
	DestroyWindow(hWnd);

	return 0;
}

/**
 * Process the WM_DESTROY message for the window.
 *
 * @param  hWnd   Window handler.
 * @param  wMsg   Message type.
 * @param  wParam Message parameter.
 * @param  lParam Message parameter.
 * @return        0 if everything worked.
 */
LRESULT WndMainDestroy(HWND hWnd, UINT wMsg, WPARAM wParam,
					   LPARAM lParam) {
	// Post quit message and return.
	PostQuitMessage(0);
	return 0;
}