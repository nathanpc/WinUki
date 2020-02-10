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
 */
int InitializeUki() {
	uki_variable_t var;
	uint8_t iv = 0;
	char *content;

	// Initialize the uki wiki.
	if ((uki_error = uki_initialize(wipath)) != UKI_OK) {
		PrintDebugConsole(uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print configurations.
	printf("Configurations:\n");
	var = uki_config(iv);
	while (var.key != NULL) {
		PrintDebugConsole("   %s <- %s\n", var.key, var.value);
		iv++;
		var = uki_config(iv);
	}
	printf("\n");

	// Print variables.
	printf("Variables:\n");
	iv = 0;
	var = uki_variable(iv);
	while (var.key != NULL) {
		PrintDebugConsole("   %s <- %s\n", var.key, var.value);
		iv++;
		var = uki_variable(iv);
	}
	printf("\n");

	// Render a page.
	if ((uki_error = uki_render_page(&content, wipage)) != UKI_OK) {
		PrintDebugConsole("ERROR: %s", uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print the page content.
	PrintDebugConsole("%s\n", content);
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
	case WM_CLOSE:
		return WndMainClose(hWnd, wMsg, wParam, lParam);
	case WM_DESTROY:
		return WndMainDestroy(hWnd, wMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, wMsg, wParam, lParam);
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

	// Ensure that the common control DLL is loaded. 
    InitCommonControls();

	// Initialize the Image List.
	InitializeImageList(hInst);

	// Create CommandBar and add exit button.
	hwndCB = CommandBar_Create(hInst, hWnd, IDC_CMDBAR);
	CommandBar_AddAdornments(hwndCB, 0, 0);

	// Initialize Uki.
	return InitializeUki();
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