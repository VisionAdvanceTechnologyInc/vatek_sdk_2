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

#include <core/base/calibration_define.h>
#include <core/ui/ui_props/ui_props_output.h>

#define _calibration_dac_val(p)			((((uint8_t)p->dac.ioffect) << 24) | (((uint8_t)p->dac.qoffect) << 16) | (((uint8_t)p->dac.igain) << 8) | ((uint8_t)p->dac.qgain))
#define _calibration_dac_ioffect(v)		((int8_t)((uint8_t)(v >> 24)))
#define _calibration_dac_qoffect(v)		((int8_t)((uint8_t)(v >> 16)))
#define _calibration_dac_igain(v)		((int8_t)((uint8_t)(v >> 8)))
#define _calibration_dac_qgain(v)		((int8_t)((uint8_t)(v)))

vatek_result calibration_set(hvatek_chip hchip, Pcalibration_param pcalibration, int32_t isapply)
{
	uint32_t val = _calibration_dac_val(pcalibration);
	vatek_result nres = vatek_chip_write_memory(hchip, HALREG_CALIBRATION_DAC, val);
	if (is_vatek_success(nres))
		nres = vatek_chip_write_memory(hchip, HALREG_CALIBRATION_CLOCK, pcalibration->clock);
	if (is_vatek_success(nres))
		nres = ui_props_write_hal(hchip, _ui_struct(r2_tune_calibration0), (uint8_t*)&pcalibration->r2);

	if (is_vatek_success(nres))
	{
		val = CALIBRATION_EN_TAG | (isapply != 0);
		nres = vatek_chip_write_memory(hchip, HALREG_CALIBRATION_CNTL, val);
	}
	return nres;
}

vatek_result calibration_adjust_gain(hvatek_chip hchip, int8_t gain, Pcalibration_param m_calibration) {
	calibration_param calibration;
	memset(&calibration, 0, sizeof(Pcalibration_param));
	calibration.clock = m_calibration->clock;


	calibration.r2.ioffset = m_calibration->r2.ioffset;
	calibration.r2.qoffset = m_calibration->r2.qoffset;
	calibration.r2.imgoffset = m_calibration->r2.imgoffset;
	calibration.r2.phaseoffset = m_calibration->r2.phaseoffset;

	calibration.dac.ioffect = m_calibration->dac.ioffect;
	calibration.dac.qoffect = m_calibration->dac.qoffect;

	if ((m_calibration->dac.qgain == 0) && (m_calibration->dac.igain == 0)) {
		calibration.dac.igain = gain;
		calibration.dac.qgain = gain;
	}
	else {
		calibration.dac.igain = m_calibration->dac.igain;
		calibration.dac.qgain = m_calibration->dac.qgain;
	}
	return calibration_set(hchip, &calibration, 1);
}

vatek_result calibration_check(hvatek_chip hchip)
{
	uint32_t val = 0;
	vatek_result nres = vatek_chip_read_memory(hchip, HALREG_CALIBRATION_CNTL, &val);
	if (is_vatek_success(nres))
	{
		if ((val & 0xFFFFFF00) != CALIBRATION_EN_TAG)
			nres = vatek_unsupport;
	}
	return nres;
}

vatek_result calibration_get(hvatek_chip hchip, Pcalibration_param pcalibration)
{
	vatek_result nres = calibration_check(hchip);
	if (is_vatek_success(nres))
	{
		uint32_t val = 0;
		nres = vatek_chip_read_memory(hchip, HALREG_CALIBRATION_CLOCK, &val);

		if (is_vatek_success(nres))
		{
			pcalibration->clock = (int32_t)val;
			nres = vatek_chip_read_memory(hchip, HALREG_CALIBRATION_DAC, &val);
			if (is_vatek_success(nres))
			{
				pcalibration->dac.ioffect = _calibration_dac_ioffect(val);
				pcalibration->dac.qoffect = _calibration_dac_qoffect(val);
				pcalibration->dac.igain = _calibration_dac_igain(val);
				pcalibration->dac.qgain = _calibration_dac_qgain(val);
				nres = ui_props_read_hal(hchip, _ui_struct(r2_tune_calibration0), (uint8_t*)&pcalibration->r2);
				
				// read PA_gain (write PA_GAIN TAG) 
				nres = vatek_chip_write_memory(hchip, HALREG_EXT_R2_GAIN, EXT_R2_GAIN_EN_READ);

			}
		}
	}
	return nres;
}
