//AnemometerWindTunnelTest.ino

uint8_t ExtInt = 11;
unsigned int HoldOff = 100; 
unsigned long ReadRate = 5000;
volatile unsigned long EventTimes[5] = {0};
volatile unsigned long InstTimes[2] = {0};
volatile uint8_t Index = 0; //Index of rolling average array

void setup() {
	Serial.begin(38400); //Begin serial

	attachInterrupt(digitalPinToInterrupt(ExtInt), Tally, FALLING); 
	pinMode(ExtInt, INPUT_PULLUP); //Pull up pin for use with reed switch
}

void loop() {
	static unsigned long LocalTime = millis();
	if(millis() - LocalTime > ReadRate) {  //Read out values
		LocalTime = millis();
		float EventsPerSecond = 0; 
		float InstantRate = 0;
		unsigned long Max = 0;
		unsigned long Min = EventTimes[0];
		for(int i = 0; i < 5; i++) {
			if(EventTimes[i] > Max) Max = EventTimes[i];
			else if(EventTimes[i] < Min) Min = EventTimes[i];
		}
		EventsPerSecond = 5000.0/float(Max - Min);

		InstantRate = (1000.0/float(InstTimes[1] - InstTimes[0]));

		Serial.print("\nAvg = "); Serial.print(EventsPerSecond); Serial.print(" [rot/s] ");
		Serial.print("Instant = "); Serial.print(InstantRate); Serial.print(" [rot/s]\n");

		for(int i = 0; i < 5; i++) {  //Clear arrays
			EventTimes[i] = 0;
		}
		InstTimes[0] = 0;
		InstTimes[1] = 0;
	}
}

void Tally() 
{
	static unsigned long LocalTime = millis(); //Time of previous event
	if(millis() - LocalTime > HoldOff) {
		LocalTime = millis();
		EventTimes[Index++] = millis();
		Index = Index % 5; //Rotate though the array

		InstTimes[0] = InstTimes[1]; //Shift left
		InstTimes[1] = millis(); //Update new value
	}
	//Else, do nothing, avoid multiple trips
}
