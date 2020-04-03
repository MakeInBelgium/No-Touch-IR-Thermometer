# MegaFIR Electrical Diagram

<img src="https://raw.githubusercontent.com/MakeInBelgium/No-Touch-IR-Thermometer/master/Pictures/MegaFIR_ElectricalDiagram_v001.png" width="450">

Note: the design is draw using Circuit Diagram, a free and Open-Source tool. [You can find the design here](https://www.circuit-diagram.org/circuits/e216c06a74a149ee9efb37499b95ee45).

## Description
This design is based on the idea of using a pin-through-hole proto-breadboard [(example)](https://www.adafruit.com/product/571) with 2x30 lanes and a +/- rail on each side. The lay-out is specifically designed to have the least amount of cables overlapping or even crossing the middle section. 

## PIN assignment
Some PINs on the ATmega are randomly chosen, some are very strict:
| ATmega PIN | Arduino PIN | Description | Strict? |
| --------: | --------: | -------- | :--------: | 
| 1 / PC6 | RESET | needed for programming the ATmega from the Arduino | X |
| 4 / PD2 | 2 | input from switch/button |  |
| VCC | VCC | Power | X |
| GND | GND | Ground | X |
| 11 / PD5 | 5 | Output LED red | |
| 12 / PD6 | 6 | Output LED green | | 
| 13 / PD7 | 7 | Output LED orange | | 
| 28 / PC5 | A5 | SCL (clock) port 1 on the MLX | X |
| 27 / PC4 | A4 | SDA (data) port 2 on the MLX | X |
| 20 / AVCC | AVCC | Power | X |
| 19 / PB5 | 13 | SCK - needed for programming the ATmega from the Arduino | X |
| 18 / PB4 | 12 | MISO - needed for programming the ATmega from the Arduino | X |
| 17 / PB3 | 11 | MOSI - needed for programming the ATmega from the Arduino | X |

## Arduino & ATmega pin alignment

<img src="https://raw.githubusercontent.com/MakeInBelgium/No-Touch-IR-Thermometer/master/Mega_FIR/Pictures/ATmega328-and-the-Arduino-pin-out.png" width="450">

*sorry I can't find and quote the original source of this image. it was the best Google search result
