#include "qrscanscreen.h"
#include <QApplication>
#include <iostream>
//#include <Windows.h>
#include "qrpicture.h"
#include "qrserver.h"
#include "QrCode.h"
#include <fstream>
#include "qrscreensave.h"
#include <filesystem>


bool    isForegroundProcess(DWORD);
bool    preventMultipleInstances();
int     startUi(std::string, int, char **);
LRESULT CALLBACK ll_keyboard_proc(int, WPARAM, LPARAM);     // function to process key-d input
std::string getPath();

int     globalArgc;
char    **globalArgv;
HWND    globalHApp;
std::string globalPathToFile;
const char globalSSDirectoryName[] = "Screenshots";
const char globalLogoDirectoryName[] = "Logo";


int main(int argc, char *argv[])
{
    // When user can't create mutex - program is already running
    if (!preventMultipleInstances()) {
        MessageBox(NULL, TEXT("Program is already running"), TEXT("ScanScreen project"), MB_OK);
        return 0;
    } else {
        //MessageBox(NULL, TEXT("Program works!"), TEXT("ScanScreen project"), MB_OK);
        ::globalPathToFile = getPath();
        CreateDirectoryA(::globalSSDirectoryName, NULL);
        CreateDirectoryA(::globalLogoDirectoryName, NULL);
    }

    HHOOK ll_test = NULL;
    MSG msg;
    BOOL bRet;
    ::globalArgc = argc;
    ::globalArgv = argv;

    //FreeConsole();

    ll_test = SetWindowsHookEx(WH_KEYBOARD_LL, ll_keyboard_proc, NULL, NULL);
    if (ll_test == NULL) {
        std::cout << "Error: can't get hook" << std::endl;
        std::cout << GetLastError() << std::endl;
        return 1;
    }

    while ((bRet = GetMessage(&msg, NULL, NULL, NULL)) != 0) {
        //TranslateMessage(&msg);   // for what is used ?
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(ll_test);

    return 0;
}

LRESULT CALLBACK ll_keyboard_proc(int code, WPARAM wParam, LPARAM lParam)
{
    static int keyCounter = 0;
    static int workCounter = 0;
    KBDLLHOOKSTRUCT *key;
    const int staticKey = VK_SNAPSHOT;              // print-screen key
    static QrServer *firstTestServer = nullptr;

    //CloseWindow(GetActiveWindow());

    if (code < 0)
        goto end;

    key = (KBDLLHOOKSTRUCT *)lParam;
    if (key->vkCode != staticKey)
        goto end;

    if (wParam == WM_KEYDOWN) {  // may be add system-key also
        keyCounter++;
        if (!workCounter) workCounter = 1;
        goto end;
    }

    if (wParam == WM_KEYUP) {
        if (keyCounter) {
            if (workCounter == 1)      // first pressed
                workCounter = 2;
            else if (workCounter == 2) {

                /*
                 * When user presses again Hot-key than we don't show window
                 * on top, we just update Browser page.
                 */

                QrScreenSave firstTestScreen{::globalPathToFile + "\\" + ::globalSSDirectoryName};
                std::string fileName;
                std::string photoPath;
                std::string firstTestQr;

                fileName = firstTestScreen.takeScreen();      // make screenshot and return file name
                photoPath = fileName;       //::globalPathToFile + "\\" + ::globalDirectoryName + "\\" + fileName;
                firstTestServer->addScreenShot(photoPath);
                goto end;       // delete later

                AttachThreadInput(
                    GetWindowThreadProcessId(GetForegroundWindow(), NULL),
                  GetCurrentThreadId(), TRUE
               );

                AllowSetForegroundWindow(ASFW_ANY);
                SetForegroundWindow(::globalHApp);
                ShowWindow(::globalHApp, SW_RESTORE);      // when window was minimized

                AttachThreadInput(
                    GetWindowThreadProcessId(GetForegroundWindow(), NULL),
                   GetCurrentThreadId(), FALSE
                );

                /*
                 * MessageBox(NULL, TEXT("Again pressed"), TEXT("Disable Low-Level Keys"), MB_OK);
                 * SetActiveWindow(globalHApp);
                 * SetFocus(globalHApp);
                 * std::cout << "PROC: " << GetCurrentThreadId() << std::endl;
                 * std::cout << "FOREGROUND: ll2 " << IsForegroundProcess( GetCurrentProcessId()) << std::endl;
                 * std::cout << "Glob: " << globalHApp << std::endl;
                 * std::cout << "Win:  " << GetForegroundWindow() << std::endl;
                 * SetWindowPos(globalHApp, HWND_TOP, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                 */

                goto end;
            }

            QrScreenSave firstTestScreen{::globalPathToFile + "\\" + ::globalSSDirectoryName};
            std::string fileName;
            std::string photoPath;
            std::string firstTestQr;

            fileName = firstTestScreen.takeScreen();      // make screenshot and return file name
            photoPath = fileName;       //::globalPathToFile + "\\" + ::globalDirectoryName + "\\" + fileName;
            //firstTestServer = QrServer(photoPath);
            firstTestServer = new QrServer(photoPath);
             // testing, QrServer firstTestServer{photoPath};
            if (firstTestServer->startQrServer())      // start server
                return 1;

            firstTestQr = firstTestServer->getUrlMultipleSS();     // firstTestQr = firstTestServer.getUrlDownloadLink();
            startUi(firstTestQr, 1, NULL);
            //std::string testFile;
            //testFile = "GET /" + firstTestServer.getFileName()  +" HTTP/1.1";
            std::cout << "Stop server: " << firstTestServer->stopServer() << std::endl;
            firstTestServer->waitTcpThread();
            keyCounter = 0;
            workCounter = 0;
            delete firstTestServer;       // call it ??

            // need to delete file
            std::cout << photoPath.c_str() << std::endl;
            // need to get array of pathes and delete in cycle
            std::cout << "File delete: " << DeleteFileA(photoPath.c_str()) << std::endl;
        }
    }

end:
    return CallNextHookEx(NULL, code, wParam, lParam);
}

int startUi(std::string qr, int argc, char *argv[])
{
    static int flag = 0;
    QApplication a(argc, argv);
    QrScanScreen w{qr};
    ::globalHApp = (HWND)w.winId();

    w.show();
    if (!flag) {        // when the Q-window has already been shown - we don't need to make it topmost, makes it Windows
        SetWindowPos(::globalHApp, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
        SetWindowPos(::globalHApp, HWND_NOTOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
        flag = 1;
    }

    return a.exec();
}


bool IsForegroundProcess(DWORD pid)
{
   HWND hwnd = GetForegroundWindow();
   if (hwnd == NULL) return false;

   DWORD foregroundPid;
   if (GetWindowThreadProcessId(hwnd, &foregroundPid) == 0) return false;

   return (foregroundPid == pid);
}


bool preventMultipleInstances()
{
    const char programName[] = "ScanScreen";

    CreateMutexA(NULL, FALSE, programName);
    // Check if mutex is created succesfully
    switch (GetLastError())
    {
    case ERROR_SUCCESS:
        // Mutex created successfully. There is no instance running
        break;

    case ERROR_ALREADY_EXISTS:
        // Mutex already exists so there is a running instance of our app.
        return false;

    default:
        // Failed to create mutex by unknown reason
        return false;
    }

    return true;
}

std::string getPath()
{
    namespace fs = std::experimental::filesystem;
    return fs::current_path().string();
}
