# dimmable_led
Arduino lighting control using dimmable led driver. Control is carried out by button:
* A single click turns off the light or turns on with the previous brightness.
* A double click will turn on immediately at maximum brightness or, if the lamp is already shining, translates it to minimum brightness.
* Holding the button changes the brightness from minimum to maximum and back.
* * if press and hold the button while the lamp is off, the lamp will turn on with a minimum brightness.
The state is stored in non-volatile memory. After external power up (or Arduino reset), the lamp is always off.

![sheme](https://github.com/ArtHome12/dimmable_led/blob/master/sheme_sheme.png)

![bread board](https://github.com/ArtHome12/dimmable_led/blob/master/sheme_bb.png)

