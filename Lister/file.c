#include "main.h"

/** Create
DESCRIPTION		Process message WM_CREATE. Open file, read it into buffer and fill file structure.
PARAM[IN]		  HWND hwnd
              HDC hdc
              CREATESTRUCT * fileData - information about file which should be opened
PARAM[IN/OUT]	FILESTRUCT * pFS - pointer to file structure
RETURNS			  error_t error - error code
*/
error_t Create(HWND hwnd, HDC hdc, LPTSTR fName, FILESTRUCT * pFS)
{
  error_t error;
  TEXTMETRIC TextMetric;
  RECT rect;

  SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

  GetTextMetrics(hdc, &TextMetric);
  pFS->nCharHeight = TextMetric.tmHeight;
  pFS->nCharWidth = TextMetric.tmAveCharWidth;

  GetClientRect(hwnd, &rect);
  pFS->nWindowWidth = rect.right - rect.left;
  pFS->nWindowHeight = rect.bottom - rect.top;

  pFS->nCaretHeight = pFS->nCharHeight;
  pFS->nCaretWidth = 1;
  pFS->nCaretPosX = 0;
  pFS->nCaretPosY = 0;

  error = LoadFile(fName, pFS);

  return error;
}


/** LoadFile
DESCRIPTION		Read file into buffer in file structure
PARAM[IN]		  LPTSTR pFileName - name of file to open
PARAM[IN/OUT]	FILESTRUCT * pFS - pointer to file structure
RETURNS			  error_t error - error code
*/
error_t LoadFile(LPTSTR pFileName, FILESTRUCT * pFS)
{
  FILE * file = NULL;
  LPTSTR pBuffer;
  LPTSTR * ppString;
  int * pStrLen;
  int nFileSize, nStrNum, nMaxStrLen;
  int i, iStr;
  struct stat stFileStat;

#ifdef _MSC_VER
  fopen_s(&file, pFileName, "rb");
#else
  file = fopen(pFileName, "rb");
#endif
  if (file == NULL)
    return ERR_OPEN_FILE;

  // find out size of file
  fstat(_fileno(file), &stFileStat);
  nFileSize = stFileStat.st_size;

  // allocate memory for the text in file and read the text
  pBuffer = (LPTSTR)calloc(nFileSize, sizeof(TCHAR));
  if (pBuffer == NULL)
    return ERR_MEMORY_ALLOCATION;
  fread(pBuffer, sizeof(TCHAR), nFileSize, file);

  // calculate number of strings
  nStrNum = 1;
  for (i = 0; i < nFileSize; ++i)
    if (pBuffer[i] == '\n')
      nStrNum++;

  // allocate memory for array of strings and array of string lengths
  ppString = (LPTSTR*)calloc(nStrNum, sizeof(LPTSTR));
  if (ppString == NULL)
  {
    free(pBuffer);
    return ERR_MEMORY_ALLOCATION;
  }
  pStrLen = (int*)calloc(nStrNum, sizeof(int));
  if (pStrLen == NULL)
  {
    free(pBuffer);
    free(pStrLen);
    return ERR_MEMORY_ALLOCATION;
  }

  // fill array of strings and array of string lengths
  iStr = 0;
  nMaxStrLen = 0;
  ppString[0] = pBuffer;
  for (i = 0; i < nFileSize; ++i)
  {
    assert(iStr <= nStrNum);
    if (pBuffer[i] == '\n')
    {
      assert(iStr < nStrNum);
      pStrLen[iStr] = pBuffer + i - ppString[iStr];
      if (pStrLen[iStr] > nMaxStrLen)
        nMaxStrLen = pStrLen[iStr];
      ++iStr;
      if (iStr < nStrNum)
        ppString[iStr] = pBuffer + i + 1;
    }
  }
  pStrLen[nStrNum - 1] = pBuffer + nFileSize - ppString[nStrNum - 1];
  if (pStrLen[nStrNum - 1] > nMaxStrLen)
    nMaxStrLen = pStrLen[nStrNum - 1];

  // fill the file structure
  pFS->pBuffer    = pBuffer;
  pFS->ppString   = ppString;
  pFS->pStrLen    = pStrLen;
  pFS->nMaxStrLen = nMaxStrLen;
  pFS->nStrNum    = nStrNum;
  pFS->nFileSize  = nFileSize;
  pFS->nCaretPosX = pFS->nCaretPosY = 0;
  pFS->iCaretStr  = pFS->iCaretChar = 0;
  SetCaretPos(0, 0);
  fclose(file);
  return NO_ERR;
}

/** DestroyFile
DESCRIPTION		Process message WM_DESTROY. Free all allocated memory.
PARAM[IN/OUT]	FILESTRUCT * pFS - pointer to file structure
*/
void DestroyFile(FILESTRUCT * pFS)
{
  if (pFS->pBuffer != NULL) {
    free(pFS->pBuffer);
    pFS->pBuffer = NULL;
  }
  if (pFS->ppString != NULL) {
    free(pFS->ppString);
    pFS->ppString = NULL;
  }
  if (pFS->pStrLen != NULL) {
    free(pFS->pStrLen);
    pFS->pStrLen = NULL;
  }
}

/** void PrintErrorMessage
DESCRIPTION		Show MessageBox with short information about error
PARAM[IN]	  	error_t error - error code
*/
void PrintErrorMessage(error_t error)
{
  switch (error) {
  case ERR_OPEN_FILE:			    MessageBox(NULL, _TEXT("Failed to open file!"), _TEXT("Error"), MB_OK); break;
  case ERR_MEMORY_ALLOCATION: MessageBox(NULL, _TEXT("Failed to allocate memory!"), _TEXT("Error"), MB_OK); break;
  case ERR_OPEN_DIALOG_BOX:   MessageBox(NULL, _TEXT("Failed to display open dialog box!"), _TEXT("Error"), MB_OK); break;
  }
}
