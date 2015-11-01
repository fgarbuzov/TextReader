#include "main.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
LPTSTR szClassName = _TEXT("ListerClass");

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC ;
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = _TEXT("MainMenu");
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

    /* Register the window class, and if it fails show message and quit the program */
    if (!RegisterClassEx (&wincl))
    {
      MessageBox(NULL, _TEXT("Failed to register class!"), _TEXT("Error"), MB_OK);
      return 0;
    }

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                                 /* Extended possibilites for variation */
           szClassName,                       /* Classname */
           _TEXT("Lister"),                   /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VSCROLL,  /* default window with scroll */
           0,                                 /* Left indent */
           0,                                 /* Top indent */
           800,                               /* The programs width */
           600,                               /* and height in pixels */
           HWND_DESKTOP,                      /* The window is a child-window to desktop */
           NULL,                              /* No menu */
           hThisInstance,                     /* Program Instance handler */
           lpszArgument                       /* No Window Creation data */
           );

    /* If creation of the window fails show message and quit the program */
    if (!hwnd) {
      MessageBox(NULL, _TEXT("Failed to create window!"), _TEXT("Error"), MB_OK);
      return 0;
    }

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  CREATESTRUCT * fileData;
  PAINTSTRUCT ps;
  HDC hdc;
	error_t error;
  static FILESTRUCT fs = { NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  static VIEWMODE vm = VM_NO_WRAPPING;

  /* handle the messages */
  switch (message)
  {
  case WM_CREATE:
    fileData = (CREATESTRUCT*)lParam;
    hdc = GetDC(hwnd);
    error = Create(hwnd, hdc, (LPTSTR)fileData->lpCreateParams, &fs);
    ReleaseDC(hwnd, hdc);
    if (error) PrintErrorMessage(error);
    break;

  case WM_PAINT:
    hdc = BeginPaint(hwnd, &ps);
    Paint(hwnd, hdc, &fs, vm);
    EndPaint(hwnd, &ps);
    break;

  case WM_SIZE:
    Size(hwnd, &fs, vm);
    break;

  case WM_VSCROLL:
    VScroll(hwnd, wParam, &fs, vm);
    break;

  case WM_HSCROLL:
    HScroll(hwnd, wParam, &fs);
    break;

  case WM_KEYDOWN:
    Keydown(hwnd, wParam, &fs, vm);
    break;

  case WM_COMMAND:
    error = Command(hwnd, wParam, &fs, &vm);
    if (error) PrintErrorMessage(error);
    break;

  case WM_SETFOCUS:
    CreateCaret(hwnd, (HBITMAP)NULL, fs.nCaretWidth, fs.nCaretHeight);
    SetCaretPos(fs.nCaretPosX * fs.nCharWidth, fs.nCaretPosY * fs.nCharHeight);
    ShowCaret(hwnd);
    break;

  case WM_KILLFOCUS:
    DestroyCaret();
    break;

  case WM_DESTROY:
    DestroyFile(&fs);
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc (hwnd, message, wParam, lParam);
  }

  return 0;
}
