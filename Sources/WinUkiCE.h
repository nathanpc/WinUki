/**
 * WinUki.h
 * A Uki wiki application for Windows CE.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _WINUKICE_H
#define _WINUKICE_H

#include "uki.h"

// Control IDs.
#define IDC_CMDBAR 1


// Uki stuff.
int InitializeUki();

// Instance operators.
int InitializeApplication(HINSTANCE hInstance);
HWND InitializeInstance(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow);
int TerminateInstance(HINSTANCE hInstance, int nDefRC);

// Window procedure.
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT wMsg, WPARAM wParam,
								LPARAM lParam);

// Message handlers.
LRESULT WndMainCreate(HWND hWnd, UINT wMsg, WPARAM wParam,
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