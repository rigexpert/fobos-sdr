# RigExpert Fobos SDR API

This is the Fobos SDR receiver host software API library and basic standalone test application. Extremely lightweight and easy to start. Full source code.

## Platforms tested on

- Linux (Ubuntu 18.04 LTS, Ubuntu 22.04 LTS, Raspbian ...)
- Windows (7, 8.1, 10, 11)

## Requirements

- git v.2.31 or later (otherwise download the repository manualy: Code->Download ZIP)
- cmake v.3.10 or later (otherwise create a project manualy: makefile, *.sln, etc)
- any c compiler (tested on gcc, g++, mingw, msvc) 

## Dependencies

- libusb-1.0-0 v.1.0.21 or higher

## How to build and evaluate

### Linux

git clone [this repo]<br />
cd fobos-sdr<br />
sudo cp fobos-sdr.rules /etc/udev/rules.d/00-fobos-sdr.rules<br />
sudo udevadm control --reload-rules<br />
sudo udevadm trigger<br />
mkdir build<br />
cd build<br />
cmake ..<br />
make<br />
./fobos_sdr<br />

### Windows

git clone [this repo]<br />
cd fobos-sdr<br />
mkdir build<br />
mkdir libusb<br />
cd build<br />
cmake ..<br />

Visit https://github.com/libusb/libusb/releases<br />
Download any libusb release 7z pack, for example  libusb-1.0.27.7z<br />
Unpack content of **libusb-1.0.27.7z** to **libusb** directory<br />

cmake build .<br />
or<br />
open fobos_sdr.sln in your favorite MS VisualStudio IDE,<br />
build, debug, trace, evaluate.<br />

## How it looks like (Ubuntu 22.04 LTS)

<img src="./showimg/Screenshot001.png" scale="100%"/><br />

## How to use Fobos SDR API elsewhere

- copy **fobos.c** and **fobos.h** from  **fobos** subdirectory to your **c/c++** project location
- link **fobos.c**  and include **fobos.h** to your project
- see **recorder/main.c** and make your own similar 
- build, run and have a fun

## What is actually Fobos SDR

For more info visit the main product page

https://rigexpert.com/en/products/kits-en/fobos-sdr/
