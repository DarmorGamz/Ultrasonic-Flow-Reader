/**
  Network header file

  Company:
    Microchip Technology Inc.

  File Name:
    network.h

  Summary:
    Header file for network helper.

  Description:
    This header file provides the API for the network helper.

 */

/*

CopyRight  [2015] Microchip Technology Inc. and its subsidiaries.  You may use this software
and any derivatives exclusively with Microchip products.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS,
IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF
NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE, OR ITS
INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE
IN ANY APPLICATION.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL
OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED
TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY
OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S
TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.

*/

#ifndef NETWORK_H
#define NETWORK_H

#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif

#include "sys_arch.h"
#include "tcpip_types.h"

#define byteSwap16(a) ((((uint16_t)a & (uint16_t)0xFF00) >> 8) | (((uint16_t)a & (uint16_t)0x00FF) << 8))
#define byteReverse32(a)                                                                                               \
	((((uint32_t)a & (uint32_t)0xff000000) >> 24) | (((uint32_t)a & (uint32_t)0x00ff0000) >> 8)                        \
	 | (((uint32_t)a & (uint32_t)0x0000ff00) << 8) | (((uint32_t)a & (uint32_t)0x000000ff) << 24))

// host to network & network to host macros
#define htons(a) byteSwap16(a)
#define ntohs(a) byteSwap16(a)
#define htonl(a) byteReverse32(a)
#define ntohl(a) byteReverse32(a)

void Network_Init(void);
void Network_Read(void);
void Network_Manage(void);
void Network_WaitForLink(void);

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif /* NETWORK_H */
