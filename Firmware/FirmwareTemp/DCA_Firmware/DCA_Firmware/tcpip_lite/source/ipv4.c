/**
  IPv4 implementation

  Company:
    Microchip Technology Inc.

  File Name:
    ipv4.c

  Summary:
    This is the implementation of IP version 4 stack

  Description:
    This source file provides the implementation of the API for the IP v4 stack.

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
#include "ipv4.h"
#include "arpv4.h"
#include "udpv4.h"
#include "udpv4_port_handler_table.h"
#include "tcpv4.h"
#include "tcpip_types.h"
#include "ethernet_driver.h"
#include "syslog.h"
#include "icmp.h"

uint32_t ipv4Address;
uint32_t remoteIpv4Address;
uint32_t ipv4DNS[2]; // allow a primary & secondary DNS
uint32_t ipv4SubnetMask;
uint32_t ipv4Router;

ipv4Header_t ipv4Header;

#ifdef ENABLE_IP_DEBUG
#define IPV4_SyslogWrite(x) SYSLOG_Write(x)
#define IPV4_Sprintf(a, b, c) sprintf(a, b, c);
#else
#define IPV4_SyslogWrite(x)
#define IPV4_Sprintf(a, b, c)
#endif

/*
 *  Callback to TCP protocol to deliver the TCP packets
 */
extern void TCP_Recv(uint32_t, uint16_t);

void IPV4_Init(void)
{
	ipv4Address = 0;
}

uint16_t IPV4_PseudoHeaderChecksum(uint16_t payloadLen)
{
	ipv4_pseudo_header_t tmp;
	uint8_t              len;
	uint32_t             cksm = 0;
	uint16_t *           v;

	tmp.srcIpAddress = ipv4Header.srcIpAddress;
	tmp.dstIpAddress = ipv4Header.dstIpAddress;
	tmp.protocol     = ipv4Header.protocol;
	tmp.z            = 0;
	tmp.length       = payloadLen;

	len = sizeof(tmp);
	len = len >> 1;

	v = (uint16_t *)&tmp;

	while (len) {
		cksm += *v;
		len--;
		v++;
	}

	// wrap the checksum
	cksm = (cksm & 0x0FFFF) + (cksm >> 16);

	// Return the resulting checksum
	return cksm;
}

error_msg IPV4_Packet(void)
{
	uint16_t cksm   = 0;
	uint16_t length = 0;
	char     msg[40];
	uint8_t  hdrLen;

	// calculate the IPv4 checksum
	cksm = ETH_RxComputeChecksum(sizeof(ipv4Header_t), 0);
	if (cksm != 0) {
		IPV4_SyslogWrite("IP Header wrong cksm");
		return IPV4_CHECKSUM_FAILS;
	}

	ETH_ReadBlock((char *)&ipv4Header, sizeof(ipv4Header_t));
	if (ipv4Header.version != 4) {
		return IP_WRONG_VERSION; // Incorrect version number
	}

	ipv4Header.dstIpAddress = ntohl(ipv4Header.dstIpAddress);
	ipv4Header.srcIpAddress = ntohl(ipv4Header.srcIpAddress);

	if ((ipv4Header.dstIpAddress == ipv4Address) || (ipv4Header.dstIpAddress == IPV4_BROADCAST)) {
		ipv4Header.length = ntohs(ipv4Header.length);

		hdrLen = (uint8_t)(ipv4Header.ihl << 2);

		if (ipv4Header.ihl > 5) {
			// Skip over the IPv4 Options field
			ETH_Dump((uint16_t)(hdrLen - sizeof(ipv4Header_t)));
		}

		switch ((ipProtocolNumbers)ipv4Header.protocol) {
		case ICMP: {
			// calculate and check the ICMP checksum
			IPV4_SyslogWrite("rx icmp");
			length = ipv4Header.length - hdrLen;
			cksm   = ETH_RxComputeChecksum(length, 0);

			if (cksm == 0) {
				ICMP_Receive(&ipv4Header);
			} else {
				IPV4_Sprintf(msg, "icmp wrong cksm : %x", cksm);
				IPV4_SyslogWrite(msg);
				return ICMP_CHECKSUM_FAILS;
			}
		} break;
		case UDP:
			// check the UDP header checksum
			IPV4_SyslogWrite("rx udp");
			length = ipv4Header.length - hdrLen;
			cksm = IPV4_PseudoHeaderChecksum(length); // Calculate pseudo header checksum
			cksm = ETH_RxComputeChecksum(
			    length, cksm); // 1's complement of pseudo header checksum + 1's complement of UDP header, data
			UDP_Receive(cksm);
			break;
		case TCP:
			// accept only uni cast TCP packets
			// check the TCP header checksum
			IPV4_SyslogWrite("rx tcp");
			length = ipv4Header.length - hdrLen;
			cksm   = IPV4_PseudoHeaderChecksum(length);
			cksm   = ETH_RxComputeChecksum(length, cksm);

			// accept only packets with valid CRC Header
			if (cksm == 0) {
				remoteIpv4Address = ipv4Header.srcIpAddress;
				TCP_Recv(remoteIpv4Address, length);
			} else {
				IPV4_SyslogWrite("rx bad tcp cksm");
			}
			break;
		default:
			ETH_Dump(ipv4Header.length);
			break;
		}
		return SUCCESS;
	} else {
		IPV4_Sprintf(msg, "ip address : %X", (unsigned int)ipv4Header.dstIpAddress);
		IPV4_SyslogWrite(msg);
		IPV4_SyslogWrite("DEST IP NOT MATCHED");
		return DEST_IP_NOT_MATCHED;
	}
}

error_msg IPv4_Start(uint32_t destAddress, ipProtocolNumbers protocol)
{
	error_msg ret = ERROR;
	// get the dest mac address
	const mac48Address_t *macAddress;
	uint32_t              targetAddress;

	// Check if we have a valid IPadress and if it's different then 127.0.0.1
	if (((ipv4Address != 0) || (protocol == UDP)) && (ipv4Address != 0x7F000001)) {
		if (destAddress != 0xFFFFFFFF) {
			if ((ipv4SubnetMask & destAddress) == (ipv4SubnetMask & ipv4Address)) // check for subnet
			{
				targetAddress = destAddress;
			} else {
				targetAddress = ipv4Router;
			}

			macAddress = ARPV4_Lookup(targetAddress);
			if (macAddress == 0) {
				ret = ARPV4_Request(targetAddress); // schedule an arp request
				return ret;
			}
		} else {
			macAddress = &broadcastMAC;
		}
		ret = ETH_WriteStart(macAddress, ETHERTYPE_IPV4);
		if (ret == SUCCESS) {
			ETH_Write16(0x4500);     // VERSION, IHL, DSCP, ECN
			ETH_Write16(0);          // total packet length
			ETH_Write32(0xAA554000); // My IPV4 magic Number..., FLAGS, Fragment Offset
			ETH_Write8(IPv4_TTL);    // TTL
			ETH_Write8(protocol);    // protocol
			ETH_Write16(0);          // checksum. set to zero and overwrite with correct value
			ETH_Write32(ipv4Address);
			ETH_Write32(destAddress);

			// fill the pseudo header for checksum calculation
			ipv4Header.srcIpAddress = ipv4Address;
			ipv4Header.dstIpAddress = destAddress;
			ipv4Header.protocol     = protocol;
		}
	}
	return ret;
}

error_msg IPV4_Send(uint16_t payloadLength)
{
	uint16_t  totalLength;
	uint16_t  cksm;
	error_msg ret;

	totalLength = 20 + payloadLength;
	totalLength = ntohs(totalLength);

	// Insert IPv4 Total Length
	ETH_Insert((char *)&totalLength, 2, sizeof(ethernetFrame_t) + offsetof(ipv4Header_t, length));

	cksm = ETH_TxComputeChecksum(sizeof(ethernetFrame_t), sizeof(ipv4Header_t), 0);
	// Insert Ipv4 Header Checksum
	ETH_Insert((char *)&cksm, 2, sizeof(ethernetFrame_t) + offsetof(ipv4Header_t, headerCksm));
	ret = ETH_Send();

	return ret;
}

uint32_t IPV4_GetMyIP(void)
{
	return (ipv4Address);
}
