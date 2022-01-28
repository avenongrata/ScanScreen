#pragma once

#include <string>
#include <iostream>
#include <winsock2.h>
#include <thread>
#include "qrhttpserver.h"

//-----------------------------------------------------------------------------

#pragma comment(lib, "ws2_32.lib")

//-----------------------------------------------------------------------------

class QrTcpServer
{
public:
    QrTcpServer(std::string, int, std::string);
    QrTcpServer();
    ~QrTcpServer();

    //-------------------------------------------------------------------------

    int QrTcpServerStart();
    int stopServer();

    void waitThread() {(*mAcceptThread).join();}

    void addScreenShot(std::string);

private:
    //-------------------------------------------------------------------------

    std::string mIp;
    std::string mPathToFile;
    int mPort;
    SOCKET mQrTcpSockfd;

    std::thread * mAcceptThread;
    static const int mSocketCount = 25;

    // hold all sockets connected to server
    LPWSAPOLLFD mFdArray;

    QrHttpServer mHttpServer;

    //-------------------------------------------------------------------------

    SOCKET mCreateSocket(int af = AF_INET, int type = SOCK_STREAM,
                         int protocol = 0);

    //-------------------------------------------------------------------------

    friend void mAcceptFunction(QrTcpServer &);

    //-------------------------------------------------------------------------

    inline void mFreeData(WSAPOLLFD &);
};

//-----------------------------------------------------------------------------

void QrTcpServer::mFreeData(WSAPOLLFD & sock)
{
    closesocket(sock.fd);
    sock.fd = -1;
    sock.revents = 0;
}

//-----------------------------------------------------------------------------

class QrServer
{
public:
    QrServer();
    QrServer(std::string, int port);
    QrServer(std::string);
    ~QrServer();

    //-------------------------------------------------------------------------

    // move operator
    QrServer & operator=(QrServer &&);
    QrServer & operator=(const QrServer &)  = delete;
    QrServer(const QrServer &)              = delete;
    QrServer(QrServer &&)                   = delete;

    //-------------------------------------------------------------------------

    int startQrServer();
    void waitTcpThread(){mTcpServer->waitThread();}
    int stopServer(){return mTcpServer->stopServer();}

    std::string getUrlMultipleSS();
    std::string getUrlDownloadLink();
    std::string getFileName();

    void addScreenShot(std::string path){mTcpServer->addScreenShot(path);}

private:
    //-------------------------------------------------------------------------

    // IP address for creating server
    std::string mCurServerIp;
    // url, which will be encoded to Qr-code
    std::string mUrlToString;
    // path to file
    std::string mPhotoPath;
    // port for creating server
    int mServerPort;

    //-------------------------------------------------------------------------

    typedef struct QrPcInterface
    {
        struct QrPcInterface * Next;
        std::string ip;
    } * QR_INTERFACE;

    // Network PC interfaces (including IP-addresses)
    QR_INTERFACE mQrInterface;

    //-------------------------------------------------------------------------

    QrTcpServer *mTcpServer;

    int mGetIpAddress(void);
    int mChooseIpAddress(void);
};


