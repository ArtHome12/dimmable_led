# dimmable_led
Arduino lighting control using dimmable led driver. Control is carried out by button:
* A single click turns off the light or turns on with the previous brightness.
* A double click will turn on immediately at maximum brightness or, if the lamp is already shining, translates it to minimum brightness.
* Holding the button changes the brightness from minimum to maximum and back.
* If press and hold the button while the lamp is off, the lamp will turn on with a minimum brightness.
The state is stored in non-volatile memory. After external power up (or Arduino reset), the lamp is always off.

![sheme](https://github.com/ArtHome12/dimmable_led/blob/master/sheme_sheme.png)

![bread board](https://github.com/ArtHome12/dimmable_led/blob/master/sheme_bb.png)

# LED driver
The driver has 3 contacts on one side: zero, phase 220 volts and ground. On the other, there are two pairs of contacts: the output (GND and 12VDC) for payload and the power control input:
1. When two control contacts are open, they have a potential of 10VDC and the driver gives maximum power (constantly 12VDC output).
2. When two control contacts are closed, that is, between them there is a potential of 0 volts, the driver does not give power (0 volts at the output).
3. When the intermediate voltage between the control contacts is between 0 and 10 volts, the PWM is working on the driver (from 8 to 12 volts an output square signal).

The driver power is declared 120W, under the cover it is already written 100W, in reality probably about 70W. As consumers - powerful LEDs on the 2mm aluminium plates.

Under the driver cover on the group of three capacitors there is a voltage of 12 VDC, from which arduino is powered. There are also 5VDC and 6.4VDC on smaller capacitors.
