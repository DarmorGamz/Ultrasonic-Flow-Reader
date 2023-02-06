/**
  UDP Port Handler header file

  Company:
    Microchip Technology Inc.

  File Name:
    udpv4_port_handler_table.h

  Summary:
    This is the header file udpv4.c

  Description:
    This file consists of the UDP call back table.

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

#ifndef UDPV4_PORT_HANDLER_TABLE_H
#define UDPV4_PORT_HANDLER_TABLE_H

#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif

#include "tcpip_types.h"
#include "dhcp_client.h"
#include "sys_arch.h"
#ifdef UDP_DEMO
#include "udp_demo.h"
#endif

// void udp_test(int len);    // print the UDP packet

#ifdef UDP_DEMO
extern void DEMO_UDP_Recv(int len);
extern void DEMO_UDP_IdlePkt(int length);
#endif

// UDP callback added by Nick - defined in Ethernet.c
extern void DNS_Handler(int len);

typedef struct {
	uint16_t                portNumber;
	ip_receive_function_ptr callBack;
} udp_handler_t;

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif /* UDPV4_PORT_HANDLER_TABLE_H */
