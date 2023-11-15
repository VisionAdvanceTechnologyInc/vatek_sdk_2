#include <vatek_bridge_2.h>

static const char* bridge_logo[] = 
{
    " ______ ______ _______ _____  _______ _______        ______ \r\n",
    "|   __ \\   __ \\_     _|     \\|     __|    ___|______|__    |\r\n",
    "|   __ <      <_|   |_|  --  |    |  |    ___|______|    __|\r\n",
    "|______/___|__|_______|_____/|_______|_______|      |______|\r\n",
    "\r\n",
    "  - power by vatek technology inc.\r\n",
    NULL,
};

#include <core/base/chip_define.h>

#define _BRIDGE_WAITCHIP_TIMEOUT		5000

#define BRIDGE_VERSION					20230606

#define _pl_title(fmt,...)				printf(" [%08x] - "fmt"\r\n",hal_system_get_tick(),##__VA_ARGS__)
#define _pl_uint32(t,v)					printf("%-11s - [%-16s] : 0x%08x\r\n","",#t,v)
#define _pl_sub(fmt,...)				printf("%-11s - " fmt "\r\n","",##__VA_ARGS__)
#define _pl_spiltline()					printf("---------------------------------------------------------------------------------------------\r\n")
#define _pl_uint8(t,v)					printf("%-11s - [%-16s] : %s\r\n","",#t,v)
#define _pl_int(t,v)					printf("%-11s - [%-16s] : [%d]\r\n","",#t,v)

extern void printf_logo(void);
extern void printf_bsource(hvatek_bridge hbridge);

extern vatek_result connect_chip(hvatek_bridge hbridge,hvatek_chip* hchip,chip_info* info);
extern vatek_result print_chip(Pchip_info pinfo);
int main(void)
{
	uint32_t tick_start,tick_stop;
	hvatek_bridge hbridge = NULL;
	hvatek_chip hchip = NULL;
	chip_info info;
	vatek_result nres = vatek_bridge_open(&hbridge);
	printf_logo();


	if(is_vatek_success(nres))
	{
		nres = connect_chip(hbridge,&hchip, &info);
		if(is_vatek_success(nres))
			printf_bsource(hbridge);
		if(!is_vatek_success(nres))vatek_bridge_close(hbridge);
	}
	
	if(is_vatek_success(nres))
	{
		nres = vatek_bridge_start(hbridge);
		if(is_vatek_success(nres))
		{
//			tick_start = vatek_get_tick_ms();
//			printf("start time = 0x%08x\r\n",tick_start);

			for(;;)
			{
				nres = vatek_bridge_polling(hbridge, info);
				if(!is_vatek_success(nres)){
					tick_stop = vatek_get_tick_ms();
					hal_system_sleep(100);
					printf("bridge polling fail during time = 0x%08x\r\n",(tick_stop-tick_start));
					break;
				}
			}
//			tick_stop = vatek_get_tick_ms();
//			printf("during time = 0x%08x\r\n",(tick_stop-tick_start));
			vatek_bridge_stop(hbridge);
		}

		if(hchip)vatek_chip_close(hchip);
//		tick_stop = vatek_get_tick_ms();
//		printf("during time = 0x%08x\r\n",(tick_stop-tick_start));
		vatek_bridge_close(hbridge);
	}
	_pl_title("bridge-2 terminal : %d",nres);
	_pl_spiltline();
	return 0;
}

vatek_result connect_chip(hvatek_bridge hbridge,hvatek_chip* hchip,chip_info* info)
{
	vatek_result nres = vatek_chip_open(hbridge,hchip);
	if(is_vatek_success(nres))
	{
		uint32_t tick = hal_system_get_tick();
		chip_status status = chip_status_fail_service;
		while((hal_system_get_tick() - tick) < _BRIDGE_WAITCHIP_TIMEOUT)
		{
			status = chip_status_get(*hchip,NULL);
			if(status == chip_status_waitcmd)break;
		}
			
		if(status == chip_status_waitcmd)
		{

			nres = chip_info_get(*hchip,info);
			if(is_vatek_success(nres))
			{
				_pl_title("chip and service connected");
				_pl_int(bridge_version,BRIDGE_VERSION);
				print_chip(info);
				//_pl_uint32(chip,info->chip_module);
				_pl_uint32(service,info->hal_service);
				_pl_uint32(version,info->version);
				_pl_uint32(input,info->input_support);
				_pl_uint32(output,info->output_support);
				_pl_uint32(peripheral,info->peripheral_en);
				_pl_spiltline();
			}else _pl_title("read chip information fail : %d",nres);
		}else _pl_title("connect chip but service not found");
	}else _pl_title("can not found chip");
	return nres;
}

void printf_logo()
{
	int32_t i = 0;
	puts("\033[2J");   // clear screen
	puts("\033[0;0H"); // set cursor to 0,0
	puts("\033[3J");
	while(bridge_logo[i])
	{
		printf("%s",bridge_logo[i]);
		i++;
	}
	_pl_spiltline();
}

void printf_bsource(hvatek_bridge hbridge)
{
	bpointer bptr = NULL;
	vatek_source_get_first(hbridge,&bptr);
	if(!bptr)
	{
	
	}else
	{
		while(bptr)
		{
			Pbdevice_source pdevice = vatek_source_get_next(&bptr);
			_pl_sub("bsource [%s: %08x]",pdevice->driver->name,pdevice->driver->id);
			if (strcmp(pdevice->driver->name, "h1") == 0){
				_pl_sub("H1 Version [%08x]",h1_version());
			}
		}
	}
	_pl_spiltline();
}


vatek_result print_chip(Pchip_info pinfo){
    if (pinfo->chip_module == ic_module_a1) printf("%-11s - [%-16s] : %s\r\n","","chip_module","A1");
    else if (pinfo->chip_module == ic_module_b1) printf("%-11s - [%-16s] : %s\r\n","","chip_module","B1");
    else if (pinfo->chip_module == ic_module_b2) printf("%-11s - [%-16s] : %s\r\n","","chip_module","B2");
    else if (pinfo->chip_module == ic_module_b3_lite) printf("%-11s - [%-16s] : %s\r\n","","chip_module","B3");
    else if (pinfo->chip_module == ic_module_b3_plus) printf("%-11s - [%-16s] : %s\r\n","","chip_module","B3 Plus");
    else if (pinfo->chip_module == ic_module_b2_plus) printf("%-11s - [%-16s] : %s\r\n","","chip_module","B2 Plus");
    else if (pinfo->chip_module == ic_module_e1) printf("%-11s - [%-16s] : %s\r\n","","chip_module","E1");
}
