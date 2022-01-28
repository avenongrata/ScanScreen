#pragma once
#include <iostream>
#include <string>

//-----------------------------------------------------------------------------

class QrHttpWebSocket
{
public:
    QrHttpWebSocket() {};
    QrHttpWebSocket(const char *);
    ~QrHttpWebSocket();

    // move operator
    QrHttpWebSocket & operator=(QrHttpWebSocket &&)         = delete;
    // copy operator
    QrHttpWebSocket & operator=(const QrHttpWebSocket &)    = delete;
    // copy constructor
    QrHttpWebSocket(const QrHttpWebSocket &)                = delete;
    // move constructor
    QrHttpWebSocket(QrHttpWebSocket &&)                     = delete;

    //-------------------------------------------------------------------------

    std::string getDecodedMessage();
    std::string & getEncodedMessage(std::string &, int format=0x1);

private:
    //-------------------------------------------------------------------------

    typedef unsigned char BYTE;
    const BYTE *mMessage;

    //-------------------------------------------------------------------------

    // see message in hex format
    void mShowMessageInHex();

    // decode WebSocket message
    std::string mDecodeMessage();

    // decode WebSocket message
    bool mDecodeWebSocketHeader(BYTE);

    // encode WebSocket message
    std::string & mEncodeMessage(std::string &, int );
};
