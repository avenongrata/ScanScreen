#include <windows.h>
#include "qrscreensave.h"
#include <atlimage.h>
#include <chrono>
#include <locale>

//-----------------------------------------------------------------------------

/*#include <stdio.h>
#include <gdiplus.h>
#include <time.h>
#include <atlstr.h>
#include <Gdiplusimaging.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uuid.lib")*/

//-----------------------------------------------------------------------------

QrScreenSave::QrScreenSave()
    : mScreenPath("lol")         // globalInstallationPath
{}

//-----------------------------------------------------------------------------

QrScreenSave::QrScreenSave(std::string path)
    : mScreenPath(path)
{}

//-----------------------------------------------------------------------------

std::string QrScreenSave::takeScreen()
{
    int x1, y1, x2, y2, w, h;
    CImage image;
    std::string fileName;
    std::wstring fileNameUnicode;

    //-------------------------------------------------------------------------

    // get screen dimensions
    x1  = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y1  = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x2  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    y2  = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    w   = x2 - x1;
    h   = y2 - y1;

    //-------------------------------------------------------------------------

    // copy screen to bitmap
    HDC     hScreen = GetDC(NULL);
    HDC     hDC     = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL    bRet    = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    //-------------------------------------------------------------------------

    // save bitmap to clipboard
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();

    //-------------------------------------------------------------------------

    // save bitmap to png file
    image.Attach(hBitmap);
    fileName = mGetFileName();
    fileName = mScreenPath + "\\" + fileName;
    fileNameUnicode.assign(fileName.begin(), fileName.end());
    image.Save(fileNameUnicode.c_str(), Gdiplus::ImageFormatPNG);

    //-------------------------------------------------------------------------

    // clean up
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);

    //-------------------------------------------------------------------------

    return fileName;
}

//-----------------------------------------------------------------------------

std::string QrScreenSave::mGetFileName()
{
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTimeStd =
            std::chrono::system_clock::to_time_t(currentTime);
    std::string fileName = std::string(PROGRAM_NAME) + "_" +
            std::to_string(currentTimeStd) + ".png";

    return fileName;
}
