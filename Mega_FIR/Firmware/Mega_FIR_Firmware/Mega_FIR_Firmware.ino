#include "Adafruit_MLX90614.h"

// MLX90614 Datasheet: https://www.melexis.com/-/media/files/documents/datasheets/mlx90614-datasheet-melexis.pdf

#define MLX90614_ERROR_BIT 0x8000

#define PIN_LOW_FOR_SERIAL_MONITORING A0 // If this pin attached to GND, serial monitoring will be done
#define PIN_LED_OK                    A1 // Green LED
#define PIN_LED_WARN                  A2 // Yellow LED
#define PIN_LED_NOK                   A3 // Red LED
#define PIN_LOW_FOR_TRIGGER           A4 // When pulled low, a new measurement cycle is triggered

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

// Globals
bool serialMonitoring;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup(){

  // Init all the things
  pinSetup();
  mlx.begin();
  
  // No messages are sent on the serial port
  // unless PIN_LOW_FOR_SERIAL_MONITORING is connected to GND
  serialMonitoring = false;
  if( !digitalRead( PIN_LOW_FOR_SERIAL_MONITORING ) ){
    serialMonitoring = true;
    Serial.begin( 9600 );
    while(!Serial);
    Serial.println( "Mega_FIR - Starting" );
  }
 
  doMeasurement();
}

void loop(){
  while( digitalRead( PIN_LOW_FOR_TRIGGER ) );
  doMeasurement();
}

void doMeasurement(){
  // Do 5 measurements, keep max
  uint16_t obj1TempRawMax = 0;
  setLedAllOn();
  for( int i = 0; i < 5; i++ ){
    uint16_t obj1TempRaw = readTempObj1Raw();
    obj1TempRawMax = max( obj1TempRawMax, obj1TempRaw );
    if( serialMonitoring ) serialPrintMeasurement(obj1TempRaw, i + 1);
    if( i < 4 ){
      delay( 200 );
    }
  }
  if( serialMonitoring ) serialPrintMax( obj1TempRawMax );
    
  // Calculate the status
  uint8_t status = getStatus( obj1TempRawMax );
        
  // Display the status on the LED's for a couple of seconds.
  setLedStatus( status );
}

void pinSetup(){
  pinMode( PIN_LOW_FOR_SERIAL_MONITORING, INPUT_PULLUP);
  pinMode( PIN_LOW_FOR_TRIGGER, INPUT_PULLUP);
  pinMode( PIN_LED_OK, OUTPUT );
  pinMode( PIN_LED_WARN, OUTPUT );
  pinMode( PIN_LED_NOK, OUTPUT );
}

void setLedAllOff(){
  digitalWrite( PIN_LED_OK, LOW );
  digitalWrite( PIN_LED_WARN, LOW );
  digitalWrite( PIN_LED_NOK, LOW );
}

void setLedAllOn(){
  digitalWrite( PIN_LED_OK, HIGH );
  digitalWrite( PIN_LED_WARN, HIGH );
  digitalWrite( PIN_LED_NOK, HIGH );
}

void setLedStatus(uint8_t status){
  setLedAllOff();
  
  switch(status){
    case STATUS_OK:{
      digitalWrite( PIN_LED_OK, HIGH );
      delay(2000);
      break;
    }
    
    case STATUS_WARN:{
       digitalWrite( PIN_LED_WARN, HIGH );
       delay(2000);
       break;
    }
    case STATUS_NOK:{
      digitalWrite( PIN_LED_NOK, HIGH );
      delay(2000);
      break;
    }
    
    case STATUS_ERROR:{ 
      // Let's blink the NOK and WARN led fast to inform the user something went horribly wrong!
      for (uint8_t i = 0; i < 15; i++) {
        setLedAllOn();
        _delay_ms(100);
        setLedAllOff();
        _delay_ms(100);
      }
      break;
    }
  }
}

uint8_t getStatus( uint16_t rawValue ){
  // MSB = 1?  => Sensor read error (Datasheet p. 30)
  if( ( rawValue & MLX90614_ERROR_BIT ) == MLX90614_ERROR_BIT){
    if( serialMonitoring ) Serial.println( "Sensor returned error => STATUS_ERROR" );
    return STATUS_ERROR;
  }
  
  if( rawValue <= TEMP_TH_TOO_LOW ){
    if( serialMonitoring ) Serial.println( "Temperature too low => STATUS_ERROR" );
    return STATUS_ERROR;
  }
  
  if( rawValue <= TEMP_TH_OK ){
    if( serialMonitoring ) Serial.println( "Temperature ok => STATUS_OK" );
    return STATUS_OK;
  }
  
  if( rawValue <= TEMP_TH_WARN ){
    if( serialMonitoring ) Serial.println( "Temperature above ok => STATUS_WARN" );
    return STATUS_WARN;
  }
  if( serialMonitoring ) Serial.println( "Temperature above warning => STATUS_NOK" );
  return STATUS_NOK;
}

uint16_t readTempObj1Raw(){
  return mlx.read16(MLX90614_TOBJ1);
}

double rawTempToCelcius( uint16_t raw ){
  return raw * 0.02 - 273.15;
}

void serialPrintMeasurement( uint16_t tempRaw, int i ){
  Serial.print( "Sensor value " );
  Serial.print( i );
  Serial.print( ": " );
  Serial.print( tempRaw ); 
  Serial.print( " = ");
  Serial.print( rawTempToCelcius( tempRaw ) );
  Serial.println( "°C" );
}

void serialPrintMax( uint16_t tempMax ){
  Serial.print( " => Maximum value: " );
  Serial.print( tempMax ); 
  Serial.print(" = ");
  Serial.print( rawTempToCelcius( tempMax ) );
  Serial.println( "°C" );
}
