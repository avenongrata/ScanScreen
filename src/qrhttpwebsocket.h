#pragma once
#include <iostream>
#include <string>


class QrHttpWebSocket {
private:
    typedef unsigned char BYTE;
    const BYTE *mMessage;

    void mShowMessageInHex();                               // see message in hex format
    std::string mDecodeMessage();                           // decode WebSocket message
    bool mDecodeWebSocketHeader(BYTE);
    std::string & mEncodeMessage(std::string &, int );      // encode WebSocket message


public:
    QrHttpWebSocket() {};
    QrHttpWebSocket(const char *);
    QrHttpWebSocket & operator=(QrHttpWebSocket &&)         = delete ;      // move operator
    QrHttpWebSocket & operator=(const QrHttpWebSocket &)    = delete ;      // copy operator
    QrHttpWebSocket(const QrHttpWebSocket &)                = delete;       // copy constructor
    QrHttpWebSocket(QrHttpWebSocket &&)                     = delete;       // move constructor

    std::string getDecodedMessage();
    std::string & getEncodedMessage(std::string &, int format=0x1);
};
