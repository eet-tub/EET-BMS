# EET_BMS

The EET-BMS is a BMS solution developed to monitor battery packs with 3 to 4 cells in series for aging tests and to provide a platform for developing and validating state-of-the-art state estimation algorithms using battery testers and hardware available at EET.

## Getting Started

To get started working with the BMS, you will need:

- 1x Assembled BMS Board
- 1x ST-Link (optional for programming and debugging)
- 1x Pack with 4s cell configuration
- 1x M8 cable lug for the positive battery terminal
- up to 3x NTC Sensors
- 1x USB-RS485 Converter
- 1x USB-UART Converter (optional)
- 1x 5V Supply
- 1x 3D-Printed Holder
- 4x M3 Screws
- 1x M8 Screw and Nut

The assembled PCB is first mounted on the 3D-printed holder using the 4 M3 Screws. In the next step, the Battery can be secured with zip ties. The Cell connector coming from the battery pack is connected to the 7-Pin connector marked with Cells. You need only 6-Pins because the BMS internal regulator is powered by the external power source, not the batteries. Make sure that the orientation of the cell connector is correct and the highest Potential is on the right side of the connector, marked with + on the PCB. The NTC Sensors are connected in a similar way with the three 2-pin connectors next to the cell connector marked with TMP1, TMP2 and TMP3.

The Potential of these connectors is directly tied to the negative Potential of the battery pack! For Power and communication, the connectors on the lower right side of the PCB are used. The 2-Pin connector is used as the 5V Power supply for the STM32, isolators, etc., while the 4-pin connectors provide a RS-485 and a UART Interface. Note that the supply as well as the interfaces are galvanically isolated from the rest of the PCB as well as each other. As a result, the _VCC_ and _GND_ Pins for the serial interfaces _are required_ and not optional!

To program the board, the SWD interface is located on the 6-Pin 2.54mm header at the bottom of the PCB. The Pinout follows the standard ST-Link layout, with pin 1 indicating the VDD_Target Pin of the ST-Link. The Debugging interface is _not_ isolated from the battery potential.

After programming and turning on the PCB for the first time, the display should show information about the cell voltage, temperature sensors, current, as well as the systick of the last measurement in milliseconds.

If everything works up to this point, the battery tester can be connected to the Battery. For that, the positive current wire of the cell tester is connected with the Load+ terminal of the PCB via a cable lug and an M8 screw. The Sense+ wire of the battery tester has to be connected with the Bat+ terminal, together with the positive terminal cable coming from the Battery. The negative terminal of the Battery is directly connected with the negative sense wire and negative current wire from the battery tester.


![[documentation_res/BMS_PCB.png]]

## Repo organization

This repository is structured in three main subfolders:

- The Hardware folder holds all necessary PCB schematic, Layout and production files
- The Firmware folder contains the STM32 Source Code and supporting files
- The Documentation Folder contains additional documentation

## Toolchains

To work on this project it is required to install a toolchain to work with STMs .ioc format. While the VSCode Plugin in combination with CubeMX is getting continuously better, I recommend installing the STM32 Cube IDE for working with the Firmware. Furthermore, to view and edit the schematic and Layout, a KiCAD 7 or newer installation is required. The 3D models for the stand are modeled in Fusion 360 but are included as .step and .stl in this repo, so any 3D CAD program should do the job.

The logging Software for a connected PC is written in Python. Dependencies can be installed via pip using the requirements.txt file. Note that the serial interface used for the main bus between the host and BMS boards is based on RS-485, so an RS-485 to USB converter is required for connecting to the bus.

## Hardware Overview

### TLE9012DQU

The TLE9012DQU is the main BMS IC for the BMS. Its task is to provide analog-to-digital conversion of cell voltages, temperatures, and current, as well as implement part of the necessary circuitry for passive cell balancing.

Documentation for this IC is scattered across the [datasheet](https://www.infineon.com/dgdl/Infineon-TLE9012DQU-DataSheet-v01_00-EN.pdf?fileId=8ac78c8c7e7124d1017f0c3d27c75737) and the [user manual](https://www.infineon.com/dgdl/Infineon-Infineon-TLE9012DQU_TLE9015DQU-UM-v01_00-EN-UserManual-v01_00-EN.pdf?fileId=8ac78c8c7e7124d1017f0c4f8750574b&da=t). The datasheet provides mainly information about the electrical characteristics of the chip, while the user manual contains the register map, a reference schematic, some additional information, and a few example commands. If there is some information missing in both the user manual and the datasheet, it is usually a good idea to check the datasheet of the rather similar predecessor, [TLE9012AQU](https://www.mouser.com/datasheet/2/196/Infineon_TLE9012AQU_DataSheet_v01_10_EN-1890780.pdf).

The PCB Schematic mainly follows the reference design outlined in the user manual, with the main difference being the reduction of the number of connected cells from 12 to just 4. Unused channels are connected to the GND potential. In addition, the single-wire UART interface is used instead of the ISO-UART, which is used in the reference design. To allow for bidirectional current measurements, the TMP3 and TMP4 channels are used with the BAVM function of the TLE9012. The BAVM function allows for repurposing the 16-bit ADC used for independently measuring the sum voltage of all cells as a bipolar ADC on inputs TMP3 and TMP4.

### STM32

To ensure enough computation power to test modern state estimation techniques, a dual-core STM32 microcontroller is used, offering an ARM Cortex M4 and M7 core, with 1Mbyte of Flash per Core as well as 1Mbyte of SRAM.

The schematic of the microcontroller section is a bare-bones implementation of the STM32H755 as described in the datasheet. The H755 offers the possibility to increase efficiency by using some external components to form a switching mode regulator. Due to reasons of simplicity, this is not implemented in this design and the internal linear regulator is used to supply the core voltage.

In terms of communication interfaces, three UARTs of the STM32 are in use, UART Peripheral 1 is used as an isolated UART, which can be connected to a UART-USB connector. For communication between a host and multiple BMS boards, UART3 is used in combination with an isolated RS485 transceiver, which implements a custom serial protocol. The third UART is used in single wire mode to communicate with the TLE9012DQU (see the known quirks and errors section for further information).

### Power Supply Scheme

While the TLE9012DQU supplies itself from the Battery, the STM32 would overload the internal regulator of the TLE, so an independent power source is necessary. The supply is therefore ensured by using an isolated DC/DC 5V to 5V converter with the primary side supplied by the Host. The DC/DC itself is unregulated and has an ensured output voltage of at least 4.5V. Therefore, two independent 3V3 regulators are used to supply digital and analog ICs with 3V3.

To monitor the current consumption of the STM32 and supporting ICs, it is possible to optionally utilize a shunt and a current sense amplifier connected to the ADC peripheral of the STM.

### Current Measurement and Solid State Relay

To protect the Battery in case the BMS detects an error, a solid-state relay consisting of two PMOS acting as a high-side switch in a common-source configuration is used. To allow current to flow in and out of the Battery, the BMS needs to actively pull the Gates of the PFETs to GND. This is achieved by a logic-level NFET connected to a GPIO on the STM.

In Series with the Solid State Relay is a 5 mOhm shunt resistor with a TSC2012 current amplifier. The Amplifier connects to the TLE9012DQU and is biased by a resistor divider to measure bidirectional currents. The reference is also measured by the TLE9012DQU, so no additional compensation is required by the user.

### Tips during assembly

Assembly is rather straightforward, but I suggest using a stencil as well as lead-free solder paste. Make sure to keep the amount of excessive solder paste as low as possible to prevent solder bridges under the Pins of the STM and TLE. While placing the components, make sure that components marked as DNP (do not populate) are not placed on the PCB, which may lead to unexpected behavior.

After soldering the SMD components, it is highly recommended to check that a debug connection to the STM can be established using the ST-Link and a lab-bench power supply.

### Pinout Table

Pinout for J2 (Battery Connector):

| Pin number | Pin name | Pin net         |
|------------|----------|-----------------|
| 1          | Pin\_1   | Bat+            |
| 2          | Pin\_2   | Cell\_4+        |
| 3          | Pin\_3   | Cell\_3+        |
| 4          | Pin\_4   | Cell\_2+        |
| 5          | Pin\_5   | Cell\_1+        |
| 6          | Pin\_6   | Cell\_1-        |
| 7          | Pin\_7   | Bat-            |

Pinout for J4 (TMP1):

| Pin number | Pin name | Pin net                |
|------------|----------|------------------------|
| 1          | Pin\_1   | NTC+                   |
| 2          | Pin\_2   | NTC-                   |

Pinout for J3 (TMP2):

| Pin number | Pin name | Pin net                |
|------------|----------|------------------------|
| 1          | Pin\_1   | NTC+                   |
| 2          | Pin\_2   | NTC-                   |

Pinout for J1 (TMP3):

| Pin number | Pin name | Pin net                |
|------------|----------|------------------------|
| 1          | Pin\_1   | NTC+                   |
| 2          | Pin\_2   | NTC-                   |

Pinout for J9 (CPower Supply):

| Pin number | Pin name | Pin net         |
|------------|----------|-----------------|
| 1          | Pin\_1   | 5V              |
| 2          | Pin\_2   | GND             |

Pinout for J6 (RS485):

| Pin number | Pin name | Pin net         |
|------------|----------|-----------------|
| 1          | Pin\_1   | RS485-VCC (3.3-5V)    |
| 2          | Pin\_2   | A               |
| 3          | Pin\_3   | B               |
| 4          | Pin\_4   | RS485-GND       |

Pinout for J8 (UART\_Shell):

| Pin number | Pin name | Pin net         |
|------------|----------|-----------------|
| 1          | Pin\_1   | UART-GND        |
| 2          | Pin\_2   | TX              |
| 3          | Pin\_3   | RX              |
| 4          | Pin\_4   | UART-VCC (3.3-5V) |

Pinout for J10 (SW\_Debug\_Connector):

| Pin number | Pin name | Pin net        |
|------------|----------|----------------|
| 1          | Pin\_1   | +3V3           |
| 2          | Pin\_2   | SWCLK          |
| 3          | Pin\_3   | GND            |
| 4          | Pin\_4   | SWDIO          |
| 5          | Pin\_5   | NRST           |
| 6          | Pin\_6   | SWO            |

## Firmware Overview

As a result of the dual core nature of the STM32H755, the software consists of two separate sub-projects which share a common .ioc file for code generation and peripheral initialization, as well as a folder named Common, containing files present in both sub-projects. Both cores operate independently by default except for a synchronization event at the start. Data between Cores is shared by a mailbox system, further described in the Inter-Processor Communication section.

To ensure the basic BMS functions, mainly monitoring and communication of log data to the host PC, these functions are running on the Cortex M4 core. The M4 core sends this data to the Cortex M7, which runs the State Estimation. This decoupling of monitoring and state estimation functions allows for testing new algorithms in real time while not compromising the monitoring function in case new state estimation algorithms are facing bugs or too high computational complexity.

### Firmware Structure

The following graphic gives a brief overview of the firmware structure. Both Cores operate on a super loop approach, so no overhead and complexity of an RTOS is introduced.

![[documentation_res/Software_Structure.drawio.png]]


Cortex M7:

In each loop iteration, the Cortex M7's main loop checks for updated measurement data from the M4. If new data is available, the state estimation code found in the BMS_State_Estimation/Src/BMS_State_Estimation.c is executed and the results can be passed back to the M4 via an Inter-Processor Communication Mailbox. The OLED is then updated with new data in the Dusplay_Functions/Src/Display_Function.c file. The OLED is based on a SSD1306 controller IC which interfaces with the STM32 via I2C. The [stm32-ssd1306 library from Aleksander Alekseev](https://github.com/afiskon/stm32-ssd1306) is used to interface with the display.

To send debug messages over the isolated UART Interface, the Cortex M7s _write and __io_putchar prototypes are redefined to print data via UART in blocking mode. This allows for using the printf function of stdio.h to send data as ascii strings. Due to the blocking nature, debug messages will influence execution time of the main loop!

Cortex M4:

The Cortex M4 loop starts with executing the BMS_loop() function found in Core/Src/BMS_Functions.c. This loop function uses the abstraction layer found in the TLE9012_Lib folder to interface with the TLE9012 chip, initiates a measurement, serves the watchdog of the TLE, checks if any error occurs, and calls the BMS_Balance() function to handle cell balancing. Measurement Data is written into a global (in the context of the Cortex M4) variable module, which is of the shared datatype Cell_Module for further use. The values of the module variable are then passed to the Cortex M7 via an Inter Processor Communication Mailbox.

After evaluating the state of the BMS, the rs485_communication_loop() function handling the serial RS485 protocol is called. The RS485 code is written in a way to prevent the communication protocol from interfering with the cell monitoring by the use of receive and transmit interrupts. In the rs485_communication_loop(), it is first checked if a packet is received successfully or if a timeout occurred. If a packet was successfully received, the rs485_process_package() function is called, processing the received packet and sending an appropriate response. The Transmit is also interrupt-based to ensure continuous monitoring of the Battery.

### Inter Processor Communciation

Both STM32 Cores can work fully independently with their own peripherals, RAM and Flash sections. To interact with each other, a mechanism has to be established. There are many different approaches to achieve this. The main methods are Hardware Semaphores and Shared Memory. Shared Memory is a region in SRAM accessed by both cores to exchange data. Hardware Semaphores are a peripheral that can be used for the management of shared resources and to pass events in the form of interrupts between cores. ST suggests using the OpenAMP Framework as a middleware to handle the transfer of data through these mechanisms.

While the OpenAMP library provides a great set of tools to reliably solve this problem, a much simpler approach was used in this project, which can be found in the Common/Inter_Process_Communication folder. SRAM4 is used as shared Memory as suggested in the application notes of STMicro. A Number of mailboxes is initialized by the Cortex M7 before the start of the Cortex M4, each containing a header consisting of a flag to signalize if new data is available, a flag to protect the mailbox during read and write accesses, a length value, and the data itself. A mailbox register table is initialized at the start of SRAM4 to keep track of mailbox addresses and states. Access to mailboxes is provided by read and write methods. To check the status of a mailbox, a check function is provided.

To ensure correct interpretation of datatypes exchanged via mailboxes, a Shared_DataTypes header is included in the common folder, ensuring access to the datatype in both the M4 and M7 program files.

### Serial Protocol

The Serial Protocol used in the RS485 interface at (115200 baud 8N1) is uses the following format

| Byte 1 | Byte 2 | Byte 3 | n Bytes  | Byte n+1 |
| ------ | ------ | ------ | -------- | ------ |
| Bus ID | Command | Payload Length n | Payload | Checksum |

The Bus ID of the device is hardcoded in the Macro RS485_ID found in RS485_Communication.h and needs to be unique for each BMS on the same bus. The BMS responds to commands using the same format, addressing the Host with the ID 0x80. In case the payload length is zero, byte 3 is directly followed by the checksum. The checksum is currently not implemented and therefore can be ignored in processing, but has to be transmitted.

Currently implemented commands are GET_MODULE_INFO which returns module info in a binary format as well as GET_MODULE_INFO_AS_STRING which returns the same data in a ascii format which is human readable at the expanse of taking more bus bandwidth.

# Extending the Firmware

The Firmware in this repository builds a foundation to experiment with more complex BMS algorithms. Therefore, it tries to provide easy expansion/callbacks for the following use cases:

## State estimation

The bmsStateEstimationCallback is called every time there are updated measurements from the cortex M4 and returns new estimations about the state of each cell. State estimation algorithms shall therefore be added to this function.

## Balancing

Balancing is currently seen as a Cortex M4 function and is part of the BMS_Functions.c. The implementation is a simple Top-Balancing algorithm based on cell voltage and called from the BMS_Loop. A proper callback function inside the Cortex M7's codebase might be implemented in the future and this documentation will be updated.

## Communication Protocol

New commands for the RS485 interface might be added in the rs485_process_package function by appending cases to the switch(command) construct. It is strongly recommended to use 1. a preprocessor macro to assign the command ID a human-readable name, and 2. use a dedicated function called from the switch case construct to keep the readability of the function high. It is also strongly recommended to follow the naming scheme rs485_command_yourFunctionName()


# Power Source

The BMS system is externally supplied. Therefore, it is vital that the power settings are configured correctly in the **IOC file (STM32CubeMX)**.
- **Incorrect Settings:** If the power supply settings are not correct, the BMS will fail to start normally. In this case, the system must be manually started by pulling the **BOOT0 pin to High**.
- **LDO Configuration:** The specific function for the supply configuration is: `HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);`
- **Modifying Settings:** This configuration can be adjusted in the STM32CubeIDE/CubeMX interface under the **RCC Tab**. Ensure that the supply mode matches your hardware design (e.g., LDO supply vs. SMPS).

![[documentation_res/Power_Source.png]]

## Recovery Procedure:

If the STM hangs due to critical faults during the boot process the Power demand usually drops down to 10-12mA. The Problem can be resolved by keeping the BOOT Pin high during the boot process using a Jumper connecting 3V3 and the STM Side of R41 while cycling the Power Supply. Power demand should increase to 30-40mA and you now can connect an STLink with the Cube Programmer and erase the flash. After this procedure, the Debug Process should work as normal.

# Known quirks and errors

### Single wire UART on STM32 is too slow

Because the TLE9012DQU uses a single-wire-based UART protocol, the basic idea was to use the single-wire UART mode of the STM as well. Turns out, the switch between TX and RX is way too slow, resulting in missed bytes when reading data from the TLE9012. To prevent this, UART4 RX was connected on a spare pin with the single-wire UART from the STM, now exclusively running in TX mode. This should be implemented in the Layout in further PCB revisions.

### STM32 doesn't connect to ST-Link after firmware update

If the STM32 doesn't connect to the ST-Link after a Firmware update, the new version of the Firmware might activate the onboard switching regulator option of the STM32. Because no switching regulator is used in this design, ensure that the LDO option is activated by software instead. To recover the chip, use a jumper to connect the STM32 side of R41 with 3V3 during a power cycle. The STM32 will then start in the internal bootloader instead of the user software, allowing for erasing the flash/reprogramming the device.

### Display was hotplugged and is not powering on anymore

These cheap OLEDs are not hotpluggable and might be dead after such an event. Solution is to buy another display.

### undefined reference to `HAL_Init` error

If you start debugging or building the project, sometime errors accrue from an undefined reference. The solution is to generate code from the `EET_BMS.ioc` file

### Debugging with two cores

By starting debugging with two cores you have most of the time that the debugger stops in `EET_BMS_CM7` or `EET_BMS_CM4`. You have to click on them and resume. 


