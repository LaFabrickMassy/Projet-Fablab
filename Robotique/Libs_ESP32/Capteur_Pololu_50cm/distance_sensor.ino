//********************************************************************
// Pololu distance sensor 50cm, product item #4064
// https://www.pololu.com/product/4064
// Circuit based on ST VL53L0X ToF ranging sensor
//********************************************************************

//********************************************************************
typedef struct {
	int pin;
	unsigned long rising_time;
	unsigned long duration;
}
sensor_t;

//********************************************************************
// Sensor 1
//********************************************************************
sensor_t sensor1;

//********************************************************************
void IRAM_ATTR isr_sensor1() 
{
	if (digitalRead(sensor1.pin))
		sensor1.rising_time = micros();
	else
	{
		sensor1.duration = micros() - sensor1.rising_time;
	}
}

//********************************************************************
void setup_sensor1(int pin)
{
	sensor1.pin = pin;
	pinMode(pin, INPUT_PULLDOWN);
	attachInterrupt(pin, isr_sensor1, CHANGE);
}

//********************************************************************
int distance_sensor1()
{
	if(sensor1.duration > 2000)
		return 0;
	else
		return (sensor1.duration-1000)*3/4;
}

//********************************************************************
// Sensor 2
//********************************************************************
sensor_t sensor2;

//********************************************************************
void IRAM_ATTR isr_sensor2() 
{
	if (digitalRead(sensor2.pin))
		sensor2.rising_time = micros();
	else
	{
		sensor2.duration = micros() - sensor2.rising_time;
	}
}

//********************************************************************
void setup_sensor2(int pin)
{
	sensor2.pin = pin;
	pinMode(pin, INPUT_PULLDOWN);
	attachInterrupt(pin, isr_sensor2, CHANGE);
}

//********************************************************************
int distance_sensor2()
{
	if(sensor2.duration > 1800)
		return 0;
	else
		return (sensor2.duration-1000)*3/4;
}

//********************************************************************
// Sensor 3
//********************************************************************
sensor_t sensor3;

//********************************************************************
void IRAM_ATTR isr_sensor3() 
{
	if (digitalRead(sensor3.pin))
		sensor3.rising_time = micros();
	else
	{
		sensor3.duration = micros() - sensor3.rising_time;
	}
}

//********************************************************************
void setup_sensor3(int pin)
{
	sensor3.pin = pin;
	pinMode(pin, INPUT_PULLDOWN);
	attachInterrupt(pin, isr_sensor3, CHANGE);
}

//********************************************************************
int distance_sensor3()
{
	if(sensor3.duration > 1800)
		return 0;
	else
		return (sensor3.duration-1000)*3/4;
}

