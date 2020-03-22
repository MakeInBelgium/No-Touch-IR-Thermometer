#include <Wire.h>
#include "Adafruit_MLX90614.h"
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// MLX90614 Datasheet: https://www.melexis.com/-/media/files/documents/datasheets/mlx90614-datasheet-melexis.pdf
#define MLX90614_I2CADDR    0x5A
#define MLX90614_REG_RAWIR1 0x04
#define MLX90614_REG_RAWIR2 0x05
#define MLX90614_REG_TA     0x06
#define MLX90614_REG_TOBJ1  0x07
#define MLX90614_REG_TOBJ2  0x08

#define MLX90614_ERROR_BIT 0x8000

#define LED_OK_bm   A1
#define LED_WARN_bm A2
#define LED_NOK_bm  A3

#define STATUS_OK   0
#define STATUS_WARN   1
#define STATUS_NOK    2
#define STATUS_ERROR  3

/*
 * Temperature tresholds:
 *
 *    Temperature in °C = ( raw sensor value * 0.02 ) - 273.15
 * => Raw sensor value = ( Temperature in °C + 273.15 ) * 50
 *
 */

// TODO: values need to be verified by med, still dummy values
const uint16_t TEMP_TH_TOO_LOW =  ( ( 34.5 + 273.15 ) * 50 ) + 0.5;
const uint16_t TEMP_TH_OK =       ( ( 37.5 + 273.15 ) * 50 ) + 0.5;
const uint16_t TEMP_TH_WARN =     ( ( 38.0 + 273.15 ) * 50 ) + 0.5;


// Function declarations
void pinSetup();
void setLEDstatus(uint8_t status);
uint16_t readTempObj1Raw(void);
uint16_t readTempObj2Raw(void);
uint8_t getStatus( uint16_t rawValue );

void setup(){
      // When we arrive here it means we just had a reset-event. Most-probably because the user pushed the button!

      // 1. Init our little MCU: pins, drivers, possible Microchip middleware if there is no other way.
      pinSetup();
      mlx.begin();
      
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

void loop(){
}

void pinSetup(){
  pinMode( LED_OK_bm, OUTPUT );
  pinMode( LED_WARN_bm, OUTPUT );
  pinMode( LED_NOK_bm, OUTPUT );
}

void setLEDstatus(uint8_t status){
  digitalWrite( LED_OK_bm, LOW );
  digitalWrite( LED_WARN_bm, LOW );
  digitalWrite( LED_NOK_bm, LOW );
  
  switch(status){
    case STATUS_OK:{
      digitalWrite( LED_OK_bm, HIGH );
      delay(2000);
      break;
    }
    
    case STATUS_WARN:{
       digitalWrite( LED_WARN_bm, HIGH );
       delay(2000);
       break;
    }
    case STATUS_NOK:{
      digitalWrite( LED_NOK_bm, HIGH );
      delay(2000);
      break;
    }
    
    case STATUS_ERROR:{ 
      // Let's blink the NOK and WARN led fast to inform the user something went horribly wrong!
      for (uint8_t i = 0; i < 10; i++) {
        digitalWrite( LED_OK_bm, LOW );
        digitalWrite( LED_WARN_bm, LOW );
        digitalWrite( LED_NOK_bm, LOW );
        _delay_ms(100);
        digitalWrite( LED_OK_bm, HIGH );
        digitalWrite( LED_WARN_bm, HIGH );
        digitalWrite( LED_NOK_bm, HIGH );
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
  return mlx.read16(MLX90614_REG_TOBJ1);
}

uint16_t readTempObj2Raw(){
  return mlx.read16( MLX90614_REG_TOBJ2);
}
