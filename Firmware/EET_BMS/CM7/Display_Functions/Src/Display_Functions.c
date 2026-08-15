#include "Display_Functions.h"
#include "ssd1306.h"
#include <stdio.h>
#include <string.h>


void printCellVoltage(Cell_Module_t module, State_Estimation_t states)
{
    ssd1306_Fill(Black);

    for(uint8_t n = 0; n < 4; n++)
    {
        char stringbuffer[32];

        sprintf(stringbuffer,
                "C%d:%.3fV",
                n + 1,
                ((float)module.cellVoltages[n]) / 65536.0f * 5.0f);

        ssd1306_SetCursor(0, 10 * n);
        ssd1306_WriteString(stringbuffer, Font_7x10, White);
    }


    for(uint8_t n = 0; n < 3; n++)
    {
        char stringbuffer[16];

        ssd1306_SetCursor(70, 10 * n);

        sprintf(stringbuffer,
                "T%d:%2.0fC",
                n + 1,
                module.Temperatures[n]);

        ssd1306_WriteString(stringbuffer, Font_7x10, White);
    }


    /*
     * Average SOC from 4 EKF cells
     */
    float soc_avg =
        (states.SoC[0] +
         states.SoC[1] +
         states.SoC[2] +
         states.SoC[3]) / 4.0f;
    ssd1306_SetCursor(0, 40);

    char stringbuffer[32];
    sprintf(stringbuffer,
            "SOC_Avg:%1.3f",
            soc_avg);

    ssd1306_WriteString(stringbuffer, Font_7x10, White);


    /*
     * Old Coulomb Counting SOC
     */
    ssd1306_SetCursor(70, 30);

    sprintf(stringbuffer,
            "SC:%1.3f",
            states.module_SoC);

    ssd1306_WriteString(stringbuffer, Font_7x10, White);


    /*
     * Current
     */
    ssd1306_SetCursor(0, 50);

    sprintf(stringbuffer,
            "Current:%1.3fA",
            module.current);

    ssd1306_WriteString(stringbuffer, Font_7x10, White);


    /*
     * Send complete framebuffer to OLED
     */
    ssd1306_UpdateScreen();
}
