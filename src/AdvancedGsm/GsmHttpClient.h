#ifndef Advanced_GsmHttpClient_h
#define Advanced_GsmHttpClient_h

#include "../api/HttpClient.h"
#include "GsmTcpClient.h"
#include "GsmLog.h"

enum UrlScheme {
  SCHEME_UNKNOWN = 0,
  SCHEME_HTTP = 1,
  SCHEME_HTTPS = 2,
};

// const char* GSM_PREFIX_HTTP = "http://";
// const int8_t GSM_PREFIX_HTTP_LENGTH = 7;
// const char* GSM_PREFIX_HTTPS = "https://";
// const int8_t GSM_PREFIX_HTTPS_LENGTH = 8;

#define GSM_PREFIX_HTTP "http://"
#define GSM_PREFIX_HTTP_LENGTH 7
#define GSM_PREFIX_HTTPS "https://"
#define GSM_PREFIX_HTTPS_LENGTH 8

class GsmHttpClient : public HttpClient
{
 public:
  GsmHttpClient(GsmTcpClient& aClient, const char* aServerName, uint16_t aServerPort = HttpPort);
  //GsmHttpClient(GsmTcpClient& aClient, const String& aServerName, uint16_t aServerPort = HttpPort);
  //HttpClient(Client& aClient, const IPAddress& aServerAddress, uint16_t aServerPort = kHttpPort);

  int get(const char* aURLPath);
  int get(const String& aURLPath);
  int post(const char* aURLPath);
  int post(const String& aURLPath);
  int post(const char* aURLPath, const char* aContentType, const char* aBody);
  int post(const String& aURLPath, const String& aContentType, const String& aBody);
  int post(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[]);
  int put(const char* aURLPath);
  int put(const String& aURLPath);
  int put(const char* aURLPath, const char* aContentType, const char* aBody);
  int put(const String& aURLPath, const String& aContentType, const String& aBody);
  int put(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[]);
  int patch(const char* aURLPath);
  int patch(const String& aURLPath);
  int patch(const char* aURLPath, const char* aContentType, const char* aBody);
  int patch(const String& aURLPath, const String& aContentType, const String& aBody);
  int patch(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[]);
  int del(const char* aURLPath);
  int del(const String& aURLPath);
  int del(const char* aURLPath, const char* aContentType, const char* aBody);
  int del(const String& aURLPath, const String& aContentType, const String& aBody);
  int del(const char* aURLPath, const char* aContentType, int aContentLength, const byte aBody[]);

  // ================================================================

  virtual int startRequest(const char* aURLPath,
                    const char* aHttpMethod,
                    const char* aContentType = NULL,
                    int aContentLength = -1,
                    const byte aBody[] = NULL) = 0;

  virtual int responseStatusCode();

//  virtual bool headerAvailable();

  virtual bool completed();

  virtual int contentLength();

  virtual String responseBody();

  // From Client
  virtual void stop() = 0;
  virtual uint8_t connected() { return iClient->connected(); };
  virtual operator bool() { return bool(iClient); };
  virtual uint32_t httpResponseTimeout() { return iHttpResponseTimeout; };
  virtual void setHttpResponseTimeout(uint32_t timeout) { iHttpResponseTimeout = timeout; };

 protected:
  /** Reset internal state data back to the "just initialised" state
  */
  virtual void resetState();

  //GsmModem& modem;
  UrlScheme scheme;

  // Client we're using
  GsmTcpClient* iClient;
  // Server we are connecting to
  const char* server_name;
  IPAddress iServerAddress;
  // Port of server we are connecting to
  uint16_t server_port;
  // Stores the status code for the response, once known
  int iStatusCode;
  // Stores the value of the Content-Length header, if present
  int iContentLength;
  uint32_t iHttpResponseTimeout;
  bool iConnectionClose;
};

#endif
