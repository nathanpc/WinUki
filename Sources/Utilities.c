/**
 * Utilities.c
 * A collection of Windows utility functions.
 *
 * @author: Nathan Campos <nathan@innoveworkshop.com>
 */

#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Converts a regular ASCII string into a Unicode string.
 *
 * @param  szUnicode Pre-allocated Unicode string.
 * @param  szASCII   Original ASCII string.
 * @return           TRUE if the conversion was successful.
 */
BOOL ConvertStringAtoW(LPTSTR szUnicode, const char *szASCII) {
	size_t nLen = strlen(szASCII) + 1;

    if(MultiByteToWideChar(CP_ACP, 0, szASCII, -1, szUnicode, nLen) == 0) {
		return FALSE;
    }
    
    return TRUE;
}

/**
 * Converts a Unicode string into a regular ASCII string.
 *
 * @param  szASCII   Pre-allocated ASCII string.
 * @param  szUnicode Original Unicode string.
 * @return           TRUE if the conversion was successful.
 */
BOOL ConvertStringWtoA(char *szASCII, LPCTSTR szUnicode) {
	size_t nLen = wcslen(szUnicode) + 1;

    if(WideCharToMultiByte(CP_ACP, 0, szUnicode, -1, szASCII, nLen,
		NULL, NULL) == 0) {
		DWORD err = GetLastError();
		return FALSE;
    }

    return TRUE;
}

/**
 * Prints to the debug console. Just like printf.
 *
 * @param format String format as if it was printf.
 * @param ...    All your variables to be inserted into the format string.
 */
void PrintDebugConsole(const char* format, ...) {
	va_list argptr;
	WCHAR szMsg[4084];
	char buffer[4084];

	// Build the string with sprintf.
	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	// Convert string to Unicode.
	if (!ConvertStringAtoW(&szMsg, buffer)) {
		OutputDebugString(L"Error while converting string to unicode.\r\n");
		return;
	}

	// Print to debug console.
	OutputDebugString(szMsg);
}
