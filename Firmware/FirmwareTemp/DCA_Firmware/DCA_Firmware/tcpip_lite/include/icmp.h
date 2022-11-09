/**
  ICMPv4 Protocol Header file

  Company:
    Microchip Technology Inc.

  File Name:
    icmp.h

  Summary:
    This is the header file for icmp.c

  Description:
    This header file provides the API for the ICMP protocol

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

#ifndef ICMP_H
#define ICMP_H

#ifdef __cplusplus // Provide C++ Compatibility
extern "C" {
#endif

/**
  Section: ICMP Functions
 */

/**Receives ICMP Packet
 *This function process only ICMP Ping Echo Requests.
 *
 * @param ipv4_hdr
 *      IPv4 Header of the received Packet
 *
 * @return
 *      (1) - SUCCESS
 * @return
 *      (!=1) - FAILURE
 */
error_msg ICMP_Receive(ipv4Header_t *ipv4Hdr);

/**This function sends an Echo Reply Packet to the destination.
 *
 * @param icmp_hdr
 *      ICMP header from the received packet.
 *
 * @param ipv4_hdr
 *      IPv4 Header of the received Packet.
 *
 * @return
 */
error_msg ICMP_EchoReply(icmpHeader_t *icmpHdr, ipv4Header_t *ipv4Hdr);

#ifdef __cplusplus // Provide C++ Compatibility
}
#endif

#endif /* ICMP_H */
