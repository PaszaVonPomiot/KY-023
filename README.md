# KY-023

Class for using KY-023 Joystick module with Arduino

## Description

Allows reading X and Y values from joystick connected to analog inputs. Includes following freatures:
- finds stable reading of joy's neutral position (fixes issue with reading wobbling when joy is not being touched)
- projects unequal axes to ideal axes (fixes issue with with reading not showing center when joy is in neutral position)
- outputs 0-255 value to fit in byte (for easier transmission via eg. nRF24L01+) instead of joy's actual 0-1023
- allows reversing axis output (0..255 -> 255..0)


## Dependencies

* <Arduino.h>

## Installing

* Copy Joy class and paste it to your Arduino project
* If using in *.ino file via Arduino IDE you don't have to copy `#include <Arduino.h>` line

## Executing program

* Create instance of Joy class in global context (outside `setup()` and `loop()` functions) eg.
```
\\ Joy(byte x_pin, byte y_pin, byte button_pin, byte deadzone_radius)
Joy joy(A0, A1, 7, 3);
```
* Add `joy.setup()` within Arduino main `setup()` section
* In Arduino `loop()` section you can now read axis position
```
joy.readJoyAxisAsByte(X_AXIS)
joy.readJoyAxisAsByte(Y_AXIS)
```
* When Arduino starts leave joy in neutral position for 1 second so it can find stable neutral read
* If needed reverse axis by using `joy.reverse(joy.readJoyAxisAsByte(X_AXIS))`

## Help

Any issues, questions and improvement ideas can be raised via GitHub.

## Acknowledgments

Thanks to the guys from freenode IRC channel ##C++-general, especially:
* Alipha
* cbreak
