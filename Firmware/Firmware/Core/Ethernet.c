/** INCLUDES ******************************************************************/
#include "driver_init.h"
#include <string.h>
#include "tcpip_lite_start.h"
#include "ipv4.h"
#include "Ethernet.h"
#include <App/DcaApp.h>


/** LOCAL (PRIVATE) TYPEDEFS, STRUCTURES AND ENUMERATIONS *********************/
#define MAX_TCP_CHUNK_SIZE                          1396    // TCP_MAX_SEG_SIZE (1460) - room for up to 64 bytes for Ethernet frame overhead
#define DNS_PORT                                    53      // Default port for DNS query
#define DNS_APP_PORT                                4096    // Application port to process DNS response
// Pull port constants from application level (if defined)
#ifdef EYEDRO_SOCKET_PORT_LOCAL_WEBSERVER
    #define ETHERNET_SOCKET_PORT_LOCAL_WEBSERVER    EYEDRO_SOCKET_PORT_LOCAL_WEBSERVER
#else
    #define ETHERNET_SOCKET_PORT_LOCAL_WEBSERVER    8080
#endif
#ifdef EYEDRO_SOCKET_PORT_HTTP
    #define ETHERNET_SOCKET_PORT_HTTP               EYEDRO_SOCKET_PORT_HTTP
#else
    #define ETHERNET_SOCKET_PORT_HTTP               80
#endif
#ifdef EYEDRO_SOCKET_PORT_HTTP_ALT
    #define ETHERNET_SOCKET_PORT_HTTP_ALT           EYEDRO_SOCKET_PORT_HTTP_ALT
#else
    #define ETHERNET_SOCKET_PORT_HTTP_ALT           8008
#endif

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
//static char             s_acEthernetStrPostHeader[HTTP_POST_HDR_BYTES];

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


// The following are required for data push functionality
#ifdef DATA_PUSH_ENABLED
static uint16_t         TIMER_ETH_DNS_PUSH_RESOLUTION_TIMEOUT;
static uint16_t         TIMER_ETH_SOCKET_PUSH_INIT_TIMEOUT;
static uint16_t         TIMER_ETH_SOCKET_PUSH_RESPONSE_TIMEOUT;

static bool             s_fEthernetQueuePush = false;
static uint16_t         s_u16EthernetPushPostNumBytes = 0;

static uint32_t         s_u32EthernetPushHostIp = 0;

static tcpTCB_t         s_stEthernetSocketPush;
static sockaddr_in_t    s_stEthernetSocketAddressPush;
#endif



/** LOCAL (PRIVATE) CONSTANT AND MACRO DEFINITIONS ****************************/


/** LOCAL (PRIVATE) FUNCTION PROTOTYPES ***************************************/
bool _DetectEthernet(void);
bool _HasLink(void);


/** PUBLIC FUNCTION IMPLEMENTATIONS *******************************************/
bool Ethernet_Detect(void) {
    // Remove device from reset
    gpio_set_pin_level(ETHERNET_RESET, 1);
    delay_ms(1);
    // Detect the device
    return _DetectEthernet();
}

int8_t Ethernet_Init(void)
{
    // First determine if the Ethernet device exists
    bool fHasEthernet = Ethernet_Detect();
    if (fHasEthernet==false) return -1;

    return 0;
}

static bool s_fLinkUp = false;
void Ethernet_PumpEvents(void){
}


void Ethernet_PowerDown(void) {
}
void Ethernet_PowerUp(void) {
}
bool Ethernet_HasLink(void) {
	
}
bool Ethernet_IsConnected(void) {
	
}
bool Ethernet_IsServerConnectionEstablished(void) {
	
}

void Ethernet_GetCfgIpAddress(uint8_t *ipaddr) {
	
}
void Ethernet_GetCfgGateway(uint8_t *gateway) {
	
}
void Ethernet_GetCfgSubnet(uint8_t *subnet) {
	
}
void Ethernet_GetCfgDns(uint8_t *dns) {
	
}

void Ethernet_QueueServerSend(void) {
	
}

void Ethernet_PumpEvents(void);

void Ethernet_PowerDown(void);
void Ethernet_PowerUp(void);

/** LOCAL (PRIVATE) FUNCTION IMPLEMENTATIONS **********************************/
bool _DetectEthernet() {
   struct io_descriptor *s_ioSPI;
    int8_t result = 0;
	printf("Result: %d", result);
    uint8_t tx_buffer[2];
    uint8_t rx_buffer[1];
	printf("here1");
    // Get the IO descriptor for the SPI peripheral
    if (spi_m_sync_get_io_descriptor(&ETHERNET_SPI, &s_ioSPI)!=0 ) {
        return false;
    };
	printf("here2");
    // Enable the SPI peripheral
    spi_m_sync_enable(&ETHERNET_SPI);
printf("here3");
    // Select register bank 3 (write control register 0x1F)
    // Prepare payload
    tx_buffer[0] = 0xA0|0x1F;   // Bit field clear | ECON1 register address
    tx_buffer[1] = 0x03;        // Bank 3
    // Enable the SPI slave (activates CSn)
    gpio_set_pin_level(ETHERNET_CS, 0);
    //delay_ms(1);
    // Write payload
    if (io_write(s_ioSPI, tx_buffer, 2) != 2) result = -1;
	printf("Result: %d", result);
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
	printf("Result: %d", result);
    // Disable the SPI slave (deactivates CSn)
    gpio_set_pin_level(ETHERNET_CS, 1);

    // Read the Ethernet revision register
    // Prepare payload (register to read) -
    tx_buffer[0] = 0x00|0x12;   // Read control register | EREVID register address
    // Enable the SPI slave (activates CSn)
    gpio_set_pin_level(ETHERNET_CS, 0);
    // Write address of register to read - SPI
    if (io_write(s_ioSPI, tx_buffer, 1) != 1) result = -1;
	printf("Result: %d", result);
    // Read register payload
    if (io_read(s_ioSPI, rx_buffer, 1) != 1)  result = -1;
	printf("Result: %d", result);
    // Disable the SPI slave (deactivates CSn)
    gpio_set_pin_level(ETHERNET_CS, 1);
printf("here4");
    // Disable the SPI peripheral
    spi_m_sync_disable(&ETHERNET_SPI);
printf("here5");
    // Failure to write/read above indicates the device is not present
    if (result==-1) return false;
    // 0x00 or 0xFF in the REV ID register indicates that the device is not present
    else if (rx_buffer[0]==0x00 || rx_buffer[0]==0xFF) return false;
    // If we make it here, the device is present
    else return true;
}