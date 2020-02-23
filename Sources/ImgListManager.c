/**
 * ImgListManager.c
 * A simple ImageList manager helper module.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#include "ImgListManager.h"

// Image definitions.
#define CX_ICON   16
#define CY_ICON   16
#define NUM_ICONS 8

// Array definitions.
#define RESID 0
#define ILIDX 1

// Global variables.
HIMAGELIST hIml;
int icnList[NUM_ICONS];
int resList[NUM_ICONS] = { IDB_ARTICLE, IDB_FOLDER, IDB_FOLDEROPEN, IDB_LIBRARY,
						   IDB_TEMPLATE, IDB_TEMPLATELIBRARY };

// Private methods.
void PopulateImageList(HINSTANCE hInst);

/**
 * Initializes the ImageList component and populates it.
 *
 * @param  hInst Application instance handle.
 * @return       ImageList handle.
 */
HIMAGELIST InitializeImageList(HINSTANCE hInst) {
    // Ensure that the common control DLL is loaded. 
    InitCommonControls(); 
	
    // Create a ImageList large enough to hold the icons. 
    hIml = ImageList_Create(CX_ICON, CY_ICON, 0, NUM_ICONS, 0); 
	
	// Load icons into the ImageList and populate the indexing array.
    PopulateImageList(hInst);

	return hIml;
}

/**
 * Gets a ImageList index value related to a icon resource ID.
 *
 * @param  iResourceID Icon resource ID.
 * @return             ImageList index value if found. -1 otherwise.
 */
int ImageListIconIndex(int iResourceID) {
	int i;

	// Go through the array trying to find a match.
	for (i = 0; i < NUM_ICONS; i++) {
		if (resList[i] == iResourceID) {
			return icnList[i];
		}
	}

	return -1;
}

/**
 * Populates the ImageList.
 *
 * @param hInst Application instance handle.
 */
void PopulateImageList(HINSTANCE hInst) {
	HBITMAP hBmp;
	int i;
	
	// Go through the resources adding them to the list.
	for (i = 0; i < NUM_ICONS; i++) {
		hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(resList[i])); 
		icnList[i] = ImageList_Add(hIml, hBmp, NULL); 
	}
}
