/**
 * WinUkiCE.c
 * A Uki wiki application for Windows CE.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include <windows.h>
#include <commctrl.h>
#include "WinUkiCE.h"
#include "Utilities.h"
#include "ImgListManager.h"
#include "TreeViewManager.h"

// Definitions.
#define LBL_MAX_LEN 100

// Global variables.
LPCTSTR szAppName = L"WinUki";
HINSTANCE hInst;
int uki_error;
const char *wipath = "\\testuki";
const char *wipage = "something";

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
	int rc;

	// Initialize the application.
	rc = InitializeApplication(hInstance);
	if (rc)
		return 0;

	// Initialize this single instance.
	hwndMain = InitializeInstance(hInstance, lpCmdLine, nShowCmd);
	if (hwndMain == 0)
		return 0x10;

	// Application message loop.
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Clean up.
	return TerminateInstance(hInstance, msg.wParam);
}

/**
 * Initializes the Uki environment.
 *
 * @param  szWikiPath Path to the uki wiki root.
 * @return            FALSE if everything went fine.
 */
int InitializeUki(const char *szWikiPath) {
	uki_variable_t var;
	uki_article_t article;
	uki_template_t template;
	size_t it = 0;
	size_t ia = 0;
	uint8_t iv = 0;
	char *content;

	// Initialize the uki wiki.
	if ((uki_error = uki_initialize(szWikiPath)) != UKI_OK) {
		PrintDebugConsole(uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print configurations.
	PrintDebugConsole("Configurations:\r\n");
	var = uki_config(iv);
	while (var.key != NULL) {
		PrintDebugConsole("   %s <- %s\r\n", var.key, var.value);
		iv++;
		var = uki_config(iv);
	}
	PrintDebugConsole("\r\n");

	// Print variables.
	PrintDebugConsole("Variables:\r\n");
	iv = 0;
	var = uki_variable(iv);
	while (var.key != NULL) {
		PrintDebugConsole("   %s <- %s\r\n", var.key, var.value);
		iv++;
		var = uki_variable(iv);
	}
	PrintDebugConsole("\r\n");

	// Print templates.
	it = 0;
	PrintDebugConsole("Templates:\r\n");
	template = uki_template(it);
	while (template.name != NULL) {
		PrintDebugConsole("   %d %s  |  %s <- %s\r\n", template.deepness,
			template.parent, template.path, template.name);
		it++;
		template = uki_template(it);
	}
	PrintDebugConsole("\r\n");

	// Print articles.
	PrintDebugConsole("Articles:\r\n");
	article = uki_article(ia);
	while (article.name != NULL) {
		PrintDebugConsole("   %d %s  |  %s <- %s\r\n", article.deepness,
			article.parent, article.path, article.name);
		ia++;
		article = uki_article(ia);
	}
	PrintDebugConsole("\r\n");

	// Render a page.
	if ((uki_error = uki_render_page(&content, wipage)) != UKI_OK) {
		PrintDebugConsole("ERROR: %s", uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print the page content.
	PrintDebugConsole("%s\r\n", content);
	return 0;
}

/**
 * Populates the Articles node in the TreeView.
 *
 * @param  htiParent     Parent TreeView node handle.
 * @param  iStartArticle Starting article index.
 * @param  nDeepness     Current deepness level.
 * @param  bRootNode     Is this the root Articles node?
 * @return               Last article index processed.
 */
size_t PopulateArticles(HTREEITEM htiParent, size_t iStartArticle,
						int nDeepness, BOOL bRootNode) {
	HTREEITEM htiLastItem;
	WCHAR szCaption[LBL_MAX_LEN];
	uki_article_t ukiArticle;
	size_t iArticle = iStartArticle;

	// Initialize state variables.
	ukiArticle = uki_article(iArticle);
	htiLastItem = (HTREEITEM)NULL;

	// Go through articles.
	while (ukiArticle.name != NULL) {
		if (ConvertStringAtoW(szCaption, ukiArticle.name)) {
			htiLastItem = TreeViewAddItem(htiParent, szCaption,
				htiLastItem, ImageListIconIndex(IDB_ARTICLE),
				(LPARAM)ukiArticle.path);
		
			// Go to the next one.
			iArticle++;
			ukiArticle = uki_article(iArticle);
		} else {
			MessageBox(NULL, L"Failed to convert ASCII string to Unicode.",
				L"Article Population Failed", MB_OK);
		}
	}

	return iArticle - 1;
}

/**
 * Populates the Templates node in the TreeView.
 *
 * @param  htiParent      Parent TreeView node handle.
 * @param  iStartTemplate Starting template index.
 * @param  nDeepness      Current deepness level.
 * @param  bRootNode      Is this the root Template node?
 * @return                Last template index processed.
 */
size_t PopulateTemplates(HTREEITEM htiParent, size_t iStartTemplate,
						 int nDeepness, BOOL bRootNode) {
	HTREEITEM htiLastItem;
	WCHAR szCaption[LBL_MAX_LEN];
	uki_template_t ukiTemplate;
	size_t iTemplate = iStartTemplate;

	// Initialize state variables.
	ukiTemplate = uki_template(iTemplate);
	htiLastItem = (HTREEITEM)NULL;

	// Go through templates.
	while (ukiTemplate.name != NULL) {
		if (ConvertStringAtoW(szCaption, ukiTemplate.name)) {
			htiLastItem = TreeViewAddItem(htiParent, szCaption,
				htiLastItem, ImageListIconIndex(IDB_TEMPLATE),
				(LPARAM)ukiTemplate.path);
		
			// Go to the next one.
			iTemplate++;
			ukiTemplate = uki_template(iTemplate);
		} else {
			MessageBox(NULL, L"Failed to convert ASCII string to Unicode.",
				L"Template Population Failed", MB_OK);
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

	// Add article library root item.
	LoadString(hInst, IDS_ARTICLE_LIBRARY, szCaption, LBL_MAX_LEN);
	htiArticles = TreeViewAddItem((HTREEITEM)NULL, szCaption,
		(HTREEITEM)TVI_ROOT, ImageListIconIndex(IDB_LIBRARY), (LPARAM)0);

	// Populate the articles.
	PopulateArticles(htiArticles, 0, 0, TRUE);

	// Add template library root item.
	LoadString(hInst, IDS_TEMPLATE_LIBRARY, szCaption, LBL_MAX_LEN);
	htiTemplates = TreeViewAddItem((HTREEITEM)NULL, szCaption,
		(HTREEITEM)TVI_ROOT, ImageListIconIndex(IDB_TEMPLATELIBRARY), (LPARAM)0);

	// Populate the templates.
	PopulateTemplates(htiTemplates, 0, 0, TRUE);


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
	hInst = hInstance;

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
	WCHAR szCaption[LBL_MAX_LEN];
	char *szPath;
	NMTREEVIEW* pnmTreeView = (LPNMTREEVIEW)lParam;

	// Get item information.
	tvItem.hItem = pnmTreeView->itemNew.hItem;
	tvItem.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE;
	tvItem.pszText = szCaption;
	tvItem.cchTextMax = LBL_MAX_LEN;
	TreeViewGetItem(&tvItem);

	// Check if it was a valid parameter.
	if (tvItem.lParam != 0) {
		// Get path from parameter.
		szPath = (char*)tvItem.lParam;

		// Check if an article or template was selected.
		if (tvItem.iImage == ImageListIconIndex(IDB_ARTICLE)) {
			MessageBox(hWnd, szCaption, L"Article Selected", MB_OK);
		} else if (tvItem.iImage == ImageListIconIndex(IDB_TEMPLATE)) {
			MessageBox(hWnd, szCaption, L"Template Selected", MB_OK);
		}
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

	// Ensure that the common control DLL is loaded. 
    InitCommonControls();

	// Initialize the Image List.
	hIml = InitializeImageList(hInst);

	// Create CommandBar and add exit button.
	hwndCB = CommandBar_Create(hInst, hWnd, IDC_CMDBAR);
	CommandBar_AddAdornments(hwndCB, 0, 0);

	// Initialize Uki.
	if (InitializeUki(wipath)) {
		MessageBox(NULL, L"Failed to initialize Uki", L"Initialization Failure",
			MB_OK);
		return 1;
	}

	// Calculate the TreeView control size and position.
	GetClientRect(hWnd, &rcTreeView);
	rcTreeView.top += CommandBar_Height(hwndCB);

	// Create the TreeView control.
	hwndTV = InitializeTreeView(hInst, hWnd, rcTreeView,
		(HMENU)IDC_TREEVIEW, hIml);
	PopulateTreeView();

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
	// Clean up.
	uki_clean();

	// Post quit message and return.
	PostQuitMessage(0);
	return 0;
}