/**
 * WinUki.h
 * A Uki wiki application for Windows CE.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _WINUKICE_H
#define _WINUKICE_H

#define IDC_CMDBAR 1

// Instance operators.
int InitializeApplication(HINSTANCE hInstance);
HWND InitializeInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
int TerminateInstance(HINSTANCE hInstance, int nDefRC);

// Window procedure.
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT wMsg, WPARAM wParam,
								LPARAM lParam);

// Message handlers.
LRESULT DoCreateMain(HWND hWnd, UINT wMsg, WPARAM wParam,
					 LPARAM lParam);
LRESULT DoPaintMain(HWND hWnd, UINT wMsg, WPARAM wParam,
					LPARAM lParam);
LRESULT DoHibernateMain(HWND hWnd, UINT wMsg, WPARAM wParam,
						LPARAM lParam);
LRESULT DoActivateMain(HWND hWnd, UINT wMsg, WPARAM wParam,
					   LPARAM lParam);
LRESULT DoDestroyMain(HWND hWnd, UINT wMsg, WPARAM wParam,
					  LPARAM lParam);

#endif  // _WINUKICE_H