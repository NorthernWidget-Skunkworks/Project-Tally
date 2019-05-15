//Tally_I2C_Test.ino
// #include <Wire.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "WireS.h"
#define DATA_PIN 10
#define CLK_PIN 9
#define RST_PIN 8
#define LOAD_PIN 7

#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)

uint8_t ADR = 0x33; //DUMMY! FIX! and make changable

uint8_t Config = 0; //Global config value
uint8_t Reg[3] = {0}; //Initialize registers
const uint8_t RegLen = 3; //Number of registers in Reg
bool Sample = false; //Flag to get a new sample value
bool Reset = false; //Reset flag
bool Clear = false; //Clear registers
bool Peek = false; //Clear values on read by default
bool Sleep = false; //Do not sleep by default

volatile bool StopFlag = false; //Used to indicate a stop condition 
volatile uint8_t RegID = 0; //Used to denote which register will be read from
volatile bool RepeatedStart = false; //Used to show if the start was repeated or not

const unsigned long WAIT_TIME = 100;

void setup() {
	pinMode(DATA_PIN, INPUT); //Setup pins
	pinMode(CLK_PIN, OUTPUT);
	pinMode(RST_PIN, OUTPUT);
	pinMode(LOAD_PIN, OUTPUT);

	pinMode(5, OUTPUT); //DEBUG!

	Wire.begin(ADR);  //Begin slave I2C FIX! Add variable address feature!

	  //Setup I2C slave
	Wire.onAddrReceive(addressEvent); // register event
	Wire.onRequest(requestEvent);     // register event
	Wire.onReceive(receiveEvent);
	Wire.onStop(stopEvent);
}

void loop() {
  // put your main code here, to run repeatedly:

  if(Sleep) { //Put device to sleep on wrap arround
    adc_disable(); // ADC uses ~320uA
    SPCR   &= ~_BV(SPE); // Disable SPI
//    PRR = 0xFF;
  	set_sleep_mode(SLEEP_MODE_PWR_DOWN);  

//    ACSR0A |= _BV(ACD0); // Disable analog comparators
//    ACSR1A |= _BV(ACD1);

    sleep_enable();
    sleep_mode(); //Waits here while in sleep mode
    
    sleep_disable();
    TWSCRA = bit(TWEN);
    Wire.begin(ADR);
  }

  int Counter = 0; //Run cycles after wakeup
  while(++Counter < WAIT_TIME) {
	Sample = BitRead(Reg[0], 0);
	Reset = BitRead(Reg[0], 1);
	Clear = BitRead(Reg[0], 2);
	Peek = BitRead(Reg[0], 3);
	Sleep = BitRead(Reg[0], 4); 

	if(Sample) {
		digitalWrite(5, HIGH);  //DEBUG!
		SplitAndLoad(1, GetTicks()); //Load tick value into register 1
		if(!Peek) ClearTicks(); //Clear counter values if peak is disabled
		Reg[0] = Reg[0] & 0xFE; //Clear sample bit in register
		Sample = false; //Clear sample bit
    digitalWrite(5, LOW);
	}
	if(Clear) {
		Reg[1] = 0x00; //Clear registers
		Reg[2] = 0x00;
		Reg[0] = Reg[0] & 0xFB; //Clear CLEAR bit
		Clear = false; //Clear flag
	}
	if(Reset) {
		ClearTicks(); //Clear registers 
		Reg[0] = Reg[0] & 0xFD; //Clear RESET bit
		Reset = false; //Clear flag
	}

  //FIX!!!
  // uint8_t TempReg = Reg[0];  //Do all manipulation in temporary register to avoid reads during writes 
  // TempReg = TempReg & 0xF8; //Clear lower 3 bits
  // TempReg = TempReg | (Clear << 2) | (Reset << 1) | (Sample); //Reload bits after results
  // Reg[0] = TempReg; 
//  digitalWrite(5, LOW); //DEBUG!
  	delay(1);
	}
	Counter = 0;

}

void SplitAndLoad(uint8_t Pos, unsigned int Val) //Write 16 bits
{
	uint8_t Len = sizeof(Val);
	for(int i = Pos; i < Pos + Len; i++) {
		Reg[i] = (Val >> (i - Pos)*8) & 0xFF; //Pullout the next byte
	}
}

boolean addressEvent(uint16_t address, uint8_t count)
{
	RepeatedStart = (count > 0 ? true : false);
	return true; // send ACK to master
}

void requestEvent()
{	
	//Allow for repeated start condition 
	// if(RepeatedStart) {
	// 	for(int i = 0; i < 2; i++) {
	// 		Wire.write(Reg[RegID + i]);
	// 	}
	// }
	// else {
		// Wire.write(Reg[RegID]);
	// }
	// Wire.write(RegID); //DEBUG!
	for(int i = 0; i < RegLen; i++) {
//    Wire.write((RegID + i) % RegLen); //DEBUG!
		Wire.write(Reg[(RegID + i) % RegLen]);  //Load all bytes (wrap around) into register
	}
}

void receiveEvent(int DataLen) 
{
    //Write data to appropriate location
    if(DataLen == 2){
	    //Remove while loop?? 
	    while(Wire.available() < 2); //Only option for writing would be register address, and single 8 bit value
	    uint8_t Pos = Wire.read();
	    uint8_t Val = Wire.read();
	    //Check for validity of write??
	    Reg[Pos] = Val; //Set register value
	}

	if(DataLen == 1){
		RegID = Wire.read(); //Read in the register ID to be used for subsequent read
	}
}

void stopEvent() 
{
	StopFlag = true;
  RegID = (RegID + Wire.getTransmitBytes()) % RegLen; //Increment appropriate amount
	//End comunication
}

unsigned int GetTicks() 
{
	unsigned int Data = 0; //Number of pulses since last read
	digitalWrite(LOAD_PIN, HIGH);  //Pulse LOAD pin low to trigger load of new data
	delayMicroseconds(5);
  	digitalWrite(LOAD_PIN, LOW); 

  	Data = digitalRead(DATA_PIN);
  	Data = Data << 1; 
	for(int i = 0; i < 15; i++) {
    digitalWrite(CLK_PIN, HIGH); //Clock new data in
		delayMicroseconds(5); //Wait to stabilize
		Data = (Data << 1) | digitalRead(DATA_PIN);
		// Serial.println(digitalRead(DATA_PIN));
    digitalWrite(CLK_PIN, LOW); 
		delayMicroseconds(5);
	} 

//	digitalWrite(LOAD_PIN, LOW);

	return Data;
}

void ClearTicks() 
{
	digitalWrite(RST_PIN, HIGH);  //Pulse reset pin high for short interval to trigger reset
	delayMicroseconds(5);
  	digitalWrite(RST_PIN, LOW); 
}

bool BitRead(uint8_t Val, uint8_t Pos) //Read the bit value at the specified position
{
	return (Val >> Pos) & 0x01;
}
