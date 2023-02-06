#ifndef ETHERNET_H_
#define ETHERNET_H_

#ifdef __cplusplus
extern "C" {
#endif


/** INCLUDES ******************************************************************/
#include <compiler.h>

// Ethernet state
typedef enum
{
	ETHERNET_STATE_INITIALIZE,
	ETHERNET_STATE_NETWORK_CONFIG,      // Retrieve network config
	ETHERNET_STATE_IDLE,                // Idle and ready for processing
	ETHERNET_STATE_ERROR
} eEthernetState;

// WiFi info
typedef struct Ethernet
{
	bool fConnected;                    // Ethernet connection flag
	eEthernetState eState;              // Ethernet state
	uint32_t u32IpAddr;                 // IP address as it comes from the stack
	uint8_t au8IpAddr[4];               // IP Address
	uint8_t au8SubnetMask[4];           // Subnet Mask
	uint8_t au8Gateway[4];              // Gateway
	uint8_t au8Dns[4];                  // DNS Address
} sEthernetInfo;

/** PUBLIC FUNCTION PROTOTYPES ************************************************/
bool Ethernet_Detect(void);
int8_t Ethernet_Init(void);
bool Ethernet_IsConnected(void);

void Ethernet_PumpEvents(void);

void Ethernet_PowerDown(void);
void Ethernet_PowerUp(void);


/** DEBUG *********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* ETHERNET_H_ */