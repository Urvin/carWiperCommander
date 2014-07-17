Car rear wiper commander
=====================

> *Firstly designed for my Lifan Smily (Lifan 320) car*

A rear wiper functionality expand device. Should be connected into a gap wires going from column switch.
Functionality scenario:
1. Turn rear wiper on;
2. Turn it of after a bit of time;
3. Turn it on again. The wiper works with a defined delay.


##Component list
* [L78L05ABUTR voltage regulator](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00000446.pdf);
* [pic12F683 MCU](http://ww1.microchip.com/downloads/en/devicedoc/41211d_.pdf);
* [IRLML2402 MOSFET](http://www.irf.com/product-info/datasheets/data/irlml2402.pdf);
* [IRML9301 MOSFET](http://www.irf.com/product-info/datasheets/data/irlml9301pbf.pdf);
* 1 x 0805 100kOhms resistors;
* 1 x 0805 10kOhms resistors;
* 1 x 0805 100Ohms resistor;
* 1 x 0805 4.7mF capacitor;
* Can be supplied with a 4x2.54connector.

##Directory tree
* circuit — device schematic diagram designed in Splan 7;
* layout — PCB layout designed in Sprint Layout 6;
* firmware — MCU firmware, Hi-Tech C STD 9.60PL3.

##Photos, etc
You can read about this device in my [car blog](http://www.drive2.ru/l/3927450/) (in Russian).