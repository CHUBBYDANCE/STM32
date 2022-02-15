/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
	(C)2016 Semtech

Description: Handling of the node configuration protocol

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis, Gregory Cristian and Matthieu Verdy
*/
#include "sx126x-hal.h"
#include "sx126x.h"
#include <stddef.h>

#include "delay.h"
#include "sys.h"


#define SX126x_NSS_SET		HAL_GPIO_WritePin(SX126x_NSS_GPIO_Port, SX126x_NSS_Pin, GPIO_PIN_SET)
#define SX126x_NSS_RESET	HAL_GPIO_WritePin(SX126x_NSS_GPIO_Port, SX126x_NSS_Pin, GPIO_PIN_RESET)

// IO口等
//#define SX126x_DIO1_Pin				GPIO_PIN_1
//#define SX126x_DIO1_GPIO_Port		GPIOA
//#define SX126x_DIO1_EXTI_IRQn		EXTI1_IRQn
//#define SX126x_SPI_NSS_Pin			GPIO_PIN_4
//#define SX126x_SPI_NSS_GPIO_Port	GPIOA
//#define SX126x_RESET_Pin			GPIO_PIN_11
//#define SX126x_RESET_GPIO_Port		GPIOA
//#define SX126x_BUSY_Pin				GPIO_PIN_12
//#define SX126x_BUSY_GPIO_Port		GPIOA
//#define SX126x_Antsw_Pin			NULL
//#define SX126x_Antsw_GPIO_Port		GPIOA

//uint16_t  DIO2, DIO3, antSwitchPower;
SPI_HandleTypeDef* RadioSpi;


/*!
 * \brief Used to block execution waiting for low state on radio busy pin.
 */
#define WaitOnBusy( )             while(GPIO_PIN_SET ==  HAL_GPIO_ReadPin(SX126x_BUSY_GPIO_Port, SX126x_BUSY_Pin)){ }

 /*!
  * \brief Used to block execution to give enough time to Busy to go up
  *        in order to respect Tsw, see datasheet ยง8.3.1
  */
#define WaitOnCounter( )          for( uint8_t counter = 0; counter < 15; counter++ ) \
									{  asm("NOP"); }

void SX126xHalInit(SPI_HandleTypeDef* hspi, RadioCallbacks_t * callbacks)
{
	RadioSpi = hspi;
	SX126xInit(callbacks);
}

void SX126xHalDeinit(void)
{
	if (RadioSpi != NULL)
	{
		//delete RadioSpi;
		RadioSpi = NULL;
	}
	/*if(DIO1 != NULL)
	{
		//delete DIO1;
		dios |= DIO1;
	}
	if(DIO2 != NULL)
	{
		//delete DIO2;
		dios |= DIO2;
	}
	if(DIO3 != NULL)
	{
		//delete DIO3;
		dios |= DIO3;
	}
	if ((DIO1 | BUSY) != NULL)
	{
		HAL_GPIO_DeInit(GPIO_Radio, DIO1 | BUSY);
	}*/
	//HAL_GPIO_DeInit(SX126x_DIO1_GPIO_Port, SX126x_DIO1_Pin);
	//HAL_GPIO_DeInit(SX126x_SPI_NSS_GPIO_Port, SX126x_SPI_NSS_Pin);
}

void SX126xReset(void)
{
	//__disable_irq();
	_disable_allirq();
	//wait_ms(20);
	SX126x_wait_ms(20);
	//RadioReset.output();
	//RadioReset = 0;
	SX126x_NSS_RESET;
	//wait_ms(50);
	SX126x_wait_ms(20);
	//RadioReset = 1;
	//RadioReset.input();  // Using the internal pull-up
	SX126x_NSS_SET;
	//wait_ms(20);
	SX126x_wait_ms(20);
	//__enable_irq();
	_enable_allirq();
}

void SX126xWakeup(void)
{
	//__disable_irq();
	_disable_allirq();
	//Don't wait for BUSY here

	if (RadioSpi != NULL)
	{
		uint8_t pTxbuff[2] = { RADIO_GET_STATUS, 0 };
		//RadioNss = 0;
		SX126x_NSS_RESET;
		//RadioSpi->write(RADIO_GET_STATUS);
		//RadioSpi->write(0);
		HAL_SPI_Transmit(RadioSpi, pTxbuff, 2, 2 * 0x50);
		//RadioNss = 1;
		SX126x_NSS_SET;
	}

	// Wait for chip to be ready.
	WaitOnBusy();

	//__enable_irq();
	_enable_allirq();
	SX126xAntSwOn();
}

void SX126xWriteCommand(RadioCommands_t command, uint8_t* buffer, uint16_t size)
{
#ifdef ADV_DEBUG
	printf("cmd: 0x%02x", command);
	for (uint8_t i = 0; i < size; i++)
	{
		printf("-%02x", buffer[i]);
	}
	printf("\n\r");
#endif

	WaitOnBusy();

	if (RadioSpi != NULL)
	{
		//RadioNss = 0;
		SX126x_NSS_RESET;
		//RadioSpi->write((uint8_t)command);
		HAL_SPI_Transmit(RadioSpi, (uint8_t*)&command, 1, 1 * 0x50);
		//for(uint16_t i = 0; i < size; i++)
		//{
		//	RadioSpi->write(buffer[i]);
		//}
		HAL_SPI_Transmit(RadioSpi, buffer, size, size * 0x50);
		//RadioNss = 1;
		SX126x_NSS_SET;
	}
	WaitOnCounter();
}

void SX126xReadCommand(RadioCommands_t command, uint8_t* buffer, uint16_t size)
{
	WaitOnBusy();

	if (RadioSpi != NULL)
	{
		uint8_t Txbuff = 0;
		//RadioNss = 0;
		SX126x_NSS_RESET;
		//RadioSpi->write((uint8_t)command);
		HAL_SPI_Transmit(RadioSpi, (uint8_t*)&command, 1, 1 * 0x50);
		//RadioSpi->write(0);
		HAL_SPI_Transmit(RadioSpi, &Txbuff, 1, 1 * 0x50);
		//for(uint16_t i = 0; i < size; i++)
		//{
		//	buffer[i] = RadioSpi->write(0);
		//}
		HAL_SPI_Receive(RadioSpi, buffer, size, size * 0x50);
		//RadioNss = 1;
		SX126x_NSS_SET;
	}
}

void SX126xWriteRegister(uint16_t address, uint8_t* buffer, uint16_t size)
{
	WaitOnBusy();

	if (RadioSpi != NULL)
	{
		uint8_t Txbuff[3] = { RADIO_WRITE_REGISTER, (address & 0xFF00) >> 8, address & 0x00FF };
		//RadioNss = 0;
		SX126x_NSS_RESET;
		//RadioSpi->write(RADIO_WRITE_REGISTER);
		//RadioSpi->write((address & 0xFF00) >> 8);
		//RadioSpi->write(address & 0x00FF);
		HAL_SPI_Transmit(RadioSpi, Txbuff, 3, 3 * 0x50);
		//for(uint16_t i = 0; i < size; i++)
		//{
		//	RadioSpi->write(buffer[i]);
		//}
		HAL_SPI_Transmit(RadioSpi, buffer, size, size * 0x50);
		//RadioNss = 1;
		SX126x_NSS_SET;
	}
}

void SX126xWriteReg(uint16_t address, uint8_t value)
{
	SX126xWriteRegister(address, &value, 1);
}

void SX126xReadRegister(uint16_t address, uint8_t* buffer, uint16_t size)
{
	WaitOnBusy();

	if (RadioSpi != NULL)
	{
		uint8_t Txbuff[4] = { RADIO_READ_REGISTER, (address & 0xFF00) >> 8, address & 0x00FF, 0 };
		//RadioNss = 0;
		SX126x_NSS_RESET;
		//RadioSpi->write(RADIO_READ_REGISTER);
		//RadioSpi->write((address & 0xFF00) >> 8);
		//RadioSpi->write(address & 0x00FF);
		//RadioSpi->write(0);
		HAL_SPI_Transmit(RadioSpi, Txbuff, 4, 4 * 0x50);
		//for(uint16_t i = 0; i < size; i++)
		//{
		//	buffer[i] = RadioSpi->write(0);
		//}
		HAL_SPI_Receive(RadioSpi, buffer, size, size * 0x50);
		//RadioNss = 1;
		SX126x_NSS_SET;
	}
}

uint8_t SX126xReadReg(uint16_t address)
{
	uint8_t data;

	SX126xReadRegister(address, &data, 1);
	return data;
}

void SX126xWriteBuffer(uint8_t offset, uint8_t* buffer, uint8_t size)
{
	WaitOnBusy();

	if (RadioSpi != NULL)
	{
		uint8_t Txbuff[2] = { RADIO_WRITE_BUFFER, offset };
		//RadioNss = 0;
		SX126x_NSS_RESET;
		//RadioSpi->write(RADIO_WRITE_BUFFER);
		//RadioSpi->write(offset);
		HAL_SPI_Transmit(RadioSpi, Txbuff, 2, 2 * 0x50);
		//for(uint16_t i = 0; i < size; i++)
		//{
		//	RadioSpi->write(buffer[i]);
		//}
		HAL_SPI_Transmit(RadioSpi, buffer, size, size * 0x50);
		//RadioNss = 1;
		SX126x_NSS_SET;
	}
}

void SX126xReadBuffer(uint8_t offset, uint8_t* buffer, uint8_t size)
{
	WaitOnBusy();

	if (RadioSpi != NULL)
	{
		uint8_t Txbuff[3] = { RADIO_READ_BUFFER, offset, 0 };
		//RadioNss = 0;
		SX126x_NSS_RESET;
		//RadioSpi->write(RADIO_READ_BUFFER);
		//RadioSpi->write(offset);
		//RadioSpi->write(0);
		HAL_SPI_Transmit(RadioSpi, Txbuff, 3, 3 * 0x50);
		//for(uint16_t i = 0; i < size; i++)
		//{
		//	buffer[i] = RadioSpi->write(0);
		//}
		HAL_SPI_Receive(RadioSpi, buffer, size, size * 0x50);
		//RadioNss = 1;
		SX126x_NSS_SET;
	}

}

uint8_t SX126xGetDioStatus(void)
{
	uint8_t tmp = 0;
	tmp = HAL_GPIO_ReadPin(SX126x_DIO1_GPIO_Port, SX126x_DIO1_Pin) << 1 | \
		HAL_GPIO_ReadPin(SX126x_BUSY_GPIO_Port, SX126x_BUSY_Pin);
	//return (*DIO3 << 3) | (*DIO2 << 2) | (*DIO1 << 1) | (BUSY << 0);
	return tmp;
}

uint8_t SX126xGetDeviceType(void)
{
	//uint16_t val = 0;
	//val = DeviceSelect.read_u16();

	/*if(val <= 0x2000)
	{
		return(SX1262);
	}
	else if(val <= 0xA000)
	{
		return(SX1268);
	}
	else
	{
		return(SX1261);
	}*/
	return SX1262;
}

uint8_t SX126xGetFreqSelect(void)
{
	/*uint16_t val = 0;
	val = FreqSelect.read_u16();

	if(val < 100)
	{
		return(MATCHING_FREQ_915);
	}
	else if(val <= 0x3000)
	{
		return(MATCHING_FREQ_780);
	}
	else if(val <= 0x4900)         // 0x4724
	{
		return(MATCHING_FREQ_490);
	}
	else if(val <= 1)
	{
		return(MATCHING_FREQ_434);
	}
	else if(val <= 1)
	{
		return(MATCHING_FREQ_280);
	}
	else if(val <= 0xF000)
	{
		return(MATCHING_FREQ_169);
	}
	else
	{
		return(MATCHING_FREQ_868);
	}*/
	return MATCHING_FREQ_434;
}

void SX126xAntSwOn(void)
{
	//antSwitchPower = 1;
	//HAL_GPIO_WritePin(SX126x_Antsw_GPIO_Port, SX126x_Antsw_Pin, GPIO_PIN_SET);
}

void SX126xAntSwOff(void)
{
	//antSwitchPower = 0;
	//HAL_GPIO_WritePin(SX126x_Antsw_GPIO_Port, SX126x_Antsw_Pin, GPIO_PIN_RESET);
}

void SX126x_wait_ms(uint16_t ms)
{
	delay_ms(ms);
}

void _disable_allirq(void)
{
	INTX_DISABLE();
}

void _enable_allirq(void)
{
	INTX_ENABLE();
}