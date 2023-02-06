/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMD20 has 8 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7

#define ETHERNET_RESET GPIO(GPIO_PORTA, 2)
#define WIRELESS_RESETn GPIO(GPIO_PORTA, 3)
#define WIRELESS_WAKE GPIO(GPIO_PORTA, 4)
#define WIRELESS_EN GPIO(GPIO_PORTA, 5)
#define ETHERNET_SCK GPIO(GPIO_PORTA, 7)
#define ETHERNET_MOSI GPIO(GPIO_PORTA, 8)
#define ETHERNET_MISO GPIO(GPIO_PORTA, 9)
#define ETHERNET_CS GPIO(GPIO_PORTA, 11)
#define ETHERNET_IRQ GPIO(GPIO_PORTA, 14)

#define WIRELESS_IRQn GPIO(GPIO_PORTA, 15)
#define WIRELESS_MOSI GPIO(GPIO_PORTA, 16)
#define WIRELESS_MISO GPIO(GPIO_PORTA, 17)
#define PA18 GPIO(GPIO_PORTA, 18)
#define WIRELESS_CSn GPIO(GPIO_PORTA, 19)
#define LED_GREEN GPIO(GPIO_PORTA, 22)
#define LED_RED GPIO(GPIO_PORTA, 23)
#define WIRELESS_SCK GPIO(GPIO_PORTA, 31)

#endif // ATMEL_START_PINS_H_INCLUDED
