//TallyTest.ino

#define DATA_PIN 11  //Define digital pin connected to data line
#define CLK_PIN 5  //Define digital pin connected to the clock line
#define POWER_PIN 19

//NOTE: Inverse clock logic used for Margay!
void setup() {
	Serial.begin(38400); 
	pinMode(DATA_PIN, INPUT);  //Set pin modes for clock and data 
	pinMode(CLK_PIN, OUTPUT);
	#if defined(POWER_PIN);
		pinMode(POWER_PIN, OUTPUT);
		digitalWrite(POWER_PIN, LOW); //Turn on external power
		delay(5000); //Allow for charge 
	#endif
}

void loop() {
  Serial.print("Counts = "); 
  Serial.println(GetTicks());  //Get and print data
  ClearTicks(); //Clear data
  #if defined(POWER_PIN)
	digitalWrite(POWER_PIN, HIGH); //Turn off power
	delay(60000); //Wait a bit
	digitalWrite(POWER_PIN, LOW); //Turn on power
	delay(10);
  #endif
}

unsigned int GetTicks() 
{
	unsigned int Data = 0; //Number of pulses since last read
	digitalWrite(CLK_PIN, HIGH);  //Pulse clock pin high for 5ms to load new data
	delay(7);
  digitalWrite(CLK_PIN, LOW); 

	for(int i = 0; i < 16; i++) {
    digitalWrite(CLK_PIN, HIGH); 
		delayMicroseconds(500); //Wait to stabilize
		Data = (Data << 1) | digitalRead(DATA_PIN);
		// Serial.println(digitalRead(DATA_PIN));
    digitalWrite(CLK_PIN, LOW); 
		delayMicroseconds(500);
	}

	return Data;
}

void ClearTicks() 
{
	digitalWrite(CLK_PIN, HIGH);  //Pulse clock pin high for 10ms to clear data
	delay(20);
  	digitalWrite(CLK_PIN, LOW); 
}
