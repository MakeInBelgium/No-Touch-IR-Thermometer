/*
 * I2C.h
 *
 * I2C Master
 *
 * Created: 10/05/2019
 * Author: Dieter Reinhardt
 *
 * Tested with Alternate Pinout
 *
 * This software is covered by a modified MIT License, see paragraphs 4 and 5
 *
 * Copyright (c) 2019 Dieter Reinhardt
 *
 * 1. Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 2. The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * 3. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * 4. This software is strictly NOT intended for safety-critical or life-critical applications.
 * If the user intends to use this software or parts thereof for such purpose additional
 * certification is required.
 *
 * 5. Parts of this software are adapted from Microchip I2C polled master driver sample code.
 * Additional license restrictions from Microchip may apply.
 */

#ifndef I2C_H_
#define I2C_H_

#define ADDR_TIMEOUT	5			// 5 ms timeout
#define READ_TIMEOUT	5
#define WRITE_TIMEOUT	5
#define MAX_LEN			8			// maximum number of bytes for read/write transaction

#define ALTERNATE_PINOUT		// use alternate pins
								// standard pinout	alternate pinout
								// SCL = PB0		SCL = PA2
								// SDA = PB1		SDA = PA1

#include <port.h>

#define TWI0_BAUD(F_SCL, T_RISE)                                                                                       \
((((((float)20000000.0 / (float)F_SCL)) - 10 - ((float)20000000.0 * T_RISE / 1000000))) / 2)

// device addresses are 8 bit, LSB set if read
// status returned from I2C transaction:
// 0	no error
// 1	slave responded with NACK
// 2	arbitration lost
// 4	bus error
// 8	master does not control bus
// ff	timeout

void		I2C_init();
void		I2C_recover(void);							// clock out I2C bus if in invalid state (e.g. after incomplete transaction)
uint8_t		I2C_start(uint8_t device_addr);				// 8 bit device address device_addr, LSB set if READ
uint8_t		I2C_wait_ACK(void);							// wait for slave response after start of Master Write
void		I2C_rep_start(uint8_t device_addr);			// send repeated Start (e.g. 2nd address for Read)	
uint8_t		I2C_read(uint8_t *data, uint8_t ack_flag);	// read data, ack_flag 0: send ACK, 1: send NACK, returns status
uint8_t		I2C_write(uint8_t *data);					// write data, return status
void		I2C_stop();

// read/write multiple bytes (maximum number of bytes MAX_LEN)
// slave device address 8 bit (LSB = 0)
// simple error processing, on error master will be reset and error returned
// addr_ptr			address of first array element to transfer
// slave reg		starting slave register
// num_bytes		number of bytes to transfer
// single byte transfer: num_bytes = 1, data will be copied between *addr_ptr and slave register

// returns: 0		no error
//			1		slave responded with NACK or no slave response
//			0xff	error, 9 clock cycles sent to clock out possibly stuck slave, master reset

// slave is organized as byte array, similar to EEPROM, see I2CSD.c
// these functions can be used to transfer data between slave array and corresponding (same size or smaller) master array
// and for single byte transfer between a slave array member and a master byte variable

uint8_t		I2C_read_bytes_from_reg(uint8_t slave_addr, uint8_t *addr_ptr, uint8_t slave_reg, uint8_t num_bytes);
uint8_t		I2C_write_bytes_to_reg(uint8_t slave_addr, uint8_t *addr_ptr, uint8_t slave_reg, uint8_t num_bytes);
uint8_t		I2C_read_bytes(uint8_t slave_addr, uint8_t *addr_ptr, uint8_t num_bytes);
uint8_t		I2C_write_single_byte(uint8_t slave_addr, uint8_t data);

#endif /* I2C_H_ */