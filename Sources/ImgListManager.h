/**
 * ImgListManager.h
 * A simple ImageList manager helper module.
 *
 * @author Nathan Campos <hi@nathancampos.me>
 */

#ifndef _IMGLISTMANAGER_H
#define _IMGLISTMANAGER_H

#include <windows.h>
#include <commctrl.h>
#include "resource.h"

HIMAGELIST InitializeImageList(HINSTANCE hInst);
int ImageListIconIndex(int nResourceID);

#endif  // _IMGLISTMANAGER_H