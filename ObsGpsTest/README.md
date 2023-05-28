 [![ObsHwModuleTests workflow](https://github.com/maehw/ObsHwModuleTests/actions/workflows/main.yml/badge.svg)](https://github.com/maehw/ObsHwModuleTests/actions/workflows/main.yml)

# OBS GPS test

This folder contains an Arduino project to test an OpenBikeSensor GPS module. It also requires an OBS display module (or at least its SSD1306 128x64 pixels I2C OLED display and optionally a push button).

Just look into the source code: [`ObsGpsTest.ino`](ObsGpsTest.ino)

> **Warning**  
> Please note that the ESP itself requires some time to boot until it can receive data from the GPS module. It also makes a difference if both the ESP and the GPS module are powered at the same time or only the ESP is reset. You can help testing and improving this test code on your modules! :)

## Usage

The firmware has different views:

1. An animated splash screen view
2. Satellite constellation, UTC time and signal strength view (derived from NMEA stream data)
3. UBX poll request status pages
4. Error/result view

### Splash screen

Dependent on the button status at startup, the sketch tries to connect to the u-blox NEO-6M module via serial at either 9600 or 115'200 baud. When the button is not pressed, the slow baudrate (9600) will be used and a snail icon is shown. When the button is pressed, the fast baudrate (115'200) will be used and a rabbit icon is shown. As soon as the rabbit icon (along with the OBS logo) appears on the display, the button can be released.

![Splash screen view](./doc/SplashScreenViewSmall.jpg)

### UBX poll request status pages

The software checks if the GPS module answers to UBX poll requests. If the poll request for a specific message could be sent one "OK" will appear on the display ("ER" indicates an error). When the poll request has been answered, an additional "OK" will appear on the screen (otherwise a "NO"). TL;DR: "OK OK" is the good case. 

"OK NO" is an indicator that ...
* the serial communication with the GPS module does not work at all (wrong wiring, wrong baudrate, missing power, etc.) or 
* that the GPS module does not support the specific command.

And strong indicator (and therefore quite likely a GPS module that's not an original u-blox) of the latter is that you see at least one "OK OK" (especially when it's only for the `CFG-PRT` message) on any of the shown status pages.


### Satellite constellation, UTC time and signal strength

When the sketch on the hardware is able to communicate with the GPS module via serial and receives parseable NMEA messages, it will show the satellite constellation, UTC time and signal strengths:

![Constellation view](./doc/ConstellationViewSmall.jpg)

Actually not signal strength, but signal-to-noise ratio (SNR) or [carrier-to-noise ratio](https://en.wikipedia.org/wiki/Carrier-to-noise_ratio) (C/N0).

The constellation view on the left side shows all satellites with their azimuth and elevation from the receiver's perspective. Satellites are drawn with a filled out body when their C/N0 is >= 15.

Below the constellation, the current UTC time is displayed.

On the right side there's a list of satellites with their name (left column) sorted descended by their "signal strength", i.e. the satellite with the strongest signal comes first. The numbers in the right column are the C/N0 values. As there's only limited display space, only up to 8 satellites are listed!


***Note**: The current version does not match the image from above. It should show 7 signal strength bars instead of 8. It also has two indicators that show if any UBX or NMEA messages have been received. (Only tested for NMEA so far.)*

### Error/result view

Something must have gone wrong when this view appears - there's either no valid communication with the GPS module at all or no NMEA stream data active. A complete communication outage is indicated with "broken link" symbol - time to check your wiring (maybe the power supply connection is broken or UBX TX/ uC RX not connected properly) and selected baudrate. In addition, the baudrate is printed as number and a "speed indicator" is shown (rabbit or snail icon).

The display will give additional info on the bottom: when NMEA or UBX messages have been received, the background is filled (white), otherwise left blank (black).

The display also lists the number of received bytes from the GPS serial shortly after startup and at the time the error view is drawn.

Reset the target hardware to try again.

You can also press the button to switch to the higher baudrate and retry again after reset (hold the button pressed until the splash screen appears as describedd above).


## Compatible hardware

Different Arduino-compatible boards<sup>1</sup> should work.

The sketch has been used and tested with ...

* the original OBS hardware (ESP32-based),
* an ESP32-based together with an OBS display module and a u-blox NEO-6M module in the [GPS module test jig](../ObsGpsModuleTestJig/README.md) of this repository.

<sup>1</sup> *This project uses a lot of code and data memory as it makes use of big libraries. It does not fit into an Arduino UNO or Arduino Leonardo. Don't try... using almost all RAM will make the sketch work very unreliably, e.g. copying memory or sorting will fail, the execution may randomly halt. You could free up space by using different libraries though.*


## Arduino library dependencies

* **TinyGPSPlus**: customizable Arduino NMEA parsing library
* **U8g2**: Library for monochrome displays, version 2
* **SoftwareSerial implementation**: ESP32 is not supported by default; **EspSoftwareSerial** works


## Credits

The following fonts have been used with **[U8G2](https://github.com/olikraus/u8g2)**:

* `u8g2_font_chikita_tn`
  (The FontStruction "Chikita" (http://fontstruct.com/Ffontstructions/show/52325) by "southernmedia" is licensed under a Creative Commons Attribution Share Alike license)
* `u8g2_font_siji_t_6x10` (https://github.com/stark/siji; License/Copyright: Siji icon font is available under the "GNU General Public License v2.0")
* `u8g2_font_streamline_*_t` (attribution link: [Free vectors icons and illustrations from Streamline](https://www.streamlinehq.com/))
* `u8g2_font_unifont_t_animals` (https://savannah.gnu.org/projects/unifont; https://github.com/olikraus/u8g2/wiki/fntgrpunifont)

**[TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)** makes it very easy to get satellite elevation, azimuth and "signal strength" from the NMEA messages. The [`SatElevTracker.ino` example](https://github.com/mikalhart/TinyGPSPlus/blob/master/examples/SatElevTracker/SatElevTracker.ino) was a very good starting point.


## References

The OBS logo bitmap (see [`ObsLogo.h`](./ObsLogo.h)) has been taken from https://github.com/openbikesensor/OpenBikeSensorFirmware/blob/b4db7c662f48321e686d175fd6e9fc4e9c56afd5/src/logo.h#L31.
