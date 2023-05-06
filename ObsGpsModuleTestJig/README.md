# OBS GPS module test jig

I've made a tiny test jig with pogo pins for u-blox NEO-6M modules in TinkerCad that can be 3D-printed. This allows to test the GPS modules without soldering.

This is what the 3D model looks like:

![Image of test jig Rev2](./NEO-6M-Testjig%20Rev2.png)

You slide the GPS module in on top and connect it via with a USB/serial converter cable to a Windows PC. You can then use the [u-blox u-center](https://www.u-blox.com/en/product/u-center?lang=de) GNSS evaluation software for Windows to test the module.

Please note that the current version (Rev2) will need "support" when being printed with a regular 3D printer.

You can view the model in GitHub's online viewer and download it here: 
[Test jig Rev2 `.stl` file](./NEO-6M-Testjig%20Rev2.stl) 

You can also access the model here:
https://www.tinkercad.com/things/4J0NRdtHvyN

Please note that the model is licensed [CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/).

## Build instructions

* Download `.stl` file
* Slice it (with "support")
* 3D-print it
* Remove "support" from 3D print and clean it
* Re-drill holes when necessary, clean the print again

![Step 1](doc/JigRev2_Step1.jpg)

* Insert pogo pins

![Step 2](doc/JigRev2_Step2.jpg)

* Fix pogo pins using some hotglue (not as much as shown in the image)

![Step 3](doc/JigRev2_Step3.jpg)

* Prepare some breadboard wire halves

![Step 4](doc/JigRev2_Step4.jpg)

* Solder breadboard wires

![Step 5](doc/JigRev2_Step5.jpg)

* Add more hotglue to fix the wires, add labels

![Step 6](doc/JigRev2_Step6.jpg)

## Usage instructions

* Place GPS module on top of the jig, fix with a rubber band (at least in Rev2, as the fixation breaks away)

![Step 7](doc/JigRev2_Step7.jpg)

* Attach GPS antenna and stow on back

![Step 8](doc/JigRev2_Step8.jpg)

* Attach breadboard wire ends to USB/serial converter
* Ready to use u-blox u-center!

## Contributing

Improvements more than welcome!

## References

- https://forum.openbikesensor.org/t/wiki-gps-module-mittels-eines-uart-testen/327
- https://forum.openbikesensor.org/t/wiki-gps-troubleshooting-fehlersuche/170