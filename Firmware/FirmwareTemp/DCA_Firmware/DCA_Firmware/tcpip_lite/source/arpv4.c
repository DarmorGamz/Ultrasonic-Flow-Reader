/**
 ARP v4 implementation

  Company:
    Microchip Technology Inc.

  File Name:
    arpv4.c

  Summary:
    This is the implementation of ARP version 4 stack

  Description:
    This source file provides the implementation of the API for the ARP v4 stack.

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
#include "tcpip_types.h"
#include "network.h"
#include "arpv4.h"
#include "ethernet_driver.h"
#include "mac_address.h"
#include "ipv4.h" // needed to know my IP address
#include "syslog.h"
#include "tcpip_config.h"

COMPILER_PACK_SET(1)
typedef struct {
	uint16_t       htype; // Hardware Type
	uint16_t       ptype; // Protocol Type
	uint8_t        hlen;  // Hardware Address Length
	uint8_t        plen;  // Protocol Address Length
	uint16_t       oper;  // Operation
	mac48Address_t sha;   // Sender Hardware Address
	uint32_t       spa;   // Sender Protocol Address
	mac48Address_t tha;   // Target Hardware Address
	uint32_t       tpa;   // Target Protocol Address
} arpHeader_t;

#define ARP_REQUEST 1
#define ARP_REPLY 2
#define ARP_NAK 10

// ARP database
typedef struct {
	mac48Address_t macAddress;
	uint32_t       ipAddress;
	uint16_t       protocolType;
	uint8_t        age; // replace oldest entry with new data as required.
} arpMap_t;

arpMap_t arpMap[ARP_MAP_SIZE]; // maintain a small database of IP address & MAC addresses

/**
 * ARP Initialization
 */

void ARPV4_Init(void)
{
	for (uint8_t x = 0; x < sizeof(arpMap); x++)
		((char *)arpMap)[x] = 0;
}

/**
 * ARP Packet received
 * @return
 */

error_msg ARPV4_Packet(void)
{
	arpHeader_t header;
	arpMap_t *  entryPointer;
	bool        mergeFlag;
	uint16_t    length;
	error_msg   ret;

	ret = ERROR;

	length = ETH_ReadBlock((char *)&header, sizeof(arpHeader_t));
	if (length == sizeof(arpHeader_t)) {
		// assume that all hardware & protocols are supported
		mergeFlag    = false;
		entryPointer = arpMap;
		// searching the arp table for a matching ip & protocol
		for (uint8_t x = ARP_MAP_SIZE; x > 0; x--) {
			if ((header.spa == entryPointer->ipAddress) && (header.ptype == entryPointer->protocolType)) {
				entryPointer->age          = 0; // reset the age
				entryPointer->macAddress.s = header.sha.s;
				mergeFlag                  = true;
				break;
			}
			entryPointer++;
		}

		if (ipv4Address
		    && (ipv4Address
		        == (((header.tpa >> 24) & 0xff) | ((header.tpa << 8) & 0xff0000) | ((header.tpa >> 8) & 0xff00)
		            | ((header.tpa << 24) & 0xff000000)))) {
			if (!mergeFlag) {
				// find the oldest entry in the table
				entryPointer     = arpMap;
				arpMap_t *arpPtr = arpMap;
				for (uint8_t x = ARP_MAP_SIZE; x != 0; x--) {
					if (entryPointer->age < arpPtr->age) {
						entryPointer = arpPtr;
					}
					/* Increment the pointer to get the next element from the array. */
					arpPtr++;
				}
				// the entry_pointer is now pointing to the oldest entry
				// replace the entry with the received data
				entryPointer->age          = 0;
				entryPointer->macAddress.s = header.sha.s;
				entryPointer->ipAddress    = ntohl(header.spa);
				entryPointer->protocolType = header.ptype;
			}
			if (header.oper == ntohs(ARP_REQUEST)) {
				ret = ETH_WriteStart(&header.sha, ETHERTYPE_ARP);
				if (ret == SUCCESS) {

					header.tha.s = header.sha.s;
					ETH_GetMAC(header.sha.mac_array);
					header.tpa  = header.spa;
					header.spa  = htonl(ipv4Address);
					header.oper = htons(ARP_REPLY);
					ETH_WriteBlock((char *)&header, sizeof(header));

					ret = ETH_Send(); // remember this could fail to send.
				}
			}
		} else {
			ret = ARP_IP_NOT_MATCHED;
		}
	}
	return ret;
}

/**
 * Updates the ARP table
 */

void ARPV4_Update(void) // call me every 10 seconds or so and I will age the arp table.
{
	arpMap_t *entryPointer = arpMap;
	for (uint8_t x = 0; x < ARP_MAP_SIZE; x++) {
		entryPointer->age++;
		entryPointer++;
	}
}

/**
 * ARP send Request
 * @param dest_address
 * @return
 */
error_msg ARPV4_Request(uint32_t destAddress)
{
	error_msg ret;

	ret = ERROR;

	arpHeader_t header;
	header.htype = htons(1);
	header.ptype = htons(0x0800);
	header.hlen  = 6;
	header.plen  = 4;
	header.oper  = htons(ARP_REQUEST);
	ETH_GetMAC(header.sha.mac_array);
	header.spa         = htonl(ipv4Address);
	header.tpa         = htonl(destAddress);
	header.tha.s.byte1 = 0;
	header.tha.s.byte2 = 0;
	header.tha.s.byte3 = 0;
	header.tha.s.byte4 = 0;
	header.tha.s.byte5 = 0;
	header.tha.s.byte6 = 0;

	ret = ETH_WriteStart(&broadcastMAC, ETHERTYPE_ARP);
	if (ret == SUCCESS) {
		ETH_WriteBlock((char *)&header, sizeof(arpHeader_t));
		ret = ETH_Send();
		if (ret == SUCCESS) {
			return MAC_NOT_FOUND;
		}
	}
	return ret;
}

/**
 * ARP Lookup Table
 * @param ip_address
 * @return
 */
mac48Address_t *ARPV4_Lookup(uint32_t ip_address)
{
	arpMap_t *entry_pointer = arpMap;
	uint8_t   x;

	for (x = 0; x < ARP_MAP_SIZE; x++) {
		if (entry_pointer->ipAddress == ip_address)
			return &entry_pointer->macAddress;
		entry_pointer++;
	}
	return 0;
}
