/**
  UDP protocol v4 implementation

  Company:
    Microchip Technology Inc.

  File Name:
    udpv4.c

  Summary:
     This is the implementation of UDP version 4 protocol

  Description:
    This source file provides the implementation of the API for the UDP v4 protocol.

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

/**
 Section: Included Files
 */
#include <sys_arch.h>
#include "ipv4.h"
#include "udpv4.h"
#include "udpv4_port_handler_table.h"
#include "network.h"
#include "tcpip_types.h"

/**
  Section: Macro Declarations
*/

#define ARRAYSIZE(a) (sizeof(a) / sizeof(*(a)))

/**
  Section: Local Variables
*/

static uint16_t     destPort;
static udpHeader_t  udpHeader;
const udp_handler_t UDP_CallBackTable[] = {
#ifdef UDP_DEMO
    {65532, &DEMO_UDP_IdlePkt},
    {DEST_PORT, &DEMO_UDP_Recv},
#endif
    {68, &DHCP_Handler} // a catcher to manage the DHCP process
};

/**
  Section: UDP Library APIs
*/
error_msg UDP_Start(uint32_t destIP, uint16_t srcPort, uint16_t dstPort)
{
	// Start a UDP Packet to Write UDP Header
	error_msg ret = ERROR;

	// Start IPv4 Packet to Write IPv4 Header
	ret = IPv4_Start(destIP, UDP);
	if (ret == SUCCESS) {
		// Start to Count the UDP payload length Bytes
		ETH_ResetByteCount();

		// Write UDP Source Port
		ETH_Write16(srcPort);

		// Write UDP Destination Port
		ETH_Write16(dstPort);

		// Write DataLength; Initially set to '0'
		ETH_Write16(0);

		// Write UDP Checksum; Initially set to '0'
		ETH_Write16(0);
	}
	return ret;
}

error_msg UDP_Send()
{
	uint16_t  udpLength;
	uint16_t  cksm;
	error_msg ret = ERROR;

	udpLength = ETH_GetByteCount();
	udpLength = ntohs(udpLength);
	ETH_Insert((char *)&udpLength, 2, sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) + offsetof(udpHeader_t, length));
	udpLength = htons(udpLength);

	// add the UDP header checksum
	cksm = udpLength + UDP;
	cksm = ETH_TxComputeChecksum(sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) - 8, udpLength + 8, cksm);

	// if the computed checksum is "0" set it to 0xFFFF
	if (cksm == 0) {
		cksm = 0xffff;
	}

	ETH_Insert((char *)&cksm, 2, sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) + offsetof(udpHeader_t, checksum));

	ret = IPV4_Send(udpLength);

	return ret;
}

error_msg UDP_Receive(uint16_t udpcksm) // catch all UDP packets and dispatch them to the appropriate callback
{
	error_msg            ret = ERROR;
	const udp_handler_t *hptr;
	uint16_t             x;

	hptr = UDP_CallBackTable;
	ETH_ReadBlock((char *)&udpHeader, sizeof(udpHeader));

	if ((udpHeader.checksum == 0) || (udpcksm == 0)) {
		udpHeader.dstPort = ntohs(udpHeader.dstPort); // reverse the port number
		destPort          = ntohs(udpHeader.srcPort);
		ret               = PORT_NOT_AVAILABLE;
		// scan the udp port handlers and find a match.
		// call the port handler callback on a match
		for (x = 0; x < ARRAYSIZE(UDP_CallBackTable); x++) {
			if (hptr->portNumber == udpHeader.dstPort) {
				hptr->callBack(ntohs(udpHeader.length) - sizeof(udpHeader));
				ret = SUCCESS;
				break;
			}
			hptr++;
		}
	} else {
		ret = UDP_CHECKSUM_FAILS;
	}
	return ret;
}

uint16_t UDP_ReadBlock(void *data, uint16_t length)
{
	return (ETH_ReadBlock(data, length));
}

/*
void udp_test(int len)    // print the UDP packet
{
    while(len--)
        putch(ETH_Read8());
}
*/

uint16_t UDP_GetDestPort(void)
{
	return (destPort);
}

uint32_t UDP_GetDestIP(void)
{
	return (ipv4Header.srcIpAddress);
}
