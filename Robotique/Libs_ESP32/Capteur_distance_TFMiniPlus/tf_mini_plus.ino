#include <HardwareSerial.h>
#include <TFMPlus.h>

HardwareSerial UART_a(1);
HardwareSerial UART_b(2);

//TFmini tfmini;
// Pins:
// Red: +5V
// White: receiving, TX, pin 17
// Blue: transmitting, RX, pin 16
// Black: GND
TFMPlus tfmP_a;
TFMPlus tfmP_b;

#define PIN_TFA_RX  5
#define PIN_TFA_TX 18
#define PIN_TFB_RX 16
#define PIN_TFB_TX 17




void setup() {
    Serial.begin( 115200);   // Intialize terminal serial port
    delay(20);               // Give port time to initalize
   
	
    UART_a.begin( 115200, SERIAL_8N1, PIN_TFA_RX, PIN_TFA_TX);   // Intialize terminal serial port
    delay(20);               // Give port time to initalize
    tfmP_a.begin( &UART_a);   // Initialize device library object and...

    UART_b.begin( 115200, SERIAL_8N1, PIN_TFB_RX, PIN_TFB_TX);  // Initialize TFMPLus device serial port.
    delay(20);               // Give port time to initalize
    tfmP_b.begin( &UART_b);   // Initialize device library object and...

    delay(500);  // added to allow the System Rest enough time to complete
	
	Serial.println("Setup done *********");

}

int16_t tfDist = 0;
int16_t tfFlux = 0;
int16_t tfTemp = 0;
void loop()
{
	unsigned long start = millis();
    //if( tfmP_a.getData( tfDist, tfFlux, tfTemp)) // Get data from the device.
    //{
		/*
		Serial.print( "A Dist: ");   // display distance in log
		Serial.print(tfDist);   // display distance in log
		Serial.print( " Flux: ");   // display distance in log
		Serial.print(tfFlux);   // display distance in log
		Serial.print( " Temp: ");   // display distance in log
		Serial.println(tfTemp);   // display distance in log
		*/
    //}

    if( tfmP_b.getData( tfDist, tfFlux, tfTemp)) // Get data from the device.
    {
      
	  Serial.print( "B Dist: ");   // display distance in log
      Serial.print(tfDist);   // display distance in log
      Serial.print( " Flux: ");   // display distance in log
      Serial.print(tfFlux);   // display distance in log
      Serial.print( " Temp: ");   // display distance in log
      Serial.println(tfTemp);   // display distance in log
		
    }

	unsigned long stop = millis();
	Serial.print("Loop in ");
	Serial.println(stop - start);
    delay(250);
}

