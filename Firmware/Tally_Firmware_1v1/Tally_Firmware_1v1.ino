/******************************************************************************
Tally_Firmware_1v1.ino
Driver for Tally event counter
Bobby Schulz @ Northern Widget LLC
5/16/2019
https://github.com/NorthernWidget-Skunkworks/Project-Tally

This firmware is to be run on the I2C capable Tally v1.1 in order to allow for interface, control, and data collection 

Firmware Version: 0.1.0

"On two occasions I have been asked, 'Pray, Mr. Babbage, if you put into the machine wrong figures, will the right answers come out?' 
I am not able rightly to apprehend the kind of confusion of ideas that could provoke such a question."
-Charles Babbage

Distributed as-is; no warranty is given.
******************************************************************************/

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "WireS.h"
#define DATA_PIN 10
#define CLK_PIN 9
#define RST_PIN 8
#define LOAD_PIN 7
#define CHARGE_SW 0
#define V_CAP_READ A7

uint8_t ADR = 0x33; //DUMMY! FIX! and make changable

// volatile bool GenCall = false; //Flag for generic address call over I2C //FIX! add generalizable address

uint8_t Config = 0; //Global config value
uint8_t Reg[5] = {0}; //Initialize registers
const uint8_t RegLen = sizeof(Reg); //Number of registers in Reg
bool Sample = false; //Flag to get a new sample value
bool Reset = false; //Reset flag
bool Clear = false; //Clear registers
bool Peek = false; //Clear values on read by default
bool Sleep = false; //Do not sleep by default
bool ReadCap = false; //Read float voltage of capacitor 

volatile bool StopFlag = false; //Used to indicate a stop condition 
volatile uint8_t RegID = 0; //Used to denote which register will be read from
volatile bool RepeatedStart = false; //Used to show if the start was repeated or not

const unsigned long WAIT_TIME = 100; //Genric wait cycle after read operation, keep device alive for repeated requests

void setup() {
	pinMode(DATA_PIN, INPUT); //Setup GPIO interface pins
	pinMode(CLK_PIN, OUTPUT);
	pinMode(RST_PIN, OUTPUT);
	pinMode(LOAD_PIN, OUTPUT);
	pinMode(CHARGE_SW, OUTPUT);
	digitalWrite(CHARGE_SW, LOW); //By default have system charging 

	Wire.begin(ADR);  //Begin slave I2C FIX! Add variable address feature!

	//Setup I2C slave
	Wire.onAddrReceive(AddressEvent); 
	Wire.onRequest(RequestEvent);    
	Wire.onReceive(ReceiveEvent);
	Wire.onStop(StopEvent);
}

void loop() {

	if(Sleep) { //Put device to sleep on wrap arround
		ADCSRA &= ~(1<<ADEN); //Disable ADC
		SPCR   &= ~_BV(SPE); //Disable SPI
		//    PRR = 0xFF;
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);  

		sleep_enable();
		sleep_mode(); //Waits here while in sleep mode

		sleep_disable(); //Wake up
		TWSCRA = bit(TWEN);  //Re-enable I2C
		Wire.begin(ADR);
	}

	int Counter = 0; //Run cycles after wakeup
	while(++Counter < WAIT_TIME) {
		Sample = BitRead(Reg[0], 0);
		Reset = BitRead(Reg[0], 1);
		Clear = BitRead(Reg[0], 2);
		Peek = BitRead(Reg[0], 3);
		Sleep = BitRead(Reg[0], 4); 
		ReadCap = BitRead(Reg[0], 5);

		if(Sample) {
			SplitAndLoad(1, GetTicks()); //Load tick value into register 1
			if(!Peek) ClearTicks(); //Clear counter values if peak is disabled
			Reg[0] = Reg[0] & 0xFE; //Clear sample bit in register
			Sample = false; //Clear sample bit
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

		if(ReadCap) {
			ReadVoltage(); //Read voltage and load into I2C regs
		}
		delay(1); //Wait during wake/I2C transaction 
	}
	Counter = 0; //Clear counter to either go to sleep or wrap around
}

void SplitAndLoad(uint8_t Pos, unsigned int Val) //Write 16 bits
{
	uint8_t Len = sizeof(Val);
	for(int i = Pos; i < Pos + Len; i++) {
		Reg[i] = (Val >> (i - Pos)*8) & 0xFF; //Pullout the next byte
	}
}

boolean AddressEvent(uint16_t Address, uint8_t count) //When a valid address is encountered on I2C
{
	// if(Address == 0x00) { //FIX! add generalizable address
	// 	GenCall = true;
	// }
	RepeatedStart = (count > 0 ? true : false);
	return true; // send ACK to master
}

void RequestEvent()  //When I2C values are requested
{	
	for(int i = 0; i < RegLen; i++) {  
		Wire.write(Reg[(RegID + i) % RegLen]);  //Load all bytes (wrap around) into register
	}
}

void ReceiveEvent(int DataLen) //When I2C values are sent 
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

	// if(DataLen == 1 && GenCall == true) {  //FIX! add generalizable address
	// 	ADR = Wire.read(); //Update address value 
	// }

	// if(DataLen == 1 && GenCall == false){
	// 	RegID = Wire.read(); //Read in the register ID to be used for subsequent read
	// }
	if(DataLen == 1){
		RegID = Wire.read(); //Read in the register ID to be used for subsequent read
	}
}

void StopEvent() //When stop value is detected on I2C
{
	StopFlag = true;
  	RegID = (RegID + Wire.getTransmitBytes()) % RegLen; //Increment register pointer appropriate amount
	//End comunication
}

unsigned int GetTicks() //Read number of ticks from digital logic 
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
    digitalWrite(CLK_PIN, LOW); 
		delayMicroseconds(5);
	} 
	return Data;
}

void ClearTicks() //Clear digital logic values 
{
	digitalWrite(RST_PIN, HIGH);  //Pulse reset pin high for short interval to trigger reset
	delayMicroseconds(5);
  	digitalWrite(RST_PIN, LOW); 
}

bool ReadVoltage() //Read the bit value at the specified position
{
	digitalWrite(CHARGE_SW, HIGH); //Connect voltage to ADC
	delay(1); //Wait to settle FIX??
	uint16_t Val = analogRead(V_CAP_READ);
	digitalWrite(CHARGE_SW, LOW); //Disconnect and continue charging 
	Reg[4] = Val >> 8; //Load MSB
	Reg[3] = Val & 0xFF; //Load LSB
}

bool BitRead(uint8_t Val, uint8_t Pos) //Read the bit value at the specified position
{
	return (Val >> Pos) & 0x01;
}
