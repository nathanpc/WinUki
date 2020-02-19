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
 * Slurps a file and stores its contents inside a buffer.
 * @remark Remember to free the contents buffer with LocalFree.
 *
 * @param  szPath         Path to the file to be read.
 * @param  szFileContents File contents buffer. Allocated globally by this
                          function.
 * @return                TRUE if the operation was successful.
 */
BOOL ReadFileContents(LPCTSTR szPath, LPTSTR *szFileContents) {
	DWORD dwFileSize;
	DWORD dwBytesRead;
	HANDLE hFile;
	BOOL bSuccess = TRUE;
	char *szaBuffer;
	
	// Open the file.
	hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		// TODO: Use GetLastError.
		MessageBox(NULL, L"Couldn't open file to read contents.",
			L"Read File Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Get file size to use to read the whole thing in one go.
	dwFileSize = GetFileSize(hFile, NULL);
	
	// Allocate the memory for the file contents.
	*szFileContents = (LPTSTR)LocalAlloc(LMEM_FIXED, (dwFileSize + 1) *
		sizeof(TCHAR));
	szaBuffer = (char*)LocalAlloc(LMEM_FIXED, (dwFileSize + 1) * sizeof(char));
	
	// Read the file into the buffer.
	if (!ReadFile(hFile, szaBuffer, dwFileSize, &dwBytesRead, NULL)) {
		// TODO: Use GetLastError.
		MessageBox(NULL, L"Failed to read the contents of the file.",
			L"Read File Error", MB_OK | MB_ICONERROR);
		bSuccess = FALSE;
	}

	// Terminate the buffer and convert it.
	szaBuffer[dwBytesRead] = '\0';
	if (!ConvertStringAtoW(*szFileContents, szaBuffer)) {
		MessageBox(NULL, L"Failed to convert file buffer from ASCII to Unicode",
			L"Conversion Failed", MB_OK | MB_ICONERROR);

		LocalFree(*szFileContents);
		bSuccess = FALSE;
	}
    
	// Clean up.
	CloseHandle(hFile);
	LocalFree(szaBuffer);

	return bSuccess;
}

/**
 * Save contents to a file.
 *
 * @param  szFilePath Path to the file to be overwritten.
 * @param  szContents Contents to place inside the file.
 * @return            TRUE if the operation was successful.
 */
BOOL SaveFileContents(LPCTSTR szFilePath, LPCTSTR szContents) {
    HANDLE hFile;
	DWORD dwTextLength;
	DWORD dwBytesWritten;
	char *szaBuffer;

	// Get text length.
	dwTextLength = wcslen(szContents);

	// Open file for writing.
    hFile = CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
		// TODO: Use GetLastError.
		MessageBox(NULL, L"Couldn't open file to write contents.",
			L"Write File Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Convert text to ASCII before writing to the file.
	szaBuffer = (char*)LocalAlloc(LMEM_FIXED, (dwTextLength + 1) * sizeof(char));
	if (!ConvertStringWtoA(szaBuffer, szContents)) {
		MessageBox(NULL, L"Failed to convert contents buffer from Unicode to "
			L"ASCII.", L"Conversion Failed", MB_OK | MB_ICONERROR);

		CloseHandle(hFile);
		LocalFree(szaBuffer);
		return FALSE;
	}

	// Write to the file.
	if (!WriteFile(hFile, szaBuffer, dwTextLength, &dwBytesWritten, NULL)) {
		// TODO: Use GetLastError.
		MessageBox(NULL, L"Couldn't write contents to file.",
			L"Write File Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	
	// Clean up.
	CloseHandle(hFile);
	LocalFree(szaBuffer);

    return TRUE;
}

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
	if (!ConvertStringAtoW(szMsg, buffer)) {
		OutputDebugString(L"Error while converting string to unicode.\r\n");
		return;
	}

	// Print to debug console.
	OutputDebugString(szMsg);
}
