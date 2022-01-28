#include <qrhttpwebsocket.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <iterator>
#include <fstream>
#include <sstream>
#include <cstring>
#include <bitset>

//-----------------------------------------------------------------------------

QrHttpWebSocket::QrHttpWebSocket(const char * msg)
    : mMessage((BYTE *)msg)
{}

//-----------------------------------------------------------------------------

std::string QrHttpWebSocket::getDecodedMessage()
{
    //mShowMessageInHex();

    return mDecodeMessage();
}

//-----------------------------------------------------------------------------

std::string & QrHttpWebSocket::getEncodedMessage(std::string & msg, int format)
{
    /*
     * text message = 0x1,
     * binary message = 0x2
     */

    return mEncodeMessage(msg, format);
}

//-----------------------------------------------------------------------------

void QrHttpWebSocket::mShowMessageInHex()
{
    std::cout << "MESSAGE ---> ";

    for(unsigned int i=0; i < strlen((char *)mMessage); i++)
    {
        // first byte
        int x = ((mMessage[i]) >> (8*0)) & 0xff;
        std::cout << "0x" << std::hex << x << " ";
    }

    std::cout << std::endl;
}

//-----------------------------------------------------------------------------

std::string QrHttpWebSocket::mDecodeMessage()
{
    /*
     * byte 0: just an indicator that a message received
     * byte 1: the length, substract 0x80 from it
     * byte 2, 3, 4, 5: the 4 byte xor key to decrypt the payload
     * the rest: payload
     */

    std::string msg;
    BYTE lengthByte = (BYTE)mMessage[1];

    //-------------------------------------------------------------------------

    // it is header byte
    if (!mDecodeWebSocketHeader(mMessage[0]))
        return "";

    //-------------------------------------------------------------------------

    if (lengthByte == 0xFE)
        return "";

    //-------------------------------------------------------------------------

    const int msgLength = (int)(lengthByte - 0x80);
    const BYTE mask[4] =
    {
        (BYTE)mMessage[2],
        (BYTE)mMessage[3],
        (BYTE)mMessage[4],
        (BYTE)mMessage[5]
    };

    //-------------------------------------------------------------------------

    for (int i = 6, j = 0; j < msgLength; i++, j++)
        msg = msg + (char)((BYTE)mMessage[i] ^ mask[j % 4]);

    //-------------------------------------------------------------------------

    return msg;
}

//-----------------------------------------------------------------------------

bool QrHttpWebSocket::mDecodeWebSocketHeader(BYTE byte)
{
    /*
     * This function will return 0 if error or close flag occured.
     * When header allows work further 1 will be returned.
     *
     * ##########################################################################
     *
     * NOTE that the most significant bit is the leftmost in the ABNF
     *
       FIN:  1 bit

          Indicates that this is the final fragment in a message.  The first
          fragment MAY also be the final fragment.

       RSV1, RSV2, RSV3:  1 bit each

          MUST be 0 unless an extension is negotiated that defines meanings
          for non-zero values.  If a nonzero value is received and none of
          the negotiated extensions defines the meaning of such a nonzero
          value, the receiving endpoint MUST _Fail the WebSocket
          Connection_.

       Opcode:  4 bits

          Defines the interpretation of the "Payload data".  If an unknown
          opcode is received, the receiving endpoint MUST _Fail the
          WebSocket Connection_.  The following values are defined.

          *  %x0 denotes a continuation frame

          *  %x1 denotes a text frame

          *  %x2 denotes a binary frame

          *  %x3-7 are reserved for further non-control frames

          *  %x8 denotes a connection close

          *  %x9 denotes a ping

          *  %xA denotes a pong

          *  %xB-F are reserved for further control frames
     */

    BYTE fin;
    BYTE rsv123;
    BYTE opcode;

    //-------------------------------------------------------------------------

    fin = rsv123 = opcode = 0;
    // get 1 bit
    fin = byte >> 7;
    // get 2,3,4 bits
    rsv123 = (byte >> 4) & 7;
    // get 5,6,7,8 bits
    opcode = byte & 15;

    //-------------------------------------------------------------------------

    // when it's not last message or there is rsv parameter on
    if (!fin || rsv123)
        return false;

    //-------------------------------------------------------------------------

    /*
     * Need to decode opcode message.
     *
     * #############
     *
     * Later I can rewrite it for more functionality. When I return false
     * than I need to close connection and clear all flags.
     */

    switch (opcode)
    {
    case 0x0:         // denotes a continuation frame
        return false;

    case 0x1:         // denotes a text frame
        return true;

    case 0x2:         // denotes a binary frame
        return false;

    case 0x8:         // denotes a connection close
        return false;

    case 0x9:         // denotes a ping
        return false;

    case 0xA:         // denotes a pong
        return false;
    }

    //-------------------------------------------------------------------------

    return false;       // Error occurred
}

//-----------------------------------------------------------------------------

std::string & QrHttpWebSocket::mEncodeMessage(std::string & msg, int opcode)
{
    /*
     * A server MUST NOT mask any frames that it sends to
     * the client.
     */

    // FIN flag set to 1
    BYTE header = 0x80;
    size_t msgLength = msg.length();
    std::string encodedMsg;

    typedef struct
    {
        BYTE byte0;
        BYTE byte1;
        BYTE byte2;
        BYTE byte3;
        BYTE byte4;
        BYTE byte5;
        BYTE byte6;
        BYTE byte7;
    } BYTE_2, BYTE_8;

    //-------------------------------------------------------------------------

    // TEXT or BINARY flag set to 1
    header |= opcode;
    encodedMsg = (char) header;

    //-------------------------------------------------------------------------

    if (msgLength < 126)
    {
        encodedMsg += (char) msgLength;
    }
    else if (msgLength < 65536)
    {
        // the following 2 bytes interpreted as a 16-bit unsigned integer
        BYTE_2 lengthHeader
        {
            BYTE ((msgLength >> 8) & 0xFF),
            BYTE (msgLength & 0xFF),
            0,
            0,
            0,
            0,
            0,
            0
        };

        encodedMsg += (char) 126;
        encodedMsg = encodedMsg +
                (char)lengthHeader.byte0 +
                (char)lengthHeader.byte1;
    }
    else
    {
        // the following 8 bytes interpreted as a 64-bit unsigned integer
        BYTE_8 lengthHeader
        {
            BYTE ((msgLength >> (8 * 7)) & 0xFF),
            BYTE ((msgLength >> (8 * 6)) & 0xFF),
            BYTE ((msgLength >> (8 * 5)) & 0xFF),
            BYTE ((msgLength >> (8 * 4)) & 0xFF),
            BYTE ((msgLength >> (8 * 3)) & 0xFF),
            BYTE ((msgLength >> (8 * 2)) & 0xFF),
            BYTE ((msgLength >> (8 * 1)) & 0xFF),
            BYTE (msgLength & 0xFF)
        };

        encodedMsg += (char) 127;
        encodedMsg = encodedMsg +
                (char)lengthHeader.byte0 +
                (char)lengthHeader.byte1 +
                (char)lengthHeader.byte2 +
                (char)lengthHeader.byte3 +
                (char)lengthHeader.byte4 +
                (char)lengthHeader.byte5 +
                (char)lengthHeader.byte6 +
                (char)lengthHeader.byte7;
    }

    //-------------------------------------------------------------------------

    encodedMsg += msg;      // get playload
    msg = encodedMsg;

    //-------------------------------------------------------------------------

    return msg;
}
