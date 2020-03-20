/*
 * I2C.c
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

#include <avr/io.h>
#include "driver_init.h"
#include <I2C.h>
#include <util/delay.h>
//#include <lx-ctrl5.h>

#define NOP() asm volatile(" nop \r\n")


extern uint8_t timeout_cnt = 0;

ISR(RTC_PIT_vect) {						// PIT interrupt handling code, 1 ms interrupt
	timeout_cnt++;;						// increment timeout counter
	RTC.PITINTFLAGS = RTC_PI_bm;		// clear interrupt flag
}


void I2C_init(void)
{
	
#ifdef ALTERNATE_PINOUT
	PORTA_set_pin_dir(2, PORT_DIR_IN);								// SCL = PA2, tri-stated high, avoids glitch
	PORTA_set_pin_level(2, false);
	PORTA_set_pin_pull_mode(2, PORT_PULL_OFF);
	PORTA_pin_set_inverted(2, false);
	PORTA_pin_set_isc(2, PORT_ISC_INTDISABLE_gc);
	PORTA_set_pin_dir(1, PORT_DIR_OUT);								// SDA = PA1
	PORTA_set_pin_level(1, false);
	PORTA_set_pin_pull_mode(1, PORT_PULL_OFF);
	PORTA_pin_set_inverted(1, false);
	PORTA_pin_set_isc(1, PORT_ISC_INTDISABLE_gc);
	PORTMUX.CTRLB |= PORTMUX_TWI0_bm;								// set the alternate pin mux
#else
	PORTB_set_pin_dir(0, PORT_DIR_IN);								// SCL = PB0, tri-stated high, avoids glitch
	PORTB_set_pin_level(0, false);
	PORTB_set_pin_pull_mode(0, PORT_PULL_OFF);
	PORTB_pin_set_inverted(0, false);
	PORTB_pin_set_isc(0, PORT_ISC_INTDISABLE_gc);
	PORTB_set_pin_dir(1, PORT_DIR_OUT);								// SDA = PB1
	PORTB_set_pin_level(1, false);
	PORTB_set_pin_pull_mode(1, PORT_PULL_OFF);
	PORTB_pin_set_inverted(1, false);
	PORTB_pin_set_isc(1, PORT_ISC_INTDISABLE_gc);
#endif
	
	TWI0.MBAUD = (uint8_t) TWI0_BAUD(100000, 0);					// set MBAUD register, TWI0_BAUD macro calculates parameter for 100 kHz
	TWI0.MCTRLB = TWI_FLUSH_bm;										// clear the internal state of the master
	TWI0.MCTRLA =	  1 << TWI_ENABLE_bp							// Enable TWI Master: enabled
					| 0 << TWI_QCEN_bp								// Quick Command Enable: disabled
					| 0 << TWI_RIEN_bp								// Read Interrupt Enable: disabled
					| 0 << TWI_SMEN_bp								// Smart Mode Enable: disabled
					| TWI_TIMEOUT_DISABLED_gc						// Bus Timeout Disabled (inoperative, see errata)
					| 0 << TWI_WIEN_bp;								// Write Interrupt Enable: disabled
#ifdef ALTERNATE_PINOUT
	PORTA_set_pin_dir(2, PORT_DIR_OUT);								// set port output direction for SCL low signal level
#else
	PORTB_set_pin_dir(0, PORT_DIR_OUT);	
#endif	
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;							// force bus idle
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm | TWI_BUSERR_bm);		// clear flags	
	
	timeout_cnt = 0;
	//PERIODIC INTERRUPT TIMER SETUP
	_PROTECTED_WRITE((CLKCTRL.OSC32KCTRLA), RTC_RUNSTDBY_bm);
	RTC.DBGCTRL = RTC_DBGRUN_bm;
	RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;
	RTC.PITINTCTRL = RTC_PI_bm;
	while (RTC.PITSTATUS & RTC_CTRLBUSY_bm);
	RTC.PITCTRLA = RTC_PERIOD_CYC4_gc | RTC_PITEN_bm;
	sei();

}

void I2C_recover(void)												// clock out I2C bus if in invalid state (e.g. after incomplete transaction)
{
	uint8_t	i;
	TWI0.MCTRLB |= TWI_FLUSH_bm;									// clear the internal state of the master
	TWI0.MCTRLA = 0;												// disable TWI Master
#ifdef ALTERNATE_PINOUT
	PORTA_set_pin_dir(1, PORT_DIR_IN);								// SDA tri-stated high
	for (i = 0; i < 9; i++)											// SCL, 9 x bit-banging
	{	
		PORTA_set_pin_dir(2, PORT_DIR_OUT);							// 5 us low
		_delay_us(5);												
		PORTA_set_pin_dir(2, PORT_DIR_IN);							// 5 us tri-stated high
		_delay_us(5);													
	}
#else
	PORTB_set_pin_dir(1, PORT_DIR_IN);								// SDA tri-stated high
	for (i = 0; i < 9; i++)											// SCL, 9 x bit-banging
	{
		PORTB_set_pin_dir(0, PORT_DIR_OUT);							// 5 us low
		_delay_us(5);
		PORTB_set_pin_dir(0, PORT_DIR_IN);							// 5 us tri-stated high
		_delay_us(5);
	}
#endif
	
// re-enable master twice
// for unknown reasons the master might get stuck if re-enabled only once
// second re-enable will fail if SDA not enabled beforehand

	TWI0.MCTRLB = TWI_FLUSH_bm;										// clear the internal state of the master
	TWI0.MCTRLA =	  1 << TWI_ENABLE_bp							// Enable TWI Master: enabled
					| 0 << TWI_QCEN_bp								// Quick Command Enable: disabled
					| 0 << TWI_RIEN_bp								// Read Interrupt Enable: disabled
					| 0 << TWI_SMEN_bp								// Smart Mode Enable: disabled
					| TWI_TIMEOUT_DISABLED_gc						// Bus Timeout Disabled (inoperative, see errata)
					| 0 << TWI_WIEN_bp;								// Write Interrupt Enable: disabled			
#ifdef ALTERNATE_PINOUT
	PORTA_set_pin_dir(1, PORT_DIR_OUT);								// re-enable SDA
#else
	PORTB_set_pin_dir(1, PORT_DIR_OUT);	
#endif
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;							// force bus idle
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm | TWI_BUSERR_bm);		// clear flags

	TWI0.MCTRLB = TWI_FLUSH_bm;										// clear the internal state of the master (glitch on SDA)
	TWI0.MCTRLA =	  1 << TWI_ENABLE_bp							// Enable TWI Master: enabled
					| 0 << TWI_QCEN_bp								// Quick Command Enable: disabled
					| 0 << TWI_RIEN_bp								// Read Interrupt Enable: disabled
					| 0 << TWI_SMEN_bp								// Smart Mode Enable: disabled
					| TWI_TIMEOUT_DISABLED_gc						// Bus Timeout Disabled (inoperative, see errata)
					| 0 << TWI_WIEN_bp;								// Write Interrupt Enable: disabled
#ifdef ALTERNATE_PINOUT
	PORTA_set_pin_dir(2, PORT_DIR_OUT);								// set port output direction for SCL low signal level
#else
	PORTB_set_pin_dir(0, PORT_DIR_OUT);
#endif
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;							// force bus idle
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm | TWI_BUSERR_bm);		// clear flags
}

uint8_t I2C_start(uint8_t device_addr)								// device_addr LSB set if READ
{
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);						// clear Read and Write interrupt flags
	if (TWI0.MSTATUS & TWI_BUSERR_bm) return 4;						// Bus Error, abort
	TWI0.MADDR = device_addr;
	return 0;
}

uint8_t I2C_wait_ACK(void)											// wait for slave response after start of Master Write
{
	timeout_cnt = 0;												// reset timeout counter, will be incremented by ms tick interrupt
	while (!(TWI0.MSTATUS & TWI_RIF_bm) && !(TWI0.MSTATUS & TWI_WIF_bm))	// wait for RIF or WIF set
	{
		_delay_us(1);
		if (timeout_cnt > ADDR_TIMEOUT) return 0xff;				// return timeout error
	}
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);						// clear Read and Write interrupt flags
	if (TWI0.MSTATUS & TWI_BUSERR_bm) return 4;						// Bus Error, abort
	if (TWI0.MSTATUS & TWI_ARBLOST_bm) return 2;					// Arbitration Lost, abort
	if (TWI0.MSTATUS & TWI_RXACK_bm) return 1;						// Slave replied with NACK, abort
	return 0;														// no error
}

// the Atmel device documentation mentions a special command for repeated start TWI_MCMD_REPSTART_gc,
// but this is not used in Atmel's demo code, so we don't use it either

void I2C_rep_start(uint8_t device_addr)								// send repeated start, device_addr LSB set if READ
{
	TWI0.MADDR = device_addr;	
}

uint8_t	I2C_read(uint8_t *data, uint8_t ack_flag)					// read data, ack_flag 0: send ACK, 1: send NACK, returns status
{
	timeout_cnt = 0;												// reset timeout counter, will be incremented by ms tick interrupt
	if ((TWI0.MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_OWNER_gc)	// if master controls bus
	{	
		while (!(TWI0.MSTATUS & TWI_RIF_bm))						// wait for RIF set (data byte received)
		{
			_delay_us(1);
			if (timeout_cnt > READ_TIMEOUT) return 0xff;			// return timeout error
		}
		TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);					// clear Read and Write interrupt flags	
		if (TWI0.MSTATUS & TWI_BUSERR_bm) return 4;					// Bus Error, abort
		if (TWI0.MSTATUS & TWI_ARBLOST_bm) return 2;				// Arbitration Lost, abort
		if (TWI0.MSTATUS & TWI_RXACK_bm) return 1;					// Slave replied with NACK, abort				
		if (ack_flag == 0) TWI0.MCTRLB &= ~(1 << TWI_ACKACT_bp);	// setup ACK
		else		TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;				// setup NACK (last byte read)
		*data = TWI0.MDATA;
		if (ack_flag == 0) TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;	// send ACK, more bytes to follow					
		return 0;
	}
	else return 8;													// master does not control bus
}

uint8_t I2C_write(uint8_t *data)									// write data, return status
{
	timeout_cnt = 0;												// reset timeout counter, will be incremented by ms tick interrupt
	if ((TWI0.MSTATUS & TWI_BUSSTATE_gm) == TWI_BUSSTATE_OWNER_gc)	// if master controls bus
	{
		TWI0.MDATA = *data;		
		while (!(TWI0.MSTATUS & TWI_WIF_bm))						// wait until WIF set, status register contains ACK/NACK bit
		{
			if (timeout_cnt > WRITE_TIMEOUT) return 0xff;			// return timeout error
		}
		if (TWI0.MSTATUS & TWI_BUSERR_bm) return 4;					// Bus Error, abort
		if (TWI0.MSTATUS & TWI_RXACK_bm) return 1;					// Slave replied with NACK, abort
		return 0;													// no error	
	}
	else return 8;													// master does not control bus
}

void I2C_stop()
{
	TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

// read/write multiple bytes (maximum number of bytes MAX_LEN)
// slave device address 8 bit (LSB = 0)
// simple error processing, on error master will be reset and error status = 1 returned
// addr_ptr		address of first array element to transfer
// slave reg	starting slave register
// num_bytes	number of bytes to transfer

uint8_t	I2C_read_bytes_from_reg(uint8_t slave_addr, uint8_t *addr_ptr, uint8_t slave_reg, uint8_t num_bytes)
{
	uint8_t status;
	if (num_bytes > MAX_LEN) num_bytes = MAX_LEN;
	status = I2C_start(slave_addr & 0xfe);							// slave write address, LSB 0
	if (status != 0) goto error;
	status = I2C_wait_ACK();										// wait for slave ACK
	if (status == 1) {
		I2C_stop();													// NACK, abort
		return 1;
	}
	if (status != 0) goto error;
	status = I2C_write(&slave_reg);									// send slave start register
	if (status != 0) goto error;
	I2C_rep_start((slave_addr & 0xfe) + 1);							// slave read address, LSB 1
	while (num_bytes > 1) {
		status = I2C_read(addr_ptr, 0);								// first bytes, send ACK
		if (status != 0) goto error;
		addr_ptr++;
		num_bytes--;
	}
	status = I2C_read(addr_ptr, 1);									// single or last byte, send NACK
	if (status != 0) goto error;
	I2C_stop();
	return 0;
	
error:
	I2C_recover();													// clock out possibly stuck slave, reset master
	return 0xff;													// flag error
}

uint8_t	I2C_write_bytes_to_reg(uint8_t slave_addr, uint8_t *addr_ptr, uint8_t slave_reg, uint8_t num_bytes)
{
	uint8_t status;
	if (num_bytes > MAX_LEN) num_bytes = MAX_LEN;
	status = I2C_start(slave_addr & 0xfe);							// slave write address, LSB 0
	if (status != 0) goto error;
	status = I2C_wait_ACK();										// wait for Slave ACK
	if (status == 1) {
		I2C_stop();													// NACK, abort	
		return 1;													
	}	 										
	if (status != 0) goto error;
	status = I2C_write(&slave_reg);
	if (status != 0) goto error;
	while (num_bytes > 0) {											// write bytes
		status = I2C_write(addr_ptr);
		if (status != 0) goto error;
		addr_ptr++;
		num_bytes--;		
	}
	I2C_stop();
	return 0;

error:
	I2C_recover();
	return 0xff;
}

uint8_t	I2C_write_single_byte(uint8_t slave_addr, uint8_t data)
{
	uint8_t status;
	status = I2C_start(slave_addr & 0xfe);							// slave write address, LSB 0
	if (status != 0) goto error;
	status = I2C_wait_ACK();										// wait for Slave ACK
	if (status == 1) {
		I2C_stop();													// NACK, abort
		return 1;
	}
	if (status != 0) goto error;
	status = I2C_write(&data);
	if (status != 0) goto error;
	I2C_stop();
	return 0;

	error:
	I2C_recover();
	return 0xff;
}

uint8_t	I2C_read_bytes(uint8_t slave_addr, uint8_t *addr_ptr, uint8_t num_bytes)
{
	uint8_t status;
	if (num_bytes > MAX_LEN) num_bytes = MAX_LEN;
	status = I2C_start((slave_addr & 0xfe)+1);						// slave read address, LSB 1
	if (status != 0) goto error;
	status = I2C_wait_ACK();										// wait for slave ACK
	if (status == 1) {
		I2C_stop();													// NACK, abort
		return 1;
	}
	if (status != 0) goto error;
	while (num_bytes > 1) {
		status = I2C_read(addr_ptr, 0);								// first bytes, send ACK
		return status;
		if (status != 0) goto error;
		addr_ptr++;
		num_bytes--;
	}
	status = I2C_read(addr_ptr, 1);									// single or last byte, send NACK
	if (status != 0) goto error;
	I2C_stop();
	return 0;
	
	error:
	I2C_recover();													// clock out possibly stuck slave, reset master
	return 0xff;													// flag error
}