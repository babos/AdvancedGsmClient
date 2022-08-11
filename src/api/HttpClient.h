#if 0
#ifndef AdvancedHttpClient_h
#define AdvancedHttpClient_h

#include <arduino::HttpClient.h>

namespace advanced {

class HttpClient
{
 public:
  static const int HttpPort = 80;
  static const int HttpsPort = 443;

  HttpClient(Client& aClient, const char* aServerName, uint16_t aServerPort = kHttpPort);
  HttpClient(Client& aClient, const String& aServerName, uint16_t aServerPort = kHttpPort);
  HttpClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort = kHttpPort);

  /** Connect to the server and start to send a GET request.
    @param aURLPath     Url to request
    @return 0 if successful, else error
  */
  int get(const char* aURLPath);
  int get(const String& aURLPath);

  /** Connect to the server and start to send a POST request.
    @param aURLPath     Url to request
    @return 0 if successful, else error
  */
  int post(const char* aURLPath);
  int post(const String& aURLPath);

  /** Connect to the server and send a POST request
      with body and content type
    @param aURLPath     Url to request
    @param aContentType Content type of request body
    @param aBody        Body of the request
    @return 0 if successful, else error
  */
  int post(const char* aURLPath, const char* aContentType, const char* aBody);
  int post(const String& aURLPath, const String& aContentType, const String& aBody);
  int post(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[]);

  /** Connect to the server and start to send a PUT request.
    @param aURLPath     Url to request
    @return 0 if successful, else error
  */
  int put(const char* aURLPath);
  int put(const String& aURLPath);

  /** Connect to the server and send a PUT request
      with body and content type
    @param aURLPath     Url to request
    @param aContentType Content type of request body
    @param aBody        Body of the request
    @return 0 if successful, else error
  */
  int put(const char* aURLPath, const char* aContentType, const char* aBody);
  int put(const String& aURLPath, const String& aContentType, const String& aBody);
  int put(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[]);

  /** Connect to the server and start to send a PATCH request.
    @param aURLPath     Url to request
    @return 0 if successful, else error
  */
  int patch(const char* aURLPath);
  int patch(const String& aURLPath);

  /** Connect to the server and send a PATCH request
      with body and content type
    @param aURLPath     Url to request
    @param aContentType Content type of request body
    @param aBody        Body of the request
    @return 0 if successful, else error
  */
  int patch(const char* aURLPath, const char* aContentType, const char* aBody);
  int patch(const String& aURLPath, const String& aContentType, const String& aBody);
  int patch(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[]);

  /** Connect to the server and start to send a DELETE request.
    @param aURLPath     Url to request
    @return 0 if successful, else error
  */
  int del(const char* aURLPath);
  int del(const String& aURLPath);

  /** Connect to the server and send a DELETE request
      with body and content type
    @param aURLPath     Url to request
    @param aContentType Content type of request body
    @param aBody        Body of the request
    @return 0 if successful, else error
  */
  int del(const char* aURLPath, const char* aContentType, const char* aBody);
  int del(const String& aURLPath, const String& aContentType, const String& aBody);
  int del(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[]);

  // ================================================================

  /** Connect to the server and start to send the request.
      If a body is provided, the entire request (including headers and body) will be sent
    @param aURLPath        Url to request
    @param aHttpMethod     Type of HTTP request to make, e.g. "GET", "POST", etc.
    @param aContentType    Content type of request body (optional)
    @param aContentLength  Length of request body (optional)
    @param aBody           Body of request (optional)
    @return 0 if successful, else error
  */
  virtual int startRequest(const char* aURLPath,
                    const char* aHttpMethod,
                    const char* aContentType = NULL,
                    int aContentLength = -1,
                    const byte aBody[] = NULL);

  /** Get the HTTP status code contained in the response.
    For example, 200 for successful request, 404 for file not found, etc.
  */
  virtual int responseStatusCode();

  /** Check if a header is available to be read.
    Use readHeaderName() to read header name, and readHeaderValue() to
    read the header value
    MUST be called after responseStatusCode() and before contentLength()
  */
  virtual bool headerAvailable();

  /** Test whether the end of the body has been reached.
    Only works if the Content-Length header was returned by the server
    @return true if we are now at the end of the body, else false
  */
  virtual bool completed();

  /** Return the length of the body.
    Also skips response headers if they have not been read already
    MUST be called after responseStatusCode()
    @return Length of the body, in bytes, or kNoContentLengthHeader if no
    Content-Length header was returned by the server
  */
  virtual int contentLength();

  /** Return the response body as a String
    Also skips response headers if they have not been read already
    MUST be called after responseStatusCode()
    @return response body of request as a String
  */
  virtual String responseBody();

  // From Client
  virtual void stop();
  virtual uint8_t connected() { return iClient->connected(); };
  virtual operator bool() { return bool(iClient); };
  virtual uint32_t httpResponseTimeout() { return iHttpResponseTimeout; };
  virtual void setHttpResponseTimeout(uint32_t timeout) { iHttpResponseTimeout = timeout; };

 protected:
  /** Reset internal state data back to the "just initialised" state
  */
  virtual void resetState();

  // Client we're using
  Client* iClient;
  // Server we are connecting to
  const char* iServerName;
  IPAddress iServerAddress;
  // Port of server we are connecting to
  uint16_t iServerPort;
  // Stores the status code for the response, once known
  int iStatusCode;
  // Stores the value of the Content-Length header, if present
  int iContentLength;
  uint32_t iHttpResponseTimeout;
  bool iConnectionClose;
};

}

#endif
#endif