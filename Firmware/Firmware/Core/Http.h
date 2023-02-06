#ifndef HTTP_H_
#define HTTP_H_

#ifdef __cplusplus
extern "C" {
#endif


/** INCLUDES ******************************************************************/
#include <compiler.h>


/** CONSTANT AND MACRO DEFINITIONS ********************************************/
#define HTTP_RESPONSE_OK                        200
#define HTTP_RESPONSE_REDIRECT                  301
#define HTTP_RESPONSE_TEMP_REDIRECT             302
#define HTTP_RESPONSE_NOTFOUND                  404

#define HTTP_RESPHDR_OK                         "HTTP/1.1 200 OK\r\n"
#define HTTP_RESPHDR_REDIRECT                   "HTTP/1.1 301 Moved Permanently\r\n"
#define HTTP_RESPHDR_TEMP_REDIRECT              "HTTP/1.1 302 Found\r\n"
#define HTTP_RESPHDR_NOTFOUND                   "HTTP/1.1 404 Not Found\r\n"
#define HTTP_RESPHDR_ACCESS_CNTRL               "Access-Control-Allow-Origin: *\r\n"
#define HTTP_RESPHDR_NO_CACHE                   "Cache-Control: no-store, must-revalidate\r\n" \
                                                "Pragma: no-cache\r\n" \
                                                "Expires: 0\r\n"

#define HTTP_HDR_POST                           "POST /%s HTTP/1.1\r\n"
#define HTTP_HDR_HOST                           "Host: %s\r\n"
#define HTTP_HDR_USER_AGENT                     "User-Agent: %s.%s.%s; %s\r\n"
#define HTTP_HDR_USER_AGENT_PUSH                "User-Agent: Eyedro.%s\r\n"
#define HTTP_HDR_CONTENT_TYPE_HTML              "Content-Type: text/html; charset=utf-8\r\n"
#define HTTP_HDR_CONTENT_TYPE_PLAIN             "Content-Type: text/plain; charset=utf-8\r\n"
#define HTTP_HDR_CONTENT_TYPE_JSON              "Content-Type: text/plain\r\n"
#define HTTP_HDR_CONTENT_TYPE_CSV               "Content-Type: text/csv\r\n"
#define HTTP_HDR_CONTENT_TYPE_BINARY            "Content-Type: application/octet-stream\r\n"
#define HTTP_HDR_CONTENT_LENGTH                 "Content-Length:     \r\n"
#define HTTP_HDR_CONTENT_EMPTY                  "Content-Length: 0\r\n"
#define HTTP_HDR_CONNECTION_CLOSE               "Connection: close\r\n"
#define HTTP_HDR_LOCATION                       "Location: "
#define HTTP_HDR_CRLF                           "\r\n"
#define HTTP_HDR_OPTIONS                        "Access-Control-Allow-Origin: *\r\n" \
                                                "Access-Control-Allow-Methods: POST, GET\r\n" \
                                                "Access-Control-Allow-Headers: content-Type, origin, x-requested-with\r\n"

#define HTTP_POST_HDR_BYTES                     128



/** TYPEDEFS, STRUCTURES AND ENUMERATIONS *************************************/
typedef enum {
    HTML_PAGE_NETWORK_SETTINGS,
    HTML_PAGE_SENSORS_SETTINGS,
    HTML_PAGE_PUSH_SETTINGS,
    HTML_PAGE_DASHBOARD,
    HTML_PAGE_GATEWAY
} eHtmlPage;


/** PUBLIC FUNCTION PROTOTYPES ************************************************/
uint16_t Http_BuildRedirect(char* url, bool temporary, char* buffer);
uint16_t Http_Build404(char* buffer);
uint16_t Http_BuildJson(char* msg, char* buffer);
uint16_t Http_PreflightResponse(char* buffer);
uint16_t Http_FetchPage(eHtmlPage page, char* buffer);

uint16_t Http_BuildPostHeaderCommon(char* buffer, char* hostname, bool useFailsafe);
#if defined(DATA_PUSH_ENABLED)
uint16_t Http_BuildPushPostHeaderCommon(char* buffer);
#endif
bool Http_UpdatePostHeaderLength(uint16_t length, char* buffer);


/** DEBUG *********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* HTTP_H_ */