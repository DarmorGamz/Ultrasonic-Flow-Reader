Microchip ENC28J60 Ethernet Controlloer
=======================================

The ENC28J60 is a stand-alone Ethernet controller
with an industry standard Serial Peripheral Interface
(SPI). It is designed to serve as an Ethernet network
interface for any controller equipped with SPI.

The ENC28J60 meets all of the IEEE 802.3 specifications.
It incorporates a number of packet filtering
schemes to limit incoming packets. It also provides an
internal DMA module for fast data throughput and hardware
assisted checksum calculation, which is used in
various network protocols. Communication with the
host controller is implemented via an interrupt pin and
the SPI, with clock rates of up to 20 MHz. Two
dedicated pins are used for LED link and network
activity indication.

Features
--------
* An SPI interface that serves as a communication channel between the host
  controller and the ENC28J60.
* Control registers which are used to control and monitor the ENC28J60.
* A dual port RAM buffer for received and transmitted data packets.
* An arbiter to control the access to the RAM buffer when requests are made
  from DMA, transmit and receive blocks.
* The bus interface that interprets data and commands received via the SPI
  interface.
* The MAC (Medium Access Control) module that implements IEEE 802.3 compliant
  MAC logic.
* The PHY (Physical Layer) module that encodes and decodes the analog data that
  is present on the twisted-pair interface.

Support
-------
[Home]: http://www.microchip.com/ENC28J60

Knows issues and workarounds
----------------------------
Currently The AVR SPI driver will not init the MOSI and SCK pin.
The Application and Example for TCPIP Lite Stack should modified after
import project into IDE(Atmel Stdio 7 or IAR).
The workaroud is put below code into driver_init.c->system_init() function:
MOSI_set_dir(PORT_DIR_OUT);
SCK_set_dir(PORT_DIR_OUT);
The MOSI and SCK keyword in above function maybe different with projects.
Please change them according the atmel_start_pins.h
