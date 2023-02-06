/*******************************************************************************
 *                    C O P Y R I G H T  (c) 2016-2021
 *             E Y E D R O   G R E E N   S O L U T I O N S   I N C.
 *                             All Rights Reserved
 *******************************************************************************
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF EYEDRO GREEN SOLUTIONS INC.
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO
 * THIS SOFTWARE. THE COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR
 * SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *            The copyright notice above does not evidence any
 *           actual or intended publication of such source code.
 *
 * The software is owned by Eyedro Green Solutions Inc. and/or its supplier,
 * and is protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the user to
 * criminal sanctions under applicable laws, as well as to civil liability for
 * the breach of the terms and conditions of this license.
 ******************************************************************************/
/**************************************************************************//**
 * @file        EyedroTcpParser.c
 * @copyright   COPYRIGHT (c) 2021 Eyedro Green Solutions Inc. All rights reserved.
 * @author      Eyedro development team
 * @brief       Contains implementation of Eyedro TCP packet parsing.
 ******************************************************************************/


/** INCLUDES ******************************************************************/
#include <string.h>
#include <stdlib.h>
#include "App/DcaApp.h"


/** VARIABLES *****************************************************************/
static char s_acRequest[16];


/** LOCAL (PRIVATE) CONSTANT AND MACRO DEFINITIONS ****************************/


/** LOCAL (PRIVATE) STRUCTURES AND ENUMERATIONS *******************************/


/** LOCAL (PRIVATE) FUNCTION PROTOTYPES ***************************************/
bool   _FindOkResponse(char* buff);
int    _FindContentLength(char* buff);
char*  _FindContentPointer(char* buff);
uint16_t _FindTokenValue(char *buff, const char *tok, char term, uint16_t max_sz, char *value);


int8_t _BuildPreflightResponse(char* pcTxBuff, uint16_t* pu16TxSize);
int8_t _BuildJsonResponse(char* json, char* pcTxBuff, uint16_t* pu16TxSize);

int8_t _BuildHtmlDashboard(char* pcTxBuff, uint16_t* pu16TxSize);
int8_t _BuildHtmlGateway(char* pcTxBuff, uint16_t* pu16TxSize);
int8_t _BuildHtmlSensorsSettings(char* pcTxBuff, uint16_t* pu16TxSize);
int8_t _BuildHtmlNetworkSettings(char* pcTxBuff, uint16_t* pu16TxSize);


int8_t _Build404(char* pcTxBuff, uint16_t* pu16TxSize);
int8_t _BuildRedirectDashboard(char* pcTxBuff, uint16_t* pu16TxSize);




bool EyedroTcpParser_ParseServerResponse(char* pcResponseBuff, uint16_t u16ResponseSize) {
    uint16_t contentLen;
    char* pContent;
    char* pBuffEnd;
    int payloadBytes;
    char* pSrc;

    // Look for "200 OK" in response
    if (_FindOkResponse(pcResponseBuff)==false) {
        DEBUG_INFO("Unable to find 200 OK. ");
		DEBUG_INFO("%s", pcResponseBuff);
        // Don't idle the socket... return false so it will try to receive more bytes in the event it's just split
        return false;
    }
    // Look for content-length
    contentLen = _FindContentLength(pcResponseBuff);
    if (contentLen==-1) {
        DEBUG_INFO("Unable to find Content-Length. ");
        //EyedroApp_SetLastError(EYEDRO_ERROR_POST_RESP_MISSING_LENGTH);
        // Don't idle the socket... return false so it will try to receive more bytes in the event it's just split
        return false;
    }
    // Find start of content "\r\n\r\n"
    pContent = _FindContentPointer(pcResponseBuff);
    if (pContent==NULL) {
        DEBUG_INFO("Unable to find start of content. ");
        //EyedroApp_SetLastError(EYEDRO_ERROR_POST_RESP_MISSING_PAYLOAD);
        // Don't idle the socket... return false so it will try to receive more bytes in the event it's just split
        return false;
    }
    // Ensure remaining packet size matches content length
    pBuffEnd = pcResponseBuff + u16ResponseSize;
    payloadBytes = (int)(pBuffEnd - pContent);
    if (payloadBytes<contentLen){
        DEBUG_INFO("Content missing or incomplete. ");
        //EyedroApp_SetLastError(EYEDRO_ERROR_POST_RESP_INCOMPLETE_PAYLOAD);
        // Don't idle the socket... return false so it will try to receive more bytes in the event it's just split
        return false;
    }

    // This looks like a valid response from the server, set the flag and timer used to determine lost communication
    //if (EyedroApp_GetStatus()==EYEDRO_STATUS_NO_SERVER_COMM) EyedroApp_SetStatus(EYEDRO_STATUS_OK);

    // Now set the pointer to the payload and pass to the application layer for processing
    pSrc = pContent;
    //EyedroApp_ProcessBinaryPayload(pSrc, contentLen);


    return true;
}


/** LOCAL (PRIVATE) FUNCTION IMPLEMENTATIONS **********************************/
/**************************************************************************//**
 *  Look for 200 OK in the buffer
 *  @param[in]  char*   buff    Pointer to buffer to search "200 OK"
 *  @param[out] None
 *  @return     true=found. false=not found.
 ******************************************************************************/
bool _FindOkResponse(char* buff) {
    char* ptr = NULL;
    ptr = strstr(buff, "200 OK");
    if (ptr!=NULL) return true;
    else return false;
}
/**************************************************************************//**
 *  Pull Content-Length from buffer
 *  @param[in]  char*   buff    Pointer to buffer to search content length
 *  @param[out] None
 *  @return     Content-Length. -1 indicates not found.
 ******************************************************************************/
int _FindContentLength(char* buff)
{
    char value[5];
    int length = -1;
    _FindTokenValue(buff, "Content-Length: ", '\r', sizeof(value), value);
    length = atoi(value);
    return length;
}
/**************************************************************************//**
 *  Find the packet content (following double CRLF)
 *  @param[in]  char*   buff    Pointer to buffer to search content length
 *  @param[out] None
 *  @return     Pointer to beginning of the content. Null if not found
 ******************************************************************************/
char* _FindContentPointer(char* buff)
{
    char* ptr = NULL;
    ptr = strstr(buff, "\r\n\r\n");
    if (ptr!=NULL) ptr += 4;
    return ptr;
}
/**************************************************************************//**
 *  Extract the given token from an input buffer
 *  @param[in]  char*   buff    Pointer to buffer to search
 *  @param[in]  char*   tok     Token to look for
 *  @param[in]  char    term    Character to terminate search
 *  @param[in]  char*   max_sz  Maximum length of response
 *  @param[out] char*   value   Destination buffer
 *  @return     Pointer to beginning of the content. 0 if token not found
 ******************************************************************************/
uint16_t _FindTokenValue(char *buff, const char *tok, char term, uint16_t max_sz, char *value)
{
    uint8_t i = 0;

    // Search for token in string, then advance pointer
    char *pch = strstr(buff, tok);
    pch += strlen(tok);

    // Clear out buffer before parsing
    memset(value, 0, max_sz);

    // Copy value into buffer up to the 'term' character (or null)
    while (i < max_sz) {
        if ((*pch == term) || (*pch == 0)) {break;}
        value[i++] = *pch++;
    }

    /* add terminating 'null' to string */
    value[i] = 0;

    return i;
}



/**************************************************************************//**
 *  Send the preflight (OPTIONS) response
 *  @param[in]  None
 *  @param[out] None
 *  @return     status of the send() call
 ******************************************************************************/
int8_t _BuildPreflightResponse(char* pcTxBuff, uint16_t* pu16TxSize)
{
    uint16_t bytesToSend = 0;
    int8_t result = 1;

    // Prepare the page to be sent
    bytesToSend = Http_PreflightResponse(pcTxBuff);
    *pu16TxSize = bytesToSend;

    return result;
}

/**************************************************************************//**
 *  Send the desired JSON response
 *  @param[in]  uint8_t*  json     Response JSON string
 *  @param[out] None
 *  @return     status of the send() call
 ******************************************************************************/
int8_t _BuildJsonResponse(char* json, char* pcTxBuff, uint16_t* pu16TxSize)
{
    uint16_t bytesToSend = 0;
    int8_t result = 1;

    // Prepare the page to be sent
    bytesToSend = Http_BuildJson(json, pcTxBuff);
    *pu16TxSize = bytesToSend;

    return result;
}

