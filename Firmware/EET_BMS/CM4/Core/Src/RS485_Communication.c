#include "RS485_Communication.h"
#include "Shared_DataTypes.h"
#include "BMS_Functions.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

volatile uint8_t rs485_tx_cplt_flag = 1;
volatile uint8_t rs485_rx_cplt_flag = 0;

uint8_t receive_in_progress = 0;
uint8_t rs485_rxbuffer[RS485_RX_BUFFERSIZE];
static uint8_t tx_buffer[255];

UART_HandleTypeDef *rs485uart;
static Cell_Module_t *rs485_module;
static State_Estimation_t *rs485_estimations;

/** @brief init routine for the rs485 bus
 * 	@note this function has to be called before any other function calls from this library
 * 	@param huart UART handle
 * 	@param hmodule pointer to the global module variable
 */

void rs485_communication_init(UART_HandleTypeDef *huart, Cell_Module_t *hmodule, State_Estimation_t *hestimations)
{
	rs485uart = huart;
	rs485_module = hmodule;
	rs485_estimations = hestimations;
}

/** @brief init routine for the rs485 bus
 * 	@note this function has to be called before any other function calls from this library
 * 	@param huart UART handle
 * 	@param hmodule pointer to the global module variable
 */

void rs485_communication_loop()
{
	static uint32_t last_xfer_count = 0;
	if(receive_in_progress == 0)
	{
		rs485_rx_cplt_flag = 0;
		HAL_UART_Receive_IT(rs485uart, rs485_rxbuffer, RS485_RX_BUFFERSIZE);
		receive_in_progress = 1;
	}
	else
	{
		uint8_t rx_bytes_received = (RS485_RX_BUFFERSIZE-rs485uart->RxXferCount);

		//Check if length byte was received
		if(rx_bytes_received > 2)
		{
			//Check if the full Packet was received
			uint8_t packetlength = rs485_rxbuffer[2];
			if(rx_bytes_received >= (packetlength+3))
			{
				//Check if ID Matches and goto callback function
				if(rs485_rxbuffer[0] == RS485_ID)
				{
					rs485_process_package(rs485_rxbuffer[1], rs485_rxbuffer[2], &rs485_rxbuffer[3]);
				}
				rx_bytes_received = 0;
				HAL_UART_AbortReceive_IT(rs485uart);
				HAL_UART_Receive_IT(rs485uart, rs485_rxbuffer, RS485_RX_BUFFERSIZE);
			}
		}

		//Check if connection hangs and restart receive process
		if((rx_bytes_received > 0) && (rx_bytes_received == last_xfer_count))
		{
			rx_bytes_received = 0;
			HAL_UART_AbortReceive_IT(rs485uart);
			HAL_UART_Receive_IT(rs485uart, rs485_rxbuffer, RS485_RX_BUFFERSIZE);
		}
		last_xfer_count = rx_bytes_received;

	}
}

/** @brief init routine for the rs485 bus
 * 	@note this function has to be called before any other function calls from this library
 * 	@param huart UART handle
 * 	@param hmodule pointer to the global module variable
 */

void rs485_process_package(uint8_t command, uint8_t payloadlen, uint8_t* payload)
{

	//Wait if RX is still in Progress
	while(rs485_tx_cplt_flag != 1);

	switch(command)	{
		case GET_MODULE_INFO_COMMAND:
			rs485_command_send_cell_data_as_struct(payloadlen, payload);
			break;

		case GET_MODULE_INFO_AS_STRING:
			rs485_command_send_cell_data_as_string(payloadlen, payload);
			break;

		case GET_CURRENT_BMS_LIMITS_AS_STRING:
			rs485_command_send_bms_limits_as_string(payloadlen, payload);
			break;

		case SET_BMS_LIMITS:
			rs485_command_set_bms_limits(payloadlen, payload);
			break;

		case GET_STATE_ESTIMATIONS_AS_STRING:
			rs485_command_send_state_estimations_as_string(payloadlen, payload);
			break;
	}
}

/** @brief init routine for the rs485 bus
 * 	@note this function has to be called before any other function calls from this library
 * 	@param huart UART handle
 * 	@param hmodule pointer to the global module variable
 */

void rs485_command_send_cell_data_as_struct(uint8_t payloadlen, uint8_t* payload)
{
	uint8_t buffersize = MAXIMUM_CELL_VOLTAGES*2 + MAXIMUM_AUX_VOLTAGES*2 + 4 + 4;


				tx_buffer[0] = 0x80;
				tx_buffer[1] = GET_MODULE_INFO_COMMAND;
				tx_buffer[2] = buffersize - 4;
				tx_buffer[buffersize-1] = '\n';

				for(uint8_t n = 0; n < MAXIMUM_CELL_VOLTAGES; n++)
				{
					tx_buffer[3+2*n] = (uint8_t)(rs485_module->cellVoltages[n]);
					tx_buffer[4+2*n] = (uint8_t)(rs485_module->cellVoltages[n]>>8);
				}

				uint8_t baseadr = MAXIMUM_CELL_VOLTAGES*2+3;

				for(uint8_t n = 0; n < MAXIMUM_AUX_VOLTAGES; n++)
				{
					tx_buffer[baseadr+2*n] =  (uint8_t)(rs485_module->auxVoltages[n]);
					tx_buffer[baseadr+1+2*n] = (uint8_t)(rs485_module->auxVoltages[n]>>8);
				}

				baseadr += 2*MAXIMUM_AUX_VOLTAGES;

				uint32_t current_int = *((uint32_t*)&rs485_module->current);
				tx_buffer[baseadr] = (uint8_t) current_int;
				tx_buffer[baseadr+1] = (uint8_t) (current_int>>8);
				tx_buffer[baseadr+2] = (uint8_t) (current_int>>16);
				tx_buffer[baseadr+3] = (uint8_t) (current_int>>24);

				rs485_tx_cplt_flag = 0;
				HAL_UART_Transmit_IT(rs485uart, tx_buffer, buffersize);
}

/** @brief init routine for the rs485 bus
 * 	@note this function has to be called before any other function calls from this library
 * 	@param huart UART handle
 * 	@param hmodule pointer to the global module variable
 */

void rs485_command_send_cell_data_as_string(uint8_t payloadlen, uint8_t* payload)
{
	static uint8_t rollingcounter = 0;
	uint8_t txlen;

	tx_buffer[0] = 0x80;
	tx_buffer[1] = GET_MODULE_INFO_AS_STRING;

	txlen = sprintf((char*) &tx_buffer[3], "%d,%d,%lu,%lu,%lu,%lu,%ld,%lu,%lu,%lu,%lu,%f\n",
	//txlen = sprintf((char*) &tx_buffer[3], "%d,%d,%lu,%lu,%lu,%lu,%ld,%lu,%f\n",
		RS485_ID,
		rollingcounter,
		(uint32_t)((float) rs485_module->cellVoltages[0]*50000)/65536,
		(uint32_t)((float) rs485_module->cellVoltages[1]*50000)/65536,
		(uint32_t)((float) rs485_module->cellVoltages[2]*50000)/65536,
		(uint32_t)((float) rs485_module->cellVoltages[3]*50000)/65536,
		(int32_t)(1000*rs485_module->current),
		(int32_t)(rs485_module->Temperatures[0]),
		(int32_t)(rs485_module->Temperatures[1]),
		(int32_t)(rs485_module->Temperatures[2]),
		HAL_GetTick(),
		rs485_estimations->module_SoC);

	tx_buffer[2] = txlen+1;
	tx_buffer[txlen+3] = '\n';
	rollingcounter ++;
	rs485_tx_cplt_flag = 0;
	HAL_UART_Transmit_IT(rs485uart, tx_buffer, txlen+3);

}

/** @brief init routine for the rs485 bus
 * 	@note this function has to be called before any other function calls from this library
 * 	@param huart UART handle
 * 	@param hmodule pointer to the global module variable
 */

void rs485_command_send_bms_limits_as_string(uint8_t payloadlen, uint8_t* payload)
{
	uint8_t txlen = 0;

	tx_buffer[0] = 0x80;
	tx_buffer[1] = GET_CURRENT_BMS_LIMITS_AS_STRING;

	bms_config_limits_t limits = BMS_GetLimits();

	txlen = sprintf((char*) &tx_buffer[3], "%f,%f,%f,%f,%f,%f,%f,%f\n",
			limits.overvoltage,
			limits.undervoltage,
			limits.max_current_charge,
			limits.max_current_discharge,
			limits.overtemp,
			limits.undertemp,
			limits.shuntvalue,
			limits.senseampgain);

	tx_buffer[2] = txlen+1;
	tx_buffer[txlen+3] = '\n';

	rs485_tx_cplt_flag = 0;
	HAL_UART_Transmit_IT(rs485uart, tx_buffer, txlen+3);
}

/** @brief init routine for the rs485 bus
 * 	@note this function has to be called before any other function calls from this library
 * 	@param huart UART handle
 * 	@param hmodule pointer to the global module variable
 */

void rs485_command_set_bms_limits(uint8_t payloadlen, uint8_t* payload)
{
	if(sizeof(bms_config_limits_t) == (payloadlen*4))
	{
		bms_config_limits_t newlimits;
		memcpy(&newlimits, (bms_config_limits_t*)payload, sizeof(bms_config_limits_t));
		BMS_SetNewLimits(newlimits);

		tx_buffer[0] = 0x80;
		tx_buffer[1] = SET_BMS_LIMITS;
		tx_buffer[2] = 0;
		tx_buffer[3] = 0;
		rs485_tx_cplt_flag = 0;
		HAL_UART_Transmit_IT(rs485uart, tx_buffer, 4);
	}
}


/** @brief init routine for the rs485 bus
 * 	@note this function has to be called before any other function calls from this library
 * 	@param huart UART handle
 * 	@param hmodule pointer to the global module variable
 */

void rs485_command_bms_enter_sleep_mode(uint8_t payloadlen, uint8_t* payload)
{

	BMS_Sleep();

	tx_buffer[0] = 0x80;
	tx_buffer[1] = BMS_ENTER_SLEEP_COMMAND;
	tx_buffer[2] = 0;
	tx_buffer[3] = 0;
	rs485_tx_cplt_flag = 0;
	HAL_UART_Transmit_IT(rs485uart, tx_buffer, 4);
}


void rs485_command_send_state_estimations_as_string(uint8_t payloadlen,uint8_t* payload)
{
	uint8_t txlen = 0;

	tx_buffer[0] = 0x80;
	tx_buffer[1] = GET_STATE_ESTIMATIONS_AS_STRING;

	txlen = sprintf((char*) &tx_buffer[3], "%f\n",rs485_estimations->module_SoC);

	tx_buffer[2] = txlen+1;
	tx_buffer[txlen+3] = '\n';

	rs485_tx_cplt_flag = 0;
	HAL_UART_Transmit_IT(rs485uart, tx_buffer, txlen+3);
}
