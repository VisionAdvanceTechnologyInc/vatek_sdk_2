#! /bin/sh

OS=`uname -s`
if [ ${OS} == "Darwin"  ];then
    echo "Other OS: Darwin"
elif [ ${OS} == "Linux"  ];then
    sudo apt update
    sudo apt install build-essential
    sudo apt install cmake
    sudo apt-get install libusb-1.0-0-dev
    sudo cp ./linux/udev/50-usb-vatek.rules /etc/udev/rules.d/
    cd ..
    mkdir bin
    cd bin
    cmake ..
    make all -j8 install
    exit 0
else
    echo "Other OS: ${OS}"
fi
