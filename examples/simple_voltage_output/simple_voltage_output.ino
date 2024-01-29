#include <DACX0501.h>

DACX0501 dac;

void setup()
{
	Serial.begin(115200);
	Serial.println("DAC Starting");
	Serial.println("Write to outputs");
	
	dac.init(0x48);
	dac.shut_down_ref(false);
	Serial.println("Ref ON");

	dacX0501_reg dac_reg;

	dac.set_dac(1.5); //sets output A to 1.5V
	Serial.println("Dac 1.5V");

	//buffer gain must be set to 2 (default) to get voltages higher than the reference (2.5V)
}


void loop()
{
	delay(1000);
}