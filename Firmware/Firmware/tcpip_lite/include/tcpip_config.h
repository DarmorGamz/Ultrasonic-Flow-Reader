/**
  TCP/IP Stack User configuration options header file

  Company:
    Microchip Technology Inc.

  File Name:
    tcpip_config.h

  Summary:
    Header file for TCP/IP Stack User configuration options

  Description:
    This header file provides the TCP/IP Stack User configuration options.

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

#ifndef TCPIP_CONFIG_H
#define TCPIP_CONFIG_H

#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif
#include <sys_arch.h>
/**
  Section: Included Files
*/
#include <tcpip_lite_config.h>

/******************************** MAC Address *********************************/
#define MAC_ADDRESS                                                                                                    \
	{                                                                                                                  \
		0x00, 0xDE, 0xAD, 0x00, 0xBE, 0XEF                                                                             \
	}

/* Build the IPv4 Address*/
#define MAKE_IPV4_ADDRESS(a, b, c, d)                                                                                  \
	((uint32_t)(((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)c << 8) | (uint32_t)d))

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif /* TCPIP_CONFIG_H */
