/**
  DHCPv4 Client Protocol header file

  Company:
    Microchip Technology Inc.

  File Name:
    dhcp_client.h

  Summary:
    This is the header file for dhcp_client.c

  Description:
    This header file provides the API for the DHCP client protocol

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

#ifndef DHCP_CLIENT_H
#define DHCP_CLIENT_H

#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif

/**
  Section: Included Files
*/
#include "sys_arch.h"
#include "tcpip_types.h"
#include "tcpip_config.h"

/**
  Section: Enumeration Definition
*/
typedef enum {
	DHCP_DISCOVER = 1,
	DHCP_OFFER,
	DHCP_REQUEST,
	DHCP_DECLINE,
	DHCP_ACK,
	DHCP_NACK,
	DHCP_RELEASE,
	DHCP_INFORM,
	DHCP_FORCERENEW,
	DHCP_LEASEQUERY,
	DHCP_LEASEUNASSIGNED,
	DHCP_LEASEUNKNOWN,
	DHCP_LEASEACTIVE,
	DHCP_BULKLEASEQUERY,
	DHCP_LEASEQUERYDONE
} dhcp_type;

typedef enum { INIT = 1, INIT_REBOOT, SELECTING, REQUESTING, BOUND } dhcp_client_state;

/**
  Section: DHCP Client Functions
 */

/**This function will sends DHCP packet to the DHCP server.
 * The type of packet is passed as a parameter.
 *
 * @param dhcp_type
 *      Type of DHCP packet.
 */
void DHCP_Request(dhcp_type);

/**This fucntion receives and process the DHCP packet.
 * The length of the packet is passed as paramater.
 *
 * @param length
 *      Length of the received DHCP packet.
 */
void DHCP_Handler(int);

/**This function updates the DHCP status at least for every one second.
 *
 */
void DHCP_Manage(void);

/**This function writes zeroes for the number of bytes passed.
 *
 * @param length
 *      Number of bytes.
 */
void DHCP_WriteZeros(uint16_t length);

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif /* DHCP_CLIENT_H */
