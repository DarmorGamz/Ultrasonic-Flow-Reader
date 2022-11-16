/**
  ICMP protocol implementation

  Company:
    Microchip Technology Inc.

  File Name:
    icmp.c

  Summary:
     This is the implementation of ICMP version 4 stack.

  Description:
    This source file provides the implementation of the API for the ICMP Echo Ping Request/Reply.

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

#include <sys_arch.h>
#include "network.h"
#include "tcpip_types.h"
#include "ethernet_driver.h"
#include "ipv4.h"
#include "icmp.h"

/**
 * ICMP packet receive
 * @param ipv4_header
 * @return
 */
error_msg ICMP_Receive(ipv4Header_t *ipv4Hdr)
{
	icmpHeader_t icmpHdr;
	error_msg    ret = ERROR;
	ETH_ReadBlock((char *)&icmpHdr, sizeof(icmpHeader_t));
	ETH_SaveRDPT();

	switch (ntohs((icmpTypeCodes_t)icmpHdr.typeCode)) {
	case ECHO_REQUEST: {
		ret = ICMP_EchoReply(&icmpHdr, ipv4Hdr);
	} break;
	default:
		break;
	}

	return ret;
}

/**
 * ICMP Packet Start
 * @param icmp_header
 * @param dest_address
 * @param protocol
 * @param payload_length
 * @return
 */

error_msg ICMP_EchoReply(icmpHeader_t *icmpHdr, ipv4Header_t *ipv4Hdr)
{
	uint16_t  cksm = 0;
	error_msg ret  = ERROR;

	ret = IPv4_Start(ipv4Hdr->srcIpAddress, ipv4Hdr->protocol);
	if (ret == SUCCESS) {
		uint16_t icmp_cksm_start;
		uint16_t ipv4PayloadLength = ipv4Hdr->length - sizeof(ipv4Header_t);

		ipv4PayloadLength = ipv4Hdr->length - (uint16_t)(ipv4Hdr->ihl << 2);

		ETH_Write16(ECHO_REPLY);
		ETH_Write16(0); // checksum
		ETH_Write16(ntohs(icmpHdr->identifier));
		ETH_Write16(ntohs(icmpHdr->sequence));

		// copy the next N bytes from the RX buffer into the TX buffer
		ret = ETH_Copy(ipv4PayloadLength - sizeof(icmpHeader_t));
		if (ret == SUCCESS) // copy can timeout in heavy network situations like flood ping
		{
			ETH_SaveRDPT();
			// compute a checksum over the ICMP payload
			cksm            = sizeof(ethernetFrame_t) + sizeof(ipv4Header_t);
			icmp_cksm_start = sizeof(ethernetFrame_t) + sizeof(ipv4Header_t);
			cksm            = ETH_TxComputeChecksum(icmp_cksm_start, ipv4PayloadLength, 0);
			ETH_Insert((char *)&cksm,
			           sizeof(cksm),
			           sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) + offsetof(icmpHeader_t, checksum));
			ret = IPV4_Send(ipv4PayloadLength);
		}
	}
	return ret;
}
