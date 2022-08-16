#include "HttpClient.h"

HttpClient::HttpClient(Client& aClient, const char* aServerName, uint16_t aServerPort)
 : iClient(&aClient), iServerName(aServerName), iServerAddress(), iServerPort(aServerPort),
   iConnectionClose(true) //, iSendDefaultRequestHeaders(true)
{
  resetState();
}

HttpClient::HttpClient(Client& aClient, const String& aServerName, uint16_t aServerPort)
 : HttpClient(aClient, aServerName.c_str(), aServerPort)
{
}

// HttpClient::HttpClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort)
//  : iClient(&aClient), iServerName(NULL), iServerAddress(aServerAddress), iServerPort(aServerPort),
//    iConnectionClose(true), iSendDefaultRequestHeaders(true)
// {
//   resetState();
// }

int HttpClient::get(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_GET);
}

int HttpClient::get(const String& aURLPath)
{
    return get(aURLPath.c_str());
}

int HttpClient::post(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_POST);
}

int HttpClient::post(const String& aURLPath)
{
    return post(aURLPath.c_str());
}

int HttpClient::post(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return post(aURLPath, aContentType, strlen(aBody), (const byte*)aBody);
}

int HttpClient::post(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return post(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClient::post(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_POST, aContentType, aContentLength, aBody);
}

int HttpClient::put(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_PUT);
}

int HttpClient::put(const String& aURLPath)
{
    return put(aURLPath.c_str());
}

int HttpClient::put(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return put(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int HttpClient::put(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return put(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClient::put(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_PUT, aContentType, aContentLength, aBody);
}

int HttpClient::patch(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_PATCH);
}

int HttpClient::patch(const String& aURLPath)
{
    return patch(aURLPath.c_str());
}

int HttpClient::patch(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return patch(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int HttpClient::patch(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return patch(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClient::patch(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_PATCH, aContentType, aContentLength, aBody);
}

int HttpClient::del(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_DELETE);
}

int HttpClient::del(const String& aURLPath)
{
    return del(aURLPath.c_str());
}

int HttpClient::del(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return del(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int HttpClient::del(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return del(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClient::del(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_DELETE, aContentType, aContentLength, aBody);
}

// ========================================================

void HttpClient::resetState()
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

void HttpClient::stop()
{
  iClient->stop();
  resetState();
}

int HttpClient::responseStatusCode()
{
}

int HttpClient::contentLength()
{
    // skip the response headers, if they haven't been read already 
    // if (!endOfHeadersReached())
    // {
    //     skipResponseHeaders();
    // }

    return iContentLength;
}

String HttpClient::responseBody()
{
}

// bool HttpClient::endOfBodyReached()
// {
//     return false;
// }
