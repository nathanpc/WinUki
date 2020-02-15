/**
 * Utilities.h
 * A collection of Windows utility functions.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UTILITIES_H
#define _UTILITIES_H

#include "windowshelper.h"

// String conversion.
BOOL ConvertStringAtoW(LPTSTR szUnicode, const char *szASCII);
BOOL ConvertStringWtoA(char *szASCII, LPCTSTR szUnicode);

// File utilities.
BOOL ReadFileContents(LPCTSTR szPath, LPTSTR *szFileContents);

// Debugging.
void PrintDebugConsole(const char* format, ...);

#endif  // _UTILITIES_H