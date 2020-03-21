#include <atmel_start.h>
#include <avr/io.h>
#include <util/delay.h>
#include <i2c_simple_master.h>

#define MLX90614_I2CADDR 0x5A
#define MLX90614_REG_RAWIR1 0x04
#define MLX90614_REG_RAWIR2 0x05
#define MLX90614_REG_TA 0x06
#define MLX90614_REG_TOBJ1 0x07
#define MLX90614_REG_TOBJ2 0x08

#define LED_OK_bm	PIN3_bm
#define LED_WARN_bm	PIN6_bm
#define LED_NOK_bm	PIN7_bm

#define GOOD 0
#define WARN 1
#define BAD  2
#define ERROR 3

/*
 * Temperature tresholds:
 *
 *    Temperature in °C = ( raw sensor value * 0.02 ) + 273.15
 * => Raw sensor value = ( Temperature in °C - 273.15 ) * 50
 *
 */

// TODO: values need to be verified by med, still dummy values
const uint16_t TEMP_TH_OK = ( 37.2 - 273.15 ) * 50 + 0.5;
const uint16_t TEMP_TH_WARN = ( 38.0 - 273.15 ) * 50 + 0.5;


// Function declarations
void pinSetup();
void setLEDstatus(uint8_t status);
uint16_t readTempObjRaw(void);

int main(void)
{
	    // When we arrive here it means we just had a reset-event. Most-probably because the user pushed the button!

	    // 1. Init our little MCU: pins, drivers, possible Microchip middleware if there is no other way.
	    pinSetup();
	    atmel_start_init();
	    
	    // 2. Do a few measurements for 1 second and average.
	    // 3. Maybe some calculations on the data. FLOATING POINT MATH IS STRICKLY OFF-LIMITS! Why? Because I say so! ;*)
	    // 4. Display the status on the LED's for a couple of seconds.
	    // 5. Go to sleep and wait for next MCU reset.
}

void pinSetup(){
	PORTA.DIRSET = LED_OK_bm | LED_WARN_bm | LED_NOK_bm;
}

void setLEDstatus(uint8_t status){
	PORTA.OUTCLR = LED_OK_bm | LED_WARN_bm | LED_NOK_bm;
	switch(status){
		case GOOD: PORTA.OUTSET = LED_OK_bm; break;
		case WARN: PORTA.OUTSET = LED_WARN_bm ; break;
		case BAD: PORTA.OUTSET = LED_NOK_bm; break;
		case ERROR: // Let's blink the NOK and WARN led fast to inform the user something went horribly wrong!
		for (uint8_t i = 0; i<10;i++)
		{
			PORTA.OUTSET = LED_WARN_bm | LED_NOK_bm;
			_delay_ms(100);
			PORTA.OUTCLR = LED_WARN_bm | LED_NOK_bm;
			_delay_ms(100);
		}
		break;
	}
}

uint16_t readTempObjRaw(uint8_t reg)
{
	return I2C_0_read2ByteRegister(MLX90614_I2CADDR, MLX90614_REG_TOBJ1);
}