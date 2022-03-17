const int SENSOR1_PIN=23;
const int SENSOR2_PIN=22;

//********************************************************************
void setup() 
{
	//**************************************************
	// Setup serial monitor
	Serial.begin(115200);
	Serial.print("************ Setup ******************");
	Serial.println();
	
	//**************************************************
	// Initialise two sensors
	setup_sensor1(SENSOR1_PIN);
	setup_sensor2(SENSOR2_PIN);
	
}

//********************************************************************
void loop(){
	// print measured distance for sensor 1
	Serial.print(distance_sensor1());
	Serial.print(" - ");
	// print duration of sensor 1 pulse
	Serial.print(sensor1.duration);
	Serial.print(" - ");
	// print measured distance for sensor 2
	Serial.print(distance_sensor2());
	Serial.print(" - ");
	// print duration of sensor 2 pulse
	Serial.print(sensor2.duration);
	Serial.println("");
	delay(500);
}
