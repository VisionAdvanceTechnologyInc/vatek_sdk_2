#include <cmds/cmd_mod.h>
#include <hal/hal_bridge_board.h>
#include "internal/bridge_internal.h"

extern vatek_result bridge_i2c_read(uint8_t dev,uint8_t* pbuf,uint32_t len);
extern vatek_result bridge_i2c_write(uint8_t dev, uint8_t* pbuf, uint32_t len);
extern vatek_result bridge_bulk_cmd(uint32_t type, uint32_t addr, uint8_t* pbuf, uint32_t len);

#define BRIDGE_MOD_GETBUF(cmd)              &cmd->param.mod.buffer[0]
#define BRIDGE_MOD_GETIDX(cmd)              hal_bridge_get_uint32(cmd->param.mod.index)
#define BRIDGE_MOD_GETADDR(cmd)             hal_bridge_get_uint32(cmd->param.mod.address)
#define BRIDGE_MOD_GETLEN(cmd)              hal_bridge_get_uint32(cmd->param.mod.len)
#define BRIDGE_MOD_GETDATATYPE(cmd)         hal_bridge_get_uint32(cmd->param.mod.datatype)

vatek_result cmd_mod(hvatek_bridge hbridge, Phid_bridge_cmd pcmd, Phid_bridge_result pres)
{
	vatek_result nres = vatek_success;
	Pbridge_mod_param modparam = &pcmd->param.mod;
	uint32_t usbcmd = hal_bridge_get_uint32(pcmd->cmd);
	uint32_t modidx = BRIDGE_MOD_GETIDX(pcmd);
	uint32_t modaddr = BRIDGE_MOD_GETADDR(pcmd);
	uint32_t modlen = BRIDGE_MOD_GETLEN(pcmd);
	uint32_t modtype = BRIDGE_MOD_GETDATATYPE(pcmd);

	if (usbcmd == MODULATOR_CMD_GETNUMS)pres->result = hal_bridge_get_uint32(1);
	else 
	{
		if (modidx == 0)
		{
			if (usbcmd == MODULATOR_CMD_OPEN)pres->result = hal_bridge_get_uint32(HALBRIDGE_ERR_SUCCESS);
			else if (usbcmd == MODULATOR_CMD_CLOSE)pres->result = hal_bridge_get_uint32(HALBRIDGE_ERR_SUCCESS);
			else if (usbcmd == MODULATOR_CMD_TRANSFER)
			{
				if (modlen <= BRIDGE_MOD_BUFFER_LEN / 4)
				{
					nres = bridge_bulk_cmd(modtype, modaddr, &modparam->buffer[0], modlen);
					if (is_vatek_success(nres))
                    {
                        if(!MOD_DATA_IS_WRITE(modtype))memcpy(&pres->data.mod, modparam, sizeof(bridge_mod_param));
                        pres->result = hal_bridge_get_uint32((uint32_t)HALBRIDGE_ERR_SUCCESS);
                    }
					else pres->result = hal_bridge_get_uint32((uint32_t)HALBRIDGE_ERR_HWFAIL);
				}
			}
		}
		else pres->result = hal_bridge_get_uint32((uint32_t)HALBRIDGE_ERR_BADPARAM);
	}
	return nres;
}

vatek_result bridge_i2c_read(uint8_t dev, uint8_t* pbuf, uint32_t len)
{
	vatek_result res = hal_i2c_start(dev,0);
	if (is_vatek_success(res))
	{
		res = hal_i2c_read(pbuf, (int32_t)len);
		if (is_vatek_success(res))res = hal_i2c_stop();
	}

	if (!is_vatek_success(res))return vatek_hwfail;
	return vatek_success;
}

vatek_result bridge_i2c_write(uint8_t dev, uint8_t* pbuf, uint32_t len)
{
	vatek_result res = hal_i2c_start(dev,0);
	if (is_vatek_success(res))
	{
		res = hal_i2c_write(pbuf, (int32_t)len);
		if (is_vatek_success(res))res = hal_i2c_stop();
	}

	if (!is_vatek_success(res))return vatek_hwfail;
	else return vatek_success;
}

#define TURNKEY_MODULATOR_BUFFER_LEN    12
#define HAL_REG_MC_I2C_MEM              0x1482

#define I2C_DEVADDR_WRITE               0x20
#define I2C_DEVADDR_READ                0x21
#define I2C_REG_READ_TAG 	            0x04
#define I2C_REG_WRITE_TAG 	            0x06
#define I2C_MEM_READ_TAG                0x05
#define I2C_MEM_WRITE_TAG	            0x07

typedef vatek_result(*fpinternal_operation)(uint8_t cmd, uint32_t addr, uint8_t* value);
extern vatek_result internal_i2c_read(uint8_t cmd, uint32_t addr, uint8_t* value);
extern vatek_result internal_i2c_write(uint8_t cmd, uint32_t addr, uint8_t* value);
extern vatek_result internal_i2c_code_read(uint8_t cmd, uint32_t addr, uint8_t* value);
extern vatek_result internal_i2c_code_write(uint8_t cmd, uint32_t addr, uint8_t* value);

vatek_result bridge_bulk_cmd(uint32_t type, uint32_t addr, uint8_t* pbuf, uint32_t len)
{
	uint8_t cmd = I2C_MEM_WRITE_TAG;
	uint8_t* wbuf = (uint8_t*)pbuf;
	uint32_t waddr = addr;
	int32_t wlen = len;
	vatek_result nres = vatek_success;
	fpinternal_operation bulktransfer = NULL;

    if (MOD_DATA_IS_WRITE(type))
    {
        bulktransfer = internal_i2c_write;
        cmd = I2C_MEM_WRITE_TAG;
        if (MOD_DATA_IS_REG(type))cmd = I2C_REG_WRITE_TAG;
        else if (MOD_DATA_IS_CODE(type))bulktransfer = internal_i2c_code_write;
    }
    else 
    {
        cmd = I2C_MEM_READ_TAG;
        bulktransfer = internal_i2c_read;
        if (MOD_DATA_IS_REG(type))cmd = I2C_REG_READ_TAG;
        else if (MOD_DATA_IS_CODE(type))bulktransfer = internal_i2c_code_read;
    }

    while (wlen)
    {
        nres = bulktransfer(cmd, waddr, wbuf);
        if (!is_vatek_success(nres))break;
        wlen--;
        waddr++;
        wbuf += 4;
    }
	return nres;
}

vatek_result internal_i2c_read(uint8_t cmd, uint32_t addr, uint8_t* value)
{
	vatek_result nres = vatek_hwfail;
	uint8_t modulator_buffer[TURNKEY_MODULATOR_BUFFER_LEN];
	modulator_buffer[0] = 0x04;
	modulator_buffer[1] = (uint8_t)(addr & 0xFF);
	modulator_buffer[2] = (uint8_t)((addr >> 8) & 0xFF);
	modulator_buffer[3] = (uint8_t)((addr >> 16) & 0xFF);
	modulator_buffer[4] = cmd;

	nres = bridge_i2c_write(I2C_DEVADDR_WRITE, &modulator_buffer[0], 5);
	if (is_vatek_success(nres))
	{
		modulator_buffer[0] = 0;
		nres = bridge_i2c_write(I2C_DEVADDR_WRITE, &modulator_buffer[0], 1);
		if (is_vatek_success(nres))nres = bridge_i2c_read(I2C_DEVADDR_READ | 1, &value[0], 4);
	}

	return nres;
}

vatek_result internal_i2c_write(uint8_t cmd, uint32_t addr, uint8_t* value)
{
	vatek_result nres = vatek_hwfail;
	uint8_t modulator_buffer[TURNKEY_MODULATOR_BUFFER_LEN];

	modulator_buffer[0] = 0x00;
	modulator_buffer[1] = *value++;
	modulator_buffer[2] = *value++;
	modulator_buffer[3] = *value++;
	modulator_buffer[4] = *value;


	modulator_buffer[5] = (uint8_t)(addr & 0xFF);
	modulator_buffer[6] = (uint8_t)((addr >> 8) & 0xFF);
	modulator_buffer[7] = (uint8_t)((addr >> 16) & 0xFF);
	modulator_buffer[8] = cmd;

	nres = bridge_i2c_write(I2C_DEVADDR_WRITE, &modulator_buffer[0], 9);
	return nres;
}

vatek_result internal_i2c_code_read(uint8_t cmd, uint32_t addr, uint8_t* value)
{
	vatek_result nres = vatek_hwfail;
	uint8_t modulator_buffer[TURNKEY_MODULATOR_BUFFER_LEN];

	modulator_buffer[0] = 0x04;
	modulator_buffer[1] = (uint8_t)(addr & 0xFF);
	modulator_buffer[2] = (uint8_t)((addr >> 8) & 0xFF);
	modulator_buffer[3] = (uint8_t)((addr >> 16) & 0xFF);
	modulator_buffer[4] = cmd;

	nres = bridge_i2c_write(I2C_DEVADDR_WRITE, &modulator_buffer[0], 5);
	if (is_vatek_success(nres))
	{
		modulator_buffer[0] = 0;
		nres = bridge_i2c_write(I2C_DEVADDR_WRITE, &modulator_buffer[0], 1);
		if (is_vatek_success(nres))
		{
			uint8_t bufval[4];
			nres = bridge_i2c_read(I2C_DEVADDR_READ, &bufval[0], 4);

			if (is_vatek_success(nres))
			{
				value[0] = bufval[3];
				value[1] = bufval[2];
				value[2] = bufval[1];
				value[3] = bufval[0];
			}
		}
	}

	return nres;
}

vatek_result internal_i2c_code_write(uint8_t cmd, uint32_t addr, uint8_t* value)
{
	vatek_result nres = vatek_success;
	uint8_t modulator_buffer[TURNKEY_MODULATOR_BUFFER_LEN];

	modulator_buffer[0] = 0x00;
	modulator_buffer[4] = *value++;
	modulator_buffer[3] = *value++;
	modulator_buffer[2] = *value++;
	modulator_buffer[1] = *value;


	modulator_buffer[5] = (uint8_t)(addr & 0xFF);
	modulator_buffer[6] = (uint8_t)((addr >> 8) & 0xFF);
	modulator_buffer[7] = (uint8_t)((addr >> 16) & 0xFF);
	modulator_buffer[8] = cmd;

	nres = bridge_i2c_write(I2C_DEVADDR_WRITE, &modulator_buffer[0], 9);
	return nres;
}
