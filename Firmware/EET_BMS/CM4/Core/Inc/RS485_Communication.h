/*
 * RS485_Communication.h
 *
 *  Created on: Jan 14, 2024
 *      Author: max
 */

#ifndef INC_RS485_COMMUNICATION_H_
#define INC_RS485_COMMUNICATION_H_

#include "stm32h7xx_hal.h"
#include "Shared_DataTypes.h"

#define RS485_ID 1
#define RS485_RX_BUFFERSIZE 128

#define GET_MODULE_INFO_COMMAND 1
#define GET_MODULE_INFO_AS_STRING 2
#define GET_CURRENT_BMS_LIMITS_AS_STRING 3
#define SET_BMS_LIMITS 4
#define BMS_ENTER_SLEEP_COMMAND 5
#define GET_STATE_ESTIMATIONS_AS_STRING 6

void rs485_communication_init(UART_HandleTypeDef *huart, Cell_Module_t *hmodule, State_Estimation_t *hestimations);
void rs485_communication_loop();
void rs485_process_package(uint8_t command, uint8_t payloadlen, uint8_t* payload);

void rs485_command_send_cell_data_as_struct(uint8_t payloadlen, uint8_t* payload);
void rs485_command_send_cell_data_as_string(uint8_t payloadlen, uint8_t* payload);
void rs485_command_send_bms_limits_as_string(uint8_t payloadlen, uint8_t* payload);
void rs485_command_set_bms_limits(uint8_t payloadlen, uint8_t* payload);
void rs485_command_bms_enter_sleep_mode(uint8_t payloadlen, uint8_t* payload);
void rs485_command_send_state_estimations_as_string(uint8_t payloadlen, uint8_t* payload);

#endif /* INC_RS485_COMMUNICATION_H_ */
