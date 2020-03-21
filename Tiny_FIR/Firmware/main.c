#include <atmel_start.h>
#include <avr/io.h>
#include <util/delay.h>
#include <i2c_simple_master.h>

// MLX90614 Datasheet: https://www.melexis.com/-/media/files/documents/datasheets/mlx90614-datasheet-melexis.pdf
#define MLX90614_I2CADDR 0x5A
#define MLX90614_REG_RAWIR1 0x04
#define MLX90614_REG_RAWIR2 0x05
#define MLX90614_REG_TA 0x06
#define MLX90614_REG_TOBJ1 0x07
#define MLX90614_REG_TOBJ2 0x08

#define MLX90614_ERROR_BIT 0x8000

#define LED_OK_bm	PIN3_bm
#define LED_WARN_bm	PIN6_bm
#define LED_NOK_bm	PIN7_bm

#define STATUS_OK		0
#define STATUS_WARN		1
#define STATUS_NOK		2
#define STATUS_ERROR	3

/*
 * Temperature tresholds:
 *
 *    Temperature in °C = ( raw sensor value * 0.02 ) - 273.15
 * => Raw sensor value = ( Temperature in °C + 273.15 ) * 50
 *
 */

// TODO: values need to be verified by med, still dummy values
const uint16_t TEMP_TH_TOO_LOW =	( ( 34.5 + 273.15 ) * 50 ) + 0.5;
const uint16_t TEMP_TH_OK =			( ( 37.5 + 273.15 ) * 50 ) + 0.5;
const uint16_t TEMP_TH_WARN =		( ( 38.0 + 273.15 ) * 50 ) + 0.5;


// Function declarations
void pinSetup();
void setLEDstatus(uint8_t status);
uint16_t readTempObj1Raw(void);
uint16_t readTempObj2Raw(void);
uint8_t getStatus( uint16_t rawValue );

int main(void){
	    // When we arrive here it means we just had a reset-event. Most-probably because the user pushed the button!

	    // 1. Init our little MCU: pins, drivers, possible Microchip middleware if there is no other way.
	    pinSetup();
	    atmel_start_init();
	    
	    // 2. Do a few measurements for 1 second and average.
		// TODO: more than 1 measurement and AVG
		// TODO: use second sensor too?
		uint16_t obj1TempRaw = readTempObj1Raw();
		
	    // 3. Maybe some calculations on the data. FLOATING POINT MATH IS STRICKLY OFF-LIMITS! Why? Because I say so! ;*)
		uint8_t status = getStatus( obj1TempRaw );
				
	    // 4. Display the status on the LED's for a couple of seconds.
		setLEDstatus( status );
	    
		// 5. Go to sleep and wait for next MCU reset.
		// No sleep on 5V-version => Datasheet p. 10: "Sleep mode is not available on the 5V version (MLX90614Axx)."
}

void pinSetup(){
	PORTA.DIRSET = LED_OK_bm | LED_WARN_bm | LED_NOK_bm;
}

void setLEDstatus(uint8_t status){
	PORTA.OUTCLR = LED_OK_bm | LED_WARN_bm | LED_NOK_bm;
	switch(status){
		case STATUS_OK:{
			PORTA.OUTSET = LED_OK_bm;
			_delay_ms(2000);
			break;
		}
		
		case STATUS_WARN:{
			 PORTA.OUTSET = LED_WARN_bm ;
			 _delay_ms(2000);
			 break;
		}
		case STATUS_NOK:{
			PORTA.OUTSET = LED_NOK_bm;
			_delay_ms(2000);
			break;
		}
		
		case STATUS_ERROR:{ 
			// Let's blink the NOK and WARN led fast to inform the user something went horribly wrong!
			for (uint8_t i = 0; i < 10; i++) {
				PORTA.OUTSET = LED_WARN_bm | LED_NOK_bm;
				_delay_ms(100);
				PORTA.OUTCLR = LED_WARN_bm | LED_NOK_bm;
				_delay_ms(100);
			}
			break;
		}
	}
}

uint8_t getStatus( uint16_t rawValue ){
	// MSB = 1?  => Sensor read error (Datasheet p. 30)
	if( ( rawValue & MLX90614_ERROR_BIT ) == MLX90614_ERROR_BIT){
		return STATUS_ERROR;
	}
	
	if( rawValue <= TEMP_TH_TOO_LOW ){
		return STATUS_ERROR;
	}
	
	if( rawValue <= TEMP_TH_OK ){
		return STATUS_OK;
	}
	
	if( rawValue <= TEMP_TH_WARN ){
		return STATUS_WARN;
	}
	
	return STATUS_NOK;
}

uint16_t readTempObj1Raw(){
	return I2C_0_read2ByteRegister(MLX90614_I2CADDR, MLX90614_REG_TOBJ1);
}

uint16_t readTempObj2Raw(){
	return I2C_0_read2ByteRegister(MLX90614_I2CADDR, MLX90614_REG_TOBJ2);
}