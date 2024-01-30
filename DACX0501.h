/**************
Arduino library for the DAC80501, DAC70501, DAC60501 DACs from Texas Instruments

***************/

#ifndef DACX0501_h
#define DACX0501_h

#include "Wire.h"

#define DACX0501_VOLTAGE_REFERENCE_V	2.5
#define DACX0501_BUF_GAIN_DEFAULT		2
#define DACX0501_REF_DIV_DEFAULT		1

#define DACX0501_MIN_I2C_ADDRESS		0x48
#define DACX0501_MAX_I2C_ADDRESS		0x4B



//############# COMMAND BYTES ###############
#define DACX0501_COMMAND_NOOP		0x00
#define DACX0501_COMMAND_DEVID		0x01
#define DACX0501_COMMAND_SYNC		0x02
#define DACX0501_COMMAND_CONFIG		0x03
#define DACX0501_COMMAND_GAIN		0x04
#define DACX0501_COMMAND_TRIGGER	0x05
#define DACX0501_COMMAND_STATUS		0x07
#define DACX0501_COMMAND_DAC		0x08

//############# REGISTER VALUES ##############
#define DACX0501_DEVID_DAC80501			0
#define DACX0501_DEVID_DAC70501			1
#define DACX0501_DEVID_DAC60501			2

#define DACX0501_BUFGAIN_1				0
#define DACX0501_BUFGAIN_2				1

#define DACX0501_REFDIV_1				0
#define DACX0501_REFDIV_2				1

#define DACX0501_TRIG_RESET				0b1010
#define DACX0501_TRIG_LDAC				1


typedef union _dacX0501_reg
{
	uint16_t reg_16;
	struct
	{
		uint8_t low;
		uint8_t high;
	}bytes;
}dacX0501_reg;

typedef union _dacX0501_devid_reg
{
	dacX0501_reg reg;
	struct
	{
		uint16_t res2: 12; //bit 11-0
		uint8_t devid: 3; // bit 14-12
		uint8_t res1: 1; //bit 15
	}bits;
}dacX0501_devid_reg;

typedef union _dacX0501_sync_reg
{
	dacX0501_reg reg;
	struct
	{
		uint8_t dac_sync_en: 1; //bit 0
		uint8_t res1: 15; //bit 15-10
	}bits;
}dacX0501_sync_reg;

typedef union _dacX0501_conf_reg
{
	dacX0501_reg reg;
	struct
	{
		uint8_t dac_pwdwn: 1; //bit 0
		uint8_t res1: 7; //bit 1-7
		uint8_t ref_pwdwn: 1; //bit 8
		uint8_t res2: 7; //bit 9-15
	}bits;
}dacX0501_conf_reg;

typedef union _dacX0501_gain_reg
{
	dacX0501_reg reg;
	struct
	{
		uint8_t buff_gain: 1; //bit 0
		uint8_t res1: 7; //bit 1-7
		uint8_t ref_div: 1; //bit 8
		uint8_t res2: 7; //bit 9-15
	}bits;
}dacX0501_gain_reg;

typedef union _dacX0501_trig_reg
{
	dacX0501_reg reg;
	struct
	{
		uint8_t soft_reset: 4; //bit 0-3
		uint8_t ldac: 1; //bit 4
		uint16_t res: 11; //bit 5-15
	}bits;
}dacX0501_trig_reg;

typedef union _dacX0501_status_reg
{
	dacX0501_reg reg;
	struct
	{
		uint8_t ref_alarm: 1; //bit 0
		uint16_t res: 15; //bit 4
	}bits;
}dacX0501_status_reg;

typedef union _dacX0501_dac_reg
{
	dacX0501_reg reg;
}dacX0501_dac_reg;

class DACX0501
{
	public:
		DACX0501(); //turns off internal reference
	
		void init(uint8_t address, TwoWire *i2c = &Wire); //set I2C address, get devid and set the type
		void reset();
		
		void set_buf_gain(uint8_t gain);
		void set_ref_div(uint8_t ref_div);
		void set_vref_value(float vref_value);//actual voltage of the reference
		
		void trigger_update();
		
		void set_dac(float voltage);
		uint16_t get_dac();
		float get_voltage();
		
		void shut_down_dac(bool status);
		void shut_down_ref(bool status);
		void shut_down_all();
		
		uint16_t get_dev_id();
		
	private:
		uint16_t _read_reg(uint8_t reg); //reads and returns the requested register
		void _write_register(uint8_t command_byte, dacX0501_reg reg);
		uint16_t _convert_voltage_to_dac_code(float voltage);
		float _convert_dac_code_to_voltage(uint16_t dac_code);
		uint8_t _wire_request_from(uint8_t num_bytes, bool stop);

		uint8_t _addr;
		TwoWire *_i2c;
		
		float _ref_v;
		uint8_t _buf_gain; //1 or 2
		uint8_t _ref_div; // 1 or 2
		uint8_t _num_bits; //hard-coded to 16 in constructor. 14 and 12 bit versions are left-aligned and will ignore lower bits, so will still work.
		bool _using_ref_internal;
};

#endif
