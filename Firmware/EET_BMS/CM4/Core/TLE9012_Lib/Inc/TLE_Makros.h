/*
 * TLE_Makros.h
 *
 *  Created on: Sep 5, 2022
 *      Author: max
 */

#ifndef INC_TLE_MAKROS_H_
#define INC_TLE_MAKROS_H_

//Everything related to Sending Frames

#define BROADCAST_FRAME_ID 0x3F
#define READ_COMMAND 0x00
#define WRITE_COMMAND 0x80
#define SYNCHRONIZATION_FRAME 0x1E
#define DEFAULT_DEVICE_ID 0x00

//Makros for the Response Frame
#define CRC_ERROR 0x20
#define REG_ADR_INVALID 0x10
#define DIAG_FAULT 0x80



//Register Map

#define PART_CONFIG 0x0001
#define OL_OV_THR 0x0002
#define OL_UV_THR 0x0003
#define TEMP_CONF 0x0004
#define INT_OT_WARN_CONF 0x0005
#define RR_ERR_CNT 0x0008
#define RR_CONFIG 0x0009
#define FAULT_MASK 0x000A
#define GEN_DIAG 0x000B
#define CELL_UV 0x000C
#define CELL_OV 0x000D
#define EXT_TEMP_DIAG 0x000E
#define DIAG_OL 0x0010
#define REG_CRC_ERR 0x0011
#define CELL_UV_DAC_COMP 0x0012
#define CELL_OV_DAC_COMP 0x0013
#define OP_MODE 0x0014
#define BAL_CURR_THR 0x0015
#define BAL_SETTINGS 0x0016
#define AVM_CONFIG 0x0017
#define MEAS_CTRL 0x0018

#define PCVM_BASE 0x0019
#define PCVM_0 0x0019
#define PCVM_1 0x001A
#define PCVM_2 0x001B
#define PCVM_3 0x001C
#define PCVM_4 0x001D
#define PCVM_5 0x001E
#define PCVM_6 0x001F
#define PCVM_7 0x0020
#define PCVM_8 0x0021
#define PCVM_9 0x0022
#define PCVM_10 0x0023
#define PCVM_11 0x0024

#define SCVM_HIGH 0x0025
#define SCVM_LOW 0x0026
#define STRESS_PCVM 0x0027
#define BVM 0x0028

#define EXT_TEMP0 0x0029
#define EXT_TEMP1 0x002A
#define EXT_TEMP2 0x002B
#define EXT_TEMP3 0x002C
#define EXT_TEMP4 0x002D

#define EXT_TEMP_R_DIAG 0x002F
#define INT_TEMP 0x0030
#define MULTI_READ 0x0031
#define MULTI_READ_CFG 0x0032
#define BAL_DIAG_OC 0x0033
#define BAL_DIAG_UC 0x0034
#define INT_TEMP_2 0x0035
#define CONFIG 0x0036
#define GPIO 0x0037
#define GPIO_PWM 0x0038
#define ICVID 0x0039
#define MAILBOX 0x003A
#define CUSTOMER_ID_0 0x003B
#define CUSTOMER_ID_1 0x003C
#define WDOG_CNT 0x003D
#define SCVM_CONFIG 0x003E
#define STRESS_AUX 0x003F

#define BAL_PWM 0x005B
#define BAL_CNT_0 0x005C
#define BAL_CNT_1 0x005D
#define BAL_CNT_2 0x005E
#define BAL_CNT_3 0x005F

#define BAL_PWM_1000 0x0
#define BAL_PWM_0875 0x1
#define BAL_PWM_0750 0x2
#define BAL_PWM_0625 0x3
#define BAL_PWM_0500 0x4
#define BAL_PWM_0375 0x5
#define BAL_PWM_0250 0x6
#define BAL_PWM_0125 0x7

typedef enum{
	SOURCE_320UA=0,
	SOURCE_80UA,
	SOURCE_20UA,
	SOURCE_5UA
}TempCurrentSource_t;



#endif /* INC_TLE_MAKROS_H_ */
