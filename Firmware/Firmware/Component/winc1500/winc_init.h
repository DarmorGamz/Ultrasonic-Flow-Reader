#ifndef WINC1500_MAIN_H
#define WINC1500_MAIN_H

#include "atmel_start.h"

#include "driver/include/m2m_wifi.h"

#define CONF_WINC_PIN_CHIP_SELECT   WIRELESS_CSn
#define CONF_WINC_PIN_CHIP_ENABLE   WIRELESS_EN
#define CONF_WINC_PIN_RESET         WIRELESS_RESETn
#define CONF_WINC_EXT_INT_PIN       WIRELESS_IRQn

int8_t wifi_init(tstrWifiInitParam *params);

#endif
