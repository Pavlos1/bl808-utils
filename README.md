# Utilities for working with BL808 BootHeaders

## Build
```bash
make
```

## Example Usage
```bash
git clone https://github.com/bouffalolab/bl808_linux
```
Follow bouffalolab's instructions to compile the images, then:
```bash
pip install blfb-iot-tool

M0_SIZE=$(stat -c '%s' bl808_linux/out/low_load_bl808_m0.bin)
D0_SIZE=$(stat -c '%s' bl808_linux/out/low_load_bl808_d0.bin)

./gen_boot_header 0x2000 $M0_SIZE bootheader_m0
./gen_boot_header -s 0x52000 $D0_SIZE bootheader_d0 # '-s' for secondary (Group 1) bootheader
# double-check bootheader contents
./print_boot_header bootheader_m0
./print_boot_header bootheader_d0
```
First flash the bootheaders, then the firmwares, then the OpenSBI/Linux image.
See [here](https://github.com/bouffalolab/bl808_linux/issues/22) and [here](https://wiki.pine64.org/images/9/9f/Linux_BL808_en.pdf).
```bash
bflb-iot-tool --firmware bootheader_m0 --addr 0x0 --chipname bl808 --interface uart --port /dev/ttyUSB0 --baudrate 2000000 --single
bflb-iot-tool --firmware bootheader_d0 --addr 0x1000 --chipname bl808 --interface uart --port /dev/ttyUSB0 --baudrate 2000000 --single

bflb-iot-tool --firmware bl808_linux/out/low_load_bl808_m0.bin --addr 0x2000 --chipname bl808 --interface uart --port /dev/ttyUSB0 --baudrate 2000000 --single
bflb-iot-tool --firmware bl808_linux/out/low_load_bl808_d0.bin --addr 0x52000 --chipname bl808 --interface uart --port /dev/ttyUSB0 --baudrate 2000000 --single

bflb-iot-tool --firmware bl808_linux/out/whole_img_linux.bin --addr 0xD2000 --chipname bl808 --interface uart --port /dev/ttyUSB0 --baudrate 2000000 --single
```
