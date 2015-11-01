#include "main.h"
#include "menu.h"

/** CalculateVScrollPos
DESCRIPTION		Calculate vertical scroll position
PARAM[IN]		  FILESTRUCT fs - file structure
RETURNS       int nVScrollPos - vertical scroll position
*/
int CalculateVScrollPos(FILESTRUCT fs)
{
  int i;
  int nVScrollPos = 0;
  int nWindowWidthInChars = fs.nWindowWidth / fs.nCharWidth;
  for (i = 0; i < fs.iFirstStr; ++i)
    nVScrollPos += 1 + fs.pStrLen[i] / nWindowWidthInChars;
  nVScrollPos += fs.iFirstChar / nWindowWidthInChars;
  return nVScrollPos;
}

/** Command
DESCRIBTION   Process message WM_COMMAND: open file, exit, change view mode.
PARAM[IN]		  HWND hwnd 
              WPARAM wParam
PARAM[IN/OUT] FILESTRUCT * pFS - pointer to file structure
              VIEWMODE * pVM - pointer to view mode
*/
error_t Command(HWND hwnd, WPARAM wParam, FILESTRUCT * pFS, VIEWMODE * pVM)
{
  HMENU hMenu = GetMenu(hwnd);
  OPENFILENAME ofn;
  TCHAR szFile[200] = "\0";
  error_t error;
  int nWindowHeightInChars;

  switch (LOWORD(wParam))
  {
  case IDM_OPEN:
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    if (GetOpenFileName(&ofn) == TRUE)
    {
      DestroyFile(pFS);
      error = LoadFile(ofn.lpstrFile, pFS);
      InvalidateRect(hwnd, NULL, TRUE);
      if (error)
        return error;
    }
    else
      return ERR_OPEN_DIALOG_BOX;
    break;

  case IDM_NO_WRAPPING:
    *pVM = VM_NO_WRAPPING;
    CheckMenuItem(hMenu, IDM_WRAPPING, MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_NO_WRAPPING, MF_CHECKED);
    nWindowHeightInChars = pFS->nWindowHeight / pFS->nCharHeight;
    if (pFS->iFirstStr + nWindowHeightInChars > pFS->nStrNum)
      pFS->iFirstStr = max(0, pFS->nStrNum - nWindowHeightInChars);
    pFS->iFirstChar = 0; // not used but need to be zero
    pFS->nHorShift = 0;
    pFS->nVScrollPos = pFS->iFirstStr;
    pFS->iCaretChar = 0;
    pFS->iCaretStr = pFS->iFirstStr;
    SetCaretPos(0, 0);
    InvalidateRect(hwnd, NULL, TRUE);
    SetScrolls(hwnd, *pFS, *pVM);
    break;

  case IDM_WRAPPING:
    *pVM = VM_WRAPPING;
    CheckMenuItem(hMenu, IDM_NO_WRAPPING, MF_UNCHECKED);
    CheckMenuItem(hMenu, IDM_WRAPPING, MF_CHECKED);
    nWindowHeightInChars = (pFS->nWindowHeight + GetSystemMetrics(SM_CYHSCROLL)) / pFS->nCharHeight;
    pFS->iFirstChar = 0;
    pFS->nHorShift = 0; // neccessary
    pFS->nVScrollPos = CalculateVScrollPos(*pFS);
    pFS->nVScrollRange = max(0, CalculateStrNumWithWrap(*pFS) - nWindowHeightInChars);
    pFS->iCaretChar = 0;
    pFS->iCaretStr = pFS->iFirstStr;
    SetCaretPos(0, 0);
    InvalidateRect(hwnd, NULL, TRUE);
    SetScrolls(hwnd, *pFS, *pVM);
    break;

  case IDM_EXIT:
    DestroyFile(pFS);
    PostQuitMessage(0);
    break;
  }

  return NO_ERR;
}