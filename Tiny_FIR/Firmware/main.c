#include "I2C.h"


int main(void)
{
    // When we arive here it means we just had a reset-event. Most-probably because the user pushed the button!

    // 1. Init our little MCU: pins, drivers, possible Microchip middleware if there is no other way.
    // 2. Do a few measurements for 1 second and average.
    // 3. Maybe some calculations on the data. FLOATING POINT MATH IS STRICKLY OFF-LIMITS! Why? Because I say so! ;*) 
    // 4. Display the status on the LED's for a couple of seconds.
    // 5. Go to sleep and wait for next MCU reset.
}