/**
  Descriptive File Name

  Company:
    Microchip Technology Inc.

  File Name:
    udpv4.h

  Summary:
    This is the header file for the UDPv4.c

  Description:
    This header file provides the API for the UDP stack.

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

/*
 * File:   udpv4.h
 * Author: joe
 *
 * Created on December 18, 2012, 3:15 PM
 */

#ifndef UDPV4_H
#define UDPV4_H

/**
  Section: Included Files
*/
#include "tcpip_types.h"
#include "dhcp_client.h"
#include "ethernet_driver.h"
#include "sys_arch.h"

/**
  Section: UDP Public Interface Functions
 */

/** Start the UDP Packet.
 * The function will start the IPv4 Packet and Writes the UDP Header.
 * Initially Checksum and Datalength are set to '0'.
 *
 * @param destIP
 *      Destination IP Address in Host Order
 *
 * @param destPort
 *      Destination Port Number in Host Order
 *
 * @param srcPort
 *      Source Port Number in Host Order
 *
 * @return
 *      (1) - UDP Header Write started successfully
 * @return
 *      (!=1) - UDP Header Write Fails
 */
error_msg UDP_Start(uint32_t destIP, uint16_t destPort, uint16_t srcPort);

/**Send UDP Packet.
 * The function will Insert the the UDP Datalength, Compute and Insert the UDP checksum.
 *
 * @param
 *      None
 *
 * @return
 *      1 - UDP Packet was sent Successfully
 * @return
 *      (!=1) - UDP Packet sent Fails
 */
error_msg UDP_Send();

/**Writes 1 byte of UDP Data in Host Order.
 *
 * @param data
 *      1 byte of data
 */
#define UDP_Write8 ETH_Write8

/**Writes 2 bytes of UDP Data in Host Order.
 *
 * @param data
 *      2 byte of data
 */
#define UDP_Write16 ETH_Write16

/**Writes 4 bytes of UDP Data in Host Order.
 *
 * @param data
 *      4 byte of data
 */
#define UDP_Write32 ETH_Write16

/**This function Writes the number of bytes passed as length from the pointer to the data in Network Order.
 *
 * @param data
 *      A pointer to the data which is to be sent.
 *
 * @param length
 *      The length of the data.
 */
#define UDP_WriteBlock ETH_WriteBlock

/**Reads 1 byte of data in Host Order.
 *
 * @return
 *      1 byte of data
 */
#define UDP_Read8 ETH_Read8

/**Reads 2 bytes of data in Host Order.
 *
 * @return
 *      2 bytes of data
 */
#define UDP_Read16 ETH_Read16

/**Reads 4 bytes of data in Host Order.
 *
 * @return
 *      4 bytes of data
 */
#define UDP_Read32 ETH_Read32

/**This function reads the number of bytes passed as a length and save to the pointer to the data in Network Order.
 *
 * @param data
 *      Pointer to the data
 *
 * @param length
 *      Length of data
 * @return
 *      Number of bytes read
 */
uint16_t UDP_ReadBlock(void *data, uint16_t length);

/** This function gets the Source Port Number of the Received UDP Packet in Host order.
 *
 * @return
 *      16-bit Port Number
 */
uint16_t UDP_GetDestPort(void);

/**This function gets the Source IP address of the Received UDP Packet in Host Order.
 *
 * @return
 *      32-bit IP address
 */
uint32_t UDP_GetDestIP(void); // get the IP aaddress of the received UDP Packet

/**Receives UDP Packet
 * This function matches the port number to the corresponding handler function.
 *
 * @param udpcksm
 * @return
 *      (1) - Received UDP Header Checksum is Correct and Port Available.
 * @return
 *      (!=1) - Received UDP Header Checksum is Incorrect or Port Unavailable.
 */
error_msg UDP_Receive(uint16_t udpcksm); // catch all UDP packets and dispatch them to the appropriate callback

#endif /* UDPV4_H */
