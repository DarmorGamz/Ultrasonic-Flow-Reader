/**
  ENC28J60 Ethernet Driver

  Company:
    Microchip Technology Inc.

  File Name:
    ENC28J60_driver.c

  Summary:
    This is the Ethernet driver implementation for ENC28J60 family devices.

  Description:
    This file provides the Ethernet driver API implementation for
    the ENC28J60 device.

 */

/*

CopyRight  [2015] Microchip Technology Inc. and its subsidiaries.  You may use this software
and any derivatives exclusively with Microchip products.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS,
IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF
NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE, OR ITS
INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE
IN ANY APPLICATION.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL
OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED
TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY
OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S
TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.

*/

#include <sys_arch.h>
#include <atmel_start.h>
#include <stdint.h>
#include <stddef.h>
#include "enc28j60.h"
#include "ethernet_driver.h"

#define ETH_SPI_READ8() SPI_ExchangeByte(0, 0)
#define ETH_SPI_WRITE8(a) SPI_ExchangeByte(1, a)

//#define ETH_IRQ_LOW()           ((ETH_IRQ == 0)?1:0)

volatile ethernetDriver_t    ethData;
static mac48Address_t        eth_MAC;
static uint16_t              nextPacketPointer;
static receiveStatusVector_t rxPacketStatusVector;
sfr_bank_t                   lastBank;

#define MAX_TX_PACKET (1500)

#define TXSTART (0x1FFF - MAX_TX_PACKET)
#define TXEND (0x1FFF)
#define RXSTART (0)
#define RXEND (TXSTART - 2)

static uint8_t  ENC28_Rcr8(enc28j60_registers_t);
static uint16_t ENC28_Rcr16(enc28j60_registers_t);

static void ENC28_Wcr8(enc28j60_registers_t, uint8_t);
static void ENC28_Wcr16(enc28j60_registers_t, uint16_t);

static void ENC28_Bfs(enc28j60_registers_t, char);
static void ENC28_Bfc(enc28j60_registers_t, char);

static uint16_t ENC28_PhyRead(enc28j60_phy_registers_t a);
static void     ENC28_PhyWrite(enc28j60_phy_registers_t a, uint16_t d);

static void ENC28_BankSel(enc28j60_registers_t);

uint16_t TXPacketSize;

/*******************************************************************************/

/**
 * Bank Select
 * @param r
 */
static void ENC28_BankSel(enc28j60_registers_t r)
{
	uint8_t a = r & BANK_MASK;

	if (a != sfr_common && a != lastBank) {
		lastBank = a;
		// clear the bank bits
		ETH_NCS_LOW();
		ETH_SPI_WRITE8(bfc_inst | 0x1F);
		ETH_SPI_WRITE8(0x03);
		ETH_NCS_HIGH();
		NOP();
		NOP();
		// set the needed bits
		ETH_NCS_LOW();
		ETH_SPI_WRITE8(bfs_inst | 0x1F);
		ETH_SPI_WRITE8(a >> 6);
		ETH_NCS_HIGH();
	}
}


/**
 * Ethernet Initialization - Initializes TX/RX Buffer, MAC and PHY
 */
void ETH_Init(void)
{
	// initialize the driver variables
	ethData.error      = false; // no error
	ethData.up         = false; // no link
	ethData.linkChange = false;
	ethData.bufferBusy = false; // transmit data buffer is free
	ethData.saveRDPT   = 0;

	lastBank = sfr_bank0;

	__delay_us(100);

	ETH_SendSystemReset(); // software reset
	__delay_ms(10);

	// Wait for the OST
	while (!(ENC28_Rcr8(J60_ESTAT) & 0x01))
		; // wait for CLKRDY to go high

	// Initialize RX tracking variables and other control state flags
	nextPacketPointer = RXSTART;

	ENC28_Bfs(J60_ECON2, 0x80); // enable AUTOINC

	// Set up TX/RX buffer addresses
	ENC28_Wcr16(J60_ETXSTL, TXSTART);
	ENC28_Wcr16(J60_ETXNDL, TXEND);
	ENC28_Wcr16(J60_ERXSTL, RXSTART);
	ENC28_Wcr16(J60_ERXNDL, RXEND);
	ENC28_Wcr16(J60_ERDPTL, nextPacketPointer);

	ENC28_Wcr16(J60_ERDPTL, RXSTART);
	ENC28_Wcr16(J60_EWRPTL, TXSTART);

	// Configure the receive filter
	ENC28_Wcr8(J60_ERXFCON, 0b10101001); // UCEN,OR,CRCEN,MPEN,BCEN (unicast,crc,magic packet,broadcast)

	//// what is my MAC address?
	//eth_MAC = MAC_getAddress();

	// Initialize the MAC
	ENC28_Wcr8(J60_MACON1, 0x0D); // TXPAUS, RXPAUS, MARXEN
	//ENC28_Wcr8(J60_MACON3, 0x32); // Pad < 60 bytes, Enable CRC, Frame Check, Half Duplex
	ENC28_Wcr8(J60_MACON3, 0xB2); // VLAN padded to 64 bytes (others pad to 60 bytes), Enable CRC, Frame Check, Half Duplex
	ENC28_Wcr8(J60_MACON4, 0x40); // DEFER set
	ENC28_Wcr16(J60_MAIPGL, 0x0c12);
	ENC28_Wcr8(J60_MABBIPG, 0x12);
	ENC28_Wcr16(J60_MAMXFLL, MAX_TX_PACKET);
    ENC28_Wcr8(J60_ECOCON, 0x00); // RNG: Disable the clock out output to reduce EMI generation
    // RNG: MAC registers are flakey when clocked < 8MHz... since MAC is tied to serial, over-ride with application set MAC
	ENC28_Wcr8(J60_MAADR1, eth_MAC.mac_array[0]);
	//NOP();
	ENC28_Wcr8(J60_MAADR2, eth_MAC.mac_array[1]);
	//NOP();
	ENC28_Wcr8(J60_MAADR3, eth_MAC.mac_array[2]);
	//NOP();
	ENC28_Wcr8(J60_MAADR4, eth_MAC.mac_array[3]);
	//NOP();
	ENC28_Wcr8(J60_MAADR5, eth_MAC.mac_array[4]);
	//NOP();
	ENC28_Wcr8(J60_MAADR6, eth_MAC.mac_array[5]);
	//NOP();

	ENC28_Wcr8(J60_ECON1, 0x04); // RXEN enabled

	// Initialize the PHY
	ENC28_PhyWrite(J60_PHCON1, 0x0000);
	ENC28_PhyWrite(J60_PHCON2, 0x0100); // Do not transmit loopback
	ENC28_PhyWrite(J60_PHLCON, 0x0472); // LED control - LEDA = Link, LEDB = TX/RX, Stretched LED
	// LEDB is grounded so default is Half Duplex

	// Configure the IRQ's
	ENC28_Wcr8(J60_EIE, 0xDB);   // Enable PKTIE,INTIE,LINKIE,TXIE,TXERIE,RXERIE
	ENC28_Wcr16(J60_PHIE, 0x12); // Enable PLNKIE and PGEIE

	// check for a preexisting link
	ETH_CheckLinkUp();
}

/**
 * Poll Ethernet Controller for new events
 */
void ETH_EventHandler(void)
{
	eir_t     eir_val;
	phstat2_t phstat2_val;

	// check for the IRQ pin
	// if (ETH_IRQ_LOW())
	//{
	// MAC is sending an interrupt
	// what is the interrupt
	eir_val.val     = ENC28_Rcr8(J60_EIR);
	phstat2_val.val = ENC28_Rcr16(J60_PHSTAT2);

	if (eir_val.LINKIF) // something about the link changed.... update the link parameters
	{
		ethData.linkChange = true;
		ethData.up         = false;
		if (ETH_CheckLinkUp()) {
		}
		if (phstat2_val.DPXSTAT) // Update MAC duplex settings to match PHY duplex setting
		{
			ENC28_Wcr16(J60_MABBIPG, 0x15); // Switching to full duplex
			ENC28_Bfs(J60_PHSTAT2, 0x01);
		} else {
			ENC28_Wcr16(J60_MABBIPG, 0x12); // Switching to half duplex
			ENC28_Bfc(J60_PHSTAT2, 0x01);
		}
	}
	if (eir_val.TXIF) // finished sending a packet
	{
		ethData.bufferBusy = false;
		ENC28_Bfc(J60_EIR, 0x08);
	}
	if (eir_val.PKTIF || ENC28_Rcr8(J60_EPKTCNT)) // Packet receive buffer has at least 1 unprocessed packet
	{
		ethData.pktReady = true;
	}
	ENC28_Wcr8(J60_EIR, eir_val.val); // write the eir value back to clear any of the interrupts
	//}
}

/**
 * Retrieve information about last received packet and the address of the next ones
 */
void ETH_NextPacketUpdate()
{
	// Set the RX Read Pointer to the beginning of the next unprocessed packet
	// Errata 14 inclusion
	if (nextPacketPointer == RXSTART)
		ENC28_Wcr16(J60_ERXRDPTL, RXEND);
	else
		ENC28_Wcr16(J60_ERXRDPTL, nextPacketPointer - 1);

	ENC28_Wcr16(J60_ERDPTL, nextPacketPointer);

	ETH_NCS_LOW();
	ETH_SPI_WRITE8(rbm_inst);
	((char *)&nextPacketPointer)[0]    = ETH_SPI_READ8();
	((char *)&nextPacketPointer)[1]    = ETH_SPI_READ8();
	((char *)&rxPacketStatusVector)[0] = ETH_SPI_READ8();
	((char *)&rxPacketStatusVector)[1] = ETH_SPI_READ8();
	((char *)&rxPacketStatusVector)[2] = ETH_SPI_READ8();
	((char *)&rxPacketStatusVector)[3] = ETH_SPI_READ8();
	ETH_NCS_HIGH();
	rxPacketStatusVector.byteCount -= 4; // I don't care about the frame checksum at the end.
	// the checksum is 4 bytes.. so my payload is the byte count less 4.
}

/**
 * Read 1 byte from SFRs
 * @param a
 * @return
 */
static uint8_t ENC28_Rcr8(enc28j60_registers_t a)
{
	uint8_t v;

	ENC28_BankSel(a);
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(rcr_inst | (a & SFR_MASK));
	v = ETH_SPI_READ8();
	ETH_NCS_HIGH();

	return v;
}

/**
 * Read 2 bytes from SFRs
 * @param a
 * @return
 */
static uint16_t ENC28_Rcr16(enc28j60_registers_t a)
{
	uint16_t v;

	ENC28_BankSel(a);
	a &= SFR_MASK;
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(rcr_inst | (a));
	((char *)&v)[0] = ETH_SPI_READ8();
	ETH_NCS_HIGH();
	NOP();
	NOP();
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(rcr_inst | (a + 1));
	((char *)&v)[1] = ETH_SPI_READ8();
	ETH_NCS_HIGH();

	return v;
}

/**
 * Write 1 byte to SFRs
 * @param a
 * @param v
 */
static void ENC28_Wcr8(enc28j60_registers_t a, uint8_t v)
{
	ENC28_BankSel(a);
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(wcr_inst | (a & SFR_MASK));
	ETH_SPI_WRITE8(v);
	ETH_NCS_HIGH();
}

/**
 * Write 2 bytes to SFRs
 * @param a
 * @param v
 */
static void ENC28_Wcr16(enc28j60_registers_t a, uint16_t v)
{
	ENC28_BankSel(a);
	a &= SFR_MASK;
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(wcr_inst | (a));
	ETH_SPI_WRITE8(((char *)&v)[0]);
	ETH_NCS_HIGH();
	NOP();
	NOP();
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(wcr_inst | (a + 1));
	ETH_SPI_WRITE8(((char *)&v)[1]);
	ETH_NCS_HIGH();
}

/**
 * SFR Bit Field Set
 * @param a
 * @param bits
 */
static void ENC28_Bfs(enc28j60_registers_t a, char bits) // can only be used for ETH Control Registers
{
	ENC28_BankSel(a);
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(bfs_inst | (a & SFR_MASK));
	ETH_SPI_WRITE8(bits);
	ETH_NCS_HIGH();
}

/**
 * SFR Bit Field Clear
 * @param a
 * @param bits
 */
static void ENC28_Bfc(enc28j60_registers_t a, char bits)
{
	ENC28_BankSel(a);
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(bfc_inst | (a & SFR_MASK));
	ETH_SPI_WRITE8(bits);
	ETH_NCS_HIGH();
}

/**
 * Write PHY  register
 * @param a
 * @param d
 */
static void ENC28_PhyWrite(enc28j60_phy_registers_t a, uint16_t d)
{
	uint8_t v = 1;

	ENC28_Wcr8(J60_MIREGADR, a);
	ENC28_Wcr16(J60_MIWRL, d);
	while (v & 0x01) {
		v = ENC28_Rcr8(J60_MISTAT);
	}
}

/**
 * Read PHY register
 * @param a
 * @return
 */
static uint16_t ENC28_PhyRead(enc28j60_phy_registers_t a)
{
	ENC28_Wcr8(J60_MIREGADR, a);
	ENC28_Bfs(J60_MICMD, 0x01); // set the read flag
	while (ENC28_Rcr8(J60_MISTAT) & 0x01)
		;                       // wait for the busy flag to clear
	ENC28_Bfc(J60_MICMD, 0x01); // clear the read flag

	return ENC28_Rcr16(J60_MIRDL);
}

/**
 * System Software Reset
 */
void ETH_SendSystemReset(void)
{
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(src_inst);
	ETH_NCS_HIGH();
}

/**
 * Check for the link presence
 * @return
 */
bool ETH_CheckLinkUp()
{
	uint16_t phstat2;

	phstat2 = ENC28_PhyRead(J60_PHSTAT2);

	if (phstat2 & 0x0400) {
		ethData.up = true;
		return true;
	} else
		return false;
}

/**
 * Read 1 byte of data from the RX Buffer
 * @return
 */

uint8_t ETH_Read8(void)
{
	uint8_t b;

	ETH_NCS_LOW();
	ETH_SPI_WRITE8(rbm_inst);
	b = ETH_SPI_READ8();
	ETH_NCS_HIGH();

	return b;
}

/**
 * Read 2 bytes of data from the RX Buffer
 * @return
 */
uint16_t ETH_Read16(void)
{
	uint16_t b;

	ETH_NCS_LOW();
	ETH_SPI_WRITE8(rbm_inst);
	b = ETH_SPI_READ8() << 8;
	b |= ETH_SPI_READ8();
	ETH_NCS_HIGH();

	return b;
}

/**
 * Read 4 bytes of data from the RX Buffer
 * @return
 */
uint32_t ETH_Read32(void)
{
	uint32_t b;

	ETH_NCS_LOW();
	ETH_SPI_WRITE8(rbm_inst);
	((char *)&b)[3] = ETH_SPI_READ8();
	((char *)&b)[2] = ETH_SPI_READ8();
	((char *)&b)[1] = ETH_SPI_READ8();
	((char *)&b)[0] = ETH_SPI_READ8();
	ETH_NCS_HIGH();

	return b;
}

/**
 * Read block of data from the RX Buffer
 * @param buffer
 * @param length
 * @return
 */
uint16_t ETH_ReadBlock(void *buffer, uint16_t length)
{
	uint16_t readCount = length;
	char *   p         = buffer;

	if (rxPacketStatusVector.byteCount < length)
		readCount = rxPacketStatusVector.byteCount;
	length = readCount;
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(rbm_inst);
	while (length--)
		*p++ = ETH_SPI_READ8();
	ETH_NCS_HIGH();

	return readCount;
}

/**
 * Write 1 byte of data to TX Buffer
 * @param data
 */
void ETH_Write8(uint8_t data)
{
	TXPacketSize += 1;
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(wbm_inst);
	ETH_SPI_WRITE8(data);
	ETH_NCS_HIGH();
}

/**
 * Write 2 bytes of data to TX Buffer
 * @param data
 */
void ETH_Write16(uint16_t data)
{
	TXPacketSize += 2;
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(wbm_inst);
	ETH_SPI_WRITE8(data >> 8);
	ETH_SPI_WRITE8(data);
	ETH_NCS_HIGH();
}

/**
 * Write 4 bytes of data to TX Buffer
 * @param data
 */
void ETH_Write32(uint32_t data)
{
	TXPacketSize += 4;
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(wbm_inst);
	ETH_SPI_WRITE8(data >> 24);
	ETH_SPI_WRITE8(data >> 16);
	ETH_SPI_WRITE8(data >> 8);
	ETH_SPI_WRITE8(data);
	ETH_NCS_HIGH();
}

/**
 * Write a block of data to TX Buffer
 * @param data
 * @param length
 */
uint16_t ETH_WriteBlock(void *data, uint16_t length)
{
	char *p = data;
	TXPacketSize += length;
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(wbm_inst);
	while (length--) {
		ETH_SPI_WRITE8(*p++);
	}
	ETH_NCS_HIGH();

	return length;
}

/**
 * start a packet.
 * If the Ethernet transmitter is idle, then start a packet.  Return is SUCCESS if the packet was started.
 * @param dest_mac
 * @param type
 * @return SUCCESS if packet started.  BUFFER_BUSY or TX_LOGIC_NOT_IDLE if buffer or transmitter is busy respectively
 */
error_msg ETH_WriteStart(const mac48Address_t *dest_mac, uint16_t type)
{
	if (ethData.bufferBusy) {
		return BUFFER_BUSY;
	}

	if ((ENC28_Rcr8(J60_ECON1) & 0x08)) {
		return TX_LOGIC_NOT_IDLE;
	}
	// Set the Window Write Pointer to the beginning of the transmit buffer
	ENC28_Wcr16(J60_ETXSTL, TXSTART);
	ENC28_Wcr16(J60_EWRPTL, TXSTART);

	TXPacketSize = 0;
	ETH_NCS_LOW();
	ETH_SPI_WRITE8(wbm_inst);
	ETH_SPI_WRITE8(Control_Byte);
	ETH_SPI_WRITE8(dest_mac->mac_array[0]);
	ETH_SPI_WRITE8(dest_mac->mac_array[1]);
	ETH_SPI_WRITE8(dest_mac->mac_array[2]);
	ETH_SPI_WRITE8(dest_mac->mac_array[3]);
	ETH_SPI_WRITE8(dest_mac->mac_array[4]);
	ETH_SPI_WRITE8(dest_mac->mac_array[5]);
	ETH_SPI_WRITE8(eth_MAC.mac_array[0]);
	ETH_SPI_WRITE8(eth_MAC.mac_array[1]);
	ETH_SPI_WRITE8(eth_MAC.mac_array[2]);
	ETH_SPI_WRITE8(eth_MAC.mac_array[3]);
	ETH_SPI_WRITE8(eth_MAC.mac_array[4]);
	ETH_SPI_WRITE8(eth_MAC.mac_array[5]);
	ETH_SPI_WRITE8(type >> 8);
	ETH_SPI_WRITE8(type & 0x0FF);
	ETH_NCS_HIGH();
	TXPacketSize += 15;
	ethData.bufferBusy = true;

	return SUCCESS;
}

/**
 * Start the Transmission
 * @return
 */
error_msg ETH_Send(void)
{
	ENC28_Wcr16(J60_ETXNDL, TXSTART + TXPacketSize);
	if (!ethData.up) {
		return LINK_NOT_FOUND;
	}
	if (!ethData.bufferBusy) {
		return BUFFER_BUSY;
	}
	ENC28_Bfs(J60_ECON1, 0x08); // start the transmission
	ethData.bufferBusy = false;

	return SUCCESS;
}

/**
 * Clears number of bytes (length) from the RX buffer
 * @param length
 */
void ETH_Dump(uint16_t length)
{
	uint16_t newRXTail;

	length = (rxPacketStatusVector.byteCount <= length) ? rxPacketStatusVector.byteCount : length;
	if (length) {
		;
		newRXTail = ENC28_Rcr16(J60_ERDPTL);
		newRXTail += length;
		// Write new RX tail
		ENC28_Wcr16(J60_ERDPTL, newRXTail);

		rxPacketStatusVector.byteCount -= length;
	}
}

/**
 * Clears all bytes from the RX buffer
 */
void ETH_Flush(void)
{
	ethData.pktReady = false;
	if (nextPacketPointer == RXSTART)
		ENC28_Wcr16(J60_ERXRDPTL, RXEND);
	else
		ENC28_Wcr16(J60_ERXRDPTL, nextPacketPointer - 1);
	ENC28_Wcr16(J60_ERDPTL, nextPacketPointer);
	// Packet decrement
	ENC28_Bfs(J60_ECON2, 0x40);
}

/**
 * Insert data in between of the TX Buffer
 * @param data
 * @param len
 * @param offset
 */
void ETH_Insert(char *data, uint16_t len, uint16_t offset)
{
	uint16_t current_tx_pointer = 0;
	offset += 1; /* sizeof(Control_Byte) */

	current_tx_pointer = ENC28_Rcr16(J60_EWRPTL);
	ENC28_Wcr16(J60_EWRPTL, TXSTART + offset);
	while (len--) {
		ETH_NCS_LOW();
		ETH_SPI_WRITE8(wbm_inst); // WBM command
		ETH_SPI_WRITE8(*data++);
		ETH_NCS_HIGH();
	}
	ENC28_Wcr16(J60_EWRPTL, current_tx_pointer);
}

/**
 * Copy the data from RX Buffer to the TX Buffer using DMA setup
 * This is used for ICMP ECHO to eliminate the need to extract the arbitrary payload
 * @param len
 */
error_msg ETH_Copy(uint16_t len)
{
	uint16_t tx_buffer_address;
	uint16_t timer;
	uint16_t temp_len;

    if (len==0) return SUCCESS; // nothing to copy

	timer = 2 * len;
	// Wait until module is idle
	while ((ENC28_Rcr8(J60_ECON1) & 0x20) != 0 && --timer)
		NOP(); // sit here until the DMAST bit is clear

	if ((ENC28_Rcr8(J60_ECON1) & 0x20) == 0) {
		tx_buffer_address = ENC28_Rcr16(J60_EWRPTL); // Current TX Write Pointer

		ENC28_Wcr16(J60_EDMADSTL, tx_buffer_address);
		ENC28_Wcr16(J60_EDMASTL, ethData.saveRDPT);

		tx_buffer_address += len;
		temp_len = ethData.saveRDPT + len;

		if (temp_len > RXEND) {
			temp_len = temp_len - (RXEND) + RXSTART;
			ENC28_Wcr16(J60_EDMANDL, temp_len);
		} else {
			ENC28_Wcr16(J60_EDMANDL, temp_len);
		}

		// Clear CSUMEN to select a copy operation
		ENC28_Bfc(J60_ECON1, 0x10);
		// start the DMA
		ENC28_Bfs(J60_ECON1, 0x20);
		timer = 40 * len;
		while ((ENC28_Rcr8(J60_ECON1) & 0x20) != 0 && --timer)
			NOP(); // sit here until the DMAST bit is clear
		if ((ENC28_Rcr8(J60_ECON1) & 0x20) == 0) {
			// clean up the source and destination window pointers
			ENC28_Wcr16(J60_EWRPTL, tx_buffer_address);

			TXPacketSize += len; // fix the packet length
			return SUCCESS;
		}
	}
	// TODO: RESET(); // reboot for now
	return DMA_TIMEOUT;
}

static uint16_t ETH_ComputeChecksum(uint16_t len, uint16_t seed)
{
	uint32_t cksm;
	uint16_t v;

	cksm = seed;

	while (len > 1) {
		v               = 0;
		((char *)&v)[1] = ETH_Read8();
		((char *)&v)[0] = ETH_Read8();
		cksm += v;
		len -= 2;
	}

	if (len) {
		v               = 0;
		((char *)&v)[1] = ETH_Read8();
		((char *)&v)[0] = 0;
		cksm += v;
	}

	// wrap the checksum
	while (cksm >> 16) {
		cksm = (cksm & 0x0FFFF) + (cksm >> 16);
	}

	// invert the number.
	cksm = ~cksm;

	// Return the resulting checksum
	return cksm;
}

/**
 * Calculate the Checksum - Hardware Checksum
 * @param position
 * @param length
 * @return
 */
uint16_t ETH_TxComputeChecksum(uint16_t position, uint16_t length, uint16_t seed)
{
	uint32_t cksm = 0;

	//    cksm = seed;
	position += 1; /* sizeof(Control_Byte) */

	while ((ENC28_Rcr8(J60_ECON1) & 0x20) != 0)
		; // sit here until the DMAST bit is clear

	ENC28_Wcr16(J60_EDMASTL, (TXSTART + position));
	ENC28_Wcr16(J60_EDMANDL, TXSTART + position + (length - 1));

	if (!(ENC28_Rcr8(J60_ECON1) & 0x10)) // Make sure CSUMEN is not set already
	{
		// Set CSUMEN and DMAST to select and start a checksum operation
		ENC28_Bfs(J60_ECON1, 0x30);
		while ((ENC28_Rcr8(J60_ECON1) & 0x20) != 0)
			; // sit here until the DMAST bit is clear
		ENC28_Bfc(J60_ECON1, 0x10);

		cksm = ENC28_Rcr16(J60_EDMACSL);
		if (seed) {
			seed = ~(seed);
			cksm += seed;
			while (cksm >> 16) {
				cksm = (cksm & 0x0FFFF) + (cksm >> 16);
			}
		}
		cksm = htons(cksm);
	}
	return cksm;
}

/**
 * Calculate RX checksum - Software checksum
 * @param len
 * @param seed
 * @return
 */
uint16_t ETH_RxComputeChecksum(uint16_t len, uint16_t seed)
{
	uint16_t rxptr;
	uint32_t cksm;

	// Save the read pointer starting address
	rxptr = ENC28_Rcr16(J60_ERDPTL);
	;

	cksm = ETH_ComputeChecksum(len, seed);

	// Restore old read pointer location
	ENC28_Wcr16(J60_ERDPTL, rxptr);

	// Return the resulting checksum
	return ((cksm & 0xFF00) >> 8) | ((cksm & 0x00FF) << 8);
}

/**
 * To get the MAC address
 * @param mac
 */
void ETH_GetMAC(uint8_t *macAddr)
{
#if 1

    // RNG: MAC registers are flakey when clocked < 8MHz... since MAC is tied to serial, over-ride with application set MAC
    memcpy(macAddr, (uint8_t*)&eth_MAC.mac_array[0], 6);

#else

	*macAddr++ = ENC28_Rcr8(J60_MAADR1);
	*macAddr++ = ENC28_Rcr8(J60_MAADR2);
	*macAddr++ = ENC28_Rcr8(J60_MAADR3);
	*macAddr++ = ENC28_Rcr8(J60_MAADR4);
	*macAddr++ = ENC28_Rcr8(J60_MAADR5);
	*macAddr++ = ENC28_Rcr8(J60_MAADR6);

#endif
}

/**
 * To set the MAC address
 * @param mac
 */
void ETH_SetMAC(uint8_t *macAddr)
{
	ENC28_Wcr8(J60_MAADR1, *macAddr++);
	ENC28_Wcr8(J60_MAADR2, *macAddr++);
	ENC28_Wcr8(J60_MAADR3, *macAddr++);
	ENC28_Wcr8(J60_MAADR4, *macAddr++);
	ENC28_Wcr8(J60_MAADR5, *macAddr++);
	ENC28_Wcr8(J60_MAADR6, *macAddr++);
}

void ETH_SaveRDPT(void)
{
	ethData.saveRDPT = ENC28_Rcr16(J60_ERDPTL);
}

void ETH_ResetByteCount(void)
{
	ethData.saveWRPT = ENC28_Rcr16(J60_EWRPTL);
}

uint16_t ETH_GetByteCount(void)
{
	uint16_t wptr;

	wptr = ENC28_Rcr16(J60_EWRPTL);

	return (wptr - ethData.saveWRPT);
}




/**This function was added by Eyedro to set the MAC address BEFORE initializing the stack and performing DHCP requests
 *
 * @param pau8MAC
 *      MAC address to use
 */
void ETH_InitMAC(uint8_t *pau8MAC) {
    memcpy((uint8_t*)&eth_MAC.mac_array[0], pau8MAC, 6);
}

