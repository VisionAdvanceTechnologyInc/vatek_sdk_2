# Vision Advance Technology - Software Development Kit

### Abstract
VATek devotes to design and manufacture the best-of-breed chips that aim at DTV modulation and multimedia encode & streaming technology. 

VAT SDK is the software to control the chip made by VATek.
- VATek provides the sample code of SDK to support users understand the SDK source code. SDK development depends on OS, and it can be developed on Windows 10(x64) and LINUX OS.

### Modulator Series
Multi-format DTV modulator chip which is simple and cost efficient than other solutions. An advanced engine to simplify complexity of TV headend and customized TV distribution system.
-	Modulator supports full modulation standards including DVB-T2 (subset of T2-base 1.3.1.)
-	REMUX Engine for PSI insertion.
-	Stream buffer regulates TS data flow, allow to integrate all types of TS source.
-	Flexible media input interface design, including TS and an USB port.

Some application case examples such as, 
-	Converted IPTV stream to Coax extension broadcasting after scheduling multiplex.
-	Software connects with security platform, broadcasting community security system over DTVs.
-	Combined with streaming or multimedia playing software, be used as advertising host, Director, smart headend equipment, and other media consoles.
-	USB modulator dongle as a portable, simple and reasonably priced multimedia broadcast product.

### Enmoder Series
A multi-standard encoder modulator chip for DTV with ultra-low media latency, simplifying complexity of TV headend and customized TV distribution system.
-	Flexible media input interfaces which allow any HDMI source.
-	Built in MPEG2 and H.264 real time encoder with modulator supporting all modulation standards.
-	Customization Colorbar and Bootlogo function, supporting PSI and CC function.
-	Cost efficiency, low power consumption, and low latency with high reliability. 

The Enmoder application is equipped with HDMI or a set of AV inputs and a RF output. The source signals may come from various devices such as USB, video Githuplayers, laptops, and monitoring devices; therefore, the output signal can be inserted into the antenna system as an additional TV channel.

As the Encmoder with both HDMI and USB 2.0 inputs, it provides outstanding solutions for hotels, hospitals, campus, and other closed system in which contents from various sources needs to be delivered to multiple televisions.


### Building & Usage
- The SDK was originally compiled in the way of CMAKE to obtain the example execution file. Vatek provides instructions for CMAKE compilation, so that users can get started quickly. Users can also compile on their own with a familiar compilation platform and read the file to get more relevant operation.
- See more detail in [SDK document](https://www.vatek.com.tw/static/files/VATEK%20%20Development%20Guideline.pdf)

	#### Windows

	- Download vatek_sdk_2. You can choose to add a new folder under vatek_sdk_2 to save CMake building files (take bin folder as example).
	- Use CMake application, select vatek_sdk_2 folder in the “source code” section, select bin folder under vatek_sdk_2 folder in the “build the binaries” section.（You can choose to use QT interface or not. If not, please uncheck sdk2_ EN_ QT.）
	- In the bin folder, there are VS project file (vatek_sdk_2.sln), execute and compile.
	- The executables and binaries (.dll) are built in directories bin\Release or bin\Debug.

	#### Linux 

	- Being tested based on Ubuntu.
	- Execute the shell-script build/Linux_build_installer.sh It downloads and installs Libusb and Cmake which are necessary to build VATek SDK.
	- The sample executables are built in directories bin/bin.
	- The shared objects (libvatek.so) are built in directories bin/lib.

### Download

- On Windows：(Windows 10) provides [binary](https://www.vatek.com.tw/static/files/VATek-Win64-3.06.exe) and [open source](https://github.com/VisionAdvanceTechnologyInc/vatek_sdk_2/archive/refs/tags/v3.06.zip).
- On Linux： (Ubuntu) provides [binary](https://www.vatek.com.tw/static/files/VATek-Linux-x86_64-3.06.tgz) and [open source](https://github.com/VisionAdvanceTechnologyInc/vatek_sdk_2/archive/refs/tags/v3.06.tar.gz). 
- On macOS： [Homebrew packager] Use `brew install libvatek`.

### License

Vision Advance Technology - Software Development Kit is distributed under the terms of the Simplified BSD License.
See the file `LICENSE` for details.

*Copyright (c) 2014-2022, Vision Advance Technology Inc.*<br/>
*All rights reserved*
