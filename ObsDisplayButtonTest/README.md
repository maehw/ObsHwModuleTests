# OBS display module test

This folder contains an Arduino project to test an OpenBikeSensor display module. The module has an SSD1306 128x64 pixels I2C OLED display and a push button. I've used the Arduino sketch with an Arduino UNO.

Just look into the source code: [`ObsDisplayButtonTest.ino`](ObsDisplayButtonTest.ino)

The OBS logo is shown on the display

- in white on black when the button is not pressed (i.e. released)
- in black on white when the button is pressed 

along with some unicode symbols.

Make sure your hardware and the display operate on compatible voltage levels!

## References

The OBS logo bitmap has been taken from https://github.com/openbikesensor/OpenBikeSensorFirmware/blob/b4db7c662f48321e686d175fd6e9fc4e9c56afd5/src/logo.h#L31.
