#include "internal/bridge_internal.h"
#include <driver/av/avsource_adv718x.h>
#include <driver/av/avsource_ep9351.h>
#include <driver/av/avsource_ep9555e.h>
#include <driver/av/avsource_h1.h>

#define _bsource(tag)	{#tag,bsource_##tag,tag##_check_support,tag##_open,tag##_config,tag##_get_status,tag##_set_output,tag##_close,},
static const bdriver_source bsource_tables[] = 
{
	_bsource(ep9351)
	_bsource(ep9555e)
	_bsource(h1)
	_bsource(adv718x)
	{NULL,0,NULL,NULL,NULL,NULL,NULL,},
};

extern void bparam_set_str(uint8_t* pdest,uint8_t* psur);
extern void bparam_enum_put(uint8_t* pbbuf, uint8_t* pdata, Pstruct_bridge_param pbparam);
extern void bparam_uint_put(uint8_t* pbbuf, uint8_t* pdata, Pstruct_bridge_param pbparam);
extern void bparam_raw_put(uint8_t* pbuf,uint8_t* pdata,Pstruct_bridge_param pbparam);

extern void bparam_enum_get(uint8_t* pbbuf, uint8_t* pdata, Pstruct_bridge_param pbparam);
extern void bparam_uint_get(uint8_t* pbbuf, uint8_t* pdata, Pstruct_bridge_param pbparam);
extern void bparam_raw_get(uint8_t* pbbuf,uint8_t* pdata,Pstruct_bridge_param pbparam);

uint32_t hal_bridge_get_uint32(uint32_t value)
{
	uint8_t* pbuf = (uint8_t*)&value;
	uint32_t val = (*pbuf++) << 24;
	val |= (*pbuf++) << 16;
	val |= (*pbuf++) << 8;
	val |= (*pbuf);
	return val;
}

uint32_t hal_bridge_get_result(vatek_result nres)
{
	typedef struct _err_table {vatek_result res;uint32_t errcode;}err_table;
    const err_table bridge_err_table[] = 
    {
        {vatek_badparam,(uint32_t)HALBRIDGE_ERR_BADPARAM,},
        {vatek_hwfail,(uint32_t)HALBRIDGE_ERR_HWFAIL,},
        {vatek_badstatus,(uint32_t)HALBRIDGE_ERR_BADSTATUS,},
        {vatek_unsupport,(uint32_t)HALBRIDGE_ERR_UNSUPPORT,},
    };
    
    #define ERR_TABLE_NUMS  (sizeof(bridge_err_table)/sizeof(err_table))
    
	if(is_vatek_success(nres))
		return hal_bridge_get_uint32(nres);
	else
	{
		int32_t i = 0;
		uint32_t err = (uint32_t)HALBRIDGE_ERR_UNKNOWN;
		for(i = 0;i < ERR_TABLE_NUMS;i++)
		{
			if(bridge_err_table[i].res == nres)
			{
				err = bridge_err_table[i].errcode;
				break;
			}
		}
		return hal_bridge_get_uint32(err);
	}
}

vatek_result map_bridge_to_buffer(uint8_t* bbuf, uint8_t* bdata, Pstruct_bridge_param params)
{
	int32_t i = 0;
	while (params[i].type != BPARAM_TYPE_NULL)
	{
		Pstruct_bridge_param pbstruct = &params[i];
		uint8_t* ptrbridge = &bbuf[pbstruct->b_offset];
		uint8_t* ptrbuffer = &bdata[pbstruct->s_offset];

		if (pbstruct->type == BPARAM_TYPE_STR)bparam_set_str(ptrbuffer, ptrbridge);
		else if (pbstruct->type == BPARAM_TYPE_ENUM)bparam_enum_get(ptrbridge, ptrbuffer, pbstruct);
		else if (pbstruct->type == BPARAM_TYPE_UINT)bparam_uint_get(ptrbridge, ptrbuffer, pbstruct);
        else if (pbstruct->type == BPARAM_TYPE_RAW)bparam_raw_get(ptrbridge,ptrbuffer,pbstruct);
		else return vatek_badparam;
		i++;
	}

	return vatek_success;
}

vatek_result map_buffer_to_bridge(uint8_t* bbuf, uint8_t* bdata, Pstruct_bridge_param params)
{
	int32_t i = 0;
	while (params[i].type != BPARAM_TYPE_NULL)
	{
		Pstruct_bridge_param pbstruct = &params[i];
		uint8_t* ptrbridge = &bbuf[pbstruct->b_offset];
		uint8_t* ptrbuffer = &bdata[pbstruct->s_offset];

		if (pbstruct->type == BPARAM_TYPE_STR)bparam_set_str(ptrbridge, ptrbuffer);
		else if (pbstruct->type == BPARAM_TYPE_ENUM)bparam_enum_put(ptrbridge, ptrbuffer, pbstruct);
		else if (pbstruct->type == BPARAM_TYPE_UINT)bparam_uint_put(ptrbridge, ptrbuffer, pbstruct);
        else if (pbstruct->type == BPARAM_TYPE_RAW)bparam_raw_put(ptrbridge,ptrbuffer,pbstruct);
		else return vatek_badparam;
        
        i++;
	}
	return vatek_success;
}

void bparam_set_str(uint8_t* pdest, uint8_t* psur)
{
	memcpy(pdest, psur, BPARAM_STR_LEN);
}

void bparam_raw_put(uint8_t* pbuf,uint8_t* pdata,Pstruct_bridge_param pbparam)
{
    memcpy(pbuf,pdata,pbparam->typelen);
}

void bparam_enum_put(uint8_t* pbbuf, uint8_t* pdata, Pstruct_bridge_param pbparam)
{
	int32_t val = 0;
	if (pbparam->typelen == 1)val = (int32_t)(*((int8_t*)pdata));
	else if (pbparam->typelen == 2)val = (int32_t)(*((int16_t*)pdata));
	else val = *(int32_t*)pdata;

	pbbuf[0] = (uint8_t)(val >> 24);
	pbbuf[1] = (uint8_t)(val >> 16);
	pbbuf[2] = (uint8_t)(val >> 8);
	pbbuf[3] = (uint8_t)(val);
}

void bparam_uint_put(uint8_t* pbbuf, uint8_t* pdata, Pstruct_bridge_param pbparam)
{
	uint32_t val = 0;
	if (pbparam->typelen == 1)val = (uint32_t)(*((uint8_t*)pdata));
	else if (pbparam->typelen == 2)val = (uint32_t)(*((uint16_t*)pdata));
	else val = *(uint32_t*)pdata;

	pbbuf[0] = (uint8_t)(val >> 24);
	pbbuf[1] = (uint8_t)(val >> 16);
	pbbuf[2] = (uint8_t)(val >> 8);
	pbbuf[3] = (uint8_t)(val);
}

void bparam_enum_get(uint8_t* pbbuf, uint8_t* pdata, Pstruct_bridge_param pbparam)
{
	int32_t val = (pbbuf[0] << 24) | (pbbuf[1] << 16) | (pbbuf[2] << 8) | pbbuf[3];

	if (pbparam->typelen == 1)*((int8_t*)pdata) = (int8_t)val;
	else if (pbparam->typelen == 2)*((int16_t*)pdata) = (int16_t)val;
	else *((int32_t*)pdata) = val;
}

void bparam_uint_get(uint8_t* pbbuf, uint8_t* pdata, Pstruct_bridge_param pbparam)
{
	uint32_t val = (pbbuf[0] << 24) | (pbbuf[1] << 16) | (pbbuf[2] << 8) | pbbuf[3];

	if (pbparam->typelen == 1)*((uint8_t*)pdata) = (uint8_t)val;
	else if (pbparam->typelen == 2)*((uint16_t*)pdata) = (uint16_t)val;
	else *((uint32_t*)pdata) = val;
}

void bparam_raw_get(uint8_t* pbbuf,uint8_t* pdata,Pstruct_bridge_param pbparam)
{
    memcpy(pdata,pbbuf,pbparam->typelen);
}

vatek_result bridge_source_get(Pbridge_handle phandle)
{
	vatek_result nres = vatek_memfail;
	int32_t i = 0;
	while(bsource_tables[i].name)
	{
		nres = bsource_tables[i].check_support();
		if(is_vatek_success(nres))
		{
			Pbsource_list pnew = (Pbsource_list)malloc(sizeof(bsource_list));
			nres = vatek_memfail;
			if(pnew)
			{
				memset(pnew,0,sizeof(bsource_list));
				nres = bsource_tables[i].open(&pnew->device.hsource);
				if(is_vatek_success(nres))
				{
#if 0
					while(1)
					{
						bridge_source bsource;
						nres = bsource_tables[i].get_status(pnew->device.hsource,&bsource);
					}
#endif

				}

				if(is_vatek_success(nres))
				{
					pnew->device.driver = (Pbdriver_source)&bsource_tables[i];
					if(!phandle->source_root)phandle->source_root = pnew;
					else phandle->source_last->next = pnew;
					phandle->source_last = pnew;
					phandle->source_count++;
				}else free(pnew);
			}
		}else if(nres == vatek_unsupport)nres = vatek_success;
		if(!is_vatek_success(nres))break;
		i++;
	}

	if(!is_vatek_success(nres))
		bridge_source_reset(phandle);
	return nres;
}

void bridge_source_reset(Pbridge_handle phandle)
{
	if(phandle->source_root)
	{
		Pbsource_list ptr = phandle->source_root;
		while(ptr)
		{
			Pbsource_list pnext = ptr->next;
			ptr->device.driver->close(ptr->device.hsource);
			free(ptr);
			ptr = pnext;
		}
	}

	phandle->source_count = 0;
	phandle->source_root = NULL;
	phandle->source_last = NULL;
}

vatek_result bstorage_start(Pbridge_handle phandle,Pbflash_command pfcmd)
{
	vatek_result nres = vatek_badstatus;
	if(!phandle->storage.section)
	{
		if(pfcmd->bcmd == BFLASH_CNTL_ERASE)
			nres = hal_storage_erase(pfcmd->nsection);
		else if(pfcmd->bcmd == BFLASH_CNTL_READ)
		{
			nres = hal_storage_lock_section(&phandle->storage.section);
			if(is_vatek_success(nres))
			{
				phandle->storage.pos = 0;
				nres = hal_storage_read(pfcmd->nsection,phandle->storage.section);
				if(!is_vatek_success(nres))hal_storage_unlock_section();
			}
		}
		else if(pfcmd->bcmd == BFLASH_CNTL_WRITE)
		{
			nres = hal_storage_lock_section(&phandle->storage.section);
			if(is_vatek_success(nres))
			{
				phandle->storage.pos = 0;
				memset(phandle->storage.section,0,BRIDGE_SECTION_LEN);
			}
		}else nres = vatek_unsupport;
	}
	return nres;
}

vatek_result bstorage_put(Pbridge_handle phandle,uint8_t* pbuf,int32_t len)
{
	vatek_result nres = vatek_badstatus;
	Pbstorage_handle pstorage = &phandle->storage;
	if(pstorage->section && pstorage->cmd.bcmd == BFLASH_CNTL_WRITE)
	{
		uint8_t* ptr = &pstorage->section[pstorage->pos];
		memcpy(ptr,pbuf,len);
		pstorage->pos += len;
		nres = (vatek_result)pstorage->pos;
	}
	return nres;
}

vatek_result bstorage_get(Pbridge_handle phandle,uint8_t** pbuf,int32_t len)
{
	vatek_result nres = vatek_badstatus;
	Pbstorage_handle pstorage = &phandle->storage;
	if(pstorage->section && pstorage->cmd.bcmd == BFLASH_CNTL_READ)
	{
		nres = (vatek_result)pstorage->pos;
		*pbuf = &pstorage->section[pstorage->pos];
		pstorage->pos += len;
	}
	return nres;	
}

vatek_result bstorage_stop(Pbridge_handle phandle)
{
	vatek_result nres = vatek_badstatus;
	Pbstorage_handle pstorage = &phandle->storage;
	if(pstorage->section)
	{
		if(pstorage->cmd.bcmd == BFLASH_CNTL_WRITE)
		{
			if(pstorage->pos == BRIDGE_SECTION_LEN)
			{
				nres = hal_storage_erase(pstorage->cmd.nsection);
				if(is_vatek_success(nres))
					nres = hal_storage_write(pstorage->cmd.nsection,pstorage->section);
			}
		}else nres = vatek_success;
	}
	bstorage_reset(phandle);
	return nres;
}

void bstorage_reset(Pbridge_handle phandle)
{
	Pbstorage_handle pstorage = &phandle->storage;
	if(pstorage->section)
		hal_storage_unlock_section();
	memset(pstorage,0,sizeof(bstorage_handle));
}
