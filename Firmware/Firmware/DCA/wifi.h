#ifndef WIFI_H_INCLUDED
#define WIFI_H_INCLUDED

#include "asf.h"
#include "common/include/nm_common.h"
#include "driver/include/m2m_wifi.h"

/** Wifi Init Settings **/
tstrWifiInitParam param;

/** AP mode Settings */
#define MAIN_WLAN_SSID                 "DCA_TEST_AP" /* < SSID */
#define MAIN_WLAN_AUTH                 M2M_WIFI_SEC_OPEN /* < Security manner */
#define MAIN_WLAN_CHANNEL              (6) /* < Channel number */
tstrM2MAPConfig strM2MAPConfig;

int8_t ret;

void Wifi_Init();
void Wifi_Init_AP();
void Wifi_Disable_AP();
void Wifi_Init_Default();
static void wifi_cb(uint8_t u8MsgType, void *pvMsg);

#endif 
