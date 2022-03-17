const int SENSOR1_PIN=23;
const int SENSOR2_PIN=22;

const int ENCODER1_PIN=1;

//********************************************************************
void setup() 
{
	//**************************************************
	// Setup serial monitor
	Serial.begin(115200);
	Serial.print("************ Setup ******************");
	Serial.println();
	
	setup_sensor1(SENSOR1_PIN);
	setup_sensor2(SENSOR2_PIN);
	
	setup_encoder1(SENSOR1_PIN);
	
}

//********************************************************************
void loop(){
	Serial.print(distance_sensor1());
	Serial.print(" - ");
	Serial.print(sensor1.duration);
	Serial.print(" - ");
	Serial.print(distance_sensor2());
	Serial.print(" - ");
	Serial.print(sensor2.duration);
	Serial.println("");
	delay(500);
}
