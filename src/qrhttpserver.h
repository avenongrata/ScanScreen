#pragma once
#include <iostream>
#include <winsock2.h>
#include <vector>
#include <map>
#include "qrhttpwebsocket.h"
#include <ws2tcpip.h>
#include <filesystem>

//-----------------------------------------------------------------------------

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define LogoName "ScanScreenLogo.jpg"

//-----------------------------------------------------------------------------

class QrHttpServer
{
public:
    QrHttpServer();
    QrHttpServer(std::string);

    ~QrHttpServer() {}

    // copy constructor
    QrHttpServer(const QrHttpServer &)              = delete;
    // move constructor
    QrHttpServer(QrHttpServer &&)                   = delete;
    // copy operator
    QrHttpServer & operator=(const QrHttpServer &)  = delete;
    // move operator
    QrHttpServer & operator=(QrHttpServer &&);

    //-------------------------------------------------------------------------

    int processRequest(char *, SOCKET &);
    void updateBrowserPage(std::string);
    void sendWebSocketNewBrowserPage(SOCKET & sock);
    void deleteScreenShots();

    //-------------------------------------------------------------------------

    enum QrHttpReturnValue
    {
        QR_HTTP_SUCCESS = 0,
        QR_HTTP_ERROR   = 1,
        QR_HTTP_EXIT    = 2
    };

private:
    //-------------------------------------------------------------------------

    SOCKET mCurrentSocket;
    // check WebSocket connection
    std::map<SOCKET, bool> mSocketArray;
    // check WebSocket state in current time
    bool mCurrentWebSocketState;
    // hold all screenshots
    std::vector<std::string> mScreenShot;
    // hold all pathes
    std::vector<std::string> mPathToFile;

    //-------------------------------------------------------------------------

    std::string mBrowserHead;
    std::string mBrowserBody;
    // make update on webpage via websocket
    std::string mBrowserWebSocketBodyAdd;
    std::string mBrowserEnd;
    // header with accept-key for WebSocket protocol
    std::string mWebSocketConnection;

    //-------------------------------------------------------------------------

    typedef struct
    {
        char ip[16];
        unsigned int port;
    } NETWORK_DATA;

    //-------------------------------------------------------------------------

    void mConstructHead();
    void mConstructBody();
    void mConstructEnd();
    void mAddFile(const std::string);
    void mAddPath(const std::string);

    //-------------------------------------------------------------------------

    int mLoadPage(void);
    int mLoadLogo(void) const;
    int mSendFile(const std::string &) const;

    //-------------------------------------------------------------------------

    int mConfirmWebSocket(const char *);
    int mWebSocketSendFile(const std::string &) const;
    int mWebSocketLoadPage(QrHttpWebSocket &);
    int mWebSocketLoadLogo(void) const;

    //-------------------------------------------------------------------------

    int mTestLoadPage(void);

    //-------------------------------------------------------------------------

    std::string mConstructFile(std::string) const;
    std::string mGetFileName(std::string) const;

    //-------------------------------------------------------------------------

    int mWebSocketProcessRequest(char *);
    int mHttpProcessRequest(char *);

    //-------------------------------------------------------------------------

    std::string mGetCurrentPath() const
    {
        return std::experimental::filesystem::current_path().string();
    }

    //-------------------------------------------------------------------------

    NETWORK_DATA mGetIpAndPort(SOCKET sock)
    {
        struct sockaddr_in addr;
        NETWORK_DATA st;

        bzero(&addr, sizeof(addr));
        int len = sizeof(addr);

        getsockname(sock, (struct sockaddr *) &addr, &len);
        inet_ntop(AF_INET, &addr.sin_addr, st.ip, sizeof(st.ip));
        st.port = ntohs(addr.sin_port);

        return st;
    };
};
