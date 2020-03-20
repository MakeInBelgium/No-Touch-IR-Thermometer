#include "I2C.h"
#include "avr/io.h"


#define LED_OK_bm	PIN3_bm
#define LED_WARN_bm	PIN6_bm
#define LED_NOK_bm	PIN7_bm

#define GOOD 0
#define WARN 1
#define BAD  2
#define ERROR 3

int main(void)
{
    // When we arive here it means we just had a reset-event. Most-probably because the user pushed the button!

    // 1. Init our little MCU: pins, drivers, possible Microchip middleware if there is no other way.
	pinSetup();
	I2C_init();
	
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