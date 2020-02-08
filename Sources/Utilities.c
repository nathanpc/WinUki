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
 * @param  szUnicode Unicode string allocated by this function.
 * @param  szASCII   Original ASCII string.
 * @return           TRUE if the conversion was successful.
 */
BOOL ConvertStringAtoW(LPTSTR *szUnicode, const char *szASCII) {
	size_t nLen = strlen(szASCII);
    *szUnicode = (LPTSTR)malloc((nLen + 1) * sizeof(wchar_t));
    
    if(MultiByteToWideChar(CP_ACP, 0, szASCII, -1, *szUnicode, nLen) != 0) {
		free(*szUnicode);
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
	LPTSTR szMsg;
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
	free(szMsg);
}
