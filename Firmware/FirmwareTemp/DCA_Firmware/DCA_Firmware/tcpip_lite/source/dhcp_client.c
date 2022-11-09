/**
  DHCP v4 client implementation

  Company:
    Microchip Technology Inc.

  File Name:
    dhcp_client.c

  Summary:
     This is the implementation of DHCP client.

  Description:
    This source file provides the implementation of the DHCP client protocol.

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
#include "ethernet_driver.h"
#include "network.h"
#include "udpv4.h"
#include "udpv4_port_handler_table.h"
#include "ipv4.h"
#include "arpv4.h"
#include "dhcp_client.h"

#define DHCP_OPTION_SIZE 25
#define DHCP_HEADER_SIZE 240

#define DHCP_PACKET_SIZE (DHCP_HEADER_SIZE + DHCP_OPTION_SIZE + DHCP_NAME_SIZE)

#if (DHCP_PACKET_SIZE & 1)
#define ZERO_PAD_DHCP
#define DHCP_REQUEST_LENGTH (DHCP_PACKET_SIZE + 1)
#else
#undef ZERO_PAD_DHCP
#define DHCP_REQUEST_LENGTH DHCP_PACKET_SIZE
#endif

const uint32_t           xidValue      = 0x4D434850;
static uint32_t          dhcpLeaseTime = 0;
static uint32_t          dhcpServerId;
static dhcp_client_state clientState = INIT;
const char               dhcpName[]  = DHCP_NAME;

void DHCP_Request(dhcp_type type)
{
	// creating a DHCP request
	mac48Address_t ethMAC;
	bool           started   = false;
	uint16_t       msglength = DHCP_REQUEST_LENGTH;

	ETH_GetMAC(ethMAC.mac_array);

	if (type == DHCP_REQUEST) {
		msglength += 12;
	} // necessary for Requests

	started = UDP_Start(0xFFFFFFFF, 68, 67);

	if (started == SUCCESS) {
		ETH_Write32(0x01010600); // OP, HTYPE, HLEN, HOPS
		ETH_Write32(xidValue);   // XID : made up number...
		ETH_Write32(0x00008000); // SECS, FLAGS (broadcast)
		if (type == DHCP_REQUEST) {
			ETH_Write32(0);
		} else {
			ETH_Write32(ipv4Address); // CIADDR
		}
		ETH_Write32(0);                     // YIADDR
		ETH_Write32(0);                     // SIADDR
		ETH_Write32(0);                     // GIADDR
		ETH_WriteBlock((char *)&ethMAC, 6); // Hardware Address
		DHCP_WriteZeros(202);               // 0 padding  + 192 bytes of BOOTP padding
		ETH_Write32(0x63825363);            // MAGIC COOKIE - Options to Follow
		// send the options
		ETH_Write8(53);
		ETH_Write8(1);
		ETH_Write8(type); // option 53 - DHCP packet type
		if (type == DHCP_REQUEST) {
			ETH_Write8(54);
			ETH_Write8(4);
			ETH_Write32(dhcpServerId);
			ETH_Write8(50);
			ETH_Write8(4);
			ETH_Write32(ipv4Address);
		}
		ETH_Write8(55);
		ETH_Write8(4);
		ETH_Write8(1);
		ETH_Write8(3);
		ETH_Write8(6);
		ETH_Write8(15);
		ETH_Write8(57);
		ETH_Write8(2);
		ETH_Write16(512); // option 57 - DHCP maximum packet length
		ETH_Write8(12);
		ETH_Write8(DHCP_NAME_SIZE);
		ETH_WriteBlock((char *)dhcpName, DHCP_NAME_SIZE);
		// send option 61 (MAC address)
		ETH_Write8(61);
		ETH_Write8(7);
		ETH_Write8(1);
		ETH_WriteBlock((char *)&ethMAC, 6);
		ETH_Write8(255); // finished
#ifdef ZERO_PAD_DHCP
		ETH_Write8(0); // add a byte of padding to make the total length even
#endif
		UDP_Send();
		if (type == DHCP_DISCOVER)
			clientState = SELECTING;
	}
}

void DHCP_Manage(void) // Call this every second to take care of DHCP activities
{
	if (ethData.linkChange) {
		if (clientState == BOUND && dhcpLeaseTime) {
			dhcpServerId = 0xFFFFFFFF;
			DHCP_Request(DHCP_REQUEST);
		} else {
			clientState = INIT;
			DHCP_Request(DHCP_DISCOVER);
		}
		ethData.linkChange = false;
	} else if (clientState == INIT || clientState == INIT_REBOOT) {
		clientState = INIT;
		DHCP_Request(DHCP_DISCOVER);
	} else if ((dhcpLeaseTime < 100) && clientState == BOUND) // 100 seconds remaining on my dhcp lease
	{
		DHCP_Request(DHCP_REQUEST);
	} else {
		if (clientState == BOUND && dhcpLeaseTime)
			dhcpLeaseTime--;
		else if (!dhcpLeaseTime)
			clientState = INIT_REBOOT;
	}
}

void DHCP_Handler(int length)
{
	// this function gets called by the UDP port handler for port 67
	uint32_t myip, v;
	uint16_t v16;
	uint16_t ip[4];

	mac48Address_t ethPktMAC;
	mac48Address_t ethMAC;

	ETH_GetMAC(ethMAC.mac_array);

	// accept the offer
	// send a request
	// accept an ACK
	//
	if (length > DHCP_HEADER_SIZE) {
		v16 = ETH_Read16();
		if (v16 == 0x0201) {
			ETH_Dump(2);
			v = ETH_Read32();
			if (v == xidValue) {
				ETH_Dump(8);          // dump SECS, FLAGS & CIADDR
				myip  = ETH_Read32(); // CIADDR
				ip[0] = myip & 0x00FF;
				ip[1] = (myip >> 8) & 0x00FF;
				ip[2] = (myip >> 16) & 0x00FF;
				ip[3] = (myip >> 24) & 0x00FF;
				ETH_Dump(8);                                            // SIADDR, GIADDR, CHADDR
				ETH_ReadBlock((char *)&ethPktMAC, sizeof(ethPktMAC.s)); // This is a DHCP for IPv4

				if (memcmp(&ethPktMAC.s, &ethMAC.s, 6) == 0) // only compare 6 bytes of MAC address.
				{
					ETH_Dump(202);
					if (ETH_Read32() == 0x63825363) {
						length -= 240;
						while (length > 0) {
							// options are here!!!
							uint8_t option, optionLength;
							option       = ETH_Read8();
							optionLength = ETH_Read8();
							length -= 2 + optionLength;
							switch (option) {
							case 1: // subnet mask
								ipv4SubnetMask = ETH_Read32();
								break;
							case 3:                        // router
								ipv4Router = ETH_Read32(); // get MAC address of this router
								break;
							case 6: // DNS List
								// extract up to 2 DNS server IP addresses
								if (optionLength >= 4) {
									ipv4DNS[0] = ETH_Read32();
									optionLength -= 4;
									if (optionLength >= 4) {
										ipv4DNS[1] = ETH_Read32();
										optionLength -= 4;
									}
								}
								break;
							case 51: // lease time
								if (optionLength >= 4) {
									dhcpLeaseTime = ETH_Read32();
									optionLength -= 4;
								}
								break;
							case 54: // DHCP server
								dhcpServerId = ETH_Read32();
								optionLength -= 4;
								break;
							case 53:
								switch (ETH_Read8()) {
								case DHCP_OFFER:
									ipv4Address = myip;
									clientState = REQUESTING;
									break;
								case DHCP_ACK:
									ipv4Address = myip;
									clientState = BOUND;
									// arp_add(dhcp_server_id);
									break;
								case DHCP_NACK:
									clientState = INIT;
									break;
								default:
									break;
								}
								optionLength -= 1;
								break;
							default:
								ETH_Dump(optionLength); // dump any unused bytes
								break;
							}
						}
						if (clientState == REQUESTING)
							DHCP_Request(DHCP_REQUEST);
					}
				} else
					ETH_Dump(length - 34);
			}
		}
	}
}

void DHCP_WriteZeros(uint16_t length)
{
	while (length--) {
		ETH_Write8(0);
	}
}
