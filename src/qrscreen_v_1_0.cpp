#include <iostream>
//#include <Windows.h>
#include "qrpicture.h"
#include "qrserver.h"
#include "QrCode.h"
#include <fstream>
#include <future>

#include <qrhttpserver.h>
#include <qrmacros.h>
#include <fstream>
#include <sstream>
#include <QCryptographicHash>


// this file I'll use for testing multithreading



int yymain()
{
    std::map<SOCKET, int> mSocketArray;
    SOCKET sock = 123;

    auto search = mSocketArray.find(sock);
    if (search == mSocketArray.end())
        mSocketArray.insert({sock, 0});     // wtf is going on ?
    mSocketArray.insert({(SOCKET) 466, 1});
    mSocketArray[(SOCKET) 456] = 0;

    for (auto it = mSocketArray.begin(); it != mSocketArray.end(); ++it)
        std::cout << it->first << ", " << it->second << '\n';






    return 0;
}


/*using namespace std;

LRESULT CALLBACK ll_keyboard_proc(int, WPARAM, LPARAM); // function to process key-d input

int main()
{
    HHOOK ll_test = NULL;
    MSG msg;

    //FreeConsole();

    ll_test = SetWindowsHookEx(WH_KEYBOARD_LL, ll_keyboard_proc, NULL, NULL); // just test it bro
    if (ll_test == NULL) {
        cout << "Error: can't get hook" << endl;
        cout << GetLastError() << endl;
        return 1;
    }

    // When we press a key and get a window with message
    // than we press again this key -> we must close window and
    // create new window.
    //MessageBox(NULL,
    //TEXT("Click \"Ok\" to re-enable these keys."),
    //TEXT("Disable Low-Level Keys"),
    //MB_OK);

    while (!GetMessage(&msg, NULL, NULL, NULL))
        ;

    UnhookWindowsHookEx(ll_test);
    //cin.get();

    return 0;
}

LRESULT CALLBACK ll_keyboard_proc(int code, WPARAM wParam, LPARAM lParam)
{
    static int f12_down = 0;
    KBDLLHOOKSTRUCT *key;

    if (code < 0)
        goto end;

    key = (KBDLLHOOKSTRUCT *)lParam;
    if (key->vkCode != 0x4B)  //VK_F12
        goto end;

    if (wParam == WM_KEYDOWN) {  // mb add sys also
        f12_down++;
        goto end;
    }

    if (wParam == WM_KEYUP) {
        if (f12_down) {
            //MessageBox(NULL, TEXT("F12 was pressed"), TEXT("Disable Low-Level Keys"), MB_OK);
            f12_down = 0;
        }
    }

end:
    return CallNextHookEx(NULL, code, wParam, lParam);
}*/





/*
 * #include "qrserver.h"
#include <iostream>
#include "qrmacros.h"
#include <fstream>
#include <thread>
#include <sstream>
#include <QNetworkInterface>
#include <qrhttpserver.h>

//#include <winsock2.h>
//#include <iphlpapi.h>
//#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "IPHLPAPI.lib")

QrServer::QrServer()
    : mCurServerIp("none"), mPhotoPath("none"), mServerPort(9918),
      mQrInterface(nullptr), mTcpServer(nullptr)
{std::cout << "From constructor!: " << mTcpServer << std::endl;}

QrServer::QrServer(std::string ip, int port)
    : mCurServerIp(ip), mPhotoPath("none"), mServerPort(port),
      mQrInterface(nullptr), mTcpServer(nullptr)
{std::cout << "From constructor!: " << mTcpServer << std::endl;}

QrServer::QrServer(std::string path)
    : mCurServerIp("none"), mPhotoPath(path), mServerPort(9918),
      mQrInterface(nullptr), mTcpServer(nullptr)
{std::cout << "From constructor!: " << mTcpServer << std::endl;}

QrServer::~QrServer()
{
    QR_INTERFACE temp = nullptr;

    // if found IP-interfaces
    while (mQrInterface) {
        temp = mQrInterface->Next;
        delete mQrInterface;
        mQrInterface = temp;
    }

    std::cout << "From destructor!: " << mTcpServer << std::endl;

    if (mTcpServer != nullptr)
        delete mTcpServer;
}

int QrServer::mGetIpAddress()  // rewrite this function to get current ip address
{
    /*
     * I need to rewrite this function, because i don't have unique way how to determine
     * current IP-interface (including IP-address) on Windows. I can get all interfaces,
     * I can try to sort them, but now I still can't start Qr-Server in 100/100 cases.
     *
     * What I can do:
     * - ask on stackoverflow
     * - read open-source code of programms
     * ///!!!

    QR_INTERFACE first, add;
    first = add = nullptr;
    std::string ip;

    foreach(QNetworkInterface tInterface, QNetworkInterface::allInterfaces())       // sort IP-interfaces to get current
    {
          if (tInterface.flags().testFlag(QNetworkInterface::IsRunning) && !tInterface.flags().testFlag(QNetworkInterface::IsLoopBack))
              foreach (QNetworkAddressEntry tEntry, tInterface.addressEntries()) {
                  if (tInterface.hardwareAddress() != "00:00:00:00:00:00" && tEntry.ip().toString().contains(".")) {
                      ip = tEntry.ip().toString().toStdString();

                      // fill structure with IP-s
                      add = new(QrPcInterface);
                      add->ip = ip;
                      add->Next = nullptr;

                      if (first == nullptr)
                          mQrInterface = first = add;
                      else
                          mQrInterface = mQrInterface->Next = add;
                  }
              }
    }
    mQrInterface = first;
    return (mQrInterface == nullptr) ? 1 : 0;
}

int QrServer::mChooseIpAddress()
{
    if (mPhotoPath == "none") {
        PRINTF_D("File path not specified\n");
        return 1;
    }

    while (mQrInterface) {
        mTcpServer = new QrTcpServer{mQrInterface->ip, mServerPort, mPhotoPath};        //

        if (mTcpServer->QrTcpServerStart()) {
            PRINTF_D("Can't create server on IP: %s\n", mQrInterface->ip.c_str());
            delete mTcpServer;
            mQrInterface = mQrInterface->Next;
        } else {
            PRINTF_D("Server is working on IP: %s\n", mQrInterface->ip.c_str());
            mCurServerIp = mQrInterface->ip;
            return 0;
        }
    }
    return 1;
}

int QrServer::startQrServer()
{
    if (mGetIpAddress()) {
        PRINTF_D("Can't determine any IP-address on local machine\n");
        return 1;
    }

    if (mChooseIpAddress()) {    // check all possible IP
        PRINTF_D("Can't create server on any IP-address\n");
        return 1;
    }

    return 0;
}

std::string QrServer::getUrlDownloadLink()          // rewrite it later
{
    std::size_t found = mPhotoPath.find_last_of("\\");
    mUrlToString = "http://" + mCurServerIp + ":" + std::to_string(mServerPort)
            + "/" + mPhotoPath.substr(found+1);

    return mUrlToString;
}

std::string QrServer::getUrlMultipleSS()
{
    mUrlToString = "http://" + mCurServerIp + ":" + std::to_string(mServerPort);

    return mUrlToString;
}


// ##################################################################
// ##################################################################


QrTcpServer::QrTcpServer()
    : mIp("none"), mPathToFile("none"), mPort(9918),
      mQrTcpSockfd(INVALID_SOCKET), mAcceptThread(nullptr),
      mFdArray(nullptr)
{}

QrTcpServer::QrTcpServer(string ip, int port, string pathToFile)
    : mIp(ip), mPathToFile(pathToFile), mPort(port),
      mQrTcpSockfd(INVALID_SOCKET), mAcceptThread(nullptr),
      mFdArray(nullptr)
{}

QrTcpServer::~QrTcpServer()
{
    if (mAcceptThread != nullptr)
        delete mAcceptThread;

    if (mQrTcpSockfd != INVALID_SOCKET)     // When it is already closed ?
        closesocket(mQrTcpSockfd);

    std::cout << "Destroyed\n";     // delete it later
}

SOCKET QrTcpServer::mCreateSocket(int af, int type, int protocol)
{
    WSAData w_data;

    WSAStartup(MAKEWORD(2, 2), &w_data);        // choose version WinSocket
    return socket(af, type, protocol);       // can do it without bracket ?

   /* if ((sockfd = socket(af, type, protocol)) == INVALID_SOCKET)
        return -1;
    else {
        mQrTcpSockfd = sockfd;
        return 0;
    } * /// !!!!
}


void mAcceptFunction(QrTcpServer & obj)
{
   /* SOCKET newSocket;
    int REQUEST_SIZE = 2048;
    char request[2048];
    int flag = 1;

    if ((newSocket = accept(obj.mQrTcpSockfd, NULL, NULL)) < 0)
        PRINTF_D("Can't accept client\n");
    else {  // rewrite for asynchronus
        QrHttpServer httpServer{newSocket, obj.mPathToFile};
        fd_set fds;
        while (flag) {
            FD_ZERO(&fds);
            FD_SET(newSocket, &fds);
            select(NULL, &fds, NULL, NULL, NULL);
            recv(newSocket, request, REQUEST_SIZE, 0);
            switch (httpServer.processRequest(request))
            {
            case httpServer.QR_HTTP_SUCCESS:
                break;

            case httpServer.QR_HTTP_ERROR:
                PRINTF_D("Error in http server\n");
                closesocket(newSocket);
                //std::terminate();       // Is it legal ? May be just return from function ?
                flag = 0;
                break;

            case httpServer.QR_HTTP_EXIT:
                PRINTF_D("File was downloaded. Stop http-server\n");
                closesocket(newSocket);
                //std::terminate();
                flag = 0;
                break;

            default:
                PRINTF_D("Unexpected error\n");
                closesocket(newSocket);
                //std::terminate();
                flag = 0;
                break;
            }
        }
    }* /// !!!!

    int ret;
    WSAPOLLFD socks[obj.mSocketCount];
    SOCKET newSocket;
    int REQUEST_SIZE = 2048;
    char request[2048];

    obj.mFdArray = socks;        // to have chance close it from
    for (int i = 0; i < obj.mSocketCount; i++)
        socks[i].fd = (SOCKET)-1, socks[i].revents = 0;       // to determine in future which structure can be used

    socks[0].fd = obj.mQrTcpSockfd;        // listen main socket
    socks[0].events = POLLRDNORM;
    socks[0].revents = 0;
    int count = 0;

    while (true) {
        ret = WSAPoll(socks, obj.mSocketCount, -1);
        std::cout << "Count: " << count++ << " " << ret << std::endl;
        if (ret == SOCKET_ERROR) {         // error on socket occured
            for (int i = 0; i < obj.mSocketCount; i++) {
                closesocket(socks[i].fd);
                socks[i].fd = -1;
                socks[i].revents = 0;
            }
            break;
        }

        if (ret == 0) {
            std::cout << "unexpected ERROR IN POLL\n";
            break;
        } else
            for (int i = 0; i < obj.mSocketCount; i++) {
                if (socks[i].fd == (SOCKET)-1) {      // pass empty structures
                    socks[i].revents = 0;
                    continue;
                }

                if (socks[i].revents & POLLIN) {     // accept some socket data
                    std::cout << i;
                    std::cout << "POLLIN\n";
                    if (socks[i].fd == obj.mQrTcpSockfd) {
                        if ((newSocket = accept(obj.mQrTcpSockfd, NULL, NULL)) < 0)
                            PRINTF_D("Can't accept client\n");
                        else {
                            std::cout << "NEW CHECK NEW SOCKET: " << newSocket << std::endl;
                            for (int i = 0; i < obj.mSocketCount; i++)
                                if (socks[i].fd == (SOCKET)-1) {
                                    socks[i].fd = newSocket;
                                    socks[i].events = POLLIN;
                                    socks[i].revents = 0;
                                    break;
                                }
                        }
                    } else {
                        recv(socks[i].fd, request, REQUEST_SIZE, 0);
                        QrHttpServer httpServer{socks[i].fd, obj.mPathToFile};      // not & ???????
                        switch (httpServer.processRequest(request))
                        {
                        case httpServer.QR_HTTP_SUCCESS:
                            break;

                        case httpServer.QR_HTTP_ERROR:
                            PRINTF_D("Error in http server\n");
                            for (int i = 0; i < obj.mSocketCount; i++) {
                                closesocket(socks[i].fd);
                                socks[i].fd = -1;
                                socks[i].revents = 0;
                            }

                            // need to free POLL structure !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                            //std::terminate();       // Is it legal ? May be just return from function ?
                            break;

                        case httpServer.QR_HTTP_EXIT:
                            PRINTF_D("File was downloaded. Stop http-server\n");
                            for (int i = 0; i < obj.mSocketCount; i++) {
                                closesocket(socks[i].fd);
                                socks[i].fd = -1;
                                socks[i].revents = 0;
                            }

                            //std::terminate();
                            break;

                        default:
                            PRINTF_D("Unexpected error\n");
                            for (int i = 0; i < obj.mSocketCount; i++) {
                                closesocket(socks[i].fd);
                                socks[i].fd = -1;
                                socks[i].revents = 0;
                            }

                            //std::terminate();
                            break;
                        }
                    }
                    //socks[i].revents = 0;
                    //continue;
                } else {
                    std::cout << i << "OTHER\n";
                    std::cout << "REVENTS" << socks[i].revents << std::endl;
                    if (socks[i].revents & POLLERR)
                        std::cout << "POLLERR\n";
                    else if (socks[i].revents & POLLHUP)
                        std::cout << "POLLHUP\n";
                    else if (socks[i].revents & POLLNVAL)
                        std::cout << "POLLNVAL\n";
                    else if (socks[i].revents & POLLPRI)
                        std::cout << "POLLPRI\n";
                    else if (socks[i].revents & POLLRDBAND)
                        std::cout << "POLLRDBAND\n";
                    else if (socks[i].revents & POLLWRNORM)
                        std::cout << "POLLWRNORM\n";
                    else if (socks[i].revents & POLLRDNORM)
                        std::cout << "POLLRDNORM\n";
                    closesocket(socks[i].fd);
                    socks[i].fd = -1;
                    socks[i].revents = 0;
                }
                //socks[i].revents = 0;
            }
    }
    //obj.mFdArray = nullptr;
}





//void QrTcpServer::mClientServer()
//{
    /*
     * In this function i will wait for a notification from Q-window thread.
     * When i get such notification i will send to listening Server-socket
     * a data, which will notify Server-socket that it must stop to listen.
     * /// !!!!



//}

int QrTcpServer::QrTcpServerStart()
{
    SOCKADDR_IN addr;
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(mPort);

   // mAddr = new SOCKADDR_IN;

   // mAddr->sin_addr.S_un.S_addr = INADDR_ANY;
   // mAddr->sin_family = AF_INET;
   // mAddr->sin_port = htons(mPort);

    if ((mQrTcpSockfd = mCreateSocket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        PRINTF_D("Can't create server socket\n");
        return 1;
    }

    if (bind(mQrTcpSockfd, reinterpret_cast<sockaddr *> (&addr), sizeof(addr)) < 0) {
        PRINTF_D("Can't bind server\n");
        closesocket(mQrTcpSockfd);
        return 1;
    }

    if (listen(mQrTcpSockfd, 3) < 0) {
        PRINTF_D("Can't listen\n");
        closesocket(mQrTcpSockfd);
        return 1;
    }

    // There is big trouble. When i create a thread to handle accept-fucntion
    // then it can be an error in this thread. So i have error in accept-thread
    // and i have OK message in main-thread-> programm still works good.
    // In such situation a need to send message from accept-thread to main-threrad
    // that there is a big problem and main-thread must handle this error -> don't stop
    // programm, because it't independent programm which has to not stop in such
    // situations. Thins about it!!!!

    // here need to start new thread and return that all is OK and server is listeting

    //std::thread acceptThread(acceptFunction, *this, qrTcpSockfd, reinterpret_cast<sockaddr *> (&addr), &addrlen);
    mAcceptThread = new std::thread(mAcceptFunction, std::ref(*this)); // transfer malloced data
    //acceptThread.join();
   // acceptThread.detach(); // need to give info from acceptThread to main thread, that there were an error!!!!

    return 0;
}

int QrTcpServer::stopServer(std::string msg)
{
    /*
     * This function I need if i wanna stop Server from Main-thread
     * when User closes Qr-code-Window without visiting site. So
     * I need to create Qr-client, which will immitate User's
     * downloading file.
     */

    /*SOCKET clientSocket;
    sockaddr_in clientService;
    char buf[1024];

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        return -1;

    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(mIp.c_str());
    clientService.sin_port = htons(mPort);

    if (connect(clientSocket, (SOCKADDR *) &clientService, sizeof (clientService))) {
        closesocket(clientSocket);
        return -1;
    } else {
        send(clientSocket, msg.c_str(), msg.length(), 0);
        recv(clientSocket , buf, 1024, 0);
        std::cout << "FROM CLIENT: " << buf << std::endl;
    }* /// !!!!

    if (mFdArray != nullptr)
        for (int i = 0; i < mSocketCount; i++) {
            closesocket(mFdArray[i].fd);
            mFdArray[i].fd = -1;
            mFdArray[i].revents = 0;
        }
    else
        return 1;

    return 0;
}


//##############################################################################
//##############################################################################
//##############################################################################
// Old functions that I do not want to lose sight of


/*int QrServer::getIpAddress()
{
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    QR_INTERFACE first, last;
    first = last = nullptr;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        PRINTF_D("Error allocating memory needed to call GetAdaptersinfo\n");
        return 1;
    }

    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            PRINTF_D("Error allocating memory needed to call GetAdaptersinfo\n");
            return 1;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            if (!first) {  // CAN REWRITE IT! THINK ABOUT IT
                first = static_cast<QR_INTERFACE> (new QrPcInterface);
                first->ip = pAdapter->IpAddressList.IpAddress.String;
                first->mask = pAdapter->IpAddressList.IpMask.String;
                first->gateway = pAdapter->GatewayList.IpAddress.String;
                first->Next = nullptr;
                last = first;
            }
            else {
                last->Next = static_cast<QR_INTERFACE> (new QrPcInterface);
                last->Next->ip = pAdapter->IpAddressList.IpAddress.String;
                last->Next->mask = pAdapter->IpAddressList.IpMask.String;
                last->Next->gateway = pAdapter->GatewayList.IpAddress.String;
                last->Next->Next = nullptr;
                last = last->Next;
            }

            PRINTF_D("IP Address: \t%s\n",
                pAdapter->IpAddressList.IpAddress.String);
            PRINTF_D("IP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

            PRINTF_D("Gateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
            //PRINTF_D("CURRENT: \t%s\n", pAdapter->CurrentIpAddress->IpAddress.String);
            PRINTF_D("***\n");

            pAdapter = pAdapter->Next;
            PRINTF_D("\n");
        }
    } else {
        PRINTF_D("GetAdaptersInfo failed with error: %ld\n", dwRetVal);
    }

    if (pAdapterInfo)
        FREE(pAdapterInfo);

    qrInterface = first;
    qrInterface = qrInterface->Next; // delete later

    return 0;
}* /// !!!
 *
 *
*/
































