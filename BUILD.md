CircuitMess-Ringo-firmware Development
--------------------------------------

CircuitMess-Ringo-firmware is built using [PlatformIO](https://platformio.org).

## Getting the code

```sh
git clone https://github.com/CircuitMess/CircuitMess-Ringo-firmware
cd CircuitMess-Ringo-firmware
git submodule update --init
```

## Develop with IDE

Install a [PlatformIO IDE](https://platformio.org/install/ide).

Open CircuitMess-Ringo-firmware in your IDE, and build and/or upload code from there, after connecting Ringo via USB.

## Build with Command Line

Install the [PlatformIO CLI](https://docs.platformio.org/en/latest/installation.html#installation-methods) according to their documentation.

```sh
# build firmware only, output in .pio/build/lolin32/firmware.bin
pio run

# OR build and install new firmware, simply connect Ringo via USB first
pio run -t upload
```
