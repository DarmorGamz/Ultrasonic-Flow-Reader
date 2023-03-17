/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include <string.h>
#include "tcpip_lite_start.h"
#include "ipv4.h"
#include "Ethernet.h"
#include <App/DcaApp.h>

#define MAX_TCP_CHUNK_SIZE                          1396    // TCP_MAX_SEG_SIZE (1460) - room for up to 64 bytes for Ethernet frame overhead
#define DNS_PORT                                    53      // Default port for DNS query
#define DNS_APP_PORT                                4096    // Application port to process DNS response

#define OUI_BYTE_1                           0x60
#define OUI_BYTE_2                           0x54
#define OUI_BYTE_3                           0x64

#define ETHERNET_SOCKET_PORT_LOCAL_WEBSERVER    8080
#define ETHERNET_SOCKET_PORT_HTTP               80
#define ETHERNET_SOCKET_PORT_HTTP_ALT           8008

// DNS resolution callback function pointer
typedef void (*dns_function_ptr)(void);

// DNS lookup state
typedef enum
{
	DNS_STATE_IDLE = 0,     // Ready to process DNS requests
	DNS_STATE_START,        // Initial state to reset client state variables
	DNS_STATE_OPEN_SOCKET,  // Open UDP socket
	DNS_STATE_QUERY,        // Send DNS query to DNS server
	DNS_STATE_GET_RESULT,   // Wait for response from DNS server
	DNS_STATE_FAIL,         // DNS server not responding
	DNS_STATE_DONE          // DNS query is finished
} eDnsState;

// Structure for the DNS header
typedef struct
{
	uint16_t u16TransactionID;
	uint16_t u16Flags;
	uint16_t u16Questions;
	uint16_t u16Answers;
	uint16_t u16AuthoritativeRecords;
	uint16_t u16AdditionalRecords;
} tsDnsHeader;

typedef struct
{
	// Response name is first, but it is variable length and must be retrieved using the _DnsDiscardName() function
	uint16_t u16ResponseType;
	uint16_t u16ResponseClass;
	uint32_t u32ResponseTTL;
	uint16_t u16ResponseLen;
} tsDnsAnswerHeader;

/** VARIABLES *****************************************************************/
static uint16_t         TIMER_ETH_SERVER_LOST_COMM;
static uint16_t         TIMER_ETH_SERVER_ESTABLISH_COMM;
static uint16_t         TIMER_ETH_CONNECION_TIMEOUT;
static uint16_t         TIMER_ETH_DNS_TIMEOUT;
static uint16_t         TIMER_ETH_DNS_REMOTE_RESOLUTION_TIMEOUT;
static uint16_t         TIMER_ETH_SOCKET_REMOTE_INIT_TIMEOUT;
static uint16_t         TIMER_ETH_SOCKET_REMOTE_RESPONSE_TIMEOUT;
static uint16_t         TIMER_ETH_SOCKET_REMOTE_CLEANUP_TIMEOUT;

static sEthernetInfo    s_stEthernetInfo;
static bool             s_fEthernetFirstTime = true;
static bool             s_fEthernetQueueServerSend = false;
static bool             s_fEthernetServerCommEstablished = false;
static bool             s_fEthernetServerCommLost = false;
static bool             s_fEthernetDisableLocalWebsvr = false;
//static bool             s_fDhcpDisabled = false;
//static bool             s_fUseStaticIPConf = false;

static uint8_t          s_u8EthernetHostIdx = 0; // alt1, alt2, primary, failsafe {repeat}

static uint16_t         s_u16EthernetPostLengthOffset = 0;
static uint16_t         s_u16EthernetPostPayloadOffset = 0;
static char             s_acEthernetStrPostHeader[HTTP_POST_HDR_BYTES];

static bool             s_fEthernetSocketsInitialized = false;
static tcpTCB_t         s_stEthernetSocketLocal;
static tcpTCB_t         s_stEthernetSocketRemote;
static sockaddr_in_t    s_stEthernetSocketAddressRemote;
static void             *s_pvEthernetRemainingLocalBuffer = NULL;
static uint16_t         s_u16EthernetRemainingLocalBytes = 0;
static void             *s_pvEthernetRemainingRemoteBuffer = NULL;
static uint16_t         s_u16EthernetRemainingRemoteBytes = 0;


static uint32_t         s_u32EthernetRemoteHostIp = 0;
static uint8_t          s_u8EthernetRemoteDnsAttempts = 0; // Will loop through host and failsafe a couple times before failing
static eDnsState        s_eEthernetDnsState = DNS_STATE_IDLE;


/** LOCAL (PRIVATE) FUNCTION PROTOTYPES ***************************************/
bool _DetectEthernet(void);
void _ManageEthernetState(void);

// Socket Functions
void _InitEthernetSockets(void);
void _DeinitEthernetSockets(void);
//void _ManageEthernetLocalSocketState(void);
void _ManageEthernetRemoteSocketState(void);
void _DnsRemoteHostCb(void);
//void _BuildRemotePostHeader(void);

// DNS Functions
void _ManageEthernetDnsState(void);
void _DnsLookup(char* pcHostname, uint32_t* pu32HostIp, dns_function_ptr pvResponseHandler);
void _DnsWriteHostname(void);
void _DnsDiscardName(void);
// Callback function to handle DNS query responses received on UDP port 4096 (called from TCP/IP Lite UDP_Receive function in udpv4.c)
void DNS_Handler(int nLength);

/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
bool Ethernet_Detect(void) {
	// Remove device from reset
    gpio_set_pin_level(ETHERNET_RESET, 1);
    delay_ms(1);
    // Detect the device
    return _DetectEthernet();
}



int8_t Ethernet_Init(void) {
	// First determine if the Ethernet device exists
    bool fHasEthernet = Ethernet_Detect();
    if (fHasEthernet==false) return -1;
	
	// Allocate required resources first time only
    if (s_fEthernetFirstTime==true) {
        s_fEthernetFirstTime = false;

        // Create any timers required by the Ethernet - nothing to go wrong (assuming TIMER_MAX_COUNT is appropriate) so intentionally ignoring return value
        Timer_AddTimer(&TIMER_ETH_CONNECION_TIMEOUT);
        Timer_AddTimer(&TIMER_ETH_DNS_TIMEOUT);
        Timer_AddTimer(&TIMER_ETH_SERVER_LOST_COMM);
        Timer_AddTimer(&TIMER_ETH_SERVER_ESTABLISH_COMM);
        Timer_AddTimer(&TIMER_ETH_DNS_REMOTE_RESOLUTION_TIMEOUT);
        Timer_AddTimer(&TIMER_ETH_SOCKET_REMOTE_INIT_TIMEOUT);
        Timer_AddTimer(&TIMER_ETH_SOCKET_REMOTE_RESPONSE_TIMEOUT);
        Timer_AddTimer(&TIMER_ETH_SOCKET_REMOTE_CLEANUP_TIMEOUT);

    // Otherwise, just reset them
    } else {
        Timer_SetTimer(TIMER_ETH_CONNECION_TIMEOUT, 0);
        Timer_SetTimer(TIMER_ETH_DNS_TIMEOUT, 0);
        Timer_SetTimer(TIMER_ETH_SERVER_LOST_COMM, 0);
        Timer_SetTimer(TIMER_ETH_SERVER_ESTABLISH_COMM, 0);
        Timer_SetTimer(TIMER_ETH_DNS_REMOTE_RESOLUTION_TIMEOUT, 0);
        Timer_SetTimer(TIMER_ETH_SOCKET_REMOTE_INIT_TIMEOUT, 0);
        Timer_SetTimer(TIMER_ETH_SOCKET_REMOTE_RESPONSE_TIMEOUT, 0);
        Timer_SetTimer(TIMER_ETH_SOCKET_REMOTE_CLEANUP_TIMEOUT, 0);
    }

    // Initialize the Ethernet info structure
    memset((uint8_t *)&s_stEthernetInfo, 0, sizeof(sEthernetInfo));
	
    // Build the host name before initializing the stack (so the initial DHCP request will have the correct name)
    char acHostname[24];
    // Build the Hostname "DCA-{SN}"
    sprintf(acHostname, "DCA-00001");
    uint8_t len = (uint8_t)strlen(acHostname)+1;

    // Build the MAC address (from OUI and serial) and set on ENC28J60 (defaults to 00DEAD00BEEF during init)
    // SN: xyz-abcde == MAC: 60:54:64:xy:zc:de
    g_au8MacAddress[0] = OUI_BYTE_1; // 0x60
    g_au8MacAddress[1] = OUI_BYTE_2; // 0x54
    g_au8MacAddress[2] = OUI_BYTE_3; // 0x64
    g_au8MacAddress[3] = 0x00;
    g_au8MacAddress[4] =  0x10;
    g_au8MacAddress[5] = 0x01;

    ETH_InitMAC((uint8_t*)&g_au8MacAddress[0]);

    // Initialize TCP/IP Lite stack
    tcpip_lite_stack_init();
	
	return 0;
}

void Ethernet_PowerDown(void) {
	struct io_descriptor *s_ioSPI;
	uint8_t tx_buffer[2];

	// Get the IO descriptor for the SPI peripheral
	if (spi_m_sync_get_io_descriptor(&ETHERNET_SPI, &s_ioSPI)!=0 ) {
		return;
	};

	// Select register bank 3 (write control register 0x1F)
	// Prepare payload
	tx_buffer[0] = 0xA0|0x1F;   // Bit field clear | ECON1 register address
	tx_buffer[1] = 0x03;        // Bank 3
	// Enable the SPI slave (activates CSn)
	gpio_set_pin_level(ETHERNET_CS, 0);
	// Write payload
	io_write(s_ioSPI, tx_buffer, 2);
	// Disable the SPI slave (deactivates CSn)
	gpio_set_pin_level(ETHERNET_CS, 1);
	// Prepare payload
	tx_buffer[0] = 0x80|0x1F;   // Bit field set | ECON1 register address
	tx_buffer[1] = 0x03;        // Bank 3
	// Enable the SPI slave (activates CSn)
	gpio_set_pin_level(ETHERNET_CS, 0);
	// Write payload
	io_write(s_ioSPI, tx_buffer, 2);
	// Disable the SPI slave (deactivates CSn)
	gpio_set_pin_level(ETHERNET_CS, 1);

	// Prepare payload to put device in power saver mode
	tx_buffer[0] = 0x80|0x1E;   // Bit field set | ECON2 register address
	tx_buffer[1] = 0x20;        // PWRSV bit set
	// Enable the SPI slave (activates CSn)
	gpio_set_pin_level(ETHERNET_CS, 0);
	// Write payload
	io_write(s_ioSPI, tx_buffer, 2);
	// Disable the SPI slave (deactivates CSn)
	gpio_set_pin_level(ETHERNET_CS, 1);

	// Disable the SPI peripheral
	spi_m_sync_disable(&ETHERNET_SPI);
}

void Ethernet_PowerUp(void) {
	struct io_descriptor *s_ioSPI;
    uint8_t tx_buffer[2];

    // Enable the SPI peripheral
    spi_m_sync_enable(&ETHERNET_SPI);

    // Get the IO descriptor for the SPI peripheral
    if (spi_m_sync_get_io_descriptor(&ETHERNET_SPI, &s_ioSPI)!=0 ) {
        return;
    };

    // Select register bank 3 (write control register 0x1F)
    // Prepare payload
    tx_buffer[0] = 0xA0|0x1F;   // Bit field clear | ECON1 register address
    tx_buffer[1] = 0x03;        // Bank 3
    // Enable the SPI slave (activates CSn)
    gpio_set_pin_level(ETHERNET_CS, 0);
    // Write payload
    io_write(s_ioSPI, tx_buffer, 2);
    // Disable the SPI slave (deactivates CSn)
    gpio_set_pin_level(ETHERNET_CS, 1);
    // Prepare payload
    tx_buffer[0] = 0x80|0x1F;   // Bit field set | ECON1 register address
    tx_buffer[1] = 0x03;        // Bank 3
    // Enable the SPI slave (activates CSn)
    gpio_set_pin_level(ETHERNET_CS, 0);
    // Write payload
    io_write(s_ioSPI, tx_buffer, 2);
    // Disable the SPI slave (deactivates CSn)
    gpio_set_pin_level(ETHERNET_CS, 1);

    // Prepare payload to put remove device from power saver mode
    tx_buffer[0] = 0xA0|0x1E;   // Bit field clear | ECON2 register address
    tx_buffer[1] = 0x20;        // PWRSV bit set
    // Enable the SPI slave (activates CSn)
    gpio_set_pin_level(ETHERNET_CS, 0);
    // Write payload
    io_write(s_ioSPI, tx_buffer, 2);
    // Disable the SPI slave (deactivates CSn)
    gpio_set_pin_level(ETHERNET_CS, 1);
}

bool _DetectEthernet() {
    struct io_descriptor *s_ioSPI;
    int8_t result = 0;
    uint8_t tx_buffer[2];
    uint8_t rx_buffer[1];

    // Get the IO descriptor for the SPI peripheral
    if (spi_m_sync_get_io_descriptor(&ETHERNET_SPI, &s_ioSPI)!=0 ) {
        return false;
    };
    // Enable the SPI peripheral
    spi_m_sync_enable(&ETHERNET_SPI);

    // Select register bank 3 (write control register 0x1F)
    // Prepare payload
    tx_buffer[0] = 0xA0|0x1F;   // Bit field clear | ECON1 register address
    tx_buffer[1] = 0x03;        // Bank 3
    // Enable the SPI slave (activates CSn)
    gpio_set_pin_level(ETHERNET_CS, 0);
    //delay_ms(1);
    // Write payload
    if (io_write(s_ioSPI, tx_buffer, 2) != 2) result = -1;
    // Disable the SPI slave (deactivates CSn)
    gpio_set_pin_level(ETHERNET_CS, 1);
    // Prepare payload
    tx_buffer[0] = 0x80|0x1F;   // Bit field set | ECON1 register address
    tx_buffer[1] = 0x03;        // Bank 3
    // Enable the SPI slave (activates CSn)
    gpio_set_pin_level(ETHERNET_CS, 0);
    //delay_ms(1);
    // Write payload
    if (io_write(s_ioSPI, tx_buffer, 2) != 2) result = -1;
    // Disable the SPI slave (deactivates CSn)
    gpio_set_pin_level(ETHERNET_CS, 1);

    // Read the Ethernet revision register
    // Prepare payload (register to read) -
    tx_buffer[0] = 0x00|0x12;   // Read control register | EREVID register address
    // Enable the SPI slave (activates CSn)
    gpio_set_pin_level(ETHERNET_CS, 0);
    // Write address of register to read - SPI
    if (io_write(s_ioSPI, tx_buffer, 1) != 1) result = -1;
    // Read register payload
    if (io_read(s_ioSPI, rx_buffer, 1) != 1)  result = -1;
    // Disable the SPI slave (deactivates CSn)
    gpio_set_pin_level(ETHERNET_CS, 1);

    // Disable the SPI peripheral
    spi_m_sync_disable(&ETHERNET_SPI);
	
    // Failure to write/read above indicates the device is not present
    if (result==-1) return false;
    // 0x00 or 0xFF in the REV ID register indicates that the device is not present
    else if (rx_buffer[0]==0x00 || rx_buffer[0]==0xFF) return false;
    // If we make it here, the device is present
    else return true;
}

bool Ethernet_HasLink(void) {
	return ETH_CheckLinkUp();
}

static bool s_fLinkUp = false;
void Ethernet_PumpEvents(void) {
    // Bail out if link not detected
    if (s_fLinkUp==false) {
        if (Ethernet_HasLink()==false) {
            App_SetStatus(STATUS_NO_SERVER_COMM);
            return;
        } else {
            s_fLinkUp = true;
        }
    }

    // Manage the Ethernet state
	//DEBUG_INFO("Manage Ethernet State");
    _ManageEthernetState();

    // Manage Socket State
    // Process remote socket if there is nothing being processed locally
    if (s_u16EthernetRemainingLocalBytes==0) {
		//DEBUG_INFO("Manage Remote Socket");
        _ManageEthernetRemoteSocketState();
    }

    // Manage DNS state
	//DEBUG_INFO("Manage DNS");
    _ManageEthernetDnsState();

    // Health check for network/server connection
    if (s_fEthernetServerCommEstablished==true) {
        // If lost comm timer expires, we've lost communication with server for some reason
        if (Timer_GetTimer(TIMER_ETH_SERVER_LOST_COMM)==0) {
            DEBUG_INFO("Server not responding. ");
            App_SetStatus(STATUS_NO_SERVER_COMM);
            // Clear established flag and re-initialize connection
            s_fEthernetServerCommEstablished = false;
            s_fEthernetServerCommLost = true;
            // Move on to the next host
            s_u8EthernetHostIdx++; // alt1, alt2, primary, failsafe
            if (s_u8EthernetHostIdx>=4) s_u8EthernetHostIdx=0;
            s_stEthernetInfo.eState = ETHERNET_STATE_ERROR;
            // Set timer for establishing comm (once connected)
            Timer_SetTimer(TIMER_ETH_SERVER_ESTABLISH_COMM, TIMEOUT_ESTABLISH_COMM); // 10 minutes before we try a soft reset
        } else {
            App_SetStatus(STATUS_OK);
            s_fEthernetServerCommLost = false;
        }
    }
    // Give system/connection a couple minutes to establish/recover before performing a soft reset
    if (s_fEthernetServerCommLost==true) {
        if (Timer_GetTimer(TIMER_ETH_SERVER_ESTABLISH_COMM)==0){
            //App_SoftReset();
        }
    }
}

void _ManageEthernetState(void){
	uint32_t ip;
	uint8_t au8Tmp[4];

	// Manage network state (TCP/IP Lite Stack)
	//DEBUG_INFO("Network Manage");
	Network_Manage();

	// Handle Ethernet state
    switch (s_stEthernetInfo.eState) {
        case ETHERNET_STATE_INITIALIZE:
			//DEBUG_INFO("ETHERNET STATE INIT");
            // Initialize the Ethernet info structure
            memset((uint8_t *)&s_stEthernetInfo, 0, sizeof(sEthernetInfo));
            s_stEthernetInfo.eState = ETHERNET_STATE_NETWORK_CONFIG;
            Timer_SetTimer(TIMER_ETH_CONNECION_TIMEOUT, 3);
            break;

        case ETHERNET_STATE_NETWORK_CONFIG:
			//DEBUG_INFO("ETHERNET STATE CONFIG");
            ip = IPV4_GetMyIP();	
            if (ip != s_stEthernetInfo.u32IpAddr) {
				DEBUG_INFO("ETHERNET STATE CONFIG (IP CHANGED)");
                DEBUG_INFO("IP Assigned. ");
                // Store the IP address as it comes from the stack (byte swapped)
                s_stEthernetInfo.u32IpAddr = ipv4Address;
                // Copy the network configuration into the info structure - make sure to perform by
                memcpy(&au8Tmp[0], (uint8_t*)&ipv4Address, 4);
                s_stEthernetInfo.au8IpAddr[0] = au8Tmp[3];
                s_stEthernetInfo.au8IpAddr[1] = au8Tmp[2];
                s_stEthernetInfo.au8IpAddr[2] = au8Tmp[1];
                s_stEthernetInfo.au8IpAddr[3] = au8Tmp[0];
				DEBUG_INFO("%u-", s_stEthernetInfo.au8IpAddr[0]);
				DEBUG_INFO("%u-", s_stEthernetInfo.au8IpAddr[1]);
				DEBUG_INFO("%u-", s_stEthernetInfo.au8IpAddr[2]);
				DEBUG_INFO("%u", s_stEthernetInfo.au8IpAddr[3]);
                memcpy(&au8Tmp[0], (uint8_t*)&ipv4SubnetMask, 4);
                s_stEthernetInfo.au8SubnetMask[0] = au8Tmp[3];
                s_stEthernetInfo.au8SubnetMask[1] = au8Tmp[2];
                s_stEthernetInfo.au8SubnetMask[2] = au8Tmp[1];
                s_stEthernetInfo.au8SubnetMask[3] = au8Tmp[0];
				DEBUG_INFO(",%u-", s_stEthernetInfo.au8SubnetMask[0]);
				DEBUG_INFO("%u-", s_stEthernetInfo.au8SubnetMask[1]);
				DEBUG_INFO("%u-", s_stEthernetInfo.au8SubnetMask[2]);
				DEBUG_INFO("%u", s_stEthernetInfo.au8SubnetMask[3]);
                memcpy(&au8Tmp[0], (uint8_t*)&ipv4Router, 4);
                s_stEthernetInfo.au8Gateway[0] = au8Tmp[3];
                s_stEthernetInfo.au8Gateway[1] = au8Tmp[2];
                s_stEthernetInfo.au8Gateway[2] = au8Tmp[1];
                s_stEthernetInfo.au8Gateway[3] = au8Tmp[0];
				DEBUG_INFO(",%u-", s_stEthernetInfo.au8Gateway[0]);
				DEBUG_INFO("%u-", s_stEthernetInfo.au8Gateway[1]);
				DEBUG_INFO("%u-", s_stEthernetInfo.au8Gateway[2]);
				DEBUG_INFO("%u", s_stEthernetInfo.au8Gateway[3]);
                memcpy(&au8Tmp[0], (uint8_t*)&ipv4DNS, 4);
                s_stEthernetInfo.au8Dns[0] = au8Tmp[3];
                s_stEthernetInfo.au8Dns[1] = au8Tmp[2];
                s_stEthernetInfo.au8Dns[2] = au8Tmp[1];
                s_stEthernetInfo.au8Dns[3] = au8Tmp[0];
				DEBUG_INFO(",%u-", s_stEthernetInfo.au8Dns[0]);
				DEBUG_INFO("%u-", s_stEthernetInfo.au8Dns[1]);
				DEBUG_INFO("%u-", s_stEthernetInfo.au8Dns[2]);
				DEBUG_INFO("%u", s_stEthernetInfo.au8Dns[3]);

                // Initialize sockets
                _InitEthernetSockets();

                // Update connected flag and move to next state
                s_stEthernetInfo.fConnected = true;
                s_stEthernetInfo.eState = ETHERNET_STATE_IDLE;
            }
            if (Timer_GetTimer(TIMER_ETH_CONNECION_TIMEOUT)==0) {
                // Go back to the beginning
                s_stEthernetInfo.fConnected = false;
                s_stEthernetInfo.eState = ETHERNET_STATE_INITIALIZE;
            }
            break;

        case ETHERNET_STATE_IDLE:
			//DEBUG_INFO("ETHERNET STATE IDLE");
            ip = IPV4_GetMyIP();
            if (ip != s_stEthernetInfo.u32IpAddr) {
				DEBUG_INFO("ETHERNET STATE IDLE (IP CHANGED)");
                DEBUG_INFO("IP address changed. ");
                // Store the IP address as it comes from the stack (byte swapped)
                s_stEthernetInfo.u32IpAddr = ipv4Address;
                // Copy the network configuration into the info structure - make sure to perform by
                memcpy(&au8Tmp[0], (uint8_t*)&ipv4Address, 4);
                s_stEthernetInfo.au8IpAddr[0] = au8Tmp[3];
                s_stEthernetInfo.au8IpAddr[1] = au8Tmp[2];
                s_stEthernetInfo.au8IpAddr[2] = au8Tmp[1];
                s_stEthernetInfo.au8IpAddr[3] = au8Tmp[0];
                memcpy(&au8Tmp[0], (uint8_t*)&ipv4SubnetMask, 4);
                s_stEthernetInfo.au8SubnetMask[0] = au8Tmp[3];
                s_stEthernetInfo.au8SubnetMask[1] = au8Tmp[2];
                s_stEthernetInfo.au8SubnetMask[2] = au8Tmp[1];
                s_stEthernetInfo.au8SubnetMask[3] = au8Tmp[0];
                memcpy(&au8Tmp[0], (uint8_t*)&ipv4Router, 4);
                s_stEthernetInfo.au8Gateway[0] = au8Tmp[3];
                s_stEthernetInfo.au8Gateway[1] = au8Tmp[2];
                s_stEthernetInfo.au8Gateway[2] = au8Tmp[1];
                s_stEthernetInfo.au8Gateway[3] = au8Tmp[0];
                memcpy(&au8Tmp[0], (uint8_t*)&ipv4DNS, 4);
                s_stEthernetInfo.au8Dns[0] = au8Tmp[3];
                s_stEthernetInfo.au8Dns[1] = au8Tmp[2];
                s_stEthernetInfo.au8Dns[2] = au8Tmp[1];
                s_stEthernetInfo.au8Dns[3] = au8Tmp[0];
            }
            break;

        case ETHERNET_STATE_ERROR:
			DEBUG_INFO("ETHERNET STATE ERROR");
            // Cleanup connection
            s_stEthernetInfo.u32IpAddr = 0;
            memset(&s_stEthernetInfo.au8IpAddr[0], 0, 4);
            memset(&s_stEthernetInfo.au8SubnetMask[0], 0, 4);
            memset(&s_stEthernetInfo.au8Gateway[0], 0, 4);
            memset(&s_stEthernetInfo.au8Dns[0], 0, 4);

            // Cleanup socket(s)
			_DeinitEthernetSockets();

            // Go back to the beginning
            s_stEthernetInfo.fConnected = false;
            s_stEthernetInfo.eState = ETHERNET_STATE_INITIALIZE;
            break;

        default:
            break;
    }
}

static char* s_pcLookupHostname = NULL;
static uint32_t* s_pu32LookupHostIp = NULL;
dns_function_ptr s_pfDnsResponseCb = NULL;
static uint16_t s_u16TransactionID = 0;

void _ManageEthernetRemoteSocketState(void) {
    static uint8_t s_u8TxAttemptCount = 0;
    static uint8_t s_u8RxAttemptCount = 0;
    static uint16_t s_u16ResponseSize = 0;
    static bool s_fPendingRepsonse = false;
    static bool s_fPendingClose = false;
    static bool s_fUseNextHost = false;
    socketState_t eSocketState;
    uint16_t u16RxSize;
    uint16_t u16RxSpace;

    // Bail out if the sockets haven't been initialized
    if (s_fEthernetSocketsInitialized==false) return;

    // Poll socket to determine state
    eSocketState = TCP_SocketPoll(&s_stEthernetSocketRemote);

    switch (eSocketState) {
        case NOT_A_SOCKET: // This is not a socketk;
			//DEBUG_INFO("Socket (NOT A SOCKET)");
			// Insert and initialize the socket
            if (TCP_SocketInit(&s_stEthernetSocketRemote) != SUCCESS) return;
			break;	
        case SOCKET_CLOSED: // Socket closed
			// First handle the case where we were waiting for a socket to close correctly
            if (s_fPendingClose==true) {
                // Remove the used socket from the list, clear the flag and bail out (so the message pump can be reprocessed next time through)
                TCP_SocketRemove(&s_stEthernetSocketRemote);
                s_fPendingRepsonse = false;
                s_fPendingClose = false;
                return;
            }
			
            // If there is something to send, try to establish a connection with the server (assuming nothing is currently being processed locally)
			if (s_fEthernetQueueServerSend==true && s_u32EthernetRemoteHostIp!=0) {
                if (s_u16EthernetPostLengthOffset==0) {
                    // Build common post header
                    DEBUG_INFO("Build common header. ");
                    char *pcHostname;
                    pcHostname = CFG_DEFAULT_HOST;
					
                    s_u16EthernetPostPayloadOffset = Http_BuildPostHeaderCommon(&s_acEthernetStrPostHeader[0], pcHostname, false);
                    s_u16EthernetPostLengthOffset = s_u16EthernetPostPayloadOffset - 8; // shift back double CRLF and 4 digits...
                }
                DEBUG_INFO("Open remote socket. ");
                s_stEthernetSocketAddressRemote.addr.s_addr = s_u32EthernetRemoteHostIp;
                s_stEthernetSocketAddressRemote.port = ETHERNET_SOCKET_PORT_HTTP;
				
                // Add receive buffer
                if (TCP_InsertRxBuffer(&s_stEthernetSocketRemote, (uint8_t*)g_acRxBuffer, RX_BUFFER_NUM_BYTES) != true) {
                    if (s_u8TxAttemptCount>5) {} //App_SetLastError(ERROR_ETH_RX_BUFFER_FAILURE);
                    else s_u8TxAttemptCount++;
                    TCP_SocketRemove(&s_stEthernetSocketRemote);
                    return;
                }
                // Attempt connection
                if (TCP_Connect(&s_stEthernetSocketRemote, &s_stEthernetSocketAddressRemote) == false) {
                    if (s_u8TxAttemptCount>5) {} //App_SetLastError(ERROR_ETH_REMOTE_SOCKET_OPEN);
                    else s_u8TxAttemptCount++;
                    TCP_SocketRemove(&s_stEthernetSocketRemote);
                    return;
                }
                Timer_SetTimer(TIMER_ETH_SOCKET_REMOTE_INIT_TIMEOUT, TIMEOUT_SOCKET_REMOTE_INIT);
            }
            break;
		case SOCKET_IN_PROGRESS: // Initiate a connection
			//DEBUG_INFO("Socket (SOCKET IN PROGRESS)");

        if (Timer_GetTimer(TIMER_ETH_SOCKET_REMOTE_INIT_TIMEOUT) == 0) {
            //App_SetLastError(ERROR_ETH_REMOTE_SOCKET_CONNECT);
            TCP_Close(&s_stEthernetSocketRemote);
        }
        break;
        case SOCKET_CONNECTED: // The TCP is in established state user can send/receive data
			//DEBUG_INFO("Socket (SOCKET CONNECTED)");
			// Sending data to the remote server
            if (s_fEthernetQueueServerSend==true && s_fPendingRepsonse==false) {
                char*  pTxDst;
                uint16_t u16FifoBytes;
                uint16_t u16PayloadBytes;
                uint16_t u16BytesToSend;

                // Remote socket now connected. Build and transmit POST

                // Put the pre-built common header into the TX buffer
                pTxDst = (char*)g_acTxBuffer;
                memcpy(pTxDst, s_acEthernetStrPostHeader, s_u16EthernetPostPayloadOffset);
                pTxDst += s_u16EthernetPostPayloadOffset;

                // Put as much of the command FIFO into the TX buffer as possible (TX buffer is big enough to hold all)
                u16FifoBytes = CmdFifo_Send(pTxDst, COMMAND_FIFO_NUM_BYTES);
				
				DEBUG_INFO("%u", u16FifoBytes);
                u16PayloadBytes = u16FifoBytes;
                pTxDst += u16FifoBytes;

                // Determine number of digits
                uint8_t digits = 0;
                if (u16PayloadBytes>999)     {digits = 4;}
                else if (u16PayloadBytes>99) {digits = 3;}
                else if (u16PayloadBytes>9)  {digits = 2;}
                else                         {digits = 1;}
                // Build length string
                char acLength[5];
                sprintf(acLength, "%d", u16PayloadBytes);
                // Append to content length
                char* pDst = &g_acTxBuffer[s_u16EthernetPostLengthOffset];
                memcpy(pDst, acLength, digits);
                pDst += digits;
                // Append double CRLF that separates headers from payload
                memcpy(pDst, "\r\n\r\n", 4);
                pDst += 4;
                // Shift the original payload to the new destination pointer (end of headers)
                if (digits < 4) {
                    memcpy(pDst, (char*)&g_acTxBuffer[s_u16EthernetPostPayloadOffset], u16PayloadBytes);
                    pTxDst -= 4;
                    pTxDst += digits;
                }


                // Transmit the buffer to the server over the TCP socket
                u16BytesToSend = pTxDst - &g_acTxBuffer[0];
                if (u16PayloadBytes>0) {
                    s_u16EthernetRemainingRemoteBytes = u16BytesToSend;
                    s_pvEthernetRemainingRemoteBuffer = (void*)&g_acTxBuffer[0];
                    // Set the response timeout (will also include the actual transmission of the data)
                    Timer_SetTimer(TIMER_ETH_SOCKET_REMOTE_RESPONSE_TIMEOUT, TIMEOUT_SOCKET_REMOTE_RESPONSE);

                } else {
                    DEBUG_INFO("Empty payload. ");
                    TCP_Close(&s_stEthernetSocketRemote);
					 DEBUG_INFO("Socket closed");
                }
                // Clear the flag
                s_fEthernetQueueServerSend = false;
                s_u8TxAttemptCount = 0;
                // Clear the response size and attempt count
                s_u16ResponseSize = 0;
                s_u8RxAttemptCount = 0;
            }

            // Check if there is anything (remaining) in the TX buffer to send (can only transfer 1460 bytes at a time)
            if (s_u16EthernetRemainingRemoteBytes!=0) {
                s_stEthernetSocketRemote.localWnd = LOCAL_RX_BUFFER_NUM_BYTES; // set the window to the RX buffer size (to avoid TCP ZeroWindow warnings in Wireshark)
                // If we're ready to send more...
                if (TCP_SendDone(&s_stEthernetSocketRemote)) {
                    // If too many bytes for socket, it will need to be broken up
                    if (s_u16EthernetRemainingRemoteBytes>MAX_TCP_CHUNK_SIZE) {
                        DEBUG_INFO("Sending %d bytes of data to server. ", MAX_TCP_CHUNK_SIZE);
						//DEBUG_INFO("%s", (char *)s_pvEthernetRemainingRemoteBuffer);
                        if (TCP_Send(&s_stEthernetSocketRemote, (uint8_t*)s_pvEthernetRemainingRemoteBuffer, MAX_TCP_CHUNK_SIZE) != true) {
                            DEBUG_INFO("Failed sending segment. ");
                            //App_SetLastError(ERROR_ETH_TX_BUFFER_FAILURE);
                            TCP_Close(&s_stEthernetSocketRemote);
                            return;
                        }
                        s_pvEthernetRemainingRemoteBuffer = (void*)(s_pvEthernetRemainingRemoteBuffer + MAX_TCP_CHUNK_SIZE);
                        s_u16EthernetRemainingRemoteBytes = s_u16EthernetRemainingRemoteBytes - MAX_TCP_CHUNK_SIZE;

                    } else {
                        DEBUG_INFO("Sending %d bytes of data to server. ", s_u16EthernetRemainingRemoteBytes);
						//DEBUG_INFO("%s", (char *)s_pvEthernetRemainingRemoteBuffer);
                        if (TCP_Send(&s_stEthernetSocketRemote, (uint8_t*)s_pvEthernetRemainingRemoteBuffer, s_u16EthernetRemainingRemoteBytes) != true) {
                            DEBUG_INFO("Failed sending. ");
                            //App_SetLastError(ERROR_ETH_TX_BUFFER_FAILURE);
                            TCP_Close(&s_stEthernetSocketRemote);
                            return;
                        }
                        s_pvEthernetRemainingRemoteBuffer = NULL;
                        s_u16EthernetRemainingRemoteBytes = 0;
                        s_fPendingRepsonse = true;
                    }

                // Has a timeout occurred before we've even got to the part where we wait for the actual response
                } else if (Timer_GetTimer(TIMER_ETH_SOCKET_REMOTE_RESPONSE_TIMEOUT)==0) {
                    DEBUG_INFO("Failed sending. ");
                    //App_SetLastError(ERROR_ETH_TX_BUFFER_FAILURE);
                    TCP_Close(&s_stEthernetSocketRemote);
                    return;
                }
            // POST sent... waiting for the server response
            } else {
                // Have we timed out?
                if (Timer_GetTimer(TIMER_ETH_SOCKET_REMOTE_RESPONSE_TIMEOUT)==0) {
                    //App_SetLastError(ERROR_ETH_RX_TIMEOUT);
                    TCP_Close(&s_stEthernetSocketRemote);
                    return;
                }

                // Make sure send is complete before trying to receive response
                if (TCP_SendDone(&s_stEthernetSocketRemote)) {
                    // Check to see if there is any received data
                    u16RxSize = TCP_GetRxLength(&s_stEthernetSocketRemote);
                    if (u16RxSize > 0) {
                        u16RxSize = TCP_GetReceivedData(&s_stEthernetSocketRemote);
                        DEBUG_INFO("Received %d bytes of data from server. ", u16RxSize);
                        s_u16ResponseSize += u16RxSize;

                        // Parse received response (and act accordingly)
                        if (TcpParser_ParseServerResponse((char*)g_acRxBuffer, s_u16ResponseSize)==true) {
                            // All good! This looks like a valid response from the server, set the flag and timer used to determine lost communication
                            s_fEthernetServerCommEstablished = true;
                            Timer_SetTimer(TIMER_ETH_SERVER_LOST_COMM, 120);
                            //s_fPendingRepsonse = false;
                            // Server will close the socket after sending response - so let it clean itself up
                            TCP_Close(&s_stEthernetSocketRemote);

                        } else {
							DEBUG_INFO("ERROR - invalid response! ");
                       }
                    }
                }
            }

            break;
        case SOCKET_CLOSING: // The user initiated the closing procedure for this socket
			//DEBUG_INFO("Socket (SOCKET CLOSING)");
			// Do nothing
            if (s_fPendingClose==false) {
                s_fPendingClose = true;
                Timer_SetTimer(TIMER_ETH_SOCKET_REMOTE_CLEANUP_TIMEOUT, 3);

            } else if (Timer_GetTimer(TIMER_ETH_SOCKET_REMOTE_CLEANUP_TIMEOUT) == 0) {
                DEBUG_INFO("ERROR - Failed remote socket cleanup! ");
                s_fPendingRepsonse = false;
                s_fPendingClose = false;
                // TODO.. reinit remote socket
            }
            break;
        default:
            DEBUG_INFO("Invalid remote socket state! ");
            s_fEthernetQueueServerSend = false;
            s_fPendingRepsonse = false;
            s_fPendingClose = false;
            // TODO.. reinit remote socket
            break;
    }

    // Check if it is time to resolve the host name
    if (Timer_GetTimer(TIMER_ETH_DNS_REMOTE_RESOLUTION_TIMEOUT)==0) {
        s_u32EthernetRemoteHostIp = 0;
        s_u16EthernetPostPayloadOffset = 0;
        s_u16EthernetPostLengthOffset = 0;
        // Attempt to access the DNS state machine
		
        if (Ethernet_IsConnected()==true && s_eEthernetDnsState==DNS_STATE_IDLE) {
            s_u8EthernetRemoteDnsAttempts = 0;

            bool fDnsRequested = false;
            do {
                // Determine the hostname
                char *hostname = NULL;
                hostname = CFG_DEFAULT_HOST;
                // Send request if valid hostname
                if (hostname!=NULL) {
                    _DnsLookup(hostname, &s_u32EthernetRemoteHostIp, &_DnsRemoteHostCb);
					DEBUG_INFO("HostName-%s", hostname);
                    fDnsRequested = true;
                } 
            } while (fDnsRequested==false);
            Timer_SetTimer(TIMER_ETH_DNS_REMOTE_RESOLUTION_TIMEOUT, TIMEOUT_DNS_RERESOLVE);
        }
    }
}


void _DnsRemoteHostCb(void) {
	uint8_t au8Tmp[4];
    memcpy(&au8Tmp[0], (uint8_t*)&s_u32EthernetRemoteHostIp, 4);
	DEBUG_INFO("%u-", au8Tmp[3]);
	DEBUG_INFO("%u-", au8Tmp[2]);
	DEBUG_INFO("%u-", au8Tmp[1]);
	DEBUG_INFO("%u-", au8Tmp[0]);

	DEBUG_INFO("DNS Remove Host CB");
	if (s_u32EthernetRemoteHostIp!=0) {
		#if 1
		DEBUG_INFO("%s resolved to IP Address 0x%X. ", s_pcLookupHostname, *s_pu32LookupHostIp);
		#else
		//DEBUG_INFO("%s resolved to IP Address 0x%X but overriding to 0x3400BB17 (52.0.187.23). ", s_pcLookupHostname, *s_pu32LookupHostIp);
		//s_u32EthernetRemoteHostIp = 0x3400BB17;
		//DEBUG_INFO("%s resolved to IP Address 0x%X but overriding to 0x3400BB17 (52.200.251.141). ", s_pcLookupHostname, *s_pu32LookupHostIp);
		//s_u32EthernetRemoteHostIp = 0x34C8FB8D;
		//DEBUG_INFO("%s resolved to IP Address 0x%X but overriding to 0x3400BB17 (52.86.132.88). ", s_pcLookupHostname, *s_pu32LookupHostIp);
		//s_u32EthernetRemoteHostIp = 0x34568458;
		#endif


		} else {
		DEBUG_INFO("Failed resolving %s. ", s_pcLookupHostname);
		if (s_u8EthernetRemoteDnsAttempts<3) {
			// Set the error state
			if (s_u8EthernetHostIdx==0) {}
			if (s_u8EthernetHostIdx==1) {}
			if (s_u8EthernetHostIdx==2) {}
			if (s_u8EthernetHostIdx==3) {} 
			// Send the next DNS request
			bool fDnsRequested = false;
			do {
				// Determine the hostname
				char *hostname = NULL;
				hostname = CFG_DEFAULT_HOST;
				// Send request if valid hostname
				if (hostname!=NULL) {
					_DnsLookup(hostname, &s_u32EthernetRemoteHostIp, &_DnsRemoteHostCb);
					fDnsRequested = true;
				}
			} while (fDnsRequested==false);

			// Clear established flag and re-initialize connection
			s_fEthernetServerCommEstablished = false;
			s_stEthernetInfo.eState = ETHERNET_STATE_ERROR;
			} else {
			DEBUG_INFO("Unable to resolve. ");
			// Failed resolution of primary and secondary - soft reset the device
			//App_SoftReset();
		}
	}
}

void _ManageEthernetDnsState(void) {
    static uint8_t su8Attempts;
    static bool sfUseSecondaryDns;
    static uint32_t su32DnsServer;

    // Bail out if we aren't even connected
    if (s_stEthernetInfo.fConnected==false) return;
    switch(s_eEthernetDnsState) {
        case DNS_STATE_IDLE: // Ready to process DNS requests
            break;

        case DNS_STATE_START: // Initial state to reset client state variables
			DEBUG_INFO("DNS STATE (START)");
            su8Attempts = 0;
            sfUseSecondaryDns = false;
            su32DnsServer = ipv4DNS[0];
			
            *s_pu32LookupHostIp = 0;
            s_eEthernetDnsState = DNS_STATE_OPEN_SOCKET;
            Timer_SetTimer(TIMER_ETH_DNS_TIMEOUT, TIMEOUT_DNS_RESOLUTION);
            // No need to break. Drop right into next state

        case DNS_STATE_OPEN_SOCKET: // Open UDP socket - will perform ARP if necessary
			DEBUG_INFO("DNS STATE (OPEN)");
            if (Timer_GetTimer(TIMER_ETH_DNS_TIMEOUT)==0) {
                s_eEthernetDnsState = DNS_STATE_FAIL;
            } else {
                if (UDP_Start(su32DnsServer, DNS_APP_PORT, DNS_PORT) != SUCCESS) {
                    break;
                } else {
                    s_eEthernetDnsState = DNS_STATE_QUERY;
                }
            }
            // No need to break, we can begin resolution immediately

        case DNS_STATE_QUERY: // Send DNS query to DNS server
			DEBUG_INFO("DNS STATE (QUERY)");
            // Put DNS query here
            s_u16TransactionID++;
            UDP_Write16(s_u16TransactionID);
            UDP_Write16(0x0100); // Standard query with recursion
            UDP_Write16(0x0001); // 0x0001 questions
            UDP_Write16(0x0000); // 0x0000 answers
            UDP_Write16(0x0000); // 0x0000 name server resource records
            UDP_Write16(0x0000); // 0x0000 additional records
            // Put host name string to resolve
            _DnsWriteHostname();
            UDP_Write16(0x0001); // Type A (host address)
            UDP_Write16(0x0001); // Class: IN (Internet)
            // Flush the UDP
            UDP_Send();
            // Reset the timeout to wait for response
            Timer_SetTimer(TIMER_ETH_DNS_TIMEOUT, TIMEOUT_DNS_RESOLUTION);
            // Move to next state
            s_eEthernetDnsState = DNS_STATE_GET_RESULT;
            break;

        case DNS_STATE_GET_RESULT: // Wait for response from DNS server (called via UDP_Receive
            if (*s_pu32LookupHostIp==0) {
                if (Timer_GetTimer(TIMER_ETH_DNS_TIMEOUT)==0) {
					DEBUG_INFO("DNS STATE (GET-FAIL)");
                    s_eEthernetDnsState = DNS_STATE_FAIL;
                }
            } else {
				DEBUG_INFO("DNS STATE (GET-DONE)");
                s_eEthernetDnsState = DNS_STATE_DONE;
            }

            break;
        case DNS_STATE_FAIL: // ARP or DNS server not responding
			DEBUG_INFO("DNS STATE (FAIL)");
            su8Attempts++;
            // Retry resolution
            if (su8Attempts<2) {
                s_eEthernetDnsState = DNS_STATE_OPEN_SOCKET;
                Timer_SetTimer(TIMER_ETH_DNS_TIMEOUT, TIMEOUT_DNS_RESOLUTION);

            // Number of retries has been exceeded
            } else {
                // Go back to the idle state so the next request can be initiated
                s_eEthernetDnsState = DNS_STATE_IDLE;
				
                // Call the registered handler - make sure it handles the IP==0 error condition
                s_pfDnsResponseCb();
            }
            break;

        case DNS_STATE_DONE: // DNS query is finished
			DEBUG_INFO("DNS STATE (DONE)");
            // Go back to the idle state so the next request can be initiated
            s_eEthernetDnsState = DNS_STATE_IDLE;
            // Call the registered handler
            s_pfDnsResponseCb();
        default:
            break;
    }
}

void _DnsWriteHostname(void) {
	char* pcHostname;
	char* pcLookAhead;
	char cByte;
	uint8_t u8Len;

	// Initialize pointers
	pcHostname = s_pcLookupHostname;
	pcLookAhead = s_pcLookupHostname;

	// Loop through the host name string formatting correctly for DNS query
	while (1) {
		u8Len = 0;
		do {
			cByte = *pcLookAhead++;
			u8Len++;
		} while((cByte!='\0') && (cByte!='.') && (cByte!='/') && (cByte!=',') && (cByte!='>'));
		// Decrement the length, to exclude the '.', and write it to the UDP
		u8Len--;
		UDP_Write8(u8Len);
		// Put the data up to (but excluding) the '.' in the host name string
		do {
			UDP_Write8(*pcHostname++);
			u8Len--;
		} while(u8Len>0);
		pcHostname++;
		// Break out of the while loop if the last byte read indicates the end of the host name
		if ((cByte=='\0') || (cByte=='/') || (cByte==',') || (cByte=='>'))
		break;
	}
	UDP_Write8(0x00); // Write the null terminator for the host name string
}

void _InitEthernetSockets(void) {
    s_fEthernetSocketsInitialized = true;
}

void _DnsLookup(char* pcHostname, uint32_t* pu32HostIp, dns_function_ptr pfResponseHandler) {
	// Bail out if the DNS is already in use
	if (s_eEthernetDnsState!=DNS_STATE_IDLE) return;

	// Store the result pointers
	s_pcLookupHostname = pcHostname;
	s_pu32LookupHostIp = pu32HostIp;
	s_pfDnsResponseCb = pfResponseHandler;

	// Initiate the DNS
	s_eEthernetDnsState = DNS_STATE_START;
}

bool Ethernet_IsConnected(void) {
	if (s_stEthernetInfo.fConnected==true && s_stEthernetInfo.eState==ETHERNET_STATE_IDLE) return true;
	else return false;
}

void _DeinitEthernetSockets(void) {
    s_fEthernetSocketsInitialized = false;
    TCP_SocketRemove(&s_stEthernetSocketLocal);
    s_pvEthernetRemainingLocalBuffer = NULL;
    s_u16EthernetRemainingLocalBytes = 0;
    TCP_SocketRemove(&s_stEthernetSocketRemote);
    s_pvEthernetRemainingRemoteBuffer = NULL;
    s_u16EthernetRemainingRemoteBytes = 0;
}

/**************************************************************************//**
 *  UDP Callback function (added to TCP/IP Lite stack by Nick) to handle
 *  responses received on UDP port 4096
 *  @param[in]  int nLength Length of UDP payload to process
 *  @param[out] None
 *  @return     Nothing
 ******************************************************************************/
void DNS_Handler(int nLength)
{
    tsDnsHeader stDnsHeader;
    tsDnsAnswerHeader stDnsAnswerHeader;

    DEBUG_INFO("Process DNS query response. ");
    // Retrieve the transaction ID - ignore if not a response to the last request
    stDnsHeader.u16TransactionID = UDP_Read16();
    if (stDnsHeader.u16TransactionID!=s_u16TransactionID) {
        DEBUG_INFO("Unexpected transaction ID. ");
        return;
    }
    // Retrieve the DNS header
    stDnsHeader.u16Flags = UDP_Read16();
    stDnsHeader.u16Questions = UDP_Read16();
    stDnsHeader.u16Answers = UDP_Read16();
    stDnsHeader.u16AuthoritativeRecords = UDP_Read16();
    stDnsHeader.u16AdditionalRecords = UDP_Read16();
    // Remove all questions (queries)
    while(stDnsHeader.u16Questions--) {
        _DnsDiscardName(); // Throw away response name
        UDP_Read16(); // Question type
        UDP_Read16(); // Question class
    }
    // Scan through answers
    while(stDnsHeader.u16Answers--) {
        _DnsDiscardName(); // Throw away response name
        stDnsAnswerHeader.u16ResponseType = UDP_Read16(); // Response type
        stDnsAnswerHeader.u16ResponseClass = UDP_Read16(); // Response class
        stDnsAnswerHeader.u32ResponseTTL = UDP_Read32(); // Time to live
        stDnsAnswerHeader.u16ResponseLen = UDP_Read16(); // Response length
        // Make sure that this is a 4 byte IP address, response type A or MX, class 1
        // Check if this is Type A or MX
        if (stDnsAnswerHeader.u16ResponseType == 0x0001u && stDnsAnswerHeader.u16ResponseClass == 0x0001u && stDnsAnswerHeader.u16ResponseLen == 0x0004u) {
            *s_pu32LookupHostIp = UDP_Read32();
            goto DoneSearchingRecords;
        } else {
            while(stDnsAnswerHeader.u16ResponseLen--) {
                UDP_Read8();
            }
        }
    }
    // Remove all Authoritative Records
    while(stDnsHeader.u16AuthoritativeRecords--) {
        _DnsDiscardName(); // Throw away response name
        stDnsAnswerHeader.u16ResponseType = UDP_Read16(); // Response type
        stDnsAnswerHeader.u16ResponseClass = UDP_Read16(); // Response class
        stDnsAnswerHeader.u32ResponseTTL = UDP_Read32(); // Time to live
        stDnsAnswerHeader.u16ResponseLen = UDP_Read16(); // Response length
        // Make sure that this is a 4 byte IP address, response type A or MX, class 1
        // Check if this is Type A or MX
        if (stDnsAnswerHeader.u16ResponseType == 0x0001u && stDnsAnswerHeader.u16ResponseClass == 0x0001u && stDnsAnswerHeader.u16ResponseLen == 0x0004u) {
            *s_pu32LookupHostIp = UDP_Read32();
            goto DoneSearchingRecords;
        } else {
            while(stDnsAnswerHeader.u16ResponseLen--) {
                UDP_Read8();
            }
        }
    }
    // Remove all Additional Records
    while(stDnsHeader.u16AdditionalRecords--) {
        _DnsDiscardName(); // Throw away response name
        stDnsAnswerHeader.u16ResponseType = UDP_Read16(); // Response type
        stDnsAnswerHeader.u16ResponseClass = UDP_Read16(); // Response class
        stDnsAnswerHeader.u32ResponseTTL = UDP_Read32(); // Time to live
        stDnsAnswerHeader.u16ResponseLen = UDP_Read16(); // Response length
        // Make sure that this is a 4 byte IP address, response type A or MX, class 1
        // Check if this is Type A or MX
        if (stDnsAnswerHeader.u16ResponseType == 0x0001u && stDnsAnswerHeader.u16ResponseClass == 0x0001u && stDnsAnswerHeader.u16ResponseLen == 0x0004u) {
            *s_pu32LookupHostIp = UDP_Read32();
            goto DoneSearchingRecords;
        } else {
            while(stDnsAnswerHeader.u16ResponseLen--) {
                UDP_Read8();
            }
        }
    }

DoneSearchingRecords:
    s_eEthernetDnsState = DNS_STATE_DONE;
}

void _DnsDiscardName(void) {
    uint8_t u8Byte;

    // This function reads a name string from the DNS socket.  Each string
    // consists of a series of labels.  Each label consists of a length prefix
    // byte, followed by the label bytes.  At the end of the string, a zero length
    // label is found as termination.  If name compression is used, this function
    // will automatically detect the pointer and discard it.

    while (1) {
        // Get first byte which will tell us if this is a 16-bit pointer or the length of a label
        u8Byte = UDP_Read8();
        if ((u8Byte & 0xC0u) == 0xC0u) {
            u8Byte = UDP_Read8();
            return;
        }

        // Exit once we reach a zero length label
        if (u8Byte == 0u) return;

        // Discard complete label
        while (u8Byte--) {
            UDP_Read8();
        }
    }
}

void Ethernet_QueueServerSend(void) {
    // No point in setting the flag if it is already set
    if (s_fEthernetQueueServerSend==true) return;
    // No point in setting the flag is still waiting on a response
    if (s_stEthernetSocketRemote.socketState==SOCKET_CONNECTED || s_stEthernetSocketRemote.socketState==SOCKET_IN_PROGRESS) return;
	
	if (CmdFifo_GetByteCount()>0) s_fEthernetQueueServerSend = true;
}