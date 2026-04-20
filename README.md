# EET_BMS 

The EET BMS is an open source battery management system for research and education purposes. It is possible to test new algorithms for state estimation and extend the hardware for more cells. Everything which is needed to rebuild the BMS can be found in this repository. 

## Reading the documentation

We created the documentation in Obsidian. The documentation can also be read in git or in visual studio, but the linking of the screenshot does not work in both of the programs. To see the documentation with the screenshot you have to use obsidian or look at the folder `documentation_res` 

## Tools and programs 

To use the BMS there are different free programs needed. 

| Program             | Description                                                                                                                         |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------- |
| KiCad               | for the Hardware schematics                                                                                                         |
| STM32CubeID         | for deploying the BMS project on the STM32 micro controller                                                                         |
| STM32CubeMX         | for deploying the BMS project on the STM32 micro controller and using the ioc file                                                  |
| STM32CubeProgrammer | reading, writing, and verifying device memory. Also used for erasing the flash after a failed deployment of the software and a boot |
| Python              | for the logging script                                                                                                              |

## Repository structure

| folder name    | Description                                                        |
| -------------- | ------------------------------------------------------------------ |
| Firmware       | the BMS firmware for deploying on the BMS                          |
| Hardware       | Hardware schematics                                                |
| Logging_script | python script for the connection of the BMS and a laptop via RS485 |
| docs           | Documentation for deploying, assembling and initialising the BMS   |

## License 

The license information is found in docs/license. There are two licenses. One for the Software which is a MIT license. The second one is for the Hardware, which is CERN Open Hardware Licence.










