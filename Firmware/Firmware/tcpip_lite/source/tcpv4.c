/**
    Transmission Control Protocol (TCP) version4 implementation

  Company:
    Microchip Technology Inc.

  File Name:
    tcpv4.c

  Summary:
    This is the implementation of for TCP version 4 stack

  Description:
    This header file provides the implementation of the API for the TCP v4 stack.

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
#include "ipv4.h"
#include "tcpv4.h"
#include "network.h"
#include "ethernet_driver.h"
#include "tcpip_types.h"
#include "syslog.h"
#include "tcpip_config.h"

tcpTCB_t *     tcbList;
socklistsize_t tcbListSize;
tcpTCB_t *     currentTCB;

static tcpHeader_t tcpHeader;
static uint16_t    nextAvailablePort;
static uint32_t    nextSequenceNumber;

static uint32_t receivedRemoteAddress;
static uint16_t rcvPayloadLen;
static uint16_t tcpMss;

#ifdef ENABLE_TCP_DEBUG
#define TCP_SyslogWrite(x) SYSLOG_Write(x)
#else
#define TCP_SyslogWrite(x)
#endif

static bool TCP_FiniteStateMachine(void);

/** The function will insert a pointer to the new TCB into the TCB pointer list.
 *
 *  @param ptr
 *      pointer to the user allocated memory for the TCB structure
 *
 * @return
 *      Status of the function
 */
static void TCB_Insert(tcpTCB_t *ptr)
{
	// Insert the new TCB at the head of the list.
	// This prevents a list traversal and saves time.
	if (tcbList != NULL) {
		// link this TCB as the previous one for the top of the list
		tcbList->prevTCB = ptr;
	}
	ptr->nextTCB = tcbList; // put the existing list at the end of this tcb...
	tcbList      = ptr;     // put this tcb at the head of the list.
	ptr->prevTCB = NULL;    // make sure that the upstream pointer is empty
	tcbListSize++;
}

/** The function will remove a pointer to a TCB from the TCB pointer list
 *  and connect any TCB's before & behind together.
 *
 *  @param ptr
 *      pointer to the user allocated memory for the TCB structure
 *
 * @return
 *      None
 */
static void TCB_Remove(tcpTCB_t *ptr)
{
	if (tcbListSize > 1) {
		// check if this is the first in list
		if (ptr->prevTCB == NULL) {
			tcbList                               = ptr->nextTCB;
			((tcpTCB_t *)(ptr->nextTCB))->prevTCB = NULL;
		} else {
			((tcpTCB_t *)(ptr->prevTCB))->nextTCB = ptr->nextTCB;
			((tcpTCB_t *)(ptr->nextTCB))->prevTCB = ptr->prevTCB;
		}
		tcbListSize--;
	} else if (tcbListSize == 1) {
		tcbList = NULL;
	}
}

/** Reseting the socket to a known state.
 *
 * @param tcbPtr
 *      pointer to socket/TCB structure
 *
 * @return
 *      None
 */
static void TCB_Reset(tcpTCB_t *tcbPtr)
{
	tcbPtr->destIP       = 0;
	tcbPtr->destPort     = 0;
	tcbPtr->localSeqno   = 0;
	tcbPtr->localLastAck = 0;
	tcbPtr->remoteSeqno  = 0;
	tcbPtr->remoteAck    = 0;
	tcbPtr->remoteWnd    = 0;

	tcbPtr->timeout            = 0;
	tcbPtr->timeoutReloadValue = 0;
	tcbPtr->timeoutsCount      = 0;
	tcbPtr->flags              = 0;

	tcbPtr->localPort   = 0;
	tcbPtr->socketState = SOCKET_CLOSED;
}

/** Check is a pointer to a socket/TCB. If the pointer is in the TCB list
 *  then it is a valid socket.
 *
 * @param tcbPtr
 *      pointer to socket/TCB structure
 *
 * @return
 *      None
 */
static bool TCB_Check(tcpTCB_t *ptr)
{
	tcpTCB_t *     tcbPtr;
	bool           ret   = false;
	socklistsize_t count = 0;

	if ((tcbList != NULL) && (ptr != NULL)) {
		// search for ptr into the active TCB/sockets list
		tcbPtr = tcbList;
		while ((tcbPtr != NULL) && (count < tcbListSize)) {
			if (tcbPtr == ptr) {
				ret = true;
				break;
			} else {
				tcbPtr = tcbPtr->nextTCB;
				count++;
			}
		}
	}
	return ret;
}

/** Internal function of the TCP Stack to send an TCP packet.
 *
 * @param tcbPtr
 *      pointer to the socket/TCB structure
 *
 * @return
 *      true - The buffer was send successfully
 * @return
 *      false - Send buffer fails.
 */
static bool TCP_Snd(tcpTCB_t *tcbPtr)
{
	bool        ret = false;
	tcpHeader_t txHeader;
	uint16_t    payloadLength;
	uint16_t    cksm;
	uint16_t    tcpDataLength;
	uint8_t *   data;

	txHeader.sourcePort = htons(tcbPtr->localPort);
	txHeader.destPort   = htons(tcbPtr->destPort);

	txHeader.sequenceNumber = htonl(tcbPtr->localSeqno);

	txHeader.ackNumber = htonl(tcbPtr->remoteAck); // ask for next packet

	txHeader.ns         = 0; // make sure we clean unused fields
	txHeader.reserved   = 0; // make sure we clean unused fields
	txHeader.dataOffset = 5; // We don't support options for now
	txHeader.windowSize = htons(tcbPtr->localWnd);
	txHeader.checksum   = 0;
	txHeader.urgentPtr  = 0;

	if ((tcbPtr->flags) & (TCP_SYN_FLAG | TCP_RST_FLAG)) {
		tcpDataLength = 0; // SYN and RST packets doesn't have any payload
	} else {
		tcpDataLength = tcbPtr->bytesToSend;

		if (tcpDataLength != 0) {
			if (tcpDataLength > tcbPtr->remoteWnd) {
				tcpDataLength = tcbPtr->remoteWnd;
			}

			if (tcpDataLength > tcbPtr->mss) {
				tcpDataLength = tcbPtr->mss;
			}
			data = tcbPtr->txBufferPtr;

			// update the pointer to the next byte that needs to be sent
			tcbPtr->txBufferPtr = tcbPtr->txBufferPtr + tcpDataLength;
			tcbPtr->bytesToSend = tcbPtr->bytesToSend - tcpDataLength;

			if (tcbPtr->bytesToSend == 0) {
				tcbPtr->flags |= TCP_PSH_FLAG;
			}
		}
	}
	// update the TCP Flags
	txHeader.flags = tcbPtr->flags;
	payloadLength  = sizeof(tcpHeader_t) + tcpDataLength;

	ret = IPv4_Start(tcbPtr->destIP, TCP);
	if (ret) {
		ETH_WriteBlock((uint8_t *)&txHeader, sizeof(tcpHeader_t));

		if (tcpDataLength > 0) {
			ETH_WriteBlock(data, tcpDataLength);
		}

		cksm = payloadLength + TCP;
		// Calculate the TCP checksum
		cksm = ETH_TxComputeChecksum(sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) - 8, payloadLength + 8, cksm);
		ETH_Insert((char *)&cksm, 2, sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) + offsetof(tcpHeader_t, checksum));

		ret = IPV4_Send(payloadLength);
	}

	// The packet wasn't transmitted
	// Use the timeout to retry again later
	if (ret == false) {
		// make sure we keep the remaining timeouts and skip this send  that failed
		// try at least once
		tcbPtr->timeoutsCount = tcbPtr->timeoutsCount + 1;

		if (tcbPtr->timeout == 0) {
			tcbPtr->timeout = TCP_START_TIMEOUT_VAL;
		}
	} else {
		// if the packet was sent increment the Seqno.
		tcbPtr->localSeqno = tcbPtr->localSeqno + tcpDataLength;
		TCP_SyslogWrite("tcp_packet sent");
	}

	return ret;
}

/** Internal function of the TCP Stack. Will copy the TCP packet payload to
 * the socket RX buffer. This function will also send the ACK for
 * the received packet and any ready to be send data.
 *
 * @param len
 *      length of the payload received
 *
 * @return
 *      true - The payload was copied to RX buffer successfully
 * @return
 *      false - Copying the payload failed.
 */
bool TCP_PayloadSave(uint16_t len)
{
	bool     ret = false;
	uint16_t buffer_size;

	// check if we have a valid buffer
	if (currentTCB->rxBufState == RX_BUFF_IN_USE) {
		// make sure we have enough space
		if (currentTCB->localWnd >= len) {
			buffer_size = len;
		} else {
			buffer_size = currentTCB->localWnd;
		}

		ETH_ReadBlock(currentTCB->rxBufferPtr, buffer_size);
		currentTCB->rxBufferPtr = currentTCB->rxBufferPtr + buffer_size;

		// update the local window to inform the remote of the available space
		currentTCB->localWnd  = currentTCB->localWnd - buffer_size;
		currentTCB->remoteAck = currentTCB->remoteSeqno + buffer_size;

		// prepare to send the ACK and maybe some data if there are any
		currentTCB->flags = TCP_ACK_FLAG;
		TCP_Snd(currentTCB);
		ret = true;
	}
	return ret;
}

/** This function will read and parse the OPTIONS field in TCP header.
 *  Each TCP header could have the options field.
 *  we will read only the ones that has SYN or SYN + ACK
 *  for the other TCP headers the options will be skipped.
 *
 * @param
 *      None
 *
 * @return
 *      true - parsing the options filed was successfully
 * @return
 *      false - parsing the options filed was failed.
 */
static bool TCP_ParseTCPOptions(void)
{
	uint8_t  opt;
	uint16_t tcpOptionsSize;
	bool     ret;

	ret = false;
	// Check for the option fields in TCP header
	tcpOptionsSize = (uint16_t)((tcpHeader.dataOffset << 2) - sizeof(tcpHeader_t));

	if (tcpOptionsSize > 0) {
		// RFC 1122, page 85, Section 4.2.2.6  Maximum Segment Size Option: RFC-793 Section 3.1
		// more explanations in RFC-6691
		tcpMss = 536;
		// parse the option only for SYN segments
		if (tcpHeader.syn) {
			// Parse for the TCP MSS option, if present.
			while (tcpOptionsSize--) {
				opt = ETH_Read8();
				switch (opt) {
				case TCP_EOP:
					// End of options.
					if (tcpOptionsSize) {
						// dump remaining unused bytes
						ETH_Dump(tcpOptionsSize);
						tcpOptionsSize = 0;
					}
					ret = true;
					break;
				case TCP_NOP:
					// NOP option.
					break;
				case TCP_MSS:
					if (tcpOptionsSize >= 3) // at least 3 more bytes
					{
						opt = ETH_Read8();
						if (opt == 0x04) {
							// An MSS option with the right option length.
							tcpMss = ETH_Read16(); // value returned in host endianess
							// Advance to the next option
							tcpOptionsSize = tcpOptionsSize - 3;

							// Limit the mss to the configured TCP_MAX_SEG_SIZE
							if (tcpMss > TCP_MAX_SEG_SIZE) {
								tcpMss = TCP_MAX_SEG_SIZE;
							}
							// so far so good
							ret = true;
						} else {
							// Bad option size length
							TCP_SyslogWrite("tcp_parseopt: bad option size length");
							// unexpected error
							tcpOptionsSize = 0;
						}
					} else {
						// unexpected error
						tcpOptionsSize = 0;
						ret            = false;
					}
					break;
				default:
					TCP_SyslogWrite("tcp_parseopt: other");
					opt = ETH_Read8();
					tcpOptionsSize--;

					if (opt > 1) // this should be at least 2 to be valid
					{
						// adjust for the remaining bytes for the current option
						opt = opt - 2;
						if (opt <= tcpOptionsSize) {
							// All other options have a length field, so that we easily can skip them.
							ETH_Dump(opt);
							tcpOptionsSize = tcpOptionsSize - opt;
							ret            = true;
						} else {
							TCP_SyslogWrite("tcp_parseopt: bad option length");
							// the options are malformed and we don't process them further.
							tcpOptionsSize = 0;
							ret            = false;
						}
					} else {
						TCP_SyslogWrite("tcp_parseopt: bad length");
						// If the length field is zero, the options are malformed
						// and we don't process them further.
						tcpOptionsSize = 0;
						ret            = false;
					}
					break;
				}
			}
		} else // jump over the Options from TCP header
		{
			ETH_Dump(tcpOptionsSize);
			ret = true;
		}
	} else {
		ret = true;
	}

	return ret;
}

/** This function will be called by the IP layer for each received TCP packet.
 * It will identify the destination socket and also parse the TCP header.
 *
 * @param remoteAddress
 *      Source IP address for the received TCP packet
 *
 * @param length
 *      Length of the TCP payload
 *
 * @return
 *      None
 */
void TCP_Recv(uint32_t remoteAddress, uint16_t length)
{
	tcpTCB_t *     tcbPtr;
	socklistsize_t count = 0;

	tcbPtr = NULL;
	// make sure we will not reuse old values
	receivedRemoteAddress = 0;
	rcvPayloadLen         = 0;

	ETH_ReadBlock((char *)&tcpHeader, sizeof(tcpHeader_t));

	currentTCB = NULL;

	// quick check on destination port
	if ((tcpHeader.destPort != 0) && (tcpHeader.sourcePort != 0)) {
		tcpHeader.sourcePort = ntohs(tcpHeader.sourcePort);
		tcpHeader.destPort   = ntohs(tcpHeader.destPort);

		// search for active TCB
		tcbPtr = tcbList;
		while ((tcbPtr != NULL) && (count < tcbListSize)) {
			if (tcpHeader.destPort == tcbPtr->localPort) {
				currentTCB = tcbPtr;
				break;
			} else {
				tcbPtr = tcbPtr->nextTCB;
				count++;
			}
		}

		if (currentTCB != NULL) {
			if ((tcpHeader.sourcePort == currentTCB->destPort) || (currentTCB->destIP == 0)) {
				// we will need this if the port is in listen mode
				// or to check for the correct TCB
				receivedRemoteAddress = remoteAddress;
				rcvPayloadLen         = length - (uint16_t)(tcpHeader.dataOffset << 2);

				// check/skip the TCP header options
				if (TCP_ParseTCPOptions() == true) {
					// we got a packet
					// sort out the events
					if (tcpHeader.syn) {
						if (tcpHeader.ack) {
							TCP_SyslogWrite("found syn&ack");
							currentTCB->connectionEvent = RCV_SYNACK;
						} else {
							TCP_SyslogWrite("found syn");
							currentTCB->connectionEvent = RCV_SYN;
						}
					} else if (tcpHeader.fin) {
						if (tcpHeader.ack) {
							TCP_SyslogWrite("found fin&ack");
							currentTCB->connectionEvent = RCV_FINACK;
						} else {
							TCP_SyslogWrite("found fin");
							currentTCB->connectionEvent = RCV_FIN;
						}
					} else if (tcpHeader.rst) {
						if (tcpHeader.ack) {
							TCP_SyslogWrite("found rst&ack");
							currentTCB->connectionEvent = RCV_RSTACK;
						} else {
							TCP_SyslogWrite("found rst");
							currentTCB->connectionEvent = RCV_RST;
						}
					} else if (tcpHeader.ack) {
						TCP_SyslogWrite("found ack");
						currentTCB->connectionEvent = RCV_ACK;
					} else {
						TCP_SyslogWrite("confused");
					}
					// convert it here to save some cycles later
					tcpHeader.ackNumber      = ntohl(tcpHeader.ackNumber);
					tcpHeader.sequenceNumber = ntohl(tcpHeader.sequenceNumber);

					TCP_FiniteStateMachine();
				} else {
					TCP_SyslogWrite("pkt dropped: bad options");
				}
			} // we will not send a reset message for PORT not open
		}
	}
}

/** This function is TCP stack state machine.
 *
 *  The function is called each time an event (opening/closing a connection,
 *  receiving TCP packet, etc) occurs.
 *
 * @param
 *      None
 *
 * @return
 *      None
 */
static bool TCP_FiniteStateMachine(void)
{
	uint16_t notAckBytes;
	bool     ret = false;

	tcp_fsm_states_t nextState = currentTCB->fsmState; // default don't change states
	tcpEvent_t       event     = currentTCB->connectionEvent;

	switch (currentTCB->fsmState) {
	case LISTEN:
		switch (event) {
		case RCV_SYN:
			TCP_SyslogWrite("LISTEN: rx_syn");
			// Start the connection on the TCB

			currentTCB->destIP   = receivedRemoteAddress;
			currentTCB->destPort = tcpHeader.sourcePort;

			// TO DO modify the start seq no to be "random"
			currentTCB->localLastAck = 0;

			currentTCB->remoteSeqno = tcpHeader.sequenceNumber;
			currentTCB->remoteAck   = currentTCB->remoteSeqno + 1; // ask for next packet

			// save data from TCP header
			currentTCB->remoteWnd = ntohs(tcpHeader.windowSize);
			currentTCB->mss       = tcpMss;

			// create and send a SYN+ACK packet
			currentTCB->flags              = TCP_SYN_FLAG | TCP_ACK_FLAG;
			currentTCB->timeout            = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutReloadValue = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutsCount      = TCP_MAX_SYN_RETRIES;

			TCP_Snd(currentTCB);
			nextState = SYN_RECEIVED;
			break;
		case CLOSE:
			TCP_SyslogWrite("LISTEN: close");
			nextState = CLOSE;
			TCB_Reset(currentTCB);
			break;
		default:
			// for all other cases the packet is invalid and will be discarded
			break;
		}
		break;
	case SYN_SENT:
		switch (event) {
		case RCV_SYN:
			TCP_SyslogWrite("SYN_SENT: rx_syn");
			// Simultaneous open
			currentTCB->remoteSeqno = tcpHeader.sequenceNumber;
			currentTCB->remoteAck   = tcpHeader.sequenceNumber + 1; // ask for next packet

			// save data from TCP header
			currentTCB->remoteWnd = ntohs(tcpHeader.windowSize);
			currentTCB->mss       = tcpMss;

			// create and send a ACK packet
			currentTCB->timeout            = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutReloadValue = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutsCount      = TCP_MAX_SYN_RETRIES;
			currentTCB->flags              = TCP_SYN_FLAG | TCP_ACK_FLAG;

			TCP_Snd(currentTCB);

			// Go to SYN_RECEIVED and waiting for the ack
			nextState = SYN_RECEIVED;
			break;
		case RCV_SYNACK:
			TCP_SyslogWrite("SYN_SENT: rx_synack");

			currentTCB->timeout = 0;

			if ((currentTCB->localSeqno + 1) == tcpHeader.ackNumber) {
				// create and send a ACK packet
				currentTCB->localSeqno = currentTCB->localSeqno + 1;
				currentTCB->flags      = TCP_ACK_FLAG;

				// save data from TCP header
				currentTCB->remoteSeqno = tcpHeader.sequenceNumber;
				// ask for next packet
				currentTCB->remoteAck = tcpHeader.sequenceNumber + 1;

				currentTCB->remoteWnd = ntohs(tcpHeader.windowSize);
				currentTCB->mss       = tcpMss;

				if (TCP_Snd(currentTCB)) {
					nextState               = ESTABLISHED;
					currentTCB->socketState = SOCKET_CONNECTED;
				}
			} else {
				// send reset
				currentTCB->localSeqno = tcpHeader.ackNumber;
				currentTCB->flags      = TCP_RST_FLAG | TCP_ACK_FLAG;
				if (TCP_Snd(currentTCB)) {
					nextState = CLOSE;
					TCB_Reset(currentTCB);
				}
			}
			break;
		case RCV_ACK:
			TCP_SyslogWrite("SYN_SENT: rx_ack");

			currentTCB->timeout = 0;

			if ((currentTCB->localSeqno + 1) == tcpHeader.ackNumber) {
				// create and send a ACK packet
				currentTCB->localSeqno = currentTCB->localSeqno + 1;
				currentTCB->flags      = TCP_ACK_FLAG;

				// save data from TCP header
				currentTCB->remoteSeqno = tcpHeader.sequenceNumber;
				currentTCB->remoteAck   = tcpHeader.sequenceNumber + 1; // ask for next packet

				currentTCB->remoteWnd = ntohs(tcpHeader.windowSize);
				currentTCB->mss       = tcpMss;

				nextState               = ESTABLISHED;
				currentTCB->socketState = SOCKET_CONNECTED;
			} else {
				// send reset
				currentTCB->localSeqno = tcpHeader.ackNumber;
				currentTCB->flags      = TCP_RST_FLAG;
				if (TCP_Snd(currentTCB)) {
					nextState = CLOSE;
					TCB_Reset(currentTCB);
				}
			}
			break;
		case CLOSE:
			TCP_SyslogWrite("SYN_SENT: close");
			// go to CLOSED state
			nextState = CLOSED;
			TCB_Reset(currentTCB);
			break;
		case TIMEOUT:
			TCP_SyslogWrite("SYN_SENT: timeout");
			// looks like the the packet was lost
			// check inside the packet to see where to jump next
			if (currentTCB->timeoutsCount) {
				if (TCP_Snd(currentTCB)) {
					if (currentTCB->flags & TCP_RST_FLAG) {
						nextState = CLOSE;
						TCB_Reset(currentTCB);
					} else if (currentTCB->flags & TCP_ACK_FLAG) {
						nextState               = ESTABLISHED;
						currentTCB->socketState = SOCKET_CONNECTED;
					}
				}
			} else {
				// just reset the connection if there is no reply
				currentTCB->flags = TCP_RST_FLAG;
				if (TCP_Snd(currentTCB)) {
					nextState = CLOSE;
					TCB_Reset(currentTCB);
				}
			}
			break;
		case RCV_RST:
		case RCV_RSTACK:
			// port seems not to be opened
			nextState = CLOSED;
			TCB_Reset(currentTCB);
			break;
		default:
			// for all other cases the packet is invalid and will be discarded
			break;
		}
		break;
	case SYN_RECEIVED:
		switch (event) {
		case RCV_SYNACK:
			TCP_SyslogWrite("SYN_RECEIVED: rx_synack");
			if (currentTCB->localPort == tcpHeader.destPort) {
				// stop the current timeout
				currentTCB->timeout = 0;

				// This is part of simultaneous open
				// TO DO: Check if the received packet is the one that we expect
				if ((currentTCB->destIP == receivedRemoteAddress) && (currentTCB->destPort == tcpHeader.sourcePort))
					if ((currentTCB->localSeqno + 1) == tcpHeader.ackNumber)

						nextState = ESTABLISHED;
				currentTCB->socketState = SOCKET_CONNECTED;
			}
			break;
		case RCV_ACK:
			TCP_SyslogWrite("SYN_RECEIVED: rx_ack");

			// check if the packet is for the curent TCB
			// we need to check the remote IP adress and remote port
			if ((currentTCB->destIP == receivedRemoteAddress) && (currentTCB->destPort == tcpHeader.sourcePort)) {
				// check the sequence numbers
				// is it the packet that I've ask for?
				if (currentTCB->remoteAck == tcpHeader.sequenceNumber) {
					// is ACK OK?
					if ((currentTCB->localSeqno + 1) == tcpHeader.ackNumber) {
						currentTCB->localSeqno = currentTCB->localSeqno + 1;
						// stop the current timeout
						currentTCB->timeout = 0;

						nextState               = ESTABLISHED;
						currentTCB->socketState = SOCKET_CONNECTED;
					}
				}
			}
			break;
		case CLOSE:
			TCP_SyslogWrite("SYN_RECEIVED: close");
			// stop the current timeout
			currentTCB->timeout = 0;
			// Need to send FIN and go to the FIN_WAIT_1
			currentTCB->flags              = TCP_FIN_FLAG;
			currentTCB->timeout            = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutReloadValue = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutsCount      = TCP_MAX_RETRIES;

			nextState = FIN_WAIT_1;
			TCP_Snd(currentTCB);
			break;
		case RCV_RSTACK:
		case RCV_RST:
			// Reset the connection
			TCP_SyslogWrite("SYN_RECEIVED:  rx_rst");
			// check if the local port match; else drop the pachet
			if (currentTCB->localPort == tcpHeader.destPort) {
				if (currentTCB->remoteAck == tcpHeader.sequenceNumber) {
					TCP_SyslogWrite("rst seq OK");
					currentTCB->destIP       = 0;
					currentTCB->destPort     = 0;
					currentTCB->localSeqno   = 0;
					currentTCB->localLastAck = 0;
					currentTCB->remoteSeqno  = 0;
					currentTCB->remoteAck    = 0;
					currentTCB->remoteWnd    = 0;
					currentTCB->mss          = 0;

					nextState = LISTEN;
				}
			}
			break;
		case TIMEOUT:
			TCP_SyslogWrite("SYN_RECEIVED:  timeout");
			if (currentTCB->timeoutsCount) {
				TCP_Snd(currentTCB);
			} else {
				// reseting the connection if there is no reply
				currentTCB->flags = TCP_RST_FLAG;
				if (TCP_Snd(currentTCB)) {
					currentTCB->destIP       = 0;
					currentTCB->destPort     = 0;
					currentTCB->localSeqno   = 0;
					currentTCB->localLastAck = 0;
					currentTCB->remoteSeqno  = 0;
					currentTCB->remoteAck    = 0;
					currentTCB->remoteWnd    = 0;
					currentTCB->mss          = 0;
					nextState                = LISTEN;
				}
			}
			break;
		default:
			// invalid packet so drop it
			break;
		}
		break;
	case ESTABLISHED:
		switch (event) {
		case RCV_ACK:
			TCP_SyslogWrite("ESTABLISHED: rx_ack");
			if (currentTCB->destIP == receivedRemoteAddress) {
				// is sequence number OK?
				// remote ACK should be equal to header sequence number
				// we don't accept out of order packet (not enough memory)
				if (currentTCB->remoteAck == tcpHeader.sequenceNumber) {
					// This is a ACK packet only
					// check the ACK sequence
					// check if this is on already received Ack
					if (currentTCB->localLastAck < tcpHeader.ackNumber) {
						// check how many bytes sent was acknowledged
						if ((currentTCB->localSeqno + 1) >= tcpHeader.ackNumber) {
							notAckBytes = currentTCB->localSeqno - tcpHeader.ackNumber;

							// update the pointer for next TX
							currentTCB->txBufferPtr = currentTCB->txBufferPtr - notAckBytes;
							currentTCB->bytesToSend = currentTCB->bytesToSend + notAckBytes;

							// Check if all TX buffer/data was acknowledged
							if (currentTCB->bytesToSend == 0) {
								if (currentTCB->txBufState == TX_BUFF_IN_USE) {
									currentTCB->txBufState = NO_BUFF;
								}
							}

							currentTCB->localLastAck = tcpHeader.ackNumber - 1;
							currentTCB->localSeqno   = tcpHeader.ackNumber;

							// check if the packet has payload
							if (rcvPayloadLen > 0) {
								currentTCB->remoteSeqno = tcpHeader.sequenceNumber;

								// copy the payload to the local buffer
								TCP_PayloadSave(rcvPayloadLen);
							}
						} else {
							// this is a wrong Ack
							// ACK a packet that wasn't transmitted
							// send a reset
							currentTCB->flags = TCP_RST_FLAG | TCP_ACK_FLAG;
							if (TCP_Snd(currentTCB)) {
								nextState = CLOSED;
								TCB_Reset(currentTCB);
							}
						}
					}
				}
			}
			break;
		case CLOSE:
			TCP_SyslogWrite("ESTABLISHED: close");
			currentTCB->flags = TCP_FIN_FLAG;
			nextState         = FIN_WAIT_1;
			TCP_Snd(currentTCB);
			break;
		case RCV_FINACK:
			TCP_SyslogWrite("ESTABLISHED: rx_finack");
		case RCV_FIN:
			TCP_SyslogWrite("ESTABLISHED: rx_fin");
			// ACK the current packet
			// TO DO  check if it's a valid packet
			currentTCB->localSeqno = tcpHeader.ackNumber;
			currentTCB->remoteAck  = currentTCB->remoteAck + 1;

			currentTCB->socketState        = SOCKET_CLOSING;
			currentTCB->timeout            = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutReloadValue = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutsCount      = TCP_MAX_RETRIES;
			// JUMP over CLOSE_WAIT state and send one packet with FIN + ACK
			currentTCB->flags = TCP_FIN_FLAG | TCP_ACK_FLAG;

			nextState = LAST_ACK;
			TCP_Snd(currentTCB);
			break;
		case RCV_RST:
		case RCV_RSTACK:
			nextState = CLOSED;
			TCB_Reset(currentTCB);
			break;
		case TIMEOUT:
			TCP_SyslogWrite("ESTABLISHED:  timeout");
			if (currentTCB->timeoutsCount) {
				TCP_Snd(currentTCB);
			} else {
				// reset the connection if there is no reply
				currentTCB->flags = TCP_RST_FLAG;
				if (TCP_Snd(currentTCB)) {
					nextState = CLOSE;
					TCB_Reset(currentTCB);
				}
			}
			break;
		default:
			break;
		}
		break;
	case FIN_WAIT_1:
		switch (event) {
		case RCV_FIN:
			TCP_SyslogWrite("FIN_WAIT_1: rx_fin");
			currentTCB->flags = TCP_ACK_FLAG;
			if (TCP_Snd(currentTCB)) {
				nextState = CLOSING;
			}
			break;
		case RCV_ACK:
			TCP_SyslogWrite("FIN_WAIT_1: rx_ack");
			// stop the current timeout
			currentTCB->timeout       = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutsCount = 1;
			nextState                 = FIN_WAIT_2;
			break;
		case RCV_FINACK:
			TCP_SyslogWrite("FIN_WAIT_1: rx_finack");
			currentTCB->flags = TCP_ACK_FLAG;
			if (TCP_Snd(currentTCB)) {
				nextState = TIME_WAIT;
			}
			break;
		case TIMEOUT:
			TCP_SyslogWrite("FIN_WAIT_1:  timeout");
			if (currentTCB->timeoutsCount) {
				TCP_Snd(currentTCB);
			} else {
				// just reset the connection if there is no reply
				currentTCB->flags = TCP_RST_FLAG;
				if (TCP_Snd(currentTCB)) {
					nextState = CLOSE;
					TCB_Reset(currentTCB);
				}
			}
			break;
		default:
			break;
		}
		break;
	case FIN_WAIT_2:
		switch (event) {
		case RCV_FIN:
			TCP_SyslogWrite("FIN_WAIT_2: rx_fin");
			if (TCP_Snd(currentTCB)) {
				nextState = TIME_WAIT;
			}
			break;
		case TIMEOUT:
			TCP_SyslogWrite("FIN_WAIT_2:  timeout");
			if (currentTCB->timeoutsCount) {
				TCP_Snd(currentTCB);
			} else {
				// just reset the connection if there is no reply
				currentTCB->flags = TCP_RST_FLAG;
				if (TCP_Snd(currentTCB)) {
					nextState = CLOSE;
					TCB_Reset(currentTCB);
				}
			}
			break;
		default:
			break;
		}
		break;
	case CLOSE_WAIT:
		// This state is defined in RFC, but is not used in the application
		break;
	case CLOSING:
		switch (event) {
		case RCV_ACK:
			TCP_SyslogWrite("CLOSING: rx_ack");
			nextState = TIME_WAIT;
			break;
		default:
			break;
		}
		break;
	case LAST_ACK:
		// check if the packet belongs to the curent TCB
		switch (event) {
		case RCV_FINACK:
		case RCV_ACK:
			if ((currentTCB->destIP == receivedRemoteAddress) && (currentTCB->destPort == tcpHeader.sourcePort)) {
				TCP_SyslogWrite("LAST_ACK: rx_ack");
				nextState = CLOSED;
				TCB_Reset(currentTCB);
			}
			break;
		case TIMEOUT:
			if (currentTCB->timeoutsCount) {
				TCP_Snd(currentTCB);
			} else {
				// just reset the connection if there is no reply
				currentTCB->flags = TCP_RST_FLAG;
				if (TCP_Snd(currentTCB)) {
					nextState = CLOSE;
					TCB_Reset(currentTCB);
				}
			}
		default:
			break;
		}
		break;
	case TIME_WAIT:
		TCP_SyslogWrite("Time Wait");
		nextState = CLOSED;
		TCB_Reset(currentTCB);
		break;
	case CLOSED:
		switch (event) {
		case ACTIVE_OPEN:
			TCP_SyslogWrite("CLOSED: active_open");
			// create and send a SYN packet
			currentTCB->timeout            = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutReloadValue = TCP_START_TIMEOUT_VAL;
			currentTCB->timeoutsCount      = TCP_MAX_SYN_RETRIES;
			currentTCB->flags              = TCP_SYN_FLAG;
			TCP_Snd(currentTCB);
			nextState = SYN_SENT;
			ret       = true;
			break;
		case PASIVE_OPEN:
			TCP_SyslogWrite("CLOSED: passive_open");
			currentTCB->destIP   = 0;
			currentTCB->destPort = 0;
			nextState            = LISTEN;
			ret                  = true;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	currentTCB->connectionEvent = DUMMY_NOP; // we are handling the event...
	currentTCB->fsmState        = nextState;
	return ret;
}

void TCP_Init(void)
{
	tcbList            = NULL;
	tcbListSize        = 0;
	nextAvailablePort  = LOCAL_TCP_PORT_START_NUMBER;
	nextSequenceNumber = 0;
}

tcbError_t TCP_SocketInit(tcpTCB_t *tcbPtr)
{
	tcbError_t ret = TCB_ERROR;

	// verify that this socket is not in the list
	if (TCB_Check(tcbPtr) == false) {
		TCB_Reset(tcbPtr);

		tcbPtr->localWnd        = 0; // here we should put the RX buffer size
		tcbPtr->mss             = TCP_MAX_SEG_SIZE;
		tcbPtr->fsmState        = CLOSED;
		tcbPtr->connectionEvent = DUMMY_NOP;
		tcbPtr->rxBufferStart   = NULL;
		tcbPtr->rxBufState      = NO_BUFF;
		tcbPtr->txBufferStart   = NULL;
		tcbPtr->txBufferPtr     = NULL;
		tcbPtr->bytesToSend     = 0;
		tcbPtr->txBufState      = NO_BUFF;

		TCB_Insert(tcbPtr);
		ret = TCB_NO_ERROR;
	}
	return ret;
}

tcbError_t TCP_SocketRemove(tcpTCB_t *tcbPtr)
{
	tcbError_t ret = TCB_ERROR;

	// verify that this socket is in the Closed State
	if (TCP_SocketPoll(tcbPtr) == SOCKET_CLOSED) {
		TCB_Remove(tcbPtr);
		ret = TCB_NO_ERROR;
	}
	return ret;
}

socketState_t TCP_SocketPoll(tcpTCB_t *socket_ptr)
{
	socketState_t tmpSocketState;

	tmpSocketState = NOT_A_SOCKET;

	if (TCB_Check(socket_ptr)) {
		tmpSocketState = socket_ptr->socketState;
	}

	return tmpSocketState;
}

bool TCP_Bind(tcpTCB_t *tcbPtr, uint16_t port)
{
	bool ret = false;

	TCP_SyslogWrite("tcp_bind");

	if (TCB_Check(tcbPtr)) {
		tcbPtr->localPort = port;
		ret               = true;
	}
	return ret;
}

bool TCP_Listen(tcpTCB_t *tcbPtr)
{
	bool ret = false;

	TCP_SyslogWrite("tcp_listen");

	if (TCB_Check(tcbPtr)) {
		tcbPtr->connectionEvent = PASIVE_OPEN;
		tcbPtr->socketState     = SOCKET_IN_PROGRESS;
		tcbPtr->localSeqno      = nextSequenceNumber;
		currentTCB              = tcbPtr;
		if (tcbPtr->localPort == 0) {
			tcbPtr->localPort = nextAvailablePort++;
		}
		ret = TCP_FiniteStateMachine();
	}
	return ret;
}

bool TCP_Connect(tcpTCB_t *tcbPtr, sockaddr_in_t *srvaddr)
{
	bool ret = false;

	if (TCP_SocketPoll(tcbPtr) == SOCKET_CLOSED) {
		tcbPtr->destIP   = srvaddr->addr.s_addr;
		tcbPtr->destPort = srvaddr->port;
		if (tcbPtr->localPort == 0) {
			// use a "random" port for the local one
			tcbPtr->localPort = nextAvailablePort++;
		}

		tcbPtr->socketState     = SOCKET_IN_PROGRESS;
		tcbPtr->localSeqno      = nextSequenceNumber;
		tcbPtr->connectionEvent = ACTIVE_OPEN;

		currentTCB = tcbPtr;
		ret        = TCP_FiniteStateMachine();
	}

	return ret;
}

bool TCP_Close(tcpTCB_t *tcbPtr)
{
	bool ret = false;

	TCP_SyslogWrite("tcp_close");

	if (TCB_Check(tcbPtr)) {
		tcbPtr->connectionEvent = CLOSE;

		tcbPtr->socketState = SOCKET_CLOSING;

		// likely to change this to a needs TX time queue
		currentTCB = tcbPtr;
		ret        = TCP_FiniteStateMachine();
	}
	return ret;
}

bool TCP_Send(tcpTCB_t *tcbPtr, uint8_t *data, uint16_t dataLen)
{
	bool ret = false;

	if (TCP_SocketPoll(tcbPtr) == SOCKET_CONNECTED) {
		if (tcbPtr->txBufState == NO_BUFF) {
			if (data != NULL) {
				tcbPtr->txBufferStart = data;
				tcbPtr->txBufferPtr   = tcbPtr->txBufferStart;
				tcbPtr->bytesToSend   = dataLen;
				tcbPtr->txBufState    = TX_BUFF_IN_USE;

				tcbPtr->timeoutReloadValue = TCP_START_TIMEOUT_VAL;
				tcbPtr->timeoutsCount      = TCP_MAX_RETRIES;

				tcbPtr->flags = TCP_ACK_FLAG;

				TCP_Snd(tcbPtr);
				ret = true;
			}
		}
	}
	return ret;
}

bool TCP_SendDone(tcpTCB_t *tcbPtr)
{
	bool ret = false;

	if (TCB_Check(tcbPtr)) {
		if (tcbPtr->txBufState == NO_BUFF) {
			ret = true;
		}
	}
	return ret;
}

bool TCP_InsertRxBuffer(tcpTCB_t *tcbPtr, uint8_t *data, uint16_t data_len)
{
	bool ret = false;

	if (TCB_Check(tcbPtr)) {
		if (tcbPtr->rxBufState == NO_BUFF) {
			if (data != NULL) {
				tcbPtr->rxBufferStart = data;
				tcbPtr->rxBufferPtr   = tcbPtr->rxBufferStart;
				tcbPtr->localWnd      = data_len; // update the available receive windows
				tcbPtr->rxBufState    = RX_BUFF_IN_USE;
				ret                   = true;
			}
		}
	}
	return ret;
}

int16_t TCP_GetReceivedData(tcpTCB_t *tcbPtr)
{
	int16_t ret = 0;

	if (TCB_Check(tcbPtr)) {
		if (tcbPtr->rxBufState == RX_BUFF_IN_USE) {
			ret = tcbPtr->rxBufferPtr - tcbPtr->rxBufferStart;

			if (ret != 0) {
				tcbPtr->localWnd   = 0;
				tcbPtr->rxBufState = NO_BUFF;
			}
		}
	}
	return ret;
}

int16_t TCP_GetRxLength(tcpTCB_t *tcbPtr)
{
	int16_t ret = 0;

	if (TCB_Check(tcbPtr)) {
		if (tcbPtr->rxBufState == RX_BUFF_IN_USE) {
			ret = tcbPtr->rxBufferPtr - tcbPtr->rxBufferStart;
		}
	}
	return ret;
}

void TCP_Update(void)
{
	tcpTCB_t *tcbPtr;
	tcbPtr    = NULL;
	int count = 0;

	// update sequence number and local port number in order to be different
	// for each new connection
	nextSequenceNumber++;

	// keep local port number in the general port range
	nextAvailablePort = nextAvailablePort + 1;
	if (nextAvailablePort < LOCAL_TCP_PORT_START_NUMBER) {
		nextAvailablePort = LOCAL_TCP_PORT_START_NUMBER;
	}
	// TO DO also local seq number should be "random"

	tcbPtr = tcbList;
	while ((tcbPtr != NULL) && (count < tcbListSize)) {
		if (tcbPtr->timeout > 0) {
			TCP_SyslogWrite("tcp timeout");
			tcbPtr->timeout = tcbPtr->timeout - 1;

			if (tcbPtr->timeout == 0) {
				if (tcbPtr->timeoutsCount > 0) {
					// MAKE sure we don't overwrite anything else
					if (tcbPtr->connectionEvent == DUMMY_NOP) {
						tcbPtr->timeout         = tcbPtr->timeoutReloadValue;
						tcbPtr->timeoutsCount   = tcbPtr->timeoutsCount - 1;
						tcbPtr->connectionEvent = TIMEOUT;
						currentTCB              = tcbPtr;
						TCP_FiniteStateMachine();
					}
				}
			}
		}
		tcbPtr = tcbPtr->nextTCB;
		count++;
	}
}
