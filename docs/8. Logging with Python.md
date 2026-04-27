To log the measured values from the Battery Management System (BMS), you need to have Python installed. After installing Python, you can use the script located at `Logging_script/main`. First, install the required packages listed in the script’s initial lines, then start the script.

When you run the script, you will be prompted to specify the port to which the BMS is connected. The available options depend on the number of USB ports in use. After selecting the correct port, enter the number of BMS units connected. The RS485 interface allows multiple BMS units to be connected simultaneously, but each BMS must have a unique ID. Note that the first BMS should use ID `1`, the second ID `2`, and so on.

Next, you will need to set the logging interval (in seconds) and specify a name for the output folder.

Once the script starts, it will create a folder containing a CSV file for each connected BMS, where the logged data will be stored.

| Date       | Time            | BMS_ID | step | Cell_V_1 | Cell_V_2 | Cell_V_3 |
| ---------- | --------------- | ------ | ---- | -------- | -------- | -------- |
| 2026-04-21 | 16:51:51.222113 | 1      | 0    | 33290    | 33300    | 33299    |

| Cell_V_4 | Current | Temp_1 | Temp_2 | Temp_3 | Tic   | SOC |
| -------- | ------- | ------ | ------ | ------ | ----- | --- |
| 33306    | 0       | 0      | 0      | 0      | 19949 | 1   |