avr_lcd
=======

HD44780 lcd driver for atmega328p and other compatible avr chips.

Includes functions for initializing lcd as well as writing different formats to
the lcd: strings, characters, and numbers.

Somewhat portable between different layouts; simply change port definitions in
lcdDriver.h file. Currently, data port must be in the lower nybble, however.
The driver uses all three control lines (RS, RW, and EN) but only D4-D7 of the
data lines.

The lcd driver plus this small demo program compile to 1096 bytes with gcc-avr -Os

TODO:
-Implement offset definition in data port lines
-Check for bad command inputs
-Hexadecimal printing
-Circular buffer + interrupts?

