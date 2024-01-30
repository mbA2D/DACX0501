#include <DACX0501.h>
#include <Wire.h>

DACX0501 dac;

void setup()
{
	Serial.begin(115200);
  	Wire.begin();
  	delay(2000); //delay so you can see serial output with STM32
	
	Serial.println("DAC Starting");
	Serial.println("Write to outputs");
	
	dac.init(0x48);
	dac.shut_down_ref(false);
  	dac.shut_down_dac(false);
	Serial.println("Ref ON");

	dac.set_buf_gain(DACX0501_BUFGAIN_1);
  	Serial.println("Setting Dac to 1.5V");
	dac.set_dac(1.5); //sets output A to 1.5V

  	Serial.print("dac code read: ");
  	Serial.println(dac.get_dac());

  	Serial.print("dac voltage set: ");
  	Serial.println(dac.get_voltage());

	//buffer gain must be set to 2 (default) to get voltages higher than the reference (2.5V)
}


void loop()
{
	delay(1000);
}