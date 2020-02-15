/**
 * UkiHelper.c
 * A simple wrapper around the Uki library.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "UkiHelper.h"
#include "Utilities.h"

/**
 * Initializes the Uki engine.
 *
 * @param  szWikiPath Path to the root of the Uki wiki.
 * @return            TRUE if everything went fine.
 */
BOOL InitializeUki(LPCTSTR szWikiPath) {
	char szaPath[UKI_MAX_PATH];
	int err;

	// Convert Unicode string to ASCII.
	if (ConvertStringWtoA(szaPath, szWikiPath)) {
		// Initialize the engine.
		if ((err = uki_initialize(szaPath)) != UKI_OK) {
			ShowUkiErrorDialog(err);
			CloseUki();

			return FALSE;
		}
	} else {
		MessageBox(NULL, L"Failed to convert the wiki path from ASCII to Unicode",
			L"Conversion Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

/**
 * Grabs an article path.
 *
 * @param  szArticlePath Pre-allocated buffer to receive the file path.
 * @param  ukiArticle    Uki article structure.
 * @return               TRUE if the operation was successful.
 */
BOOL GetUkiArticlePath(LPTSTR szArticlePath, const UKIARTICLE ukiArticle) {
	char szaPath[UKI_MAX_PATH];
	int err;

	// Get the file path.
	if ((err = uki_article_fpath(szaPath, ukiArticle)) != UKI_OK) {
		ShowUkiErrorDialog(err);
		return FALSE;
	}

	// Convert string to Unicode.
	if (!ConvertStringAtoW(szArticlePath, szaPath)) {
		MessageBox(NULL, L"Failed to convert article path from ASCII to Unicode",
			L"Conversion Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;
}

/**
 * Grabs a template path.
 *
 * @param  szTemplatePath Pre-allocated buffer to receive the file path.
 * @param  ukiTemplate    Uki template structure.
 * @return                TRUE if the operation was successful.
 */
BOOL GetUkiTemplatePath(LPTSTR szTemplatePath, const UKITEMPLATE ukiTemplate) {
	char szaPath[UKI_MAX_PATH];
	int err;

	// Get the file path.
	if ((err = uki_template_fpath(szaPath, ukiTemplate)) != UKI_OK) {
		ShowUkiErrorDialog(err);
		return FALSE;
	}

	// Convert string to Unicode.
	if (!ConvertStringAtoW(szTemplatePath, szaPath)) {
		MessageBox(NULL, L"Failed to convert template path from ASCII to Unicode",
			L"Conversion Error", MB_OK | MB_ICONERROR);
	}

	return TRUE;
}

/**
 * Grabs a Uki article by its index.
 *
 * @param  ukiArticle Pointer to a Uki article structure to be populated.
 * @param  nIndex     Index of the article to be fetched.
 * @return            TRUE if we found the article.
 */
BOOL GetUkiArticle(UKIARTICLE *ukiArticle, size_t nIndex) {
	*ukiArticle = uki_article(nIndex);
	return ukiArticle->name != NULL;
}

/**
 * Grabs a Uki template by its index.
 *
 * @param  ukiTemplate Pointer to a Uki template structure to be populated.
 * @param  nIndex      Index of the template to be fetched.
 * @return             TRUE if we found the template.
 */
BOOL GetUkiTemplate(UKITEMPLATE *ukiTemplate, size_t nIndex) {
	*ukiTemplate = uki_template(nIndex);
	return ukiTemplate->name != NULL;
}

/**
 * Cleans our mess.
 */
void CloseUki() {
	uki_clean();
}

/**
 * Displays a Uki error dialog.
 *
 * @param nErrorCode Uki error code.
 */
void ShowUkiErrorDialog(int nErrorCode) {
	LPTSTR szErrorMsg;
	const char *szaMsg = uki_error_msg(nErrorCode);

	// Allocate memory for the message string.
	szErrorMsg = (LPTSTR)LocalAlloc(LMEM_FIXED, (strlen(szaMsg) + 1) *
		sizeof(TCHAR));

	// Convert the message and display the dialog.
	if (ConvertStringAtoW(szErrorMsg, szaMsg)) {
		MessageBox(NULL, szErrorMsg, L"Uki Error", MB_OK | MB_ICONERROR);
	}

	// Free the allocated buffer.
	LocalFree(szErrorMsg);
}

/**
 * Tests the Uki engine.
 *
 * @param  szaWikiPath Path to the Uki wiki root.
 * @param  szaWikiPage Relative path a Uki page.
 * @return             FALSE if everything went fine.
 */
int TestInitializeUki(const char *szaWikiPath, const char *szaWikiPage) {
	uki_variable_t var;
	uki_article_t article;
	uki_template_t template;
	size_t it = 0;
	size_t ia = 0;
	uint8_t iv = 0;
	char *content;
	int uki_error;

	// Initialize the uki wiki.
	if ((uki_error = uki_initialize(szaWikiPath)) != UKI_OK) {
		PrintDebugConsole(uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print configurations.
	PrintDebugConsole("Configurations:\r\n");
	var = uki_config(iv);
	while (var.key != NULL) {
		PrintDebugConsole("   %s <- %s\r\n", var.key, var.value);
		iv++;
		var = uki_config(iv);
	}
	PrintDebugConsole("\r\n");

	// Print variables.
	PrintDebugConsole("Variables:\r\n");
	iv = 0;
	var = uki_variable(iv);
	while (var.key != NULL) {
		PrintDebugConsole("   %s <- %s\r\n", var.key, var.value);
		iv++;
		var = uki_variable(iv);
	}
	PrintDebugConsole("\r\n");

	// Print templates.
	it = 0;
	PrintDebugConsole("Templates:\r\n");
	template = uki_template(it);
	while (template.name != NULL) {
		PrintDebugConsole("   %d %s  |  %s <- %s\r\n", template.deepness,
			template.parent, template.path, template.name);
		it++;
		template = uki_template(it);
	}
	PrintDebugConsole("\r\n");

	// Print articles.
	PrintDebugConsole("Articles:\r\n");
	article = uki_article(ia);
	while (article.name != NULL) {
		PrintDebugConsole("   %d %s  |  %s <- %s\r\n", article.deepness,
			article.parent, article.path, article.name);
		ia++;
		article = uki_article(ia);
	}
	PrintDebugConsole("\r\n");

	// Render a page.
	if ((uki_error = uki_render_page(&content, szaWikiPage)) != UKI_OK) {
		PrintDebugConsole("ERROR: %s", uki_error_msg(uki_error));
		uki_clean();

		return 1;
	}

	// Print the page content.
	PrintDebugConsole("%s\r\n", content);
	return 0;
}