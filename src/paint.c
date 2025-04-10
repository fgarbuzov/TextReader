#include "main.h"

/** Paint
DESCRIPTION		Process message WM_PAINT. Print visible text in selected view mode.
PARAM[IN]		  HWND hwnd
              HDC hdc
              FILESTRUCT fs - file structure
              VIEWMODE ViewMode
*/
void Paint(HWND hwnd, HDC hdc, FILESTRUCT * pFS, VIEWMODE ViewMode)
{
  int x = 0, y = 0;
  int i, iStr;
  int nPrintedStrLen, nRestStrLen;
  int nVisibleStrLen;
  int nVisibleStrNum = pFS->nWindowHeight / pFS->nCharHeight;
  int nWindowWidthInChars = pFS->nWindowWidth / pFS->nCharWidth;
  LPTSTR pCurrentStr;

  // exit if buffers are not filled
  if (pFS->pBuffer == NULL || pFS->ppString == NULL || pFS->pStrLen == NULL)
    return;
  
  switch (ViewMode)
  {
  case VM_NO_WRAPPING:
    for (i = pFS->iFirstStr; (i < pFS->iFirstStr + nVisibleStrNum) && (i < pFS->nStrNum); ++i)
    {
      nVisibleStrLen = min(nWindowWidthInChars, pFS->pStrLen[i] - pFS->nHorShift);
      if (nVisibleStrLen > 0)
        TextOut(hdc, x, y, pFS->ppString[i] + pFS->nHorShift, nVisibleStrLen);
      y += pFS->nCharHeight;
    }
    break;

  case VM_WRAPPING:
    iStr = pFS->iFirstStr;
    pCurrentStr = pFS->ppString[iStr] + pFS->iFirstChar;
    nPrintedStrLen = pFS->iFirstChar;
    for (i = 0; i < nVisibleStrNum; ++i)
    {
      nRestStrLen = pFS->pStrLen[iStr] - nPrintedStrLen;
      nVisibleStrLen = min(nWindowWidthInChars, nRestStrLen);
      TextOut(hdc, x, y, pCurrentStr, nVisibleStrLen);
      if (nVisibleStrLen == nRestStrLen)
      {
        ++iStr;
        if (iStr == nVisibleStrNum + pFS->iFirstStr) 
          assert(i == nVisibleStrNum - 1);
        if (iStr == pFS->nStrNum || iStr == nVisibleStrNum + pFS->iFirstStr) break;
        pCurrentStr = pFS->ppString[iStr];
        nPrintedStrLen = 0;
      }
      else
      {
        pCurrentStr += nVisibleStrLen;
        nPrintedStrLen += nVisibleStrLen;
      }
      y += pFS->nCharHeight;
    }
    pFS->iLastStr  = iStr;
    pFS->iLastChar = nPrintedStrLen;
    break;
  }
}

/*SetScrollRange(hwnd, SB_VERT, 0, max(0, pFS->nStrNum - nVisibleStrNum), TRUE);
SetScrollRange(hwnd, SB_HORZ, 0, max(0, pFS->nMaxStrLen - nMaxCharToPrint), TRUE);
SetScrollPos(hwnd, SB_VERT, pFS->iFirstStr, TRUE);
SetScrollPos(hwnd, SB_HORZ, pFS->nHorShift, TRUE);*/

/*SetScrollRange(hwnd, SB_VERT, 0, pFS->nVScrollRange, TRUE);
SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
SetScrollPos(hwnd, SB_VERT, pFS->nVScrollPos, TRUE);*/