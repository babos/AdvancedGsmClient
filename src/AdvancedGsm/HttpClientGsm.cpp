#include "../API/HttpClientGsm.h"

HttpClientGsm::HttpClientGsm(Client& aClient, const char* aServerName, uint16_t aServerPort)
 : iClient(&aClient), iServerName(aServerName), iServerAddress(), iServerPort(aServerPort),
   iConnectionClose(true), iSendDefaultRequestHeaders(true)
{
  resetState();
}

HttpClientGsm::HttpClientGsm(Client& aClient, const String& aServerName, uint16_t aServerPort)
 : HttpClient(aClient, aServerName.c_str(), aServerPort)
{
}

HttpClientGsm::HttpClientGsm(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort)
 : iClient(&aClient), iServerName(NULL), iServerAddress(aServerAddress), iServerPort(aServerPort),
   iConnectionClose(true), iSendDefaultRequestHeaders(true)
{
  resetState();
}

void HttpClientGsm::resetState()
{
  iState = eIdle;
  iStatusCode = 0;
  iContentLength = kNoContentLengthHeader;
  iBodyLengthConsumed = 0;
  iContentLengthPtr = kContentLengthPrefix;
  iTransferEncodingChunkedPtr = kTransferEncodingChunked;
  iIsChunked = false;
  iChunkLength = 0;
  iHttpResponseTimeout = kHttpResponseTimeout;
}

void HttpClientGsm::stop()
{
  iClient->stop();
  resetState();
}

int HttpClientGsm::startRequest(const char* aURLPath, const char* aHttpMethod, 
                                const char* aContentType, int aContentLength, const byte aBody[])
{
}

int HttpClientGsm::get(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_GET);
}

int HttpClientGsm::get(const String& aURLPath)
{
    return get(aURLPath.c_str());
}

int HttpClientGsm::post(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_POST);
}

int HttpClientGsm::post(const String& aURLPath)
{
    return post(aURLPath.c_str());
}

int HttpClientGsm::post(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return post(aURLPath, aContentType, strlen(aBody), (const byte*)aBody);
}

int HttpClientGsm::post(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return post(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClientGsm::post(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_POST, aContentType, aContentLength, aBody);
}

int HttpClientGsm::put(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_PUT);
}

int HttpClientGsm::put(const String& aURLPath)
{
    return put(aURLPath.c_str());
}

int HttpClientGsm::put(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return put(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int HttpClientGsm::put(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return put(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClientGsm::put(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_PUT, aContentType, aContentLength, aBody);
}

int HttpClientGsm::patch(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_PATCH);
}

int HttpClientGsm::patch(const String& aURLPath)
{
    return patch(aURLPath.c_str());
}

int HttpClientGsm::patch(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return patch(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int HttpClientGsm::patch(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return patch(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClientGsm::patch(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_PATCH, aContentType, aContentLength, aBody);
}

int HttpClientGsm::del(const char* aURLPath)
{
    return startRequest(aURLPath, HTTP_METHOD_DELETE);
}

int HttpClientGsm::del(const String& aURLPath)
{
    return del(aURLPath.c_str());
}

int HttpClientGsm::del(const char* aURLPath, const char* aContentType, const char* aBody)
{
    return del(aURLPath, aContentType, strlen(aBody),  (const byte*)aBody);
}

int HttpClientGsm::del(const String& aURLPath, const String& aContentType, const String& aBody)
{
    return del(aURLPath.c_str(), aContentType.c_str(), aBody.length(), (const byte*)aBody.c_str());
}

int HttpClientGsm::del(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[])
{
    return startRequest(aURLPath, HTTP_METHOD_DELETE, aContentType, aContentLength, aBody);
}

int HttpClientGsm::responseStatusCode()
{
}

int HttpClientGsm::contentLength()
{
    // skip the response headers, if they haven't been read already 
    if (!endOfHeadersReached())
    {
        skipResponseHeaders();
    }

    return iContentLength;
}

String HttpClientGsm::responseBody()
{
}

bool HttpClientGsm::endOfBodyReached()
{
    return false;
}

