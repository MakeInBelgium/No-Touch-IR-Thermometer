So, you have a Tiny_FIR contraption in your hand? And you want the MCU to start making measurements?
Here is how you get the code running on the onboard ATtiny402:
1. <u>Get the JTAG2UPDI project:</u>
Download or clone the [JTAG2UPDI](https://github.com/ElTangas/jtag2updi) repository from ElTangas.
(Maybe it would be nice to have this repo as a submodule?)
2. <u>Compile JTAG2UPDI code and flash a tool:</u>:
Follow the instruction in the jtag2updi repo. You can even just use the arduino environment to compile an flash an Arduino Uno! Isn't that simple?
3. <u>(optional) Check communication with the ATtiny ROM bootloader using avrdude:</u>
You can execute the following avrdude command to read the device signature. Replace the comport number with the correct comport for the Arduino on your PC.
```
.\avrdude.exe -C .\avrdude.conf -c jtag2updi -P com17 -p t402
avrdude.exe: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.30s

avrdude.exe: Device signature = 0x1e9227 (probably t402)

avrdude.exe done.  Thank you.
```
4.<u>Flashing the firmware to the ATTiny:</u>
WIP....
