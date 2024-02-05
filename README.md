# Open Source Firmware for Quansheng UV-K* Radios
An attempt to port unify as many of the many other fork's features as possible

[![Compile with Docker CI](https://github.com/qshosfw/matoz/actions/workflows/cwd.yml/badge.svg)](https://github.com/qshosfw/matoz/actions/workflows/cwd.yml) 
![GitHub License](https://img.shields.io/github/license/qshosfw/matoz) 
![GitHub issues](https://img.shields.io/github/issues/qshosfw/matoz)  


> [!WARNING]  
> Use this firmware at your own risk (entirely). There is absolutely no guarantee that it will work in any way shape or form on your radio(s), it may even brick your radio(s), in which case, you'd need to buy another radio.
Anyway, have fun.

For improved/better firmware and new features, you can find the following repositories by other collaborators:

* https://github.com/fagci/uv-k5-firmware-fagci-mod
* https://github.com/OneOfEleven/uv-k5-firmware-custom
* https://github.com/Tunas1337/uv-k5-firmware (Check the branches)
* https://github.com/rebezhir/openquack for Russian users

## Credits

Many thanks to various people on Telegram for putting up with me during this effort and helping:

* [OneOfEleven](https://github.com/OneOfEleven)
* [DualTachyon](https://github.com/DualTachyon)
* [Joaquim](https://github.com/joaquimorg)
* [Mikhail](https://github.com/fagci)
* [kamilsss655](https://github.com/kamilsss655)
* [Andrej](https://github.com/Tunas1337)
* [Manuel](https://github.com/manujedi)
* [Adam Mnemonic](https://github.com/amnemonic)
* [egzumer](https://github.com/egzumer)
* [Matoz](https://github.com/spm81)
* [ludwich66](https://github.com/ludwich66)
* [Chris](https://github.com/RE3CON)
* [sq5bpf](https://github.com/sq5bpf)
* [Juan Antonio Aldea](https://github.com/JuantAldea)
* [LolloDev5123](https://github.com/LolloDev5123)

## Radio performance

Please note that the Quansheng UV-Kx radios are not professional quality transceivers, their
performance is strictly limited. The RX front end has no track-tuned band pass filtering
at all, and so are wide band/wide open to any and all signals over a large frequency range.

Using the radio in high intensity RF environments will most likely make reception anything but
easy (AM mode will suffer far more than FM ever will), the receiver simply doesn't have a
great dynamic range, which results in distorted AM audio with stronger RX'ed signals.
There is nothing more anyone can do in firmware/software to improve that, once the RX gain
adjustment I do (AM fix) reaches the hardwares limit, your AM RX audio will be all but
non-existent (just like Quansheng's firmware).
On the other hand, FM RX audio will/should be fine.

But, they are nice toys for the price, fun to play with.

## Building

### Github Codespace build method

This is the least demanding option as you don't have to install enything on your computer. All you need is Github account.

1. Go to https://github.com/egzumer/uv-k5-firmware-custom
1. Click green `Code` button
1. Change tab from `Local` to `Codespace`
1. Click green `Create codespace on main` button

<img src="media/codespace1.png" width=700 />

5. Open `Makefile`
1. Edit build options, save `Makefile` changes
1. Run `./compile-with-docker.sh` in terminal window
1. Open folder `compiled-firmware`
1. Right click `firmware.packed.bin`
1. Click `Download`, now you should have a firmware on your computer that you can proceed to flash on your radio. You can use [online flasher](https://egzumer.github.io/uvtools)

<img src="media/codespace2.png" width=700 />

### Docker build method

If you have docker installed you can use [compile-with-docker.bat](./compile-with-docker.bat) (Windows) or [compile-with-docker.sh](./compile-with-docker.sh) (Linux/Mac), the output files are created in `compiled-firmware` folder. This method gives significantly smaller binaries, I've seen differences up to 1kb, so it can fit more functionalities this way. The challenge can be (or not) installing docker itself.

### Windows environment build method

1. Open windows command line and run:
    ```
    winget install -e -h git.git Python.Python.3.8 GnuWin32.Make
    winget install -e -h Arm.GnuArmEmbeddedToolchain -v "10 2021.10"
    ```
2. Close command line, open a new one and run:
    ```
    pip install --user --upgrade pip
    pip install crcmod
    mkdir c:\projects & cd /D c:/projects
    git clone https://github.com/egzumer/uv-k5-firmware-custom.git
    ```
3. From now on you can build the firmware by going to `c:\projects\uv-k5-firmware-custom` and running `win_make.bat` or by running a command line:
    ```
    cd /D c:\projects\uv-k5-firmware-custom
    win_make.bat
    ```
4. To reset the repository and pull new changes run (!!! it will delete all your changes !!!):
    ```
    cd /D c:\projects\uv-k5-firmware-custom
    git reset --hard & git clean -fd & git pull
    ```

I've left some notes in the win_make.bat file to maybe help with stuff.


# Compiler

arm-none-eabi GCC version 10.3.1 is recommended, which is the current version on Ubuntu 22.04.03 LTS.
Other versions may generate a flash file that is too big.
You can get an appropriate version from: https://developer.arm.com/downloads/-/gnu-rm


# Flashing with the official updater

* Use the firmware.packed.bin file

# Flashing with [k5prog](https://github.com/piotr022/k5prog)

* ./k5prog -F -YYY -b firmware.bin

# Flashing with SWD

* If you own a JLink or compatible device and want to use the Segger software, you can find a flash loader [here](https://github.com/DualTachyon/dp32g030-flash-loader)
* If you want to use OpenOCD instead, you can use run "make flash" off this repo.
* The DP32G030 has flash masking to move the bootloader out of the way. Do not try to flash your own way outside of the above methods or risk losing your bootloader.

# License

Copyright 2023 Dual Tachyon
https://github.com/DualTachyon

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

