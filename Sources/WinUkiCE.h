/**
 * WinUki.h
 * A Uki wiki application for Windows CE.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _WINUKICE_H
#define _WINUKICE_H

#include "resource.h"
#include "UkiHelper.h"

// Control IDs.
#define IDC_CMDBAR   201
#define IDC_TREEVIEW 202
#define IDC_EDITPAGE 203
#define IDC_VIEWPAGE 204

// CommandBar buttons.
#define IDC_BTNEW     211
#define IDC_BTOPEN    212
#define IDC_BTSAVE    213
#define IDC_BTCUT     214
#define IDC_BTCOPY    215
#define IDC_BTPASTE   216
#define IDC_BTUNDO    217
#define IDC_BTFIND    218
#define IDC_BTREPLACE 219

// Number of bitmaps in the standard and view image lists.
#define STD_BMPS_LEN  STD_PRINT + 1
#define VIEW_BMPS_LEN VIEW_NEWFOLDER + 1

// Instance operators.
int InitializeApplication(HINSTANCE hInstance);
HWND InitializeInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
int TerminateInstance(HINSTANCE hInstance, int nDefRC);

// Uki workspace.
BOOL CheckForUnsavedChanges();
LRESULT CloseWorkspace(BOOL fDestroy);
LRESULT LoadWorkspace(BOOL fReload);

// Control managers.
LONG PopulateArticles(HTREEITEM htiParent);
LONG PopulateTemplates(HTREEITEM htiParent);
LRESULT PopulateTreeView();

// Window procedure.
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT wMsg, WPARAM wParam,
								LPARAM lParam);

// TreeView message handlers.
LRESULT TreeViewSelectionChanged(HWND hWnd, UINT wMsg, WPARAM wParam,
								 LPARAM lParam);

// Window message handlers.
LRESULT WndMainCreate(HWND hWnd, UINT wMsg, WPARAM wParam,
					  LPARAM lParam);
LRESULT WndMainCommand(HWND hWnd, UINT wMsg, WPARAM wParam,
					   LPARAM lParam);
LRESULT WndMainInitMenuPopUp(HWND hWnd, UINT wMsg, WPARAM wParam,
							 LPARAM lParam);
LRESULT WndMainNotify(HWND hWnd, UINT wMsg, WPARAM wParam,
					  LPARAM lParam);
LRESULT WndMainHibernate(HWND hWnd, UINT wMsg, WPARAM wParam,
						 LPARAM lParam);
LRESULT WndMainActivate(HWND hWnd, UINT wMsg, WPARAM wParam,
						LPARAM lParam);
LRESULT WndMainClose(HWND hWnd, UINT wMsg, WPARAM wParam,
					 LPARAM lParam);
LRESULT WndMainDestroy(HWND hWnd, UINT wMsg, WPARAM wParam,
					   LPARAM lParam);

#endif  // _WINUKICE_H