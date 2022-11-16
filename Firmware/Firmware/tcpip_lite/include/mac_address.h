/**
  MAC Address Header File

  Company:
    Microchip Technology Inc.

  File Name:
    mac_address.h

  Summary:
    Public Interface definition for MAC Address access.

  Description:
    This files include definitions of public interface for accessing the MAC Address.

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

#ifndef MAC_ADDRESS_H
#define MAC_ADDRESS_H

#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif

#include "sys_arch.h"
#include "tcpip_types.h"

typedef union {
	uint8_t mac_array[6];
	struct {
		uint8_t byte1, byte2, byte3, byte4, byte5, byte6;
	} s;
} mac48Address_t;

extern const mac48Address_t broadcastMAC;
extern const mac48Address_t macAddress;

const mac48Address_t *MAC_getAddress(void);

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif /* MAC_ADDRESS_H */
