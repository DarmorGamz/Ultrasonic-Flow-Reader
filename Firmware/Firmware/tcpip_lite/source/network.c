/**
  Network layer implementation

  Company:
    Microchip Technology Inc.

  File Name:
    network.c

  Summary:
    Network layer handling.

  Description:
    This file provides the network layer implementation for TCP/IP stack.

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
#include "dhcp_client.h"
#include "arpv4.h"
#include "ipv4.h"
#include "tcpv4.h"
#include "ethernet_driver.h"
#include "syslog.h"

time_t arpTimer;

#ifdef ENABLE_NET_DEBUG
#define NET_SyslogWrite(x) SYSLOG_Write(x)
#else
#define NET_SyslogWrite(x)
#endif

void Network_Init(void)
{
	ETH_Init();
	ARPV4_Init();
	IPV4_Init();
	TCP_Init();

	time(&arpTimer);
	arpTimer += 10;
}

void Network_WaitForLink(void)
{
	while (!ETH_CheckLinkUp()) /* NOP() */
		;
}

void Network_Manage(void)
{
	time_t        now;
	static time_t nowPv = 0;

	ETH_EventHandler();
	Network_Read(); // handle any packets that have arrived...

	// manage any outstanding timeouts
	time(&now);
	if (now >= arpTimer) {
		ARPV4_Update();
		arpTimer += 10;
	}
	if (now > nowPv) // at least 1 second has elapsed
	{
		DHCP_Manage(); // update the DHCP status every second
		// is defined as a minimum of 1 seconds in RFC973
		TCP_Update(); // handle timeouts
	}
	nowPv = now;
}

void Network_Read(void)
{
	ethernetFrame_t header;
	char            debug_str[80];

	if (ETH_packetReady()) {
		ETH_NextPacketUpdate();
		ETH_ReadBlock((char *)&header, sizeof(header));
		header.id.type = ntohs(header.id.type); // reverse the type field
		switch (header.id.type) {
		case ETHERTYPE_VLAN:
			NET_SyslogWrite("VLAN Packet Dropped");
			break;
		case ETHERTYPE_ARP:
			NET_SyslogWrite("RX ARPV4 Packet");
			ARPV4_Packet();
			break;
		case ETHERTYPE_IPV4:
			NET_SyslogWrite("RX IPV4 Packet");
			IPV4_Packet();
			break;
		case ETHERTYPE_IPV6:
			NET_SyslogWrite("IPV6 Packet Dropped");
			break;
		default:
			sprintf(debug_str, "%x : %d", header.id.type, header.id.length);
			NET_SyslogWrite(debug_str);
			break;
		}
		ETH_Flush();
	}
}
