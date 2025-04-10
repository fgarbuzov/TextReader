#include "main.h"

bool ScrollUpIsPossible(FILESTRUCT fs, VIEWMODE vm);
bool ScrollDownIsPossible(FILESTRUCT fs, VIEWMODE vm);
int RestStrNum(FILESTRUCT fs);

/** VScroll
DESCRIBTION   Process message WM_VSCROLL. Process different types of scroll.
PARAM[IN]		  HWND hwnd
              WPARAM wParam - word with information about command
              VIEWMODE vm - view mode (with or without line wrapping)
PARAM[IN/OUT] FILESTRUCT * pFS - pointer to file structure
*/
void VScroll(HWND hwnd, WPARAM wParam, FILESTRUCT * pFS, VIEWMODE vm)
{
  int nVisibleStrNum, nVScrollNewPos, nDiff;
  int i;

  switch (LOWORD(wParam))
  {
  case SB_LINEDOWN:
    LineDown(hwnd, pFS, vm);
    break;

  case SB_LINEUP:
    LineUp(hwnd, pFS, vm);
    break;

  case SB_PAGEDOWN:
    nVisibleStrNum = pFS->nWindowHeight / pFS->nCharHeight;
    for (i = 0; i < nVisibleStrNum; ++i)
      LineDown(hwnd, pFS, vm);
    break;

  case SB_PAGEUP:
    nVisibleStrNum = pFS->nWindowHeight / pFS->nCharHeight;
    for (i = 0; i < nVisibleStrNum; ++i)
      LineUp(hwnd, pFS, vm);
    break;

  case SB_THUMBPOSITION:
  case SB_THUMBTRACK:
    nVScrollNewPos = HIWORD(wParam);
    nDiff = nVScrollNewPos - pFS->nVScrollPos;
    if (nDiff > 0)
    {
      for (i = 0; i < nDiff; ++i)
        LineDown(hwnd, pFS, vm);
    }
    else if (nDiff < 0)
    {
      for (i = 0; i < -nDiff; ++i)
        LineUp(hwnd, pFS, vm);
    }
    break;
  }
}

void HScroll(HWND hwnd, WPARAM wParam, FILESTRUCT * pFS)
{
  int nHScrollNewPos, nDiff;

  switch (LOWORD(wParam))
  {
  case SB_LINERIGHT:
    LineRight(hwnd, pFS);
    break;

  case SB_LINELEFT:
    LineLeft(hwnd, pFS);
    break;

  case SB_THUMBPOSITION:
  case SB_THUMBTRACK:
    nHScrollNewPos = HIWORD(wParam);
    nDiff = nHScrollNewPos - pFS->nHorShift;
    pFS->nHorShift += nDiff;
    if (pFS->nHorShift > pFS->nMaxStrLen - 1)
      pFS->nHorShift = pFS->nMaxStrLen - 1;
    else if (pFS->nHorShift < 0)
      pFS->nHorShift = 0;
    InvalidateRect(hwnd, NULL, TRUE);
    SetScrolls(hwnd, *pFS, VM_NO_WRAPPING);
    break;
  }
}

/** LineRight
DESCRIBTION   Process one line right scroll
PARAM[IN/OUT] FILESTRUCT * pFS - pointer to file structure
*/
void LineRight(HWND hwnd, FILESTRUCT * pFS)
{
  if (pFS->nHorShift < pFS->nMaxStrLen - pFS->nWindowWidth / pFS->nCharWidth)
  {
    pFS->nHorShift++;
    InvalidateRect(hwnd, NULL, TRUE);
    SetScrolls(hwnd, *pFS, VM_NO_WRAPPING);
  }
}

/** LineLeft
DESCRIBTION   Process one line left scroll
PARAM[IN/OUT] FILESTRUCT * pFS - pointer to file structure
*/
void LineLeft(HWND hwnd, FILESTRUCT * pFS)
{
  if (pFS->nHorShift > 0)
  {
    pFS->nHorShift--;
    InvalidateRect(hwnd, NULL, TRUE);
    SetScrolls(hwnd, *pFS, VM_NO_WRAPPING);
  }
}

/** LineDown
DESCRIBTION   Process one line down scroll
PARAM[IN]     VIEWMODE vm - view mode (with or without line wrapping)
PARAM[IN/OUT] FILESTRUCT * pFS - pointer to file structure
*/
void LineDown(HWND hwnd, FILESTRUCT * pFS, VIEWMODE vm)
{
  int nRestFirstStrLen;
  int nWindowHeightInChars;
  int nWindowWidthInChars;

  switch (vm)
  {
  case VM_NO_WRAPPING:
    nWindowHeightInChars = pFS->nWindowHeight / pFS->nCharHeight;
    if (ScrollDownIsPossible(*pFS, vm))
    {
      pFS->iFirstStr++;
      pFS->nVScrollPos = pFS->iFirstStr;
      InvalidateRect(hwnd, NULL, TRUE);
      SetScrolls(hwnd, *pFS, vm);
    }
    break;

  case VM_WRAPPING:
    nRestFirstStrLen = pFS->pStrLen[pFS->iFirstStr] - pFS->iFirstChar;
    nWindowWidthInChars = pFS->nWindowWidth / pFS->nCharWidth;
    if (ScrollDownIsPossible(*pFS, vm))
    {
      if (nRestFirstStrLen > nWindowWidthInChars)
        pFS->iFirstChar += nWindowWidthInChars;
      else
      {
        pFS->iFirstChar = 0;
        pFS->iFirstStr++;
      }
      pFS->nVScrollPos++;
      InvalidateRect(hwnd, NULL, TRUE);
      SetScrolls(hwnd, *pFS, vm);
    }
  }
}

/** LineUp
DESCRIBTION   Process one line up scroll
PARAM[IN]     VIEWMODE vm - view mode (with or without line wrapping)
PARAM[IN/OUT] FILESTRUCT * pFS - pointer to file structure
*/
void LineUp(HWND hwnd, FILESTRUCT * pFS, VIEWMODE vm)
{
  int nWindowWidthInChars;

  switch (vm)
  {
  case VM_NO_WRAPPING:
    if (ScrollUpIsPossible(*pFS, vm))
    {
      pFS->iFirstStr--;
      pFS->nVScrollPos = pFS->iFirstStr;
      InvalidateRect(hwnd, NULL, TRUE);
      SetScrolls(hwnd, *pFS, vm);
    }
    break;

  case VM_WRAPPING:
    nWindowWidthInChars = pFS->nWindowWidth / pFS->nCharWidth;
    if (ScrollUpIsPossible(*pFS, vm))
    {
      if (pFS->iFirstChar > 0)
      {
        pFS->iFirstChar -= nWindowWidthInChars;
      }
      else
      {
        pFS->iFirstStr--;
        pFS->iFirstChar = pFS->pStrLen[pFS->iFirstStr] - pFS->pStrLen[pFS->iFirstStr] % nWindowWidthInChars;
      }
      pFS->nVScrollPos--;
      InvalidateRect(hwnd, NULL, TRUE);
      SetScrolls(hwnd, *pFS, vm);
    }
    break;
  }
}

/** SetScrolls
DESCRIBTION   Set scrolls range and scrolls position
PARAM[IN]		  HWND hwnd
              FILESTRUCT fs - file structure
              VIEWMODE vm - view mode (with or without line wrapping)
*/
void SetScrolls(HWND hwnd, FILESTRUCT fs, VIEWMODE vm)
{
  switch (vm)
  {
  case VM_NO_WRAPPING:
    SetScrollRange(hwnd, SB_VERT, 0, max(0, fs.nStrNum - fs.nWindowHeight / fs.nCharHeight), TRUE);
    SetScrollRange(hwnd, SB_HORZ, 0, max(0, fs.nMaxStrLen - fs.nWindowWidth / fs.nCharWidth), TRUE);
    SetScrollPos(hwnd, SB_VERT, fs.nVScrollPos, TRUE);
    SetScrollPos(hwnd, SB_HORZ, fs.nHorShift, TRUE);
    break;
  case VM_WRAPPING:
    SetScrollRange(hwnd, SB_VERT, 0, fs.nVScrollRange, TRUE);
    SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
    SetScrollPos(hwnd, SB_VERT, fs.nVScrollPos, TRUE);
    break;
  }
}

/** ScrollUpIsPossible
DESCRIBTION   Check the possibility of one line scroll up
PARAM[IN]		  FILESTRUCT fs - file structure
              VIEWMODE vm - view mode (with or without line wrapping)
RETURNS       Possibility: true or false
*/
bool ScrollUpIsPossible(FILESTRUCT fs, VIEWMODE vm)
{
  switch (vm)
  {
  case VM_NO_WRAPPING:
    if (fs.iFirstStr > 0)
      return true;
    else
      return false;

  case VM_WRAPPING:
    if (fs.iFirstStr > 0 || fs.iFirstChar >= fs.nWindowWidth / fs.nCharWidth)
      return true;
    else
      return false;
  }
  return false;
}

/** ScrollDownIsPossible
DESCRIBTION   Check the possibility of one line scroll down
PARAM[IN]		  FILESTRUCT fs - file structure
              VIEWMODE vm - view mode (with or without line wrapping)
RETURNS       Possibility: true or false
*/
bool ScrollDownIsPossible(FILESTRUCT fs, VIEWMODE vm)
{
  int nWindowHeightInChars = fs.nWindowHeight / fs.nCharHeight;
  
  switch (vm)
  {
  case VM_NO_WRAPPING:
    if (fs.iFirstStr < fs.nStrNum - nWindowHeightInChars)
      return true;
    else
      return false;
  
  case VM_WRAPPING:
    if (fs.iFirstStr > fs.nStrNum)
      return true;
    else
      return false;
  }
  return false;
}

/** ScrollDownIsPossible
DESCRIBTION   Check the possibility of one line scroll down
PARAM[IN]		  FILESTRUCT fs - file structure
RETURNS       int nRestStrNum - number of strings consider wrapping
*/
int RestStrNum(FILESTRUCT fs)
{
  int nWindowWidthInChars = fs.nWindowWidth / fs.nCharWidth;
  int nRestStrNum = 1 + (fs.pStrLen[fs.iFirstStr] - fs.iFirstChar) / nWindowWidthInChars;
  int i;
  for (i = fs.iFirstStr + 1; i < fs.nStrNum; ++i)
    nRestStrNum += 1 + fs.pStrLen[i] / nWindowWidthInChars;
  return nRestStrNum;
}