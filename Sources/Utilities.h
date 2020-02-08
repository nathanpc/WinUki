/**
 * Utilities.h
 * A collection of Windows utility functions.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#ifndef _UTILITIES_H
#define _UTILITIES_H

#include "windowshelper.h"

BOOL ConvertStringAtoW(LPTSTR *szUnicode, const char *szASCII);
void PrintDebugConsole(const char* format, ...);

#endif  // _UTILITIES_H