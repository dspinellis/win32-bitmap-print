#include "resource.h"
#include <Winspool.h>
#include <CommDlg.h>
#include <fstream>


/*   variables  */
char szClassName[ ] = "Snoopy's Bitmap Printing Demo";
static OPENFILENAME ofn;
char szFileName[500]= "\0";
RECT rect;
HBITMAP hBitmap;
BITMAP bitmap;
int bxWidth, bxHeight, flag=0;
HDC hdc,hdcMem;
HMENU menu;
HPALETTE hpal;
int cxsize = 0, cxpage = 0;
int cysize = 0, cypage = 0;

/*  Declare  procedures  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
HDC GetPrinterDC (HWND Hwnd);
BOOL OpenFileDialog(HWND hwnd, LPTSTR pFileName ,LPTSTR pTitleName);
void InitialiseDialog(HWND hwnd);

/*Start of Program Entry point*/

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = NULL;
    wincl.hIconSm = NULL;
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(hMenu);                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               szClassName,       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, SW_MAXIMIZE);

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

    PAINTSTRUCT ps;
	

    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
        InitialiseDialog(hwnd);
        menu = GetMenu(hwnd);
        return 0;

    case WM_QUERYNEWPALETTE:
        if (!hpal)
            return FALSE;
        hdc = GetDC(hwnd);
        SelectPalette (hdc, hpal, FALSE);
        RealizePalette (hdc);
        InvalidateRect(hwnd,NULL,TRUE);
        ReleaseDC(hwnd,hdc);
        return TRUE;

    case WM_PALETTECHANGED:
        if (!hpal || (HWND)wParam == hwnd)
            break;
        hdc = GetDC(hwnd);
        SelectPalette (hdc, hpal, FALSE);
        RealizePalette (hdc);
        UpdateColors(hdc);
        ReleaseDC(hwnd,hdc);
        break;




    case WM_COMMAND:
        switch LOWORD(wParam)
        {

        case IDM_OPEN_BM:
        {
            
            OpenFileDialog(hwnd, szFileName, "Open a Bitmap File.");

            if(szFileName)
            {
                ZeroMemory(&hBitmap, sizeof(HBITMAP));
                hBitmap = (HBITMAP)LoadImage(NULL,szFileName,IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION|LR_DEFAULTSIZE|LR_LOADFROMFILE|LR_VGACOLOR);
                if(hBitmap)
                {
                    EnableMenuItem(menu, IDM_SAVE_BM, MF_ENABLED);
                    EnableMenuItem(menu, IDM_PRINT_BM, MF_ENABLED);

                    cxpage = GetDeviceCaps (hdc, HORZRES);
                    cypage = GetDeviceCaps (hdc, VERTRES);
                    GetObject(hBitmap,sizeof(BITMAP),&bitmap);
                    bxWidth = bitmap.bmWidth;
                    bxHeight = bitmap.bmHeight;

                    rect.left = 0;
                    rect.top =0;
                    rect.right = (long)&cxpage;
                    rect.bottom = (long)&cypage;


                    InvalidateRect(hwnd,&rect,true);
                }
            }
            return 0;
		}

            case IDM_PRINT_BM:
            {
                DOCINFO di= { sizeof (DOCINFO), TEXT ("Printing Picture...")};

                HDC prn = GetPrinterDC(hwnd);
                hdcMem = CreateCompatibleDC(prn);
                HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

                cxpage = GetDeviceCaps (prn, HORZRES);
                cypage = GetDeviceCaps (prn, VERTRES);
                StartDoc (prn, &di);
                StartPage (prn) ;
                SetMapMode (prn, MM_ISOTROPIC);
                SetWindowExtEx(prn, cxpage,cypage, NULL);
                SetViewportExtEx(prn, cxpage, cypage,NULL);

                SetViewportOrgEx(prn, 0, 0, NULL);
                StretchBlt(prn, 0, 0, cxpage, cypage, hdcMem, 0, 0,bxWidth,bxHeight, SRCCOPY);
                EndPage (prn);
                EndDoc(prn);
                DeleteDC(prn);
                SelectObject(hdcMem, hbmOld);
                DeleteDC(hdcMem);

                return 0;
            }

            case IDM_EXIT:
            {
                PostQuitMessage(0);
                return 0;
            }
       



        break;
        }
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        hdcMem = CreateCompatibleDC(hdc);

        SelectObject(hdcMem, hBitmap);


        cxpage = GetDeviceCaps (hdc, HORZRES);
        cypage = GetDeviceCaps (hdc, VERTRES);

        SetMapMode (hdc, MM_ISOTROPIC);
        SetWindowExtEx(hdc, cxpage,cypage, NULL);
        SetViewportExtEx(hdc, cxsize, cysize,NULL);

        SetViewportOrgEx(hdc, 0, 0, NULL);

        SetStretchBltMode(hdc,COLORONCOLOR);

        StretchBlt(hdc, 0, 0, bxWidth, bxHeight, hdcMem, 0, 0,bxWidth,bxHeight, SRCCOPY);

        DeleteDC(hdcMem);
        EndPaint(hwnd, &ps);
        DeleteDC(hdcMem);
        return 0;

    case  WM_SIZE:

        cxsize = LOWORD(lParam);
        cysize = HIWORD(lParam);
        rect.right = cxsize;
        rect.bottom = cysize;

        InvalidateRect(hwnd, &rect, true);
        return 0;




    case WM_DESTROY:
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);

    }
    return 0;

}

HDC GetPrinterDC (HWND Hwnd)
{

// Initialize a PRINTDLG structure's size and set the PD_RETURNDC flag set the Owner flag to hwnd.
// The PD_RETURNDC flag tells the dialog to return a printer device context.
    HDC hdc;
    PRINTDLG pd = {0};
    pd.lStructSize = sizeof( pd );
    pd.hwndOwner = Hwnd;
    pd.Flags = PD_RETURNDC;

// Retrieves the printer DC
    PrintDlg(&pd);
    hdc =pd.hDC;
    return hdc ;


}

BOOL OpenFileDialog(HWND hwnd, LPTSTR pFileName ,LPTSTR pTitleName)

{
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hInstance = GetModuleHandle(NULL);
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;

    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pFileName;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrTitle = pTitleName;
    ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
    ofn.lpstrFilter = TEXT("Bitmap Files (*.bmp)\0*.bmp\0\0");



    return GetOpenFileName(&ofn);
}

void InitialiseDialog(HWND hwnd)
{
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = NULL;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = 500;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = 0;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    ofn.lCustData = 0L;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
}
