/*
 * TLE_LL_Driver.h
 *
 *  Created on: Sep 5, 2022
 *      Author: max
 */


#include "TLE_LL_Driver.h"
#include <string.h>
#include "RS485_Communication.h"

UART_HandleTypeDef *tle_uart;

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;

extern volatile uint8_t rs485_tx_cplt_flag;
extern volatile uint8_t rs485_rx_cplt_flag;

uint8_t volatile uart_crc_error = 0;
uint8_t volatile tx_cplt_flag = 0;

volatile uint8_t tleuart_rxcplt = 0;
volatile uint8_t rx_cplt_flag = 0;
uint16_t multiread_config = 0;

uint8_t crc3_table[8] = {0b0000,0b011,0b110,0b101,0b111,0b100,0b001,0b010};


uint8_t TLE_LL_Init(UART_HandleTypeDef* huart)
{
	tle_uart = huart;
	return 0;
}

uint8_t writeReg(uint8_t deviceadr, uint8_t regadr, uint16_t data)
{

	static uint8_t response[1];

	for(int n = 0; n < 1; n++)
	{
		response[n] = 255;
	}

	uint8_t uartbuffer[6];
	uartbuffer[0] = SYNCHRONIZATION_FRAME;
	uartbuffer[1] = (deviceadr & 0x3F) | WRITE_COMMAND;
	uartbuffer[2] = regadr;
	uartbuffer[3] = ((data>>8) & 0xFF);
	uartbuffer[4] = (data & 0xFF);
	uartbuffer[5] = 0;
	uartbuffer[5] = calculateCRC8(uartbuffer, 6);

	//Send Buffer and immediately switch to receive
//	HAL_StatusTypeDef txstatus = uartWrite(uartbuffer, 6);
	//HAL_StatusTypeDef rxstatus =
//	(void) uartRead(response, 1);

	HAL_StatusTypeDef txstatus = readWrite(uartbuffer, 6, response, 1);
	if(txstatus != HAL_OK)
	{
		return 16;
	}





	//TODO Propper Error Checking
	uint8_t rx_Frame = response[0];

	if((rx_Frame &0x07) != calculateCRC3(rx_Frame)){
		return 1;
	}
	else
	{
		return 0;
	}
	/*uint16_t checkdata;
	readReg(deviceadr, regadr, &checkdata);
	if(checkdata == data)
	{
		volatile uint8_t test = 0;
	}
	else
	{
		volatile uint8_t fail = 0;
	}
	if(calculateCRC3(rx_Frame) && (rx_Frame != 0))
	{
		uart_crc_error++;
		if(uart_crc_error >= MAX_CRC_ERRORS)	//End Condition for recursion
		{
			uart_crc_error = 0;
			return 1;
		}
		else
		{
			return writeReg(deviceadr, regadr, data);
		}
		return 8;
	}
	else
	{
		uart_crc_error = 0;
		return (rx_Frame>>3) & 0x07;
	}*/
	return 0;
}

uint8_t readReg(uint8_t deviceadr, uint8_t regadr, uint16_t* data)
{

	//Abort any current UART DMA Trafic (should be redundant)

	uint8_t response[16];	//Setup Response buffer

	for(uint8_t n = 0; n <16;n++)	//Clear buffer just to be sure
		response[n] = 0;


	//HAL_UART_Receive_DMA(tle_uart, response, 9); //Start Receive DMA with buffersize = Command Frame to TLE + Response Frame from TLE

	uint8_t uartbuffer[4];	//Initialize Buffer for Command Frame
	uartbuffer[0] = SYNCHRONIZATION_FRAME;
	uartbuffer[1] = deviceadr & 0x3F;
	uartbuffer[2] = regadr;
	uartbuffer[3] = 0;
	uartbuffer[3] = calculateCRC8(uartbuffer, 4);

	//uartWrite(uartbuffer, 4); //Write Command Frame to TLE
	//uartRead(response, 5);

	readWrite(uartbuffer, 4, response, 5);

	uint16_t respdata = ((uint16_t) response[2] << 8) | ((uint16_t)response[3]);	//Read Response Data
	*data = respdata;

	uint8_t returncrc = calculateCRC8(&response[0], 5);	//Check if the CRC8 of the response is valid

	if(response[4] == returncrc)
	{
		uart_crc_error = 0;
		return 0;
	}
	else
	{
		uart_crc_error++;
		if(uart_crc_error >= MAX_CRC_ERRORS)	//End Condition for recursion
		{
			uart_crc_error = 0;
			return 1;
		}
		else
		{
			return readReg(deviceadr,regadr,data);
		}
	}
}

uint8_t wakeUpSignal()
{
	uint8_t wakeupsignal[2] = {0x55,0x55};
	uartWrite(wakeupsignal, 2);

	return 0;
}



uint8_t serveWatchdog(uint8_t deviceadr)
{
	return writeReg(deviceadr, WDOG_CNT, 0x0002); //Reset The to ~30min
}


uint8_t configMultiread(uint8_t deviceadr, uint8_t numofcells, uint8_t blockvoltage, uint8_t exttemp, uint8_t temp_r, uint8_t int_temp, uint8_t scvm, uint8_t pcvm_stress)
{
	multiread_config = numofcells + (blockvoltage<<4) + (exttemp<<5) + (temp_r << 8) + (int_temp<<9) + (scvm << 10) + (pcvm_stress << 11);

	return writeReg(deviceadr, MULTI_READ_CFG, multiread_config);

}

uint8_t multiRead(uint8_t deviceadr, uint16_t* cellVoltages, uint16_t* blockVoltage, uint16_t* externalTemp, uint16_t* internalTemp, uint16_t* scvm, uint16_t* pcvm_stress)
{


	uint8_t multiread_cells = (multiread_config & 0xF);
	uint8_t multiread_bvm = ((multiread_config>>4) &0x01);
	uint8_t multiread_extemp = ((multiread_config>>5) &0x07);
	uint8_t multiread_temp_r = ((multiread_config>>8) &0x01);
	uint8_t multiread_int_temp = ((multiread_config>>9) &0x01);
	uint8_t multiread_scvm = ((multiread_config>>10) &0x01);
	uint8_t multiread_pcvm_stress = ((multiread_config>>11) &0x01);

	uint8_t n_multiread =  multiread_cells + multiread_bvm + multiread_extemp + multiread_temp_r + multiread_int_temp + 2*multiread_scvm + multiread_pcvm_stress;


	uint8_t responselength = n_multiread*5;
	uint8_t response[responselength];
	for(uint8_t n = 0; n < responselength; n++)
		response[n] = 0;



	//HAL_UART_Receive_DMA(tle_uart, response, responselength+4);

	uint8_t uartbuffer[4];
	uartbuffer[0] = SYNCHRONIZATION_FRAME;
	uartbuffer[1] = deviceadr & 0x3F;
	uartbuffer[2] = MULTI_READ;

	uartbuffer[3] = 0;
	uartbuffer[3] = calculateCRC8(uartbuffer, 4);

	/*uartWrite(uartbuffer, 4);
	uartRead(response, 1);*/
	HAL_StatusTypeDef status = readWrite(uartbuffer,4,response,responselength);
	//HAL_StatusTypeDef status = uartRead(response, 16);


	for(uint8_t n = 0; n < multiread_cells; n++)
	{
		cellVoltages[n] = (response[5*n+2]<<8) | (response[5*n+3]);
	}

	if(multiread_bvm)
	{
		uint8_t baseadr = 5*multiread_cells;
		*blockVoltage = (response[baseadr+2]<<8) | (response[baseadr+3]);
	}

	for(uint8_t n = 0; n < multiread_extemp; n++)
	{
		uint8_t baseadr = 5*multiread_cells + 5*multiread_bvm + 5*n;
		externalTemp[n] = ((response[baseadr+2]<<8) | (response[baseadr+3]));
	}

	if(multiread_int_temp)
	{
		uint8_t baseadr = 5*multiread_cells+ 5*multiread_bvm + 5*multiread_extemp;
		*internalTemp = ((response[baseadr+2]<<8) | (response[baseadr+3])) & 0x3FF;
	}

	if(multiread_scvm)
	{
		uint8_t baseadr = 5*(multiread_cells+ multiread_bvm + multiread_extemp+multiread_int_temp);
		scvm[0] = (response[baseadr+2]<<8) | (response[baseadr+3]);
		scvm[1] = (response[baseadr+7]<<8) | (response[baseadr+8]);
	}

	if(multiread_pcvm_stress)
	{
		uint8_t baseadr = 5*(multiread_cells+ multiread_bvm + multiread_extemp+multiread_int_temp+2*multiread_scvm);
		*pcvm_stress = (response[baseadr+2]<<8) | (response[baseadr+3]);
	}

	uint8_t crcFault = 0;

	for(uint8_t n = 0; n < n_multiread; n++)
	{
		uint8_t crc = calculateCRC8(&response[5*n], 5);
		if(crc != response[5*n])
		{
			crcFault++;
		}
	}

	if(crcFault == 0)
	{
		uart_crc_error = 0;
		return 0;
	}
	else
	{
		uart_crc_error++;
		if(uart_crc_error >= MAX_CRC_ERRORS)	//End Condition for recursion
		{
			return 1;
		}
		else
		{
			return multiRead(deviceadr, cellVoltages, blockVoltage, externalTemp, internalTemp, scvm, pcvm_stress);
			return 0;
		}
	}
}



uint8 calculateCRC8(uint8* data, uint8 datalen)
{
    uint8 currentcrc = INITAL_CRC8;
    if(datalen >= 3)
    {
        for(int i = 0; i < (datalen-1); i++)
        {
            for(int n = 0; n < 8;n++)
            {
                uint8 din = data[i] << (n);
                currentcrc = updateCRC8(currentcrc, din);
            }
        }

        //data[datalen-1] = currentcrc^0xFF;
        return currentcrc^0xFF;
    }
    else
    {
        return 1;
    }
}

uint8 updateCRC8(uint8 currentCRC, uint8 din)
{
    uint8 newCRC = 0;
    din = (din>>7) & 0x01;

    uint8 in0 = din ^ ((currentCRC >> 7) &0x01);
    uint8 in2 = in0 ^ ((currentCRC >> 1) &0x01);
    uint8 in3 = in0 ^ ((currentCRC >> 2) &0x01);
    uint8 in4 = in0 ^ ((currentCRC >> 3) &0x01);


    newCRC |= (currentCRC & (0x01<<6))<<1;
    newCRC |= (currentCRC & (0x01<<5))<<1;
    newCRC |= (currentCRC & (0x01<<4))<<1;
    newCRC |= in4 <<4;
    newCRC |= in3 <<3;
    newCRC |= in2 <<2;
    newCRC |= (currentCRC & (0x01<<0))<<1;
    newCRC |= in0 & 0x01;


    return newCRC;
}

uint8_t calculateCRC3(uint8_t data) //Calculate CRC for Response Frame
{
	/*
	data = (data) & 0x07;
	return crc3_table[data];
	*/
	volatile uint8_t data_opti_guarded = data;
	data_opti_guarded ^= (0b1011<<4);
	data_opti_guarded ^= (0b1011<<3);
	data_opti_guarded ^= (0b1011<<2);
	data_opti_guarded ^= (0b1011<<1);
	data_opti_guarded ^= (0b1011);
	data_opti_guarded &= 0x07;

	if(data_opti_guarded == 0)
	{
		return 0; //Checksum correct
	}
	else
	{
		return 1; //Checksum Error
	}
}

uint8_t uartWrite(uint8* buffer, uint8 datalen)
{
	HAL_HalfDuplex_EnableTransmitter(tle_uart);
	HAL_StatusTypeDef uartstatus = HAL_UART_Transmit(tle_uart, buffer, datalen, UART_TIMEOUT);
	HAL_HalfDuplex_EnableReceiver(tle_uart);
	return uartstatus;
}
uint8_t uartRead(uint8* buffer, uint8 datalen)
{
	HAL_HalfDuplex_EnableReceiver(tle_uart);
	HAL_StatusTypeDef uartstatus = HAL_UART_Receive(tle_uart, buffer, datalen, UART_TIMEOUT);
	return uartstatus;
}

HAL_StatusTypeDef readWrite(uint8_t* txbuffer, uint8_t txlen, uint8_t* rxbuffer, uint8_t rxlen)
{
	tx_cplt_flag = 0;
	rx_cplt_flag = 0;

	uint8_t uart_rx_buffer[rxlen+txlen];
	HAL_UART_Receive_IT(&huart4, uart_rx_buffer, rxlen+txlen);
	HAL_HalfDuplex_EnableTransmitter(tle_uart);
	HAL_UART_Transmit_IT(tle_uart, txbuffer, txlen);
	//HAL_HalfDuplex_EnableReceiver(tle_uart);
	uint32_t starttime = HAL_GetTick();
	while(!(tx_cplt_flag && rx_cplt_flag))
	{
		if((HAL_GetTick() - starttime) > UART_TIMEOUT)
		{
			HAL_UART_AbortReceive_IT(&huart4);
			return HAL_TIMEOUT;
		}
	}
	memcpy(rxbuffer, &uart_rx_buffer[txlen],rxlen);

	return HAL_OK;
}

uint8_t awaitRxCplt()
{
	uint32_t startTick = HAL_GetTick();
	uint32_t currentTick = startTick;

	while((tleuart_rxcplt == 0) && ((currentTick - startTick) < UART_TIMEOUT))
	{
		currentTick = HAL_GetTick();
	}

	if(tleuart_rxcplt == 1)
	{
		tleuart_rxcplt = 0;
		return 0;	//RX Successfull
	}
	else
	{
		tleuart_rxcplt = 0;
		return 1;	//Timeout
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart2)
	{
		HAL_HalfDuplex_EnableReceiver(huart);
		tx_cplt_flag = 1;
	}
	if(huart == &huart3)
	{
		rs485_tx_cplt_flag = 1;
	}
}

//TODO Refactor in Separate File
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart4)
	{
		rx_cplt_flag = 1;
	}
	if(huart == &huart3)
	{
		rs485_rx_cplt_flag = 1;
	}
}
/*
Implementation in UART_Console.c due to Conflict with Multiple Callbacks
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	uint8_t tleuart_rxcplt = 1;

}*/

