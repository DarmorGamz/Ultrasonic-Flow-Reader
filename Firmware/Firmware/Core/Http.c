/** INCLUDES ******************************************************************/
#include <stdio.h>
#include <string.h>
#include "Http.h"
#include "App/DcaApp.h"




/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
uint16_t Http_BuildRedirect(char* url, bool temporary, char* buffer) {
    uint16_t  totalBytes = 0;
    char    *pDst = buffer;

    if (temporary==true) {
        // HTTP response (302 Found) - with CRLF
        sprintf(pDst, HTTP_RESPHDR_TEMP_REDIRECT);
        pDst += strlen(HTTP_RESPHDR_TEMP_REDIRECT);
    } else {
        // HTTP response (301 Moved Permanently) - with CRLF
        sprintf(pDst, HTTP_RESPHDR_REDIRECT);
        pDst += strlen(HTTP_RESPHDR_REDIRECT);
    }
    // Location for redirection
    sprintf(pDst, HTTP_HDR_LOCATION);
    pDst += strlen(HTTP_HDR_LOCATION);
    // Append the actual location url for redirection
    sprintf(pDst, url);
    pDst += strlen(url);
    sprintf(pDst, HTTP_HDR_CRLF);
    pDst += strlen(HTTP_HDR_CRLF);
    // Content length is 0
    sprintf(pDst, HTTP_HDR_CONTENT_EMPTY);
    pDst += strlen(HTTP_HDR_CONTENT_EMPTY);
    // Close socket
    sprintf(pDst, HTTP_HDR_CONNECTION_CLOSE);
    pDst += strlen(HTTP_HDR_CONNECTION_CLOSE);
    // Close out by adding second CRLF to begin actual response payload
    sprintf(pDst, HTTP_HDR_CRLF);
    pDst += strlen(HTTP_HDR_CRLF);

    // Return the total byte count
    totalBytes = (uint16_t)(pDst - buffer);
    return totalBytes;
}

uint16_t Http_Build404(char* buffer) {
    uint16_t  totalBytes = 0;
    char    *pDst = buffer;

    // HTTP response (404 Not Found) - with CRLF
    sprintf(pDst, HTTP_RESPHDR_NOTFOUND);
    pDst += strlen(HTTP_RESPHDR_NOTFOUND);
    // Content length is 0
    sprintf(pDst, HTTP_HDR_CONTENT_EMPTY);
    pDst += strlen(HTTP_HDR_CONTENT_EMPTY);
    // Close socket
    sprintf(pDst, HTTP_HDR_CONNECTION_CLOSE);
    pDst += strlen(HTTP_HDR_CONNECTION_CLOSE);
    // Close out by adding second CRLF to begin actual response payload
    sprintf(pDst, HTTP_HDR_CRLF);
    pDst += strlen(HTTP_HDR_CRLF);

    // Return the total byte count
    totalBytes = (uint16_t)(pDst - buffer);
    return totalBytes;
}

uint16_t Http_BuildJson(char* json, char* buffer) {
    char    *pContentLength;
    char    *pPayload;
    uint16_t  totalBytes = 0;
    uint16_t  contentBytes = 0;
    char    *pDst = buffer;


    // Standard response headers -----------------------------------------------
    // HTTP response (200 OK) - with CRLF
    sprintf(pDst, HTTP_RESPHDR_OK);
    pDst += strlen(HTTP_RESPHDR_OK);
    // Access-Control-Allow-Origin - with CRLF
    sprintf(pDst, HTTP_RESPHDR_ACCESS_CNTRL);
    pDst += strlen(HTTP_RESPHDR_ACCESS_CNTRL);
    // Content-Type - with CRLF
    sprintf(pDst, HTTP_HDR_CONTENT_TYPE_JSON);
    pDst += strlen(HTTP_HDR_CONTENT_TYPE_JSON);
    // Content-Length - with placeholder for 4-digits and CRLF
    sprintf(pDst, HTTP_HDR_CONTENT_LENGTH);
    pDst += strlen(HTTP_HDR_CONTENT_LENGTH);
    // Capture offset (pre-CRLF) so we can modify later...
    pContentLength = pDst-2;
    // Close socket
    sprintf(pDst, HTTP_HDR_CONNECTION_CLOSE);
    pDst += strlen(HTTP_HDR_CONNECTION_CLOSE);
    // Close out by adding second CRLF to begin actual response payload
    sprintf(pDst, HTTP_HDR_CRLF);
    pDst += strlen(HTTP_HDR_CRLF);
    // Capture offset to the content payload
    pPayload = pDst;


    // JSON payload ---------------------------------------------------------
    sprintf(pDst, "{%s}", json);
    pDst += strlen(json) + 2;


    // Update the content length in the header ---------------------------------
    uint8_t count = 0;
    contentBytes = (uint16_t)(pDst - pPayload);
    if (contentBytes>999)     {count = 4; pContentLength -= 4;}
    else if (contentBytes>99) {count = 3; pContentLength -= 3;}
    else if (contentBytes>9)  {count = 2; pContentLength -= 2;}
    else                      {count = 1; pContentLength -= 1;}
    char acLength[5];
    sprintf(acLength, "%d", contentBytes);
    memcpy(pContentLength, acLength, count);


    // Return the total byte count
    totalBytes = (uint16_t)(pDst - buffer);
    return totalBytes;
}


uint16_t Http_PreflightResponse(char* buffer) {
    uint16_t  totalBytes = 0;
    char    *pDst = buffer;

    // HTTP response (200 OK) - with CRLF
    sprintf(pDst, HTTP_RESPHDR_OK);
    pDst += strlen(HTTP_RESPHDR_OK);
    // OPTIONS
    sprintf(pDst, HTTP_HDR_OPTIONS);
    pDst += strlen(HTTP_HDR_OPTIONS);
    // Content length is 0
    sprintf(pDst, HTTP_HDR_CONTENT_EMPTY);
    pDst += strlen(HTTP_HDR_CONTENT_EMPTY);
    // Close out by adding second CRLF to begin actual response payload
    sprintf(pDst, HTTP_HDR_CRLF);
    pDst += strlen(HTTP_HDR_CRLF);

    // Return the total byte count
    totalBytes = (uint16_t)(pDst - buffer);
    return totalBytes;
}


uint16_t Http_BuildPostHeaderCommon(char* buffer, char* hostname, bool useFailsafe) {
    uint16_t  totalBytes = 0;
    char    *pDst = buffer;

    // POST {path} HTTP/1.1\r\n
    sprintf(g_acBuffer256, HTTP_HDR_POST, "index.php");
    sprintf(pDst, g_acBuffer256); pDst += strlen(g_acBuffer256);
    // Host: {domain}\r\n
    sprintf(g_acBuffer256, HTTP_HDR_HOST, hostname);
    sprintf(pDst, g_acBuffer256); pDst += strlen(g_acBuffer256);
    // Connection: close\r\n
    sprintf(pDst, HTTP_HDR_CONNECTION_CLOSE);
    pDst += strlen(HTTP_HDR_CONNECTION_CLOSE);
	
	sprintf(pDst, HTTP_HDR_CONTENT_TYPE_PLAIN);
    pDst += strlen(HTTP_HDR_CONTENT_TYPE_PLAIN);
	
    // Content-Length: xxxx\r\n\r\n
    // It is critical that this is always at the end and doesn't change size - it gets updated later
    sprintf(pDst, HTTP_HDR_CONTENT_LENGTH);
    pDst += strlen(HTTP_HDR_CONTENT_LENGTH);
    sprintf(pDst, HTTP_HDR_CRLF);
    pDst += strlen(HTTP_HDR_CRLF);

    // Return the total byte count
    totalBytes = (uint16_t)(pDst - buffer);
    return totalBytes;
}
bool Http_UpdatePostHeaderLength(uint16_t length, char* pDst) {
    if (length==0) return false;
    if (pDst==NULL) return false;

    // Wipe any previous length written
    memset(pDst, ' ', 4);
    pDst += 4;

    // Build new length string
    char acLength[5];
    sprintf(acLength, "%d", length);
    uint8_t count = 0;
    if (length>999)     {count = 4;}
    else if (length>99) {count = 3;}
    else if (length>9)  {count = 2;}
    else                {count = 1;}

    // Copy the new length string in the buffer
    pDst -= count;
    memcpy(pDst, acLength, count);

    return true;
}


/** LOCAL (PRIVATE) FUNCTION IMPLEMENTATIONS **********************************/

