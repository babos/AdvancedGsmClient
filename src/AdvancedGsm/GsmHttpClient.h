#ifndef Advanced_GsmHttpClient_h
#define Advanced_GsmHttpClient_h

#include "../api/HttpClient.h"

class GsmHttpClient : public HttpClient
{
 public:
  GsmHttpClient(Client& aClient, const char* aServerName, uint16_t aServerPort = HttpPort);
  GsmHttpClient(Client& aClient, const String& aServerName, uint16_t aServerPort = HttpPort);
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
                    const byte aBody[] = NULL);

  virtual int responseStatusCode();

  virtual bool headerAvailable();

  virtual bool completed();

  virtual int contentLength();

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

#endif
