# Introduction

The EET BMS is an open source battery management system for research and education purposes. It is possible to test new algorithms for state estimation and extend the hardware for more cells. Everything which is needed to rebuild the BMS can be found in this repository. 

## Tools and programs 

To use the BMS there are different free programs needed. For the STM32 programs you can create an account or download as a guest. It is recommended to use an account, because we had sometimes problems with the ST-Link.  

| Program             | Description                                                                                                                                                  |
| ------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| KiCad               | for the Hardware schematics                                                                                                                                  |
| STM32CubeID         | for deploying the BMS project on the STM32 micro controller                                                                                                  |
| STM32CubeMX         | for deploying the BMS project on the STM32 micro controller and using the ioc file                                                                           |
| STM32CubeProgrammer | reading, writing, and verifying device memory. Also used for erasing the flash after a failed deployment of the software and a boot                          |
| Python              | for the logging script; We have used VS Code and installed python. You have to install some packages, which are more discussed in the chapter {ref}`logging` |
## Repository structure

| folder name    | Description                                                        |
| -------------- | ------------------------------------------------------------------ |
| Firmware       | the BMS firmware for deploying on the BMS                          |
| Hardware       | Hardware schematics                                                |
| Logging_script | python script for the connection of the BMS and a laptop via RS485 |
| docs           | Documentation for deploying, assembling and initialising the BMS   |

## License 

The license information is found in {ref}`License`. There are two licenses. One for the Software which is a MIT license. The second one is for the Hardware, which is CERN Open Hardware Licence.










