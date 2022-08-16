#include "GsmHttpClient.h"

GsmHttpClient::GsmHttpClient(GsmTcpClient& aClient, const char* aServerName, uint16_t aServerPort)
 : iClient(&aClient), server_name(aServerName), iServerAddress(), server_port(aServerPort),
   iConnectionClose(true) //, iSendDefaultRequestHeaders(true)
{
  resetState();
}

// GsmHttpClient::GsmHttpClient(GsmTcpClient& aClient, const String& aServerName, uint16_t aServerPort)
//  : GsmHttpClient(aClient, aServerName.c_str(), aServerPort)
// {
// }

// GsmHttpClient::HttpClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort)
//  : iClient(&aClient), iServerName(NULL), iServerAddress(aServerAddress), iServerPort(aServerPort),
//    iConnectionClose(true), iSendDefaultRequestHeaders(true)
// {
//   resetState();
// }

int GsmHttpClient::get(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_GET);
}

int GsmHttpClient::get(const String& aURLPath)
{
    return get(aURLPath.c_str());
}

int GsmHttpClient::post(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_POST);
}

int GsmHttpClient::post(const String& aURLPath)
{
    return post(aURLPath.c_str());
}

int GsmHttpClient::post(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return post(aURLPath, aContentType, strlen(aBody), (const byte*)aBody);
}

int GsmHttpClient::post(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return post(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int GsmHttpClient::post(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_POST, aContentType, aContentLength, aBody);
}

int GsmHttpClient::put(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_PUT);
}

int GsmHttpClient::put(const String& aURLPath)
{
    return put(aURLPath.c_str());
}

int GsmHttpClient::put(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return put(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int GsmHttpClient::put(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return put(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int GsmHttpClient::put(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_PUT, aContentType, aContentLength, aBody);
}

int GsmHttpClient::patch(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_PATCH);
}

int GsmHttpClient::patch(const String& aURLPath)
{
    return patch(aURLPath.c_str());
}

int GsmHttpClient::patch(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return patch(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int GsmHttpClient::patch(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return patch(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int GsmHttpClient::patch(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_PATCH, aContentType, aContentLength, aBody);
}

int GsmHttpClient::del(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_DELETE);
}

int GsmHttpClient::del(const String& aURLPath)
{
    return del(aURLPath.c_str());
}

int GsmHttpClient::del(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return del(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int GsmHttpClient::del(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return del(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int GsmHttpClient::del(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_DELETE, aContentType, aContentLength, aBody);
}

// ========================================================

bool GsmHttpClient::completed()
{
    return false;
}

void GsmHttpClient::resetState()
{
  //iState = eIdle;
  iStatusCode = 0;
  //iContentLength = kNoContentLengthHeader;
  //iBodyLengthConsumed = 0;
  //iContentLengthPtr = kContentLengthPrefix;
  //iTransferEncodingChunkedPtr = kTransferEncodingChunked;
  //iIsChunked = false;
  //iChunkLength = 0;
  //iHttpResponseTimeout = kHttpResponseTimeout;
}

// void GsmHttpClient::stop()
// {
//   iClient->stop();
//   resetState();
// }

int GsmHttpClient::responseStatusCode()
{
    return 0;
}

int GsmHttpClient::contentLength()
{
    // skip the response headers, if they haven't been read already 
    // if (!endOfHeadersReached())
    // {
    //     skipResponseHeaders();
    // }

    return iContentLength;
}

String GsmHttpClient::responseBody()
{
    return "";
}

// bool GsmHttpClient::endOfBodyReached()
// {
//     return false;
// }
