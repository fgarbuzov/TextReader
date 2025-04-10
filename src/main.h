#ifndef MAIN_HEADER_INCLUDED
#define MAIN_HEADER_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <sys\stat.h>
#include <assert.h>
#include <math.h>

#ifdef UNICODE
  #define _TEXT(x) L ## x
#else
  #define _TEXT(x) x
#endif

#ifndef _BOOL
  #define bool unsigned char
  #define false ((bool)0)
  #define true  ((bool)1)
#endif

typedef enum error {
	NO_ERR,
	ERR_OPEN_FILE,
	ERR_MEMORY_ALLOCATION,
  ERR_OPEN_DIALOG_BOX
} error_t;

typedef enum ViewMode {
	VM_NO_WRAPPING,
	VM_WRAPPING
} VIEWMODE;

typedef struct FileStruct {
  LPTSTR pBuffer;                           /* buffer to store the whole text */
  LPTSTR * ppString;                        /* array of pointers to beginnings of strings */
  int * pStrLen;                            /* array of string lengths */
  int nMaxStrLen;                           /* maximum string length */
  int nStrNum;                              /* number of strings in text */
  int nFileSize;                            /* size of file (in bytes) */
  int nWindowWidth, nWindowHeight;          /* window metric */
  int nCharWidth, nCharHeight;              /* text metric */
  int nHorShift;                            /* horizontal text shift due to scroll */
  int nVScrollPos;                          /* vertical scroll position */
  int nVScrollRange;                        /* vertical scroll range (max scroll position) */
  int iFirstStr, iFirstChar;                /* indicies of first printed string and char */
  int iLastStr, iLastChar;                  /* indicies of last printed string and char */
  int nCaretWidth, nCaretHeight;            /* caret metric */
  int nCaretPosX, nCaretPosY;               /* caret position */
  int iCaretStr, iCaretChar;                /* caret location in text */
} FILESTRUCT;

// file.c
error_t Create(HWND hwnd, HDC hdc, LPTSTR , FILESTRUCT * pFS);
error_t LoadFile(LPTSTR pFileName, FILESTRUCT * pFS);
void DestroyFile(FILESTRUCT * ptrFS);
void PrintErrorMessage(error_t);

// paint.c
void Paint(HWND hwnd, HDC hdc, FILESTRUCT * pFS, VIEWMODE ViewMode);

// size.c
void Size(HWND hwnd, FILESTRUCT * pFileStruct, VIEWMODE ViewMode);

// scroll.c
void VScroll(HWND hwnd, WPARAM wParam, FILESTRUCT * pFileStruct, VIEWMODE vm);
void HScroll(HWND hwnd, WPARAM wParam, FILESTRUCT * pFS);
void LineDown(HWND hwnd, FILESTRUCT * pFS, VIEWMODE vm);
void LineUp(HWND hwnd, FILESTRUCT * pFS, VIEWMODE vm);
void LineRight(HWND hwnd, FILESTRUCT * pFS);
void LineLeft(HWND hwnd, FILESTRUCT * pFS);
void SetScrolls(HWND hwnd, FILESTRUCT fs, VIEWMODE vm);

// keydown.c
void Keydown(HWND hwnd, WPARAM wParam, FILESTRUCT * pFS, VIEWMODE vm);
int CalculateStrNumWithWrap(FILESTRUCT fs);

// command.c
error_t Command(HWND hwnd, WPARAM wParam, FILESTRUCT * pFS, VIEWMODE * pVM);


#endif