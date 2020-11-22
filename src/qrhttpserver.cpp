#include <qrhttpserver.h>
#include <qrmacros.h>
#include <fstream>
#include <sstream>
#include <QCryptographicHash>
#include <qrhttpwebsocket.h>


QrHttpServer::QrHttpServer() {}

QrHttpServer::QrHttpServer(std::string pathToFile)
{
    mCurrentSocket = -1;
    mAddPath(pathToFile);
    mAddFile(pathToFile);
    mConstructHead();
    mConstructEnd();
    mConstructBody();
}

QrHttpServer & QrHttpServer::operator=(QrHttpServer && x)
{
    if (&x == this)
        return *this;

    mCurrentSocket = x.mCurrentSocket;
    mSocketArray = x.mSocketArray;
    mScreenShot = x.mScreenShot;                        // hold all screenshots
    mPathToFile = x.mPathToFile;                        // hold all pathes
    mCurrentWebSocketState = x.mCurrentWebSocketState;
    mWebSocketConnection = x.mWebSocketConnection;
    mBrowserWebSocketBodyAdd = x.mBrowserWebSocketBodyAdd;

    mBrowserHead = x.mBrowserHead;
    mBrowserBody = x.mBrowserBody;
    mBrowserEnd = x.mBrowserEnd;

    return *this;
}

std::string QrHttpServer::mGetFileName(std::string pathToFile) const
{
    std::string file = pathToFile.substr(pathToFile.find_last_of("\\") + 1);
    return file;
}


int QrHttpServer::processRequest(char *request, SOCKET & sock)
{
    /*
     * Thi function is used to process request from client
     * and send response back.
     */

    mSocketArray.insert({sock, false});                         // When key exist - operation won't be exucuted
    mCurrentSocket = sock;
    mCurrentWebSocketState = mSocketArray[mCurrentSocket];

    if (mCurrentWebSocketState)     // WebSocket protocol is used
        return mWebSocketProcessRequest(request);
    else
        return mHttpProcessRequest(request);
}

int QrHttpServer::mWebSocketProcessRequest(char *request)
{
    QrHttpWebSocket webSocket(request);
    std::string msg = webSocket.getDecodedMessage();

    if (!msg.length()) {
        std::cout << "Error or close connection from WebSocket received\n";
        mSocketArray[mCurrentSocket] = false;
        return QR_HTTP_EXIT;
    } else if (msg == "nongrata")
        return mWebSocketLoadPage(webSocket);


    return QR_HTTP_ERROR;
}

int QrHttpServer::mWebSocketLoadPage(QrHttpWebSocket & webSocket)
{
    std::stringstream response_body;
    std::string msg;

    response_body \
            << mBrowserHead
            << mBrowserBody
            << mBrowserEnd;

    msg = response_body.str();
    msg = webSocket.getEncodedMessage(msg);

    if ((send(mCurrentSocket, msg.c_str(), (int)msg.length(), 0)) < 0)
        return QR_HTTP_ERROR;

    return QR_HTTP_SUCCESS;
}

int QrHttpServer::mHttpProcessRequest(char *request)
{
    const char *requestLogo         = "GET /favicon.ico HTTP/1.1";
    const char *requestWebSocket    = "GET /chat HTTP/1.1";
    const char *requestFile;
    std::string tmp;

    // get header from request
    char *firstEntry = strchr(request, '\r');
    const int firstStringLength = firstEntry - request;
    char *firstString = new char [firstStringLength + 1];
    strncpy(firstString, request, firstStringLength);
    firstString[firstStringLength] = '\0';

    if (strstr(firstString, requestWebSocket))
        return mConfirmWebSocket(request);

    if (strstr(firstString, requestLogo))    // webclient requests logo of browser page
        return mLoadLogo();

    // check when file was requested
    for (unsigned long long i = 0; i < mScreenShot.size(); i++) {
        const std::string & elem = mScreenShot[i];
        std::stringstream tmpRequestFile;
        tmpRequestFile << "GET /" << elem  <<" HTTP/1.1";
        tmp = tmpRequestFile.str();     // can't make like tmpRequestFile.str().c_str(); It doesn't work, why ?
        requestFile = tmp.c_str();

        if (strstr(firstString, requestFile))
            return mSendFile(mPathToFile[i]);
    }

    // When client requests incorrect file or webpage just load the page.
    return mTestLoadPage(); //mLoadPage();
}

void QrHttpServer::sendWebSocketNewBrowserPage(SOCKET & sock)
{
    mSocketArray.insert({sock, false});
    mCurrentSocket = sock;
    mCurrentWebSocketState = mSocketArray[mCurrentSocket];        // WebServer connection


    if (!mCurrentWebSocketState)
        return;

    std::stringstream response_body;
    QrHttpWebSocket test;

    response_body \
            << mBrowserWebSocketBodyAdd;

    std::string msg = response_body.str();
    msg = test.getEncodedMessage(msg);
    if ((send(mCurrentSocket, msg.c_str(), (int)msg.length(), 0)) < 0)
        std::cout << "Can't add ScreenShoot to user browser page\n";
}

/*int QrHttpServer::loadPage(void) const
{
    using namespace std;
    stringstream response;
    stringstream response_body;

    response_body \
            << "<!DOCTYPE html>"
            << "<html>"
            << "<body>"
            << "<h1>Developed by <strong><em>nongrata</em></strong></h1>"
            << "<h2>ScanScreen project v.1.0.</h2>"
            << "</a>"
            << "</body>"
            << "</html>";

    // Формируем весь ответ вместе с заголовками
    response \
            << "HTTP/1.1 200 OK\r\n"
            << "Version: HTTP/1.1\r\n"
            << "Content-Type: text/html; charset=utf-8\r\n"
            << "Content-Length: " << response_body.str().length()
            << "\r\n\r\n"
            << response_body.str();

    // Отправляем ответ клиенту с помощью функции send
    if ((send(mSocket, response.str().c_str(), (int)response.str().length(), 0)) < 0)
        return QR_HTTP_ERROR;

    return QR_HTTP_SUCCESS;
}*/

int QrHttpServer::mSendFile(const std::string & pathToFile) const
{
    using namespace std;
    stringstream response;
    stringstream response_body;
    streampos size;
    char * memblock;

    ifstream file (pathToFile, ios::binary|ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        memblock = new char [size];
        file.seekg (0, ios::beg);
        file.read (memblock, size);
        file.close();
    } else
        return QR_HTTP_ERROR;

    // form response with headers
    response \
            << "HTTP/1.1 200 OK\r\n"
            //<< "Version: HTTP/1.1\r\n"
            << "Content-Type: application/octet-stream\r\n"
            << "Content-Length: " << size
            << "\r\n\r\n";

    // send response to client
    if ((send(mCurrentSocket, response.str().c_str(), (int)response.str().length(), 0)) < 0)
        return QR_HTTP_ERROR;
    if ((send(mCurrentSocket, memblock, (int)size, 0)) < 0)
        return QR_HTTP_ERROR;

    return QR_HTTP_EXIT;
}

int QrHttpServer::mLoadLogo() const
{
    /*
     * In this function i need to know path to folder
     * where this program will be installed. In thisw folder
     * will be installed a photo to logo.
     */

    using namespace std;
    stringstream response;
    stringstream response_body;

    streampos size;
    char * memblock;

    std::string logoPath = mGetCurrentPath();
    logoPath = logoPath + "\\Logo\\" + LogoName;

    ifstream file (logoPath, ios::binary|ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        memblock = new char [size];
        file.seekg (0, ios::beg);
        file.read (memblock, size);
        file.close();
    } //else    // who needs logo ? nobody !
       // return QR_HTTP_ERROR;

    // form response with headers
    response \
            << "HTTP/1.1 200 OK\r\n"
            //<< "Version: HTTP/1.1\r\n"
            << "Content-Type: application/octet-stream\r\n"
            << "Content-Length: " << size
            << "\r\n\r\n";

    // send response to client
    if ((send(mCurrentSocket, response.str().c_str(), (int)response.str().length(), 0)) < 0)
        return QR_HTTP_ERROR;
    if ((send(mCurrentSocket, memblock, (int)size, 0)) < 0)
        return QR_HTTP_ERROR;

    return QR_HTTP_SUCCESS;
}

void QrHttpServer::mAddFile(const std::string pathToFile)
{
    mScreenShot.push_back(mGetFileName(pathToFile));
}

void QrHttpServer::mAddPath(const std::string path)
{
    mPathToFile.push_back(path);
}

void QrHttpServer::deleteScreenShots()
{
    for (const std::string & elem : mPathToFile)
        DeleteFileA(elem.c_str());
}

void QrHttpServer::mConstructHead()
{
    mBrowserHead =
            "<!DOCTYPE html>"
            "<html>"
            "<body>"
            "<h1>Developed by <strong><em>nongrata</em></strong></h1>";
}

void QrHttpServer::mConstructBody()
{
    mBrowserBody +=
            "<p style=\"font-size:24px\"><i>Click on the image to download it:<p></i>" +
            mConstructFile(mScreenShot[mScreenShot.size() - 1]);

    mBrowserWebSocketBodyAdd =
            "<p style=\"font-size:24px\"><i>Click on the image to download it:<p></i>" +
            mConstructFile(mScreenShot[mScreenShot.size() - 1]);        // delete later ???
}

std::string QrHttpServer::mConstructFile(std::string fileName) const
{
    std::string browserFile =
            "<a href=\"" + fileName + "\" download>"
            "<img src=\"" + fileName + "\" alt=\"ScanScreen\" width=\"640\" height=\"360\">"
            "</a>";

    return browserFile;
}

void QrHttpServer::mConstructEnd()
{
    mBrowserEnd =
            "</body>"
            "</html>";
}

void QrHttpServer::updateBrowserPage(std::string pathToFile)
{
    mAddPath(pathToFile);
    mAddFile(pathToFile);
    mConstructBody();       // new body of browser page
}


//########################################################################################
//########################################################################################
//########################################################################################


int QrHttpServer::mLoadPage(void)
{
    using namespace std;
    stringstream response;
    stringstream response_body;

    response_body \
            << mBrowserHead
            << mBrowserBody
            << mBrowserEnd;

    // form response with headers
    response \
            << "HTTP/1.1 200 OK\r\n"
            << "Version: HTTP/1.1\r\n"
            << "Content-Type: text/html; charset=utf-8\r\n"
            << "Content-Length: " << response_body.str().length()
            << "\r\n\r\n"
            << response_body.str();

    // send response to client
    if ((send(mCurrentSocket, response.str().c_str(), (int)response.str().length(), 0)) < 0)
        return QR_HTTP_ERROR;

    return QR_HTTP_SUCCESS;;
}

int QrHttpServer::mTestLoadPage(void)
{
    using namespace std;
    stringstream response;
    stringstream response_body;
    NETWORK_DATA st = mGetIpAndPort(mCurrentSocket);

    // Need to get IP-address and port from runtime !
    std::string body =
            "﻿  <!DOCTYPE html>\
              <meta charset=\"utf-8\" />\
              <title>ScanScreen</title>\
              <script language=\"javascript\" type=\"text/javascript\">\
            ";
            std::string temp = std::string("var wsUri = \"ws://") + std::string(st.ip) + ":" + std::to_string(st.port) + std::string("/chat\";");
    body += temp;
    body += " \
              var output;\
            \
              function init()\
              {\
                output = document.getElementById(\"output\");\
                QrWebSocket();\
              }\
            \
              function QrWebSocket()\
              {\
                websocket = new WebSocket(wsUri);\
                websocket.onopen = function(evt) { onOpen(evt) };\
                websocket.onclose = function(evt) { onClose(evt) };\
                websocket.onmessage = function(evt) { onMessage(evt) };\
                websocket.onerror = function(evt) { onError(evt) };\
              }\
            \
              function onOpen(evt)\
              {\
                doSend(\"nongrata\");\
              }\
            \
              function onClose(evt)\
              {\
                writeToScreen(\"DISCONNECTED\");\
              }\
            \
              function onMessage(evt)\
              {\
                writeToScreen(evt.data);\
              }\
            \
              function onError(evt)\
              {\
                writeToScreen(\'<span style=\"color: red;\">ERROR:</span> \' + evt.data);\
              }\
            \
              function doSend(message)\
              {\
                websocket.send(message);\
              }\
            \
              function writeToScreen(message)\
              {\
                var pre = document.createElement(\"p\");\
                pre.style.wordWrap = \"break-word\";\
                pre.innerHTML = message;\
                output.appendChild(pre);\
              }\
            \
              window.addEventListener(\"load\", init, false);\
            \
              </script>\
            \
            \
              <div id=\"output\"></div>";


    response_body \
            << body;

    // form response with headers
    response \
            << "HTTP/1.1 200 OK\r\n"
            << "Version: HTTP/1.1\r\n"
            << "Content-Type: text/html; charset=utf-8\r\n"
            << "Content-Length: " << response_body.str().length() << "\r\n"
            << "\r\n"
            << response_body.str();

    // send response
    if ((send(mCurrentSocket, response.str().c_str(), (int)response.str().length(), 0)) < 0)
        return QR_HTTP_ERROR;

    return QR_HTTP_SUCCESS;
}

int QrHttpServer::mConfirmWebSocket(const char *request)
{
    const char *addString = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    const char *webSocketHeader = "Sec-WebSocket-Key: ";
    const int webSocketHeaderLen = strlen(webSocketHeader);
    const char *key = nullptr;
    std::stringstream response;
    QByteArray hash;

    if (!(key = strstr(request, webSocketHeader)))
        return QR_HTTP_ERROR;

    key += webSocketHeaderLen;
    while (*key) {
        if (*key != '=')
            hash.append(*key);
        else {
            hash.append("==");
            break;
        }
        key++;
    }

    hash.append(addString);
    hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha1);
    mWebSocketConnection = "Sec-WebSocket-Accept: " + hash.toBase64().toStdString() + "\r\n";

    response \
            << "HTTP/1.1 101 Switching Protocols\r\n"
            << "Upgrade: websocket\r\n"
            << "Connection: Upgrade\r\n"
            << mWebSocketConnection
            << "\r\n";

    if ((send(mCurrentSocket, response.str().c_str(), (int)response.str().length(), 0)) < 0)
        return QR_HTTP_ERROR;

    // when WebSocket connection is established need to change flag
    auto search = mSocketArray.find(mCurrentSocket);
    if (search != mSocketArray.end())
        mSocketArray[mCurrentSocket] = true;
    else
        return QR_HTTP_ERROR;

    return QR_HTTP_SUCCESS;
}





