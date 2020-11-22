#pragma once
#include <string>
#include <iostream>
#include <winsock2.h>
#include <thread>
#include "qrhttpserver.h"
#pragma comment(lib, "ws2_32.lib")

class QrTcpServer
{
private:
    std::string mIp;
    std::string mPathToFile;
    int mPort;
    SOCKET mQrTcpSockfd;
    std::thread *mAcceptThread;
    static const int mSocketCount = 25;
    LPWSAPOLLFD mFdArray;                   // hold all sockets connected to server
    QrHttpServer mHttpServer;

    SOCKET mCreateSocket(int af = AF_INET,  int type = SOCK_STREAM, int protocol = 0);
    friend void mAcceptFunction(QrTcpServer &);
    inline void mFreeData(WSAPOLLFD &);

public:
    QrTcpServer(std::string, int, std::string);
    QrTcpServer();
    ~QrTcpServer();
    int QrTcpServerStart();
    void waitThread() {(*mAcceptThread).join();}
    int stopServer();
    void addScreenShot(std::string);
};

void QrTcpServer::mFreeData(WSAPOLLFD & sock)
{
    closesocket(sock.fd);
    sock.fd = -1;
    sock.revents = 0;
}

class QrServer
{
private:
    std::string mCurServerIp;               // IP address for creating server
    std::string mUrlToString;               // url, which will be encoded to Qr-code
    std::string mPhotoPath;                 // path to file
    int mServerPort;                        // port for creating server

    typedef struct QrPcInterface {
        struct QrPcInterface* Next;
        std::string ip;
    } *QR_INTERFACE;
    QR_INTERFACE mQrInterface;              // Network PC interfaces (including IP-addresses)

    QrTcpServer *mTcpServer;

    int mGetIpAddress(void);
    int mChooseIpAddress(void);

public:
    QrServer();
    QrServer(std::string, int port);
    QrServer(std::string);
    ~QrServer();
    QrServer & operator=(QrServer &&);          // move operator
    QrServer & operator=(const QrServer &)  = delete;
    QrServer(const QrServer &)              = delete;
    QrServer(QrServer &&)                   = delete;
    int startQrServer();
    std::string getUrlMultipleSS();
    std::string getUrlDownloadLink();
    void waitTcpThread(){mTcpServer->waitThread();}
    int stopServer(){return mTcpServer->stopServer();}
    std::string getFileName();
    void addScreenShot(std::string path){mTcpServer->addScreenShot(path);}

    //int createServer(std::string, int);
    //int uploadToServer(void);
    //int stopServer(void);
    //std::string retServerString(void);

};


