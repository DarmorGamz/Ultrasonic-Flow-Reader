/* Auto-generated config file tcpip_lite_config.h */
#ifndef TCPIP_LITE_CONFIG_H
#define TCPIP_LITE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> ARP Protocol Configurations
// <o> ARP Map Size <0-255>
// <i> ARP Map Size of Stack
// <id> arp_map_size
#ifndef ARP_MAP_SIZE
#define ARP_MAP_SIZE 8
#endif
// </h>

// <h> DHCP Protocol Configurations
// <s.100> DHCP Name
// <i> A name which define DHCP
// <id> dhcp_name
#ifndef DHCP_NAME
#define DHCP_NAME "DCA-XXXYYYYY"
#endif

// <o> DHCP Name Length <1-255>
// <i> Application must defined the DCHP Name Length
// <id> dhcp_name_size
#ifndef DHCP_NAME_SIZE
#define DHCP_NAME_SIZE 12
#endif
// </h>

// <h> IP Protocol Configurations
// <o> TTL Length <0-65535>
// <i> TTL Length
// <id> ipv4_ttl
#ifndef IPv4_TTL
#define IPv4_TTL 64
#endif
// </h>

// <h> TCP Protocol Configurations

// <o> Maximum Segment Size <0-65535>
// <i> Maximum Segment Size
// <id> tcp_max_seg_size
#ifndef TCP_MAX_SEG_SIZE
#define TCP_MAX_SEG_SIZE 1460
#endif

// <o> Tick Second <0-65535>
// <i> Tick Second
// <id> tick_second
#ifndef TICK_SECOND
#define TICK_SECOND 1
#endif

/* Timeout to retransmit unacked data */
#define TCP_START_TIMEOUT_VAL ((unsigned long)TICK_SECOND * 2)

// <o> TCP Max Retries Times <0-65535>
// <i> Maximum number of retransmission attempts
// <id> tcp_max_retries
#ifndef TCP_MAX_RETRIES
#define TCP_MAX_RETRIES 5
#endif

// <o> TCP Max SYN Retries Times <0-65535>
// <i> Smaller than all other retries to reduce SYN flood DoS duration
// <id> tcp_max_syn_retries
#ifndef TCP_MAX_SYN_RETRIES
#define TCP_MAX_SYN_RETRIES 3
#endif

// <o> Local TCP Port Start Number <0-65535>
// <i> define the lower port number to be used as a local port
// <id> local_tcp_port_start_number
#ifndef LOCAL_TCP_PORT_START_NUMBER
#define LOCAL_TCP_PORT_START_NUMBER 1024
#endif

// <o> Local TCP Port Start Number <0-65535>
// <i> define the highest port number to be used as a local port
// <id> local_tcp_port_end_number
#ifndef LOCAL_TCP_PORT_END_NUMBER
#define LOCAL_TCP_PORT_END_NUMBER 65535
#endif

// </h>

// <h> TCP/IP Stack Debug Configurations

// <q> Enable SYSLOG Printf
// <i> Enable SYSLOG Printf or Not, if Enabled, a STDIO middleware must be add.
// <id> enable_syslog_debug
#ifndef ENABLE_SYSLOG_DEBUG
#define ENABLE_SYSLOG_DEBUG 0
#endif

// <q> Enable TCP Debug
// <i> Enable TCP Debug or Not
// <id> enable_tcp_debug
#ifndef ENABLE_TCP_DEBUG
#define ENABLE_TCP_DEBUG 0
#endif

// <q> Enable IP Debug
// <i> Enable IP Debug or Not
// <id> enable_ip_debug
#ifndef ENABLE_IP_DEBUG
#define ENABLE_IP_DEBUG 0
#endif

// <q> Enable NET Debug
// <i> Enable NET Debug or Not
// <id> enable_net_debug
#ifndef ENABLE_NET_DEBUG
#define ENABLE_NET_DEBUG 0
#endif
// </h>

// <q> Enable UDP Demo
// <i> Enable UDP Demo
// <id> enable_udp_demo
#ifndef ENABLE_UDP_DEMO
#define ENABLE_UDP_DEMO 0
#endif

#if ENABLE_UDP_DEMO == 1
#undef UDP_DEMO
#define UDP_DEMO
#endif

// <<< end of configuration section >>>

#endif // TCPIP_LITE_CONFIG_H
