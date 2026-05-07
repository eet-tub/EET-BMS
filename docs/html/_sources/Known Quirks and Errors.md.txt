# Known quirks and errors

## Single wire UART on STM32 is too slow

Because the TLE9012DQU uses a single-wire-based UART protocol, the basic idea was to use the single-wire UART mode of the STM as well. Turns out, the switch between TX and RX is way too slow, resulting in missed bytes when reading data from the TLE9012. To prevent this, UART4 RX was connected on a spare pin with the single-wire UART from the STM, now exclusively running in TX mode. This should be implemented in the Layout in further PCB revisions.

## STM32 doesn't connect to ST-Link after firmware update

If the STM32 doesn't connect to the ST-Link after a Firmware update, the new version of the Firmware might activate the onboard switching regulator option of the STM32. Because no switching regulator is used in this design, ensure that the LDO option is activated by software instead. To recover the chip, use a jumper to connect the STM32 side of R41 with 3V3 during a power cycle. The STM32 will then start in the internal bootloader instead of the user software, allowing for erasing the flash/reprogramming the device.

## Display was hotplugged and is not powering on anymore

These cheap OLEDs are not hotpluggable and might be dead after such an event. Solution is to buy another display.

## undefined reference to `HAL_Init` error

If you start debugging or building the project, sometime errors accrue from an undefined reference. The solution is to generate code from the `EET_BMS.ioc` file

## Debugging with two cores

By starting debugging with two cores you have most of the time that the debugger stops in `EET_BMS_CM7` or `EET_BMS_CM4`. You have to click on them and resume. 

## GDB Server is not starting

Sometime it helps to restart CubeID or look for an update of the ST-Link 