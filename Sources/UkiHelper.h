/**
 * UkiHelper.h
 * A simple wrapper around the Uki library.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _UKIHELPER_H
#define _UKIHELPER_H

#include <windows.h>
#include "uki.h"

// Generic definitions to make the API look Win32zy.
#define UKITEMPLATE uki_template_t
#define UKIARTICLE  uki_article_t

// Messages.
void ShowUkiErrorDialog(int nErrorCode);

// Initialization and destruction.
void CloseUki();
BOOL InitializeUki(LPCTSTR szWikiPath);

// Lookup.
BOOL GetUkiTemplate(UKITEMPLATE *ukiTemplate, size_t nIndex);
BOOL GetUkiArticle(UKIARTICLE *ukiArticle, size_t nIndex);
BOOL GetUkiArticlePath(LPTSTR szArticlePath, const UKIARTICLE ukiArticle);
BOOL GetUkiTemplatePath(LPTSTR szTemplatePath, const UKITEMPLATE ukiTemplate);

// Debugging.
int TestInitializeUki(const char *szaWikiPath, const char *szaWikiPage);

#endif  // _UKIHELPER_H