#include "Adafruit_MLX90614.h"

#define MLX90614_ERROR_BIT 0x8000

#define PIN_LOW_FOR_SERIAL_MONITORING A0 // If this pin attached to GND, monitoring messages will be sent on the serial port
#define PIN_LED_OK                    A1 // Green LED
#define PIN_LED_WARN                  A2 // Yellow LED
#define PIN_LED_NOK                   A3 // Red LED
#define PIN_LOW_FOR_TRIGGER           A4 // When pulled low, a new measurement cycle is triggered (can be used to connect a push-button)

#define STATUS_OK             0
#define STATUS_WARN           1
#define STATUS_NOK            2
#define STATUS_ERROR          3
#define STATUS_INITIALIZING   4

/*
 * Temperature tresholds:
 *
 *    Temperature in °C = ( raw sensor value * 0.02 ) - 273.15
 * => Raw sensor value = ( Temperature in °C + 273.15 ) * 50
 *
 */

// TODO: values need to be verified by med, still dummy values
const uint16_t TEMP_TH_TOO_LOW =  ( ( 34.5 + 273.15 ) * 50 ) + 0.5;
const uint16_t TEMP_TH_OK =       ( ( 36.5 + 273.15 ) * 50 ) + 0.5;
const uint16_t TEMP_TH_WARN =     ( ( 38.0 + 273.15 ) * 50 ) + 0.5;

// Globals
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
bool serialMonitoring;    // If true, monitoring messages are sent over the serial port
uint16_t statuses [50];  // Keep last x statuses
uint16_t numStatuses;     // The number of elements in the array
uint16_t idxStatuses;     // The index to which the current measurement will be written

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
  }

  // Initialize statuses array
  idxStatuses = 0;
  numStatuses = sizeof( statuses ) / sizeof( uint16_t );
  for( uint16_t i = 0; i < numStatuses; i++ ){
    statuses[i] = STATUS_INITIALIZING;
  }
}

void loop(){
  // Get the current sensor value
  uint16_t obj1TempRaw = readTempObj1Raw();
  if( serialMonitoring ) serialPrintMeasurement( obj1TempRaw );

  // Calculate & store a status
  uint8_t status = getStatus( obj1TempRaw );
  statuses[idxStatuses] = status;
  idxStatuses = (idxStatuses + 1 ) % numStatuses;

  // Calculate the worst of last x measurements
  uint8_t worst = STATUS_OK;
  for( int i = 0; i < numStatuses; i++ ){
    worst = max( worst, statuses[i] );
  }
  
  // Set some LEDs
  displayStatus( worst );
  delay( 10 );
}

void pinSetup(){
  pinMode( PIN_LOW_FOR_SERIAL_MONITORING, INPUT_PULLUP);
  pinMode( PIN_LOW_FOR_TRIGGER, INPUT_PULLUP);
  pinMode( PIN_LED_OK, OUTPUT );
  pinMode( PIN_LED_WARN, OUTPUT );
  pinMode( PIN_LED_NOK, OUTPUT );
}

uint16_t readTempObj1Raw(){
  return mlx.read16(MLX90614_TOBJ1);
}

uint8_t getStatus( uint16_t rawValue ){
  // MSB = 1?  => Sensor read error (Datasheet p. 30)
  if( ( rawValue & MLX90614_ERROR_BIT ) == MLX90614_ERROR_BIT){
    if( serialMonitoring ) Serial.println( " - Sensor error => STATUS_ERROR" );
    return STATUS_ERROR;
  }
  
  if( rawValue <= TEMP_TH_TOO_LOW ){
    if( serialMonitoring ) Serial.println( " - T too low => STATUS_ERROR" );
    return STATUS_ERROR;
  }
  
  if( rawValue <= TEMP_TH_OK ){
    if( serialMonitoring ) Serial.println( " - T ok => STATUS_OK" );
    return STATUS_OK;
  }
  
  if( rawValue <= TEMP_TH_WARN ){
    if( serialMonitoring ) Serial.println( " - T above ok => STATUS_WARN" );
    return STATUS_WARN;
  }
  if( serialMonitoring ) Serial.println( " - T above warning => STATUS_NOK" );
  return STATUS_NOK;
}

void displayStatus( uint8_t status ){
  switch(status){
    case STATUS_OK:{
      digitalWrite( PIN_LED_OK, HIGH );
      digitalWrite( PIN_LED_WARN, LOW );
      digitalWrite( PIN_LED_NOK, LOW );
      break;
    }

    case STATUS_WARN:{
      digitalWrite( PIN_LED_OK, LOW );
      digitalWrite( PIN_LED_WARN, HIGH );
      digitalWrite( PIN_LED_NOK, LOW );
      break;
    }

    case STATUS_NOK:{
      digitalWrite( PIN_LED_OK, LOW );
      digitalWrite( PIN_LED_WARN, LOW );
      digitalWrite( PIN_LED_NOK, HIGH );
      break;
    }
    
    case STATUS_ERROR:{
      // Make the warning LED flash
      uint32_t m = millis() % 200;
      bool ledIsOn = m < 20;
      digitalWrite( PIN_LED_OK, LOW );
      digitalWrite( PIN_LED_WARN, ledIsOn );
      digitalWrite( PIN_LED_NOK, LOW );
      break;
    }

    case STATUS_INITIALIZING:{
      // Make the warning LED flash
      uint32_t m = millis() % 200;
      digitalWrite( PIN_LED_OK, m < 20 );
      digitalWrite( PIN_LED_WARN, m >= 66 && m < 86 );
      digitalWrite( PIN_LED_NOK, m >= 132 && m < 152 );
      break;
    }
  }
}

void serialPrintMeasurement( uint16_t tempRaw ){
  Serial.print( millis() );
  Serial.print( " - Sensor value: " );
  Serial.print( tempRaw ); 
  Serial.print( " = ");
  Serial.print( rawTempToCelcius( tempRaw ) );
  Serial.print( "°C " );
}

double rawTempToCelcius( uint16_t raw ){
  return raw * 0.02 - 273.15;
}
