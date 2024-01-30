#include <DACX0501.h>

DACX0501 dac;

const float sine_wave_points[] = {0.0, 0.15643447, 0.30901699, 0.4539905, 0.58778525, 0.70710678, 0.80901699, 0.89100652, 0.95105652, 0.98768834, 1.0};
const uint8_t num_sine_table_points = 10;

const float frequency_hz = 50; //max frequency is about 50Hz (limited by I2C transaction time and processing time)
const float amplitude = 1.0;
const float offset = 3.0;

#define MAX_V 5.0
#define MIN_V 0.0
float period;
uint16_t point_delay_ms;
uint8_t num_sine_points;
float voltage;

void setup()
{
	Serial.begin(115200);
	Wire.begin();
	delay(2000); //delay so you can see serial output with STM32

	Serial.println("DACX0501 Sine Wave Example");
	
	dac.init(0x48);
	dac.shut_down_ref(false);
	dac.shut_down_dac(false);
	Serial.println("DAC Started");
	Serial.println("Ref ON");
	
	if (amplitude - offset < 0)
	{
		Serial.println(F("Amplitude and offset create negative portions of the sine wave, \
		which the DAC cannot reproduce. Wave will be chopped."));
	}

	dac.set_dac(0);
	
	num_sine_points = 4*num_sine_table_points;
	period = 1.0/frequency_hz;
  	Serial.println(period);
	point_delay_ms = period / num_sine_points * 1000.0;
  	Serial.println(point_delay_ms);
}


void loop()
{
	for(int index = 0; index < num_sine_table_points; index++) //0 to 9
	{
    voltage = sine_wave_points[index] * amplitude + offset;
		voltage = constrain(voltage, MIN_V, MAX_V);
		dac.set_dac(voltage);
		delay(point_delay_ms);
	}
	for(int index = num_sine_table_points-2; index > 0; index--)//8 to 1
	{
		voltage = sine_wave_points[index] * amplitude + offset;
		voltage = constrain(voltage, MIN_V, MAX_V);
		dac.set_dac(voltage);
		delay(point_delay_ms);
	}
	for(int index = 0; index < num_sine_table_points; index++)//0 to 9
	{
		voltage = -1 * (sine_wave_points[index] * amplitude) + offset;
		voltage = constrain(voltage, MIN_V, MAX_V);
		dac.set_dac(voltage);
		delay(point_delay_ms);
	}
	for(int index = num_sine_table_points-2; index > 0; index--)//8 to 1
	{
		voltage =  -1 * (sine_wave_points[index] * amplitude) + offset;
		voltage = constrain(voltage, MIN_V, MAX_V);
		dac.set_dac(voltage);
		delay(point_delay_ms);
	}
}