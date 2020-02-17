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

// Instance operators.
int InitializeApplication(HINSTANCE hInstance);
HWND InitializeInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
int TerminateInstance(HINSTANCE hInstance, int nDefRC);

// Control managers.
size_t PopulateArticles(HTREEITEM htiParent, size_t iStartArticle,
						int nDeepness, BOOL bRootNode);
size_t PopulateTemplates(HTREEITEM htiParent, size_t iStartTemplate,
						 int nDeepness, BOOL bRootNode);
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