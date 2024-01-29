#include <DACX0501.h>
#include "Arduino.h"

DACX0501::DACX0501()
{
	_ref_v = DACX0501_VOLTAGE_REFERENCE_V;
	_buf_gain = DACX0501_BUF_GAIN_DEFAULT;
	_ref_div = DACX0501_REF_DIV_DEFAULT;
	_using_ref_internal = true;
	_num_bits = 16;
}

void DACX0501::init(uint8_t address, TwoWire *i2c) //set I2C address, get devid and set the type, turn off ref
{
	_addr = address;
	_i2c = i2c;
	
	//_i2c->setWireTimeout(3000, true);
	shut_down_ref(true);
	reset();
}

void DACX0501::reset()
{
	dacX0501_trig_reg trig_reg;
	trig_reg.reg.reg_16 = 0;
	trig_reg.bits.soft_reset = DACX0501_TRIG_RESET;
	
	_write_register(DACX0501_COMMAND_TRIGGER, trig_reg.reg);
	
	_buf_gain = DACX0501_BUF_GAIN_DEFAULT;
	_ref_div = DACX0501_REF_DIV_DEFAULT;
	
	if (!_using_ref_internal)
	{
		shut_down_ref(true);
	}
}

void DACX0501::set_buf_gain(uint8_t gain)
{
	if (gain == DACX0501_BUFGAIN_1 or gain == DACX0501_BUFGAIN_2)
	{
		dacX0501_gain_reg gain_reg;
		gain_reg.reg.reg_16 = _read_reg(DACX0501_COMMAND_GAIN);
		if (gain == DACX0501_BUFGAIN_1)
		{
			gain_reg.bits.buff_gain = DACX0501_BUFGAIN_1;
			_buf_gain = 1;
		}
		else if (gain == DACX0501_BUFGAIN_2)
		{
			gain_reg.bits.buff_gain = DACX0501_BUFGAIN_2;
			_buf_gain = 2;
		}
		
		_write_register(DACX0501_COMMAND_GAIN, gain_reg.reg);
	}
}

void DACX0501::set_ref_div(uint8_t ref_div)
{
	if (ref_div == 1 or ref_div == 2)
	{
		dacX0501_gain_reg gain_reg;
		gain_reg.reg.reg_16 = _read_reg(DACX0501_COMMAND_GAIN);
		if (ref_div == 1)
		{
			gain_reg.bits.ref_div = DACX0501_REFDIV_1;
			_ref_div = 1;
		}
		else if (ref_div == 2)
		{
			gain_reg.bits.ref_div = DACX0501_REFDIV_2;
			_ref_div = 2;
		}
		
		_write_register(DACX0501_COMMAND_GAIN, gain_reg.reg);
	}
}

void DACX0501::trigger_update() //for synchronous updates on the DAC outputs when not using broadcast mode
{
	dacX0501_trig_reg trig_reg;
	trig_reg.reg.reg_16 = 0;
	trig_reg.bits.ldac = 1;
	
	_write_register(DACX0501_COMMAND_TRIGGER, trig_reg.reg);
}

void DACX0501::set_vref_value(float vref_value)//actual voltage of the reference
{
	_ref_v = vref_value;
}

void DACX0501::set_dac(float voltage)
{
	dacX0501_dac_reg dac_reg;
	dac_reg.reg.reg_16 = _convert_voltage_to_dac_code(voltage);
	
	_write_register(DACX0501_COMMAND_DAC, dac_reg.reg);
}

uint16_t DACX0501::get_dac()
{
	return _read_reg(DACX0501_COMMAND_DAC);
}

float DACX0501::get_voltage()
{
	return _convert_dac_code_to_voltage(get_dac());
}

uint16_t DACX0501::_convert_voltage_to_dac_code(float voltage)
{
	//vout = DAC_DATA / 2^N * VREFIO / DIV * GAIN
	//DAC_DATA = VOUT * 2^N * DIV / VREFIO / GAIN
	
	if(voltage <= 0.0)
	{
		voltage = 0.0;
	}
	else if (voltage >= _ref_v*float(_buf_gain)/float(_ref_div))
	{
		voltage = _ref_v * _buf_gain / float(_ref_div);
	}
	
	uint32_t code = uint32_t((voltage * float(uint32_t(1)<<_num_bits) * float(_ref_div) / float(_ref_v) / float(_buf_gain)) + 0.5); //+0.5 to round truncate to nearest int
	if (code >= (uint16_t(1<<_num_bits) -1))
	{
		code = (uint16_t(1<<_num_bits) -1);
	}
	return uint16_t(code);
}

uint16_t DACX0501::_convert_dac_code_to_voltage(uint16_t dac_code)
{
	//vout = DAC_DATA / 2^N * VREFIO / DIV * GAIN
	//DAC_DATA = VOUT * 2^N * DIV / VREFIO / GAIN
	
	return (float(dac_code) / float(uint32_t(1)<<_num_bits) * float(_ref_v) / float(_ref_div) * float(_buf_gain));
}

void DACX0501::shut_down_dac(bool status)
{
	dacX0501_conf_reg conf_reg;
	conf_reg.reg.reg_16 = _read_reg(DACX0501_COMMAND_CONFIG);
	conf_reg.bits.dac_pwdwn = status;
	
	_write_register(DACX0501_COMMAND_CONFIG, conf_reg.reg);
}

void DACX0501::shut_down_ref(bool status)
{
	dacX0501_conf_reg conf_reg;
	conf_reg.reg.reg_16 = _read_reg(DACX0501_COMMAND_CONFIG);
	conf_reg.bits.ref_pwdwn = status;
	
	_write_register(DACX0501_COMMAND_CONFIG, conf_reg.reg);
	_using_ref_internal = !status;
}

void DACX0501::shut_down_all()
{
	dacX0501_conf_reg conf_reg;
	conf_reg.reg.reg_16 = 0;
	conf_reg.bits.dac_pwdwn = 1;
	conf_reg.bits.ref_pwdwn = 1;
	
	_write_register(DACX0501_COMMAND_CONFIG, conf_reg.reg);
}

uint16_t DACX0501::get_dev_id()
{
	dacX0501_devid_reg devid_reg;
	devid_reg.reg.reg_16 = _read_reg(DACX0501_COMMAND_DEVID);
	return devid_reg.reg.reg_16;
}

void DACX0501::_write_register(uint8_t command_reg, dacX0501_reg reg)
{
	_i2c->beginTransmission(_addr);
	_i2c->write(command_reg);
	_i2c->write(reg.bytes.high);
	_i2c->write(reg.bytes.low);
	_i2c->endTransmission(true);
}

uint16_t DACX0501::_read_reg(uint8_t reg) //reads and returns the requested register
{
	_i2c->beginTransmission(_addr);
	_i2c->write(reg);
	_i2c->endTransmission(false); //repeated start message
	
	uint8_t byte1, byte2;
	uint8_t response_length = 2;
	uint16_t result = 0;
	_wire_request_from(response_length, true); //send stop true
	
	byte1 = _i2c->read(); //MSB first
	byte2 = _i2c->read();
	
	result = (result | byte1) << 8;
	result = (result | byte2);
	
	return result;
}

uint8_t DACX0501::_wire_request_from(uint8_t num_bytes, bool stop)
{
	uint8_t num_bytes_read = 0;
	num_bytes_read = _i2c->requestFrom(_addr, num_bytes, stop);
	//uint8_t timeout_count = 0;
	//while (timeout_count < 10 && _i2c->getWireTimeoutFlag())
	//{
		//if a timeout occurred, and there were less than 10 timeouts, then try again.
		//_i2c->clearWireTimeoutFlag();
		//num_bytes_read = _i2c->requestFrom(_addr, num_bytes, stop);
		//timeout_count++;
	//}
	return num_bytes_read;
}
