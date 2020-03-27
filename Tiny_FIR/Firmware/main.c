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

#define STATUS_OK			0
#define STATUS_WARN			1
#define STATUS_NOK			2
#define STATUS_ERROR		3
#define STATUS_INITIALIZING	4

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

// Globals
uint32_t millis;

// Function declarations
void pinSetup();
void setLedsAllOff();
void setLEDstatus(uint8_t status);
uint16_t readTempObj1Raw(void);
uint8_t getStatus( uint16_t rawValue );
void displayStatus( uint8_t status );

int main(void){
	    // When we arrive here it means we just had a reset-event. Most-probably because the user pushed the button!

	    // Init our little MCU: pins, drivers, possible Microchip middleware if there is no other way.
	    pinSetup();
	    atmel_start_init();
	    
	    // Wait until button is pushed
	    setLedsAllOff();
	   
	    // Initialize statuses array
	    uint16_t statuses [200]; // Keep last x statuses
	    uint16_t numStatuses = sizeof( statuses ) / sizeof( uint16_t ); // The number of elements in the array
	    for( uint16_t i = 0; i < numStatuses; i++ ){
		    statuses[i] = STATUS_INITIALIZING;
	    }

		millis = 0;
	    uint8_t worst = STATUS_OK;
	    for( uint16_t i = 0; i < numStatuses; i++ ){
		   
		    uint16_t obj1TempRaw = readTempObj1Raw();

		    uint8_t status = getStatus( obj1TempRaw );
		    statuses[i] = status;
		    worst = status > worst ? status : worst;
		    displayStatus( STATUS_INITIALIZING );
		    _delay_ms( 5 );
	    }
	     
		while(true)
		{
			displayStatus( worst );
			_delay_ms( 5 );
		}
}

void pinSetup(){
	PORTA.DIRSET = LED_OK_bm | LED_WARN_bm | LED_NOK_bm ;
}

uint16_t readTempObj1Raw(){
	
	/*
	 * Some hardcoded values you can return for testing
	 *
	// return TEMP_TH_TOO_LOW - 1;
	// return TEMP_TH_OK - 1;
	// return TEMP_TH_WARN - 1;
	*/
	return TEMP_TH_WARN + 1;
	
	/*
	 * Doesn't work --> seems to block... 
	 * 
    return I2C_0_read2ByteRegister(MLX90614_I2CADDR, MLX90614_REG_TOBJ1);
	*/
	
	/*
	 * Mimics logic in adafruit Arduino lib
	 * Doesn't work --> seems to always return 0 (After timeout?)...
	 
	  
	uint8_t reg = MLX90614_REG_TOBJ1;
	I2C_0_writeNBytes(MLX90614_I2CADDR, &reg, 1 );
	
	uint16_t regValue = 0;
	I2C_0_readNBytes(MLX90614_I2CADDR, &regValue, 2 );
	regValue = regValue << 8 | regValue >> 8;
	
	uint8_t temp;
	I2C_0_readNBytes(MLX90614_I2CADDR, &temp, 1 );
		
	return regValue;
	*/
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

void setLedsAllOff(){
  PORTA.OUTCLR = LED_OK_bm | LED_WARN_bm | LED_NOK_bm;
}

void displayStatus( uint8_t status ){
  millis += 5;
	
  switch(status){
    case STATUS_OK:{
	  PORTA.OUTCLR = LED_WARN_bm | LED_NOK_bm;
	  PORTA.OUTSET = LED_OK_bm;
      break;
    }

    case STATUS_WARN:{
      PORTA.OUTCLR = LED_OK_bm | LED_NOK_bm;
      PORTA.OUTSET = LED_WARN_bm;
      break;
    }

    case STATUS_NOK:{
      PORTA.OUTCLR = LED_OK_bm | LED_WARN_bm;
      PORTA.OUTSET = LED_NOK_bm;
      break;
    }
    
    case STATUS_ERROR:{
      // Make the warning LED flash
      uint32_t m = millis % 200;
      if( m < 20 ){
		  PORTA.OUTCLR = LED_OK_bm | LED_NOK_bm;
		  PORTA.OUTSET = LED_WARN_bm;
		  
	  }
	  else{
		  PORTA.OUTCLR = LED_OK_bm | LED_WARN_bm | LED_NOK_bm;
	  }
      break;
    }

    case STATUS_INITIALIZING:{
      // Make the warning LED flash
      uint32_t m = millis % 450;
	  PORTA.OUTCLR =  (( m < 150 ) ? 0 : LED_OK_bm )
					| (( m >= 150 && m < 300 ) ? 0 : LED_WARN_bm )
					| (( m >= 300 ) ? 0 : LED_NOK_bm );
	  PORTA.OUTSET = (( m < 150 ) ? LED_OK_bm : 0 )
				   | (( m >= 150 && m < 300 ) ? LED_WARN_bm : 0 )
				   | (( m >= 300 ) ? LED_NOK_bm : 0 );
      break;
    }
  }
}

