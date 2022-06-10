#include <tsMain.h>
#include <vatek_sdk_device.h>
#include <core/ui/ui_props_api.h>
#include <core/ui/ui_props/ui_props_chip.h>

TS_MAIN(MainCode);

namespace {
	class vatekOptions : public ts::Args
	{
		TS_NOBUILD_NOCOPY(vatekOptions);
	public:
		vatekOptions(int argc, char* argv[]);
		~vatekOptions();

		int32_t device;
	};
}

int MainCode(int argc, char* argv[])
{
	vatekOptions opts(argc, argv);
	hvatek_devices hdevices = NULL;
	vatek_result nres = vatek_device_list_enum(DEVICE_BUS_USB, service_transform, &hdevices);
	if (nres > vatek_success)
	{
		if (opts.device != -1)
		{
			if (opts.device >= nres)printf("Bad deice index only %d devices in system", nres);
			else
			{
				hvatek_chip hchip = NULL;
				nres = vatek_device_open(hdevices, opts.device,&hchip);
				if (is_vatek_success(nres))
				{
					printf("Device %2d : %s\r\n", opts.device, vatek_device_get_name(hchip));
					ui_props_printf(" - [%-20s] : %-8s - %s\r\n",NULL, _ui_struct(chip_info), (uint8_t*)vatek_device_get_info(hchip));
				}
			}
		}
		else
		{
			int32_t i = 0;
			printf("Found VATEK Devices : [%d]\r\n", nres);
			for (i = 0; i < nres; i++)
			{
				printf(" - DEVICE %2d - [%s]\r\n", i, vatek_device_list_get_name(hdevices, i));
			}
		}
	}
	else if (nres == vatek_success)printf("No VATEK device found\r\n");
	else printf("Enumeration VATEK device fail : [%d]\r\n", nres);

	if(is_vatek_success(nres))return EXIT_SUCCESS;
	return EXIT_FAILURE;
}

vatekOptions::vatekOptions(int argc, char* argv[]) :
	Args(u"VATEK modulation devices control"),
	device(-1)
{
	option(u"device", 'd', INT32);
	help(u"device", u"Specify the VATEK index number to list. Show detail device information");

	analyze(argc, argv);

	if (present(u"device"))
		device = intValue(u"device", -1);
}

vatekOptions::~vatekOptions()
{

}
