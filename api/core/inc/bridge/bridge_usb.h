//----------------------------------------------------------------------------
//
// Vision Advance Technology - Software Development Kit
// Copyright (c) 2014-2022, Vision Advance Technology Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef _BRIDGE_USB_
#define _BRIDGE_USB_

#include <core/vatek_base.h>

#define USB_BRIDGE_VID_OLD          0x1934
#define USB_BRIDGE_VID				0x2C42
#define USB_BRIDGE_PID				0x0122

#define USB_CMD_TAG					0x80000000
#define IS_USB_CMD(a)				((a & USB_CMD_TAG) == USB_CMD_TAG)
	
#define USB_CMD_LOCK				(USB_CMD_TAG | 0x1000)
#define USB_CMD_UNLOCK				(USB_CMD_LOCK + 1)
#define USB_CMD_GETSTATUS			(USB_CMD_UNLOCK + 1)

typedef enum _bridge_device_status
{
    hid_status_idle = 100,
    hid_status_waitlock = 101,
    hid_status_locked = 102,
    hid_status_waitunlock = 103,
}bridge_device_status;

#define is_current_usb_hid_status(status)  (status >= hid_status_idle && status <= hid_status_waitunlock)

/* modulator chip command */
#define MODULATOR_CMD_TAG       0x02000000
#define IS_MODULATOR_CMD(a)     ((a & MODULATOR_CMD_TAG) == MODULATOR_CMD_TAG)

#define MODULATOR_CMD_OPEN      MODULATOR_CMD_TAG
#define MODULATOR_CMD_CLOSE     (MODULATOR_CMD_OPEN + 1)
#define MODULATOR_CMD_TRANSFER  (MODULATOR_CMD_CLOSE + 1)
#define MODULATOR_CMD_GETNUMS   (MODULATOR_CMD_TRANSFER + 1)

/* board bridge command (used base param)*/
#define BOARDBRIDGE_CMD_TAG		0x04000000
#define IS_BOARDBRIDGE_CMD(a)	((a & BOARDBRIDGE_CMD_TAG) == BOARDBRIDGE_CMD_TAG)


const static char hid_bridge_tag[4] =
{
    'v','a','-','1',
};

typedef struct _bridge_base_param
{
    uint32_t params[13];
}bridge_base_param, *Pbridge_base_param;

#define BRIDGE_PARAM_MAX_LEN	52


/* each param cost 52 bytes */
#define TURNKEY_I2C_MAX_BUFFER_LEN 36
typedef struct _bridge_i2c_param
{
    uint32_t handle;
    uint32_t address;
    uint32_t len;
    uint8_t buffer[TURNKEY_I2C_MAX_BUFFER_LEN];
}bridge_i2c_param, *Pbridge_i2c_param;


#define BRIDGE_MOD_BUFFER_LEN   32

#define FLAG_MOD_REGISTER       0x00000100
#define FLAG_MOD_WRITE          0x00000001
#define FLAG_MOD_CODE           0x01000000
#define FLAG_MOD_BRIDGE			0x00010000

#define MOD_RD_REG              (FLAG_MOD_REGISTER)
#define MOD_WR_REG              (FLAG_MOD_REGISTER | FLAG_MOD_WRITE)
#define MOD_RD_MEM              0
#define MOD_WR_MEM              FLAG_MOD_WRITE
#define MOD_WR_CODE             (FLAG_MOD_WRITE | FLAG_MOD_CODE)
#define MOD_RD_CODE             (FLAG_MOD_CODE)

#define MOD_DATA_IS_WRITE(a)    ((a & FLAG_MOD_WRITE) == FLAG_MOD_WRITE)
#define MOD_DATA_IS_REG(a)      ((a & FLAG_MOD_REGISTER) == FLAG_MOD_REGISTER)
#define MOD_DATA_IS_CODE(a)     ((a & FLAG_MOD_CODE) == FLAG_MOD_CODE)

typedef struct _bridge_mod_param
{
    uint32_t address;
    uint32_t len;
    int32_t index;
    uint32_t datatype;
    uint8_t buffer[BRIDGE_MOD_BUFFER_LEN];
}bridge_mod_param,*Pbridge_mod_param;

typedef struct _hid_bridge_cmd
{
    uint8_t tag[4];
    uint32_t cmd;
    uint32_t recv;
    union
    {
        uint8_t raw[BRIDGE_PARAM_MAX_LEN];
        bridge_base_param base;
        bridge_i2c_param i2c;
        bridge_mod_param mod;
    }param;
}hid_bridge_cmd, *Phid_bridge_cmd;

#define HALBRIDGE_ERR_NODEVICE		-9
#define HALBRIDGE_ERR_BUSY			-8
#define HALBRIDGE_ERR_TIMEOUT		-7
#define HALBRIDGE_ERR_HWFAIL		-6
#define HALBRIDGE_ERR_BADSTATUS		-5
#define HALBRIDGE_ERR_BUFOVERFLOW	-4
#define HALBRIDGE_ERR_BADPARAM		-3
#define HALBRIDGE_ERR_UNSUPPORT		-2
#define HALBRIDGE_ERR_UNKNOWN		-1
#define HALBRIDGE_ERR_SUCCESS		0

typedef struct _hid_bridge_result
{
    uint8_t tag[4];
    uint32_t cmd;
    int32_t result;
    union _bridge_result_data
    {
        uint8_t raw[BRIDGE_PARAM_MAX_LEN];
        bridge_i2c_param i2c;
        bridge_mod_param mod;
    }data;
}hid_bridge_result, *Phid_bridge_result;

#endif
