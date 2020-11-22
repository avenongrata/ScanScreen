#pragma once
#include <iostream>
#include <winsock2.h>
#include <vector>
#include <map>
#include "qrhttpwebsocket.h"
#include <ws2tcpip.h>
#include <filesystem>

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define LogoName "ScanScreenLogo.jpg"


class QrHttpServer {
private:
    SOCKET mCurrentSocket;
    std::map<SOCKET, bool> mSocketArray;                            // check WebSocket connection
    bool mCurrentWebSocketState;                                    // check WebSocket state in current time
    std::vector<std::string> mScreenShot;                           // hold all screenshots
    std::vector<std::string> mPathToFile;                           // hold all pathes

    std::string mBrowserHead;
    std::string mBrowserBody;
    std::string mBrowserWebSocketBodyAdd;                           // make update on webpage via websocket
    std::string mBrowserEnd;
    std::string mWebSocketConnection;                               // header with accept-key for WebSocket protocol

    void mConstructHead();
    void mConstructBody();
    void mConstructEnd();
    void mAddFile(const std::string);
    void mAddPath(const std::string);

    int mLoadPage(void);
    int mLoadLogo(void) const;
    int mSendFile(const std::string &) const;

    int mConfirmWebSocket(const char *);
    int mWebSocketSendFile(const std::string &) const;
    int mWebSocketLoadPage(QrHttpWebSocket &);
    int mWebSocketLoadLogo(void) const;

    int mTestLoadPage(void);

    std::string mConstructFile(std::string) const;
    std::string mGetFileName(std::string) const;

    int mWebSocketProcessRequest(char *);
    int mHttpProcessRequest(char *);

    typedef struct {
        char ip[16];
        unsigned int port;
    }NETWORK_DATA;

    NETWORK_DATA mGetIpAndPort(SOCKET sock) {
        struct sockaddr_in addr;
        NETWORK_DATA st;
        bzero(&addr, sizeof(addr));
        int len = sizeof(addr);
        getsockname(sock, (struct sockaddr *) &addr, &len);
        inet_ntop(AF_INET, &addr.sin_addr, st.ip, sizeof(st.ip));
        st.port = ntohs(addr.sin_port);
        return st;
    };

    std::string mGetCurrentPath() const
    {return std::experimental::filesystem::current_path().string();}



public:
    enum QrHttpReturnValue {
        QR_HTTP_SUCCESS = 0,
        QR_HTTP_ERROR   = 1,
        QR_HTTP_EXIT    = 2
    };

    QrHttpServer();
    QrHttpServer(std::string);
    QrHttpServer(const QrHttpServer &)              = delete;       // copy constructor
    QrHttpServer(QrHttpServer &&)                   = delete;       // move constructor

    QrHttpServer & operator=(QrHttpServer &&);                      // move operator
    QrHttpServer & operator=(const QrHttpServer &)  = delete;       // copy operator

    int processRequest(char *, SOCKET &);
    void updateBrowserPage(std::string);
    void sendWebSocketNewBrowserPage(SOCKET & sock);
    void deleteScreenShots();
};
