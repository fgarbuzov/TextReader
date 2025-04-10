#include "main.h"

/** CalculateStrNumWithWrap
DESCRIPTION		Calculate number of strings in case of line wrapping
PARAM[IN]		  FILESTRUCT fs - file structure
RETURNS       int nStrNumWithWrapping - number of strings in case of line wrapping
*/
int CalculateStrNumWithWrap(FILESTRUCT fs)
{
  int i;
  int nStrNumWithWrapping = 0;
  for (i = 0; i < fs.nStrNum; ++i)
    nStrNumWithWrapping += 1 + (fs.pStrLen[i] * fs.nCharWidth) / fs.nWindowWidth;
  return nStrNumWithWrapping;
}

/** Size
DESCRIBTION   Process message WM_SIZE. Change window metric in file structure
PARAM[IN]		  HWND hwnd
PARAM[IN/OUT] FILESTRUCT * pFS - pointer to file structure
*/
void Size(HWND hwnd, FILESTRUCT * pFS, VIEWMODE vm)
{
  RECT rect;
  int nNewMaxCharToPrint, nOldMaxCharToPrint = pFS->nWindowWidth / pFS->nCharWidth;
  int nNewVisibleStrNum;
  int nOldLinesInStr, nNewLinesInStr;
  int nWindowWidthOld = pFS->nWindowWidth;
  int i;
  int nNewWndWidthInChars, nOldWndWidthInChars = pFS->nWindowWidth / pFS->nCharWidth;
  int nNewWndHeightInChars, nOldWndHeightInChars;

  GetClientRect(hwnd, &rect);
  pFS->nWindowWidth  = rect.right - rect.left;
  pFS->nWindowHeight = rect.bottom - rect.top;

  switch (vm)
  {
  case VM_NO_WRAPPING:
    nNewWndWidthInChars = pFS->nWindowWidth / pFS->nCharWidth;
    if (pFS->nHorShift + nNewWndWidthInChars >= pFS->nMaxStrLen && pFS->nHorShift > 0)
      pFS->nHorShift--;

    nNewWndHeightInChars = pFS->nWindowHeight / pFS->nCharHeight;
    if (pFS->iFirstStr + nNewWndHeightInChars > pFS->nStrNum && pFS->iFirstStr > 0)
      pFS->iFirstStr--;
    break;

  case VM_WRAPPING:
    nNewVisibleStrNum = pFS->nWindowHeight / pFS->nCharHeight;
    /* ATTENTION: for VM_WRAPPING essential to calculate new vertical scroll range! */
    pFS->nVScrollRange = CalculateStrNumWithWrap(*pFS);
    //pFS->nVScrollRange = max(0, CalculateStrNumWithWrap(*pFS) - nNewVisibleStrNum);
    if (pFS->nVScrollPos > pFS->nVScrollRange && pFS->nVScrollPos > 0)
    {
      nNewMaxCharToPrint = pFS->nWindowWidth / pFS->nCharWidth;
      for (i = 0; i < pFS->iFirstStr; ++i)
      {
        nOldLinesInStr = 1 + (pFS->pStrLen[i] * pFS->nCharWidth) / nWindowWidthOld;
        nNewLinesInStr = 1 + (pFS->pStrLen[i] * pFS->nCharWidth) / pFS->nWindowWidth;
        if (nOldLinesInStr > nNewLinesInStr)
          pFS->nVScrollPos--;
      }
      for (i = pFS->iFirstStr; i < pFS->nStrNum; ++i)
      {
        nOldLinesInStr = 1 + (pFS->pStrLen[i] * pFS->nCharWidth) / nWindowWidthOld;
        nNewLinesInStr = 1 + (pFS->pStrLen[i] * pFS->nCharWidth) / pFS->nWindowWidth;
        if (nOldLinesInStr > nNewLinesInStr)
          LineUp(hwnd, pFS, vm);
      }
      //pFS->nVScrollPos = pFS->nVScrollRange;
    }
  }

  InvalidateRect(hwnd, NULL, TRUE);
  SetScrolls(hwnd, *pFS, vm);
}
