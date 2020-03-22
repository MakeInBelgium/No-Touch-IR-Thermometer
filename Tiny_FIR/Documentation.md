# Theory of operation
## Non-contact thermometers
First a small disclaimer. Normal body temperature is considered to be 37°C; however, a wide variation is seen.<sup>[1](#cit1)</sup> So a measument taken  from a DIY device ánd judged to be either norminal or fibrile by that DIY device should *never be used in critical situations!*

There are several methods to measure this body temperature. In the category of radiometric (infrared) measurements there are 3 main methods<sup>[2](#cit2)</sup>:
1. Tympanic thermometer (eardrum temperature)
2. Handheld infrared skin thermometers
3. Thermal scanners (infrared cameras)

The Tiny_FIR project aims to build a sensor that uses this second measuring method, measuring the surface temperature of the skin. Because the sensor used in the Tiny_FIR project is also widely used for Tympanic thermometers, care must be taken to use and reference the right information online. The fibrile temperature is different for the eardrum than the skin surface and most probably a different compensation approuch should be used to determine the body temperature.

Most of the clinical radiometric thermometers that use the tympanic or skin surface measurements will report the body temperature. This temperature is a calculated, or indirect, measurement. Most of the Thermal cameras are general purpose devices that only report the direct measurent not the indirect body temperature.

The ideal fibrile threshold temperature of the skin surface seems to range between 37.5°C and 38°C. <sup>[3](#cit3)</sup>

## FIR sensors
The MLX90614 comes in many flavors. Melexis proposes to use the MLX90614-DCC. 
- D: This means the device is calibrated for 0.2degC accuracy around body temperature.
- C: The device is compensating for (but not eliminating) thermal gradients. 
- C: There is a 35 degree FOV.

In order to achieve as much accuracy as possible, there are many good practices to be followe:
### Ambient temperature
The temperature readout from the sensor (To) is compensated internally by the ambient temperature (Ta) as such: V<sub>ir</sub> = A x (To<sup>4</sup>-Ta<sup>4</sup>) The measurment of Ta is taken internally by a PTAT sensor. So to achieve high accuracy this PTAT sensor should measure the actual ambiant temperature. In other words: the sensor should be in total thermal equilibrium with the surroundings.

The electronic design of the Tiny-FIR is in such a way so there is no large power-dissipation near the sensor. Furthermore, the sensor is kept powered all the time in between measurements as long as it is used (plugged into the USB powerbank). This avoid thermal gradients due to self-heating.

Lastly the user should be informed that usage of this sensor is limited to thermally static conditions. This means, for example, that the sensor cannot be used in outdoor high-wind conditions or when switching between area's with different ambient temperatures, like indoor and outdoor.

As a side-note it should be noted that a more complex compensation algorithm to calculated the body temperature should also use the ambiant temperature. The thermal gradient from body to skin surface will be depend on the ambiant temperature. So we can conclude that the fibrile temperature of the skin surface will also be ambient temperature dependent. This is also the reason why Ta is available for readout on the MLX90614.

### Field of view
The FOV of the sensor is defined as the region where it has more that 50% sensitivity. It is important that the surface that needs to be measured in entirely in the FOV. Some commercial thermometers achieve this by having two cocussed light beams converge, so when the dot allign the sensor is at the right distance.

Further investigation here is still required on how to implement this.


# References:
1. <a name="cit1"></a>: https://www.ncbi.nlm.nih.gov/books/NBK331/
2. <a name="cit2"></a>: https://www.ncbi.nlm.nih.gov/books/NBK263242
3. <a name="cit3"></a>: https://www.eurosurveillance.org/images/dynamic/EE/V14N06/art19115.pdf