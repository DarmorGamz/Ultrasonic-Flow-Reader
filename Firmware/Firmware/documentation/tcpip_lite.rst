Microchip TCP/IP Lite Stack
===========================

The purpose of the TCP/IP lite stack implementation is to provide optimized
(low Flash and RAM footprint) TCP/IP stacks for microcontrollers with 
8KB Flash (UDP only) and 16KB Flash (TCP/IP), while still having full functional
TCP/IP v4 stack. The stack will allow customers to add wired communication and
interoperability with other systems to their applications over Ethernet.

The Microchip TCP/IP lite stack is implemented in a configurable and modular
way allowing users to include only the intended features or functionality to
their application.

Features
--------

* Protocols: IP, ARP, ICMP, UDP, TCP 
* DHCP client 

TCP/IP Stack Buffer Management
------------------------------

The TCP/IP stack uses by default the least possible
memory, so that users have maximum possible
memory available to be allocated for their applications.
Users are responsible for providing all the needed
buffers for each TCP/IP protocol/connection used, as
described further on.

Ethernet controller Random Access Memory (RAM)
used by the TCP/IP stack:

* Ethernet received packets are kept into the
  Ethernet controller RAM memory. The Ethernet
  controller will receive and store multiple received
  Ethernet packets until the TCP/IP stack has time
  to process them. The buffer for each received
  packet is managed by the Ethernet controller
  automatically and a buffer descriptor will be
  available for the user. The Ethernet controller will
  start dropping the received packets if it has no
  more available memory.

* Ethernet packet to be transmitted is also built and
  kept in the Ethernet controller memory. The TCP/
  IP stack supports only one transmit (TX) packet at
  a time.

* Buffers Used by the UDP Protocol
  For creating UDP packets the stack allows the user to
  directly use the Ethernet controller RAM memory for
  storing the user payload. The user will call the API to
  start a UDP packet, transfer the payload and send the
  packet through the wire.

When receiving data over the UDP protocol, the
Ethernet Controller will manage the buffer for the
received packet. If the packet was received
successfully and there is a user callback registered for
the incoming port, the stack will call the registered
function (callback) and give the user the opportunity to
access the payload directly from the Ethernet
controller. This will avoid copying the payload multiple
times.

Dependencies
------------
* The TCP/IP stack needs a background task called periodically by the user, in
  order to handle asynchronous events like managing a timeout, checking the status
  for the Ethernet controller and parsing the received buffers. Typlically is a
  Timer driver.

* Ethernet Controller with on-chip buffer for TX and RX operations. Like:
	* ENC28J60 10Base Ethernet Controller with 8-Kbytes buffer
	* ENC424J600/624J600 10/100Base with 24-Kbytes buffer


Limitations
-----------
The TCP/IP stack has some limitations because of the limited memory, for both
RAM and Flash, available on an 8-bit microcontroller.

Address Resolution Protocol (ARP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The number of entries in the Address Resolution
Protocol (ARP) table should be limited to eight. The
limit of the maximum entries in the table is user
configurable (refer to the tcpip_lite_config.h file). The
number of entries in the ARP table should be at least
equal to the maximum number of device connections,
otherwise there will be a performance degradation
caused by the ARP request for each IP address that is
not found in the ARP table.

IPv4
^^^^
The stack supports only ICMPv4, UDPv4 and TCPv4
packets. All the other types of packets will be discarded
by the stack.

The stack does not support IP fragmentation.
Fragmented IP packets will be dropped by the stack.
Fragmentation is required if the IP packet size is larger
than the maximum transmit unit, usually 1500 bytes. If
such packets have to be sent, the user will be required
to break them down into multiple smaller sized packets
to avoid any fragmentation.

The TCP/IP stack will not interpret the IP header
options.

The TCP/IP stack does not support the loop-back
address 127.0.0.1. The 127.0.0.1 address is handled
as a general IPv4 address and the stack will try to find
the MAC address associated with this IP address.
Currently, the stack does not support multi-casting. The
user cannot send and receive packets with multi-cast
addresses.

Internet Control Message Protocol (ICMP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
ICMP is an optional protocol and only the ICMP
ECHO_REPLY message is implemented. The stack
will reply only to an ICMP ECHO_REQUEST, all the
other ICMP messages will be ignored.

User Datagram Protocol (UDP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
In order to save some memory, when UDP packets are
transmitted, the stack allows building the packets
directly inside the Ethernet controller in transmit buffer.
Since there is only one transmit buffer, no transmission
can take place once the UDP packet creation has
started and until the packet has been sent. The time
between starting a UDP packet and the moment when
the packet is sent should be kept to a minimum.

Dynamic Host Configuration Protocol (DHCP) Client
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Only DHCP DISCOVER and DHCP REQUEST
message types are supported. The client will be able to
inquire and retrieve an IP address. DHCP INFORM
type message to request more information from the
DHCP server is not supported. DHCP RELEASE type
message to release DHCP information and deactivate
the Client's IP address is also not supported.

Transmission Control Protocol (TCP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Packets received out of order are dropped and the
stack requests retransmission of the expected packet.
Urgent Pointer is ignored and the data will pass to the
application in the same way as data received without
Urgent Pointer.


  The Push Flag is ignored and the data is passed by the
  user every time there is something in the socket's RX
  buffer.

  In order to reduce the protocol complexity, the number
  of timeouts and the duration between, the messages'
  retransmission is defined as a fixed-time interval and it
  is user configurable.

  The delayed Acknowledgment packet (ACK)
  functionality is not implemented. Each ACK packet will
  be sent with payload if there are any outstanding TX
  data or without payload right away.

  Only the Maximum Segment Size (MSS) option from
  the TCP header options is supported. All the other
  options are silently ignored by the stack.

