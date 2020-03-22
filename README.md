# No-Touch-IR-Thermometer

## Introduction
Especially in the second phase and during the recovery of this pandemic, we will need to monitor both oursleves and our peers at work or in public places. One of the methods being used is checking whether someone has a fever or not. In order to do this without risking even more contamination, we can use *NO-TOUCH Thermometers*. Like the ones you use to test a baby's temperature.

We already anticipate a lack of enough available devices and are looking for **affordable** and **scalable** solutions. 

## GOALs
1) Using the occam's razor method we want to strip the concept of all unnecessary parts. The *need* we want to solve here is not knowing how high someone's temperature is. We merely want to know whether or not someone is a risk. Basic **triage**. Therefor we could eliminate an LED screen for example from the list of components and use just a GREEN/ORANGE/RED status indicator. 
2) We also want this to be something that van be (re)created wherever in the world with as little as possible equipment and components. This to drive down the cost, limit the skillset needed to finish the project, and limit the materials that need to be sourced.

## Concepts
There are 2 design concepts we have come up with. Both are based on the exact same principles but have a slightly different approach in building. Both are using an 8-bit Microcontroller that can be flashed through an Arduino set-up, and a [Melexis FIR](https://www.melexis.com/en/product/MLX90614/Digital-Plug-Play-Infrared-Thermometer-TO-Can) (Far Infra Red Temperature Sensor). Furthermore there's just a push button, a green/orange/red LED light and some electrical components. That's it!

### Project *Tiny-FIR*
The Tiny-FIR is based on the [ATtiny85](https://www.microchip.com/wwwproducts/en/ATtiny85) and will be a fully assembled PCB board. The designs for the board and all necessary information to recreate van be fond in the Tiny_FIR folder. 
*Audience*: this model is targeted at makerspaces and manufacturers

![Rear Close](/Pictures/Enclosure_Design_Rear_close.jpg)
![Rear Close](/Pictures/Enclosure_Design_Front_close.jpg)

### Project *Mega-FIR*
the Mage-FIR is based on the [ATmega328](https://www.microchip.com/wwwproducts/en/ATmega328) and will be a default prototype breadboard with pin-though-hole components. The idea here is that all components are readily available off the shelf without the need for 3rd party manufacturers.
*Audience*: literally everyone with an Arduino board and a soldering iron.

## Supported by:
* [EuroCircuits](https://www.eurocircuits.com/) - is helping with the first batches of the PCB-boards
* [MELEXIS](https://www.melexis.com/en) - has provided a first set sensors for the prototypes
