#include <core/vatek_base.h>
#include <hal/hal_bridge_board.h>
#include <stm32_core.h>
#include <stdarg.h>

#define HAL_I2CCMD_BUF_LEN		9

#define HAL_I2CCHIP_WRITE		0x06
#define HAL_I2CCHIP_READ		0x04
#define HAL_I2CCMD_WRITE		0x07
#define HAL_I2CCMD_READ			0x05

#define HAL_I2CCHIP_ADDRESS		0x20
#define HAL_I2CCHIP_WRADDR		(HAL_I2CCHIP_ADDRESS)
#define HAL_I2CCHIP_RDADDR		(HAL_I2CCHIP_WRADDR | 1)

static uint8_t hali2c_cmd[9];
static vatek_debug_level haldbg_level = debug_level_warning;

void chip_operation_wait() { int32_t i = 100; while (i--); }

extern vatek_result chip_i2c_send_read(Pstm32_i2c pi2c,uint8_t cmd,int32_t addr,uint32_t* val);
extern vatek_result chip_i2c_send_write(Pstm32_i2c pi2c,uint8_t cmd,int32_t addr,uint32_t val);

extern vatek_result chip_i2c_write(Pstm32_i2c pi2c, uint8_t* pbuf, int32_t len);
extern vatek_result chip_i2c_read(Pstm32_i2c pi2c, uint8_t* pbuf, int32_t len);

extern vatek_result chip_write_buffer_word(Pstm32_i2c pi2c, int32_t addr, uint32_t val);
extern vatek_result chip_read_buffer_word(Pstm32_i2c pi2c, int32_t addr, uint32_t* val);

uint32_t vatek_get_tick_ms(void)
{
	return hal_system_get_tick();
}

void vatek_sleep_ms(uint32_t ms)
{
	hal_system_sleep(ms);
}

_HAL_WEAK(void,vatek_debug_set)(vatek_debug_level level)
{
	haldbg_level = level;
}

_HAL_WEAK(void,vatek_debug_print)(vatek_debug_level level,const char* fmt,...)
{
	if(level >= haldbg_level)
	{
		va_list arg;
		va_start(arg, fmt);
		vprintf(fmt, arg);
		va_end(arg);
	}
}

vatek_result vatek_chip_read_register(hvatek_chip hchip, int32_t addr, uint32_t* val)
{
	return chip_i2c_send_read((Pstm32_i2c)hchip,HAL_I2CCHIP_READ,addr,val);
}

vatek_result vatek_chip_write_register(hvatek_chip hchip, int32_t addr, uint32_t val)
{
	return chip_i2c_send_write((Pstm32_i2c)hchip,HAL_I2CCHIP_WRITE,addr,val);
}

vatek_result vatek_chip_read_memory(hvatek_chip hchip, int32_t addr, uint32_t* val)
{
	return chip_i2c_send_read((Pstm32_i2c)hchip,HAL_I2CCMD_READ,addr,val);
}

vatek_result vatek_chip_write_memory(hvatek_chip hchip, int32_t addr, uint32_t val)
{
	return chip_i2c_send_write((Pstm32_i2c)hchip,HAL_I2CCMD_WRITE,addr,val);
}

vatek_result vatek_chip_write_buffer(hvatek_chip hchip, int32_t addr,uint8_t* buf,int32_t wlen)
{
	vatek_result res = vatek_success;
	uint32_t* wvals = (uint32_t*)buf;
	while (wlen--)
	{
            
		res = chip_write_buffer_word((Pstm32_i2c)hchip, addr, *wvals);
		addr++;
		wvals++;
	}
	return res;	
}

vatek_result vatek_chip_read_buffer(hvatek_chip hchip,int32_t addr,uint8_t* buf,int32_t wlen)
{
	vatek_result res = vatek_success;
	uint32_t* wvals = (uint32_t*)buf;
	while (wlen--)
	{
		res = chip_read_buffer_word((Pstm32_i2c)hchip, addr, wvals);
		addr++;
		wvals++;
	}
	return res;
}

vatek_result chip_i2c_send_read(Pstm32_i2c pi2c,uint8_t cmd,int32_t addr,uint32_t* val)
{
	vatek_result res = vatek_success;
	hali2c_cmd[0] = 0x04;					/* i2c_ip buffer position*/
	hali2c_cmd[1] = (uint8_t)addr;
	hali2c_cmd[2] = (uint8_t)(addr >> 8);
	hali2c_cmd[3] = (uint8_t)(addr >> 16);
	hali2c_cmd[4] = cmd;

	res = chip_i2c_write(pi2c, &hali2c_cmd[0], 5);
	if (is_vatek_success(res))
	{
		chip_operation_wait();
		hali2c_cmd[0] = 0;
		res = chip_i2c_write(pi2c, &hali2c_cmd[0], 1);
		if (is_vatek_success(res))
			res = chip_i2c_read(pi2c, &hali2c_cmd[0], 4);
	}

	if (is_vatek_success(res))
		*val = hali2c_cmd[0] | (hali2c_cmd[1] << 8) | (hali2c_cmd[2] << 16) | (hali2c_cmd[3] << 24);
	return res;
}

vatek_result chip_i2c_send_write(Pstm32_i2c pi2c,uint8_t cmd,int32_t addr,uint32_t val)
{
	hali2c_cmd[0] = 0x00;
	hali2c_cmd[1] = (uint8_t)val;
	hali2c_cmd[2] = (uint8_t)(val >> 8);
	hali2c_cmd[3] = (uint8_t)(val >> 16);
	hali2c_cmd[4] = (uint8_t)(val >> 24);


	hali2c_cmd[5] = (uint8_t)addr;
	hali2c_cmd[6] = (uint8_t)(addr >> 8);
	hali2c_cmd[7] = (uint8_t)(addr >> 16);
	hali2c_cmd[8] = cmd;

	return chip_i2c_write(pi2c, &hali2c_cmd[0], 9);
}

vatek_result chip_i2c_write(Pstm32_i2c pi2c, uint8_t* pbuf, int32_t len)
{
	vatek_result res = pi2c->start(pi2c->hi2c, HAL_I2CCHIP_WRADDR,0);
	if (is_vatek_success(res))
	{
		res = pi2c->write(pi2c->hi2c, pbuf, len);
		if (is_vatek_success(res))res = pi2c->stop(pi2c->hi2c);
	}
	if (is_vatek_success(res))chip_operation_wait();
	return res;
}

vatek_result chip_i2c_read(Pstm32_i2c pi2c, uint8_t* pbuf, int32_t len)
{
	vatek_result res = pi2c->start(pi2c->hi2c, HAL_I2CCHIP_RDADDR,0);
	if (is_vatek_success(res))
	{
		res = pi2c->read(pi2c->hi2c, pbuf, len);
		if (is_vatek_success(res))res = pi2c->stop(pi2c->hi2c);
	}
	if (is_vatek_success(res))chip_operation_wait();
	return res;
}

vatek_result chip_write_buffer_word(Pstm32_i2c pi2c, int32_t addr, uint32_t val)
{
	hali2c_cmd[0] = 0x00;
	hali2c_cmd[4] = (uint8_t)val;
	hali2c_cmd[3] = (uint8_t)(val >> 8);
	hali2c_cmd[2] = (uint8_t)(val >> 16);
	hali2c_cmd[1] = (uint8_t)(val >> 24);


	hali2c_cmd[5] = (uint8_t)addr;
	hali2c_cmd[6] = (uint8_t)(addr >> 8);
	hali2c_cmd[7] = (uint8_t)(addr >> 16);
	hali2c_cmd[8] = HAL_I2CCMD_WRITE;

	return chip_i2c_write(pi2c, &hali2c_cmd[0], 9);
}

vatek_result chip_read_buffer_word(Pstm32_i2c pi2c, int32_t addr, uint32_t* val)
{
	vatek_result res = vatek_success;
	hali2c_cmd[0] = 0x04;					/* i2c_ip buffer position*/
	hali2c_cmd[1] = (uint8_t)addr;
	hali2c_cmd[2] = (uint8_t)(addr >> 8);
	hali2c_cmd[3] = (uint8_t)(addr >> 16);
	hali2c_cmd[4] = HAL_I2CCMD_READ;

	res = chip_i2c_write(pi2c, &hali2c_cmd[0], 5);
	if (is_vatek_success(res))
	{
		chip_operation_wait();
		hali2c_cmd[0] = 0;
		res = chip_i2c_write(pi2c, &hali2c_cmd[0], 1);
		if (is_vatek_success(res))
		{
			res = chip_i2c_read(pi2c, &hali2c_cmd[0], 4);
		}
	}

	if (is_vatek_success(res))
		*val = hali2c_cmd[3] | (hali2c_cmd[2] << 8) | (hali2c_cmd[1] << 16) | (hali2c_cmd[0] << 24);
	return res;
}

