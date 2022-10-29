/** Include **/
#include "asf.h"
#include "common/include/nm_common.h"
#include "driver/include/m2m_wifi.h"
#include "wifi.h"
#include "uart.h"
#include "main.c"

/** Prototypes **/

/** Data Members **/

/** Methods **/
void Wifi_Init() {
	/* Initialize the BSP. */
	nm_bsp_init();

	/* Initialize Wi-Fi parameters structure. */
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));

	/* Initialize Wi-Fi driver with data and status callbacks. */
	param.pfAppWifiCb = wifi_cb;
	m2m_wifi_init(&param);
}

void Wifi_Init_AP() {
	/* Initialize AP mode parameters structure with SSID, channel and OPEN security type. */
	memset(&strM2MAPConfig, 0x00, sizeof(tstrM2MAPConfig));
	strcpy((char *)&strM2MAPConfig.au8SSID, MAIN_WLAN_SSID);
	strM2MAPConfig.u8ListenChannel = MAIN_WLAN_CHANNEL;
	strM2MAPConfig.u8SecType = MAIN_WLAN_AUTH;
	strM2MAPConfig.au8DHCPServerIP[0] = 0xC0; /* 192 */
	strM2MAPConfig.au8DHCPServerIP[1] = 0xA8; /* 168 */
	strM2MAPConfig.au8DHCPServerIP[2] = 0x01; /* 1 */
	strM2MAPConfig.au8DHCPServerIP[3] = 0x01; /* 1 */
	
	m2m_wifi_start_provision_mode((tstrM2MAPConfig *)&strM2MAPConfig, (char*)"dca-provision.com", 1);
	printf("Provision Mode started.\r\nConnect to [%s] via AP[%s] and fill up the page.\r\n","dca-provision.com", strM2MAPConfig.au8SSID);

	printf("AP Provision mode started.\r\nOn the android device, connect to %s then run setting app.\r\n", MAIN_WLAN_SSID);
}

void Wifi_Disable_AP() {
	m2m_wifi_disable_ap();
}

void Wifi_Init_Default() {
	ret = m2m_wifi_default_connect();
	printf("%d", ret);
}

static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
	 case M2M_WIFI_RESP_PROVISION_INFO:
	 {
		 printf("\r\nM2M_WIFI_RESP_PROVISION_INFO");
		 tstrM2MProvisionInfo *pstrProvInfo = (tstrM2MProvisionInfo *)pvMsg;
		 if (pstrProvInfo->u8Status == M2M_SUCCESS) {			m2m_wifi_connect((char *)pstrProvInfo->au8SSID, strlen((char *)pstrProvInfo->au8SSID), pstrProvInfo->u8SecType, pstrProvInfo->au8Password, M2M_WIFI_CH_ALL);
		 }
		 break;
	 }
	 case M2M_WIFI_REQ_DHCP_CONF:
    {
		printf("\r\nM2M_WIFI_REQ_DHCP_CONF");
        m2m_wifi_get_connection_info();
        break;
    }
    case M2M_WIFI_RESP_CONN_INFO:
    {
		printf("\r\nM2M_WIFI_RESP_CONN_INFO\n");
        tstrM2MConnInfo        *pstrConnInfo = (tstrM2MConnInfo*)pvMsg;
        printf("AP Connection Information\n*********************************\n");
        printf("Local IP Address    : %d.%d.%d.%d\n", pstrConnInfo->au8IPAddr[0], pstrConnInfo->au8IPAddr[1], pstrConnInfo->au8IPAddr[2], pstrConnInfo->au8IPAddr[3]);
        printf("SSID             : %s\n",pstrConnInfo->acSSID);
        printf("SEC TYPE         : %d\n",pstrConnInfo->u8SecType);
        printf("AP MAC Address        : %02x:%02x:%02x:%02x:%02x:%02x\n",
        pstrConnInfo->au8MACAddress[0], pstrConnInfo->au8MACAddress[1],pstrConnInfo->au8MACAddress[2],pstrConnInfo->au8MACAddress[3],
        pstrConnInfo->au8MACAddress[4],pstrConnInfo->au8MACAddress[5]);
        printf("Signal Strength        : %d\n", pstrConnInfo->s8RSSI);
        printf("Current Channel        : %d\n", pstrConnInfo->u8CurrChannel);   
        break;
    }
	case M2M_WIFI_REQ_CONNECT:
	{
		printf("\r\nM2M_WIFI_REQ_CONNECT");
		break;
	}
	case M2M_WIFI_REQ_DEFAULT_CONNECT:
	{
		printf("\r\nM2M_WIFI_REQ_DEFAULT_CONNECT");
		break;
	}
	case M2M_WIFI_RESP_DEFAULT_CONNECT:
	{
		printf("\r\nM2M_WIFI_RESP_DEFAULT_CONNECT");
		break;
	}
	case M2M_WIFI_REQ_GET_CONN_INFO:
	{
		printf("\r\nM2M_WIFI_REQ_GET_CONN_INFO");
		break;
	}
	case M2M_WIFI_REQ_DISCONNECT:
	{
		printf("\r\nM2M_WIFI_REQ_DISCONNECT");
		break;
	}
	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		printf("\r\nM2M_WIFI_RESP_CON_STATE_CHANGED");
		break;
	}
	case M2M_WIFI_REQ_SLEEP:
	{
		printf("\r\nM2M_WIFI_REQ_SLEEP");
		break;
	}
	case M2M_WIFI_REQ_SCAN:
	{
		printf("\r\nM2M_WIFI_REQ_SCAN");
		break;
	}
	case M2M_WIFI_REQ_WPS_SCAN:
	{
		printf("\r\nM2M_WIFI_REQ_WPS_SCAN");
		break;
	}
	case M2M_WIFI_RESP_SCAN_DONE:
	{
		printf("\r\nM2M_WIFI_RESP_SCAN_DONE");
		break;
	}
	case M2M_WIFI_REQ_SCAN_RESULT:
	{
		printf("\r\nM2M_WIFI_REQ_SCAN_RESULT");
		break;
	}
	case M2M_WIFI_RESP_SCAN_RESULT:
	{
		printf("\r\nM2M_WIFI_RESP_SCAN_RESULT");
		break;
	}
	case M2M_WIFI_REQ_WPS:
	{
		printf("\r\nM2M_WIFI_REQ_WPS");
		break;
	}
	case M2M_WIFI_REQ_START_WPS:
	{
		printf("\r\nM2M_WIFI_REQ_START_WPS");
		break;
	}
	case M2M_WIFI_REQ_DISABLE_WPS:
	{
		printf("\r\nM2M_WIFI_REQ_DISABLE_WPS");
		break;
	}
	case M2M_WIFI_RESP_IP_CONFIGURED:
	{
		printf("\r\nM2M_WIFI_RESP_IP_CONFIGURED");
		break;
	}
	case M2M_WIFI_RESP_IP_CONFLICT:
	{
		printf("\r\nM2M_WIFI_RESP_IP_CONFLICT");
		break;
	}
	case M2M_WIFI_REQ_ENABLE_MONITORING:
	{
		printf("\r\nM2M_WIFI_REQ_ENABLE_MONITORING");
		break;
	}
	case M2M_DEFAULT_CONN_SCAN_MISMATCH:
	{
		printf("\r\nM2M_DEFAULT_CONN_SCAN_MISMATCH ");
		break;
	}
	case M2M_DEFAULT_CONN_EMPTY_LIST:
	{
		printf("\r\nM2M_DEFAULT_CONN_EMPTY_LIST ");
		break;
	}
	default:
		break;
	}
}