# Battery Management System (BMS) Configuration

The parameters for the BMS are in following header file defined: 
`Firmware\EET_BMS\CM4\Core\Inc\BMS_Functions.h`

| Settings                                                                                                                                 | parameter                   |
| ---------------------------------------------------------------------------------------------------------------------------------------- | --------------------------- |
| maximum current for charging                                                                                                             | `MAXIMUM_CHARGE_CURRENT`    |
| maximum current for discharging                                                                                                          | `MAXIMUM_DISCHARGE_CURRENT` |
| maximum voltage                                                                                                                          | `MAXIMUM_VOLTAGE`           |
| minimum voltage                                                                                                                          | `MINIMUM_VOLTAGE`           |
| maximum temperature                                                                                                                      | `MAXIMUM_TEMPERATURE`       |
| minimum temperature                                                                                                                      | `MINIMUM_TEMPERATURE`       |
| numbers of cells in series                                                                                                               | `NUMBEROFCELLS`             |
| number of temperature sensors                                                                                                            | `NUMBEROFTEMPS`             |
| balancing threshold for the possibility to start the balancing. The balancing starts only if the voltage delta is exceeded between cells | `BALANCING_THRESHOLD`       |
| The balancing starts only if the voltage delta is exceeded between cells                                                                 | `BALANCING_VOLTAGE_DELTA`   |

# Interface RS485 

If multiple BMS systems are used within the same network, every unit must be assigned a **unique ID** to ensure proper communication and avoid addressing conflicts.
The ID is defined in the source code. To change it, you must edit the following header file: 
`Firmware\EET_BMS\CM4\Core\Inc\RS485_Communication.h`

| Settings            | parameter  |
| ------------------- | ---------- |
| the ID for each BMS | `RS485_ID` |

