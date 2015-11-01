#include "main.h"

/** Keydown
DESCRIBTION   Process message WM_KEYDOWN. Send message with certain scroll type
as the reaction on pushing certain key
PARAM[IN]		  HWND hwnd
WPARAM wParam - word with information about command
VIEWMODE vm - view mode: with or without line wrapping
PARAM[IN/OUT] FILESTRUCT * pFS - pointer to file structure
*/
void Keydown(HWND hwnd, WPARAM wParam, FILESTRUCT * pFS, VIEWMODE vm)
{
  int nMaxCharToPrint = pFS->nWindowWidth / pFS->nCharWidth;
  int nVisibleStrNum = pFS->nWindowHeight / pFS->nCharHeight;
  int i;

  switch (wParam)
  {
  case VK_DOWN:
    pFS->nCaretPosY++;
    if (vm == VM_NO_WRAPPING)
    {
      if (pFS->iCaretStr < pFS->nStrNum - 1)
      {
        pFS->iCaretStr++;
        if (pFS->iCaretChar > pFS->pStrLen[pFS->iCaretStr])
        {
          pFS->iCaretChar = pFS->pStrLen[pFS->iCaretStr];
          pFS->nCaretPosX = pFS->pStrLen[pFS->iCaretStr] - pFS->nHorShift;
          if (pFS->iCaretChar < pFS->nHorShift)
          {
            pFS->nCaretPosX = 0;
            for (i = 0; i < pFS->nHorShift - pFS->iCaretChar; ++i)
              SendMessage(hwnd, WM_HSCROLL, SB_LINELEFT, 0L);
          }
        }
      }
      if (pFS->nCaretPosY >= min(nVisibleStrNum, pFS->nStrNum))
      {
        LineDown(hwnd, pFS, vm);
        pFS->nCaretPosY--;
      }
    }
    else
    {
      pFS->iCaretChar += nMaxCharToPrint;
      if (pFS->iCaretChar > pFS->pStrLen[pFS->iCaretStr])
      {
        if (pFS->iCaretChar % nMaxCharToPrint <= pFS->pStrLen[pFS->iCaretStr] % nMaxCharToPrint)
        {
          if (pFS->iCaretStr < pFS->nStrNum - 1)
          {
            pFS->iCaretStr++;
            pFS->iCaretChar = pFS->nCaretPosX;
            if (pFS->iCaretChar > pFS->pStrLen[pFS->iCaretStr]) {
              pFS->iCaretChar = pFS->pStrLen[pFS->iCaretStr];
              pFS->nCaretPosX = pFS->pStrLen[pFS->iCaretStr] - 1;
            }
          }
          else
          {
            pFS->iCaretChar -= nMaxCharToPrint;
            pFS->nCaretPosY--;
          }
        }
        else
        {
          pFS->nCaretPosX = pFS->pStrLen[pFS->iCaretStr] % nMaxCharToPrint;
          pFS->iCaretChar = pFS->pStrLen[pFS->iCaretStr];
        }
      }
      if (pFS->nCaretPosY >= nVisibleStrNum)
      {
        LineDown(hwnd, pFS, vm);
        pFS->nCaretPosY--;
      }
      else if (nVisibleStrNum < pFS->nStrNum)
      {
        if (pFS->nCaretPosY >= CalculateStrNumWithWrap(*pFS))
        {
          LineDown(hwnd, pFS, vm);
          pFS->nCaretPosY--;
        }
      }
    }
    SetCaretPos(pFS->nCaretPosX * pFS->nCharWidth, pFS->nCaretPosY * pFS->nCharHeight);
    break;

  case VK_UP:
    pFS->nCaretPosY--;
    if (vm == VM_NO_WRAPPING)
    {
      if (pFS->iCaretStr > 0)
      {
        pFS->iCaretStr--;
        if (pFS->iCaretChar > pFS->pStrLen[pFS->iCaretStr])
        {
          pFS->iCaretChar = pFS->pStrLen[pFS->iCaretStr];
          pFS->nCaretPosX = pFS->pStrLen[pFS->iCaretStr] - pFS->nHorShift;
          if (pFS->iCaretChar < pFS->nHorShift)
          {
            pFS->nCaretPosX = 0;
            for (i = 0; i < pFS->nHorShift - pFS->iCaretChar; ++i)
              SendMessage(hwnd, WM_HSCROLL, SB_LINELEFT, 0L);
          }
        }
      }
      if (pFS->nCaretPosY < 0)
      {
        LineUp(hwnd, pFS, vm);
        pFS->nCaretPosY++;
      }
    }
    else
    {
      pFS->iCaretChar -= nMaxCharToPrint;
      if (pFS->iCaretChar < 0)
      {
        if (pFS->iCaretStr > 0)
        {
          pFS->iCaretStr--;
          pFS->iCaretChar = pFS->pStrLen[pFS->iCaretStr];
          if (pFS->pStrLen[pFS->iCaretStr] % nMaxCharToPrint < pFS->nCaretPosX)
            pFS->nCaretPosX = pFS->pStrLen[pFS->iCaretStr] % nMaxCharToPrint;
        }
        else
        {
          pFS->iCaretChar += nMaxCharToPrint;
          pFS->nCaretPosY++;
        }
      }
      if (pFS->nCaretPosY < 0)
      {
        LineUp(hwnd, pFS, vm);
        pFS->nCaretPosY++;
      }
    }
    SetCaretPos(pFS->nCaretPosX * pFS->nCharWidth, pFS->nCaretPosY * pFS->nCharHeight);
    break;

  case VK_NEXT:
    for (i = 0; i < nVisibleStrNum; ++i)
      LineDown(hwnd, pFS, vm);
    pFS->iCaretChar = pFS->iFirstChar + pFS->nHorShift;
    pFS->iCaretStr = pFS->iFirstStr;
    pFS->nCaretPosX = pFS->nCaretPosY = 0;
    SetCaretPos(pFS->nCaretPosX * pFS->nCharWidth, pFS->nCaretPosY * pFS->nCharHeight);
    break;

  case VK_PRIOR:
    for (i = 0; i < nVisibleStrNum; ++i)
      LineUp(hwnd, pFS, vm);
    pFS->iCaretChar = pFS->iFirstChar + pFS->nHorShift;
    pFS->iCaretStr = pFS->iFirstStr;
    pFS->nCaretPosX = pFS->nCaretPosY = 0;
    SetCaretPos(pFS->nCaretPosX * pFS->nCharWidth, pFS->nCaretPosY * pFS->nCharHeight);
    break;

  case VK_LEFT:
    nVisibleStrNum = pFS->nWindowHeight / pFS->nCharHeight;
    pFS->nCaretPosX--;
    pFS->iCaretChar--;
    if (vm == VM_NO_WRAPPING)
    {
      if (pFS->nCaretPosX < 0 && pFS->nHorShift == 0)
      {
        if (pFS->iCaretStr > 0) {
          pFS->iCaretStr--;
          pFS->iCaretChar = pFS->pStrLen[pFS->iCaretStr];
          if (pFS->pStrLen[pFS->iCaretStr] > nMaxCharToPrint)
            pFS->nCaretPosX = nMaxCharToPrint;
          else
            pFS->nCaretPosX = pFS->pStrLen[pFS->iCaretStr];
          if (pFS->nCaretPosX > nMaxCharToPrint)
            pFS->nHorShift = pFS->pStrLen[pFS->iCaretStr] - nMaxCharToPrint;
          InvalidateRect(hwnd, NULL, TRUE);
        }
        else {
          pFS->nCaretPosX++;
          pFS->iCaretChar++;
        }
        pFS->nCaretPosY--;
      }
      if (pFS->nCaretPosY < 0)
      {
        LineUp(hwnd, pFS, vm);
        pFS->nCaretPosY++;
      }
      if (pFS->nCaretPosX < 0 && pFS->nHorShift > 0)
      {
        SendMessage(hwnd, WM_HSCROLL, SB_LINELEFT, 0L);
        pFS->nCaretPosX++;
      }
    }
    else
    {
      if (pFS->iCaretChar < 0)
      {
        if (pFS->iCaretStr > 0) {
          pFS->iCaretStr--;
          pFS->iCaretChar = pFS->pStrLen[pFS->iCaretStr];
          pFS->nCaretPosX = pFS->pStrLen[pFS->iCaretStr] % nMaxCharToPrint;
          pFS->nCaretPosY--;
          InvalidateRect(hwnd, NULL, TRUE);
        }
        else {
          pFS->iCaretChar++;
          pFS->nCaretPosX++;
        }
      }
      else if (pFS->nCaretPosX < 0)
      {
        pFS->nCaretPosX = nMaxCharToPrint;
        pFS->nCaretPosY--;
      }
      if (pFS->nCaretPosY < 0)
      {
        LineUp(hwnd, pFS, vm);
        pFS->nCaretPosY = 0;
      }
    }
    SetCaretPos(pFS->nCaretPosX * pFS->nCharWidth, pFS->nCaretPosY * pFS->nCharHeight);
    break;

  case VK_RIGHT:
    pFS->nCaretPosX++;
    pFS->iCaretChar++;
    if (vm == VM_NO_WRAPPING)
    {
      if (pFS->nCaretPosX + pFS->nHorShift > pFS->pStrLen[pFS->iCaretStr])
      {
        if (pFS->iCaretStr < pFS->nStrNum - 1) {
          pFS->nCaretPosX = 0;
          pFS->nHorShift = 0;
          pFS->iCaretChar = 0;
          pFS->nCaretPosY++;
          pFS->iCaretStr++;
          InvalidateRect(hwnd, NULL, TRUE);
        }
        else {
          pFS->nCaretPosX--;
          pFS->iCaretChar--;
        }
      }
      if (pFS->nCaretPosY >= nVisibleStrNum)
      {
        LineDown(hwnd, pFS, vm);
        pFS->nCaretPosY--;
      }
      if (pFS->nCaretPosX * pFS->nCharWidth >= pFS->nWindowWidth)
      {
        SendMessage(hwnd, WM_HSCROLL, SB_LINERIGHT, 0L);
        pFS->nCaretPosX--;
      }
    }
    else
    {
      nMaxCharToPrint = pFS->nWindowWidth / pFS->nCharWidth;
      if (pFS->iCaretChar > pFS->pStrLen[pFS->iCaretStr])
      {
        if (pFS->iCaretStr < pFS->nStrNum - 1) {
          pFS->nCaretPosX = 0;
          pFS->nCaretPosY++;
          pFS->iCaretStr++;
          pFS->iCaretChar = 0;
        }
        else
        {
          pFS->iCaretChar--;
          pFS->nCaretPosX--;
        }
        InvalidateRect(hwnd, NULL, TRUE);
      }
      else if (pFS->nCaretPosX * pFS->nCharWidth >= pFS->nWindowWidth)
      {
        pFS->nCaretPosX = 0;
        pFS->nCaretPosY++;
      }
      if (pFS->nCaretPosY >= nVisibleStrNum)
      {
        LineDown(hwnd, pFS, vm);
        pFS->nCaretPosY--;
      }
    }
    SetCaretPos(pFS->nCaretPosX * pFS->nCharWidth, pFS->nCaretPosY * pFS->nCharHeight);
    break;
  }
}
