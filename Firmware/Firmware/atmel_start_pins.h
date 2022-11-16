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

// SAMD21 has 8 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7

#define ETHERNET_CS GPIO(GPIO_PORTA, 2)
#define WIRELESS_MISO GPIO(GPIO_PORTA, 4)
#define WIRELESS_MOSI GPIO(GPIO_PORTA, 6)
#define WIRELESS_SCK GPIO(GPIO_PORTA, 7)
#define ETHERNET_MISO GPIO(GPIO_PORTA, 16)
#define ETHERNET_MOSI GPIO(GPIO_PORTA, 18)
#define ETHERNET_SCK GPIO(GPIO_PORTA, 19)
#define ETHERNET_RESET GPIO(GPIO_PORTA, 20)
#define VIRTUAL_TX GPIO(GPIO_PORTA, 22)
#define VIRTUAL_RX GPIO(GPIO_PORTA, 23)
#define WIRELESS_IRQ GPIO(GPIO_PORTB, 4)
#define WIRELESS_EN GPIO(GPIO_PORTB, 5)
#define WIRELESS_RESET GPIO(GPIO_PORTB, 6)
#define WIRELESS_CS GPIO(GPIO_PORTB, 7)
#define ETHERNET_IRQ GPIO(GPIO_PORTB, 14)

#endif // ATMEL_START_PINS_H_INCLUDED
