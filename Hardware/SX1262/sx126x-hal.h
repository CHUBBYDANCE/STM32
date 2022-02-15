/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2015 Semtech

Description: Handling of the node configuration protocol

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis, Gregory Cristian & Gilbert Menth
*/
#ifndef __SX126x_HAL_H__
#define __SX126x_HAL_H__


#include "radio.h"
#include "main.h"
/*!
 * \brief Constructor for SX126xHal with SPI support
 *
 * Represents the physical connectivity with the radio and set callback functions on radio interrupts
 */
void SX126xHalInit(SPI_HandleTypeDef* hspi, RadioCallbacks_t * callbacks);

/*!
 * \brief Destructor for SX126xHal
 *
 * Take care of the correct destruction of the communication objects
 */
void SX126xHalDeinit(void);

/*!
 * \brief Soft resets the radio
 */
void SX126xReset(void);

/*!
 * \brief Wakes up the radio
 */
void SX126xWakeup(void);

/*!
 * \brief Send a command that write data to the radio
 *
 * \param [in]  opcode        Opcode of the command
 * \param [in]  buffer        Buffer to be send to the radio
 * \param [in]  size          Size of the buffer to send
 */
void SX126xWriteCommand(RadioCommands_t opcode, uint8_t* buffer, uint16_t size);

/*!
 * \brief Send a command that read data from the radio
 *
 * \param [in]  opcode        Opcode of the command
 * \param [out] buffer        Buffer holding data from the radio
 * \param [in]  size          Size of the buffer
 */
void SX126xReadCommand(RadioCommands_t opcode, uint8_t* buffer, uint16_t size);

/*!
 * \brief Write data to the radio memory
 *
 * \param [in]  address       The address of the first byte to write in the radio
 * \param [in]  buffer        The data to be written in radio's memory
 * \param [in]  size          The number of bytes to write in radio's memory
 */
void SX126xWriteRegister(uint16_t address, uint8_t* buffer, uint16_t size);

/*!
 * \brief Write a single byte of data to the radio memory
 *
 * \param [in]  address       The address of the first byte to write in the radio
 * \param [in]  value         The data to be written in radio's memory
 */
void SX126xWriteReg(uint16_t address, uint8_t value);

/*!
 * \brief Read data from the radio memory
 *
 * \param [in]  address       The address of the first byte to read from the radio
 * \param [out] buffer        The buffer that holds data read from radio
 * \param [in]  size          The number of bytes to read from radio's memory
 */
void SX126xReadRegister(uint16_t address, uint8_t* buffer, uint16_t size);

/*!
 * \brief Read a single byte of data from the radio memory
 *
 * \param [in]  address       The address of the first byte to write in the
 *                            radio
 *
 * \retval      value         The value of the byte at the given address in
 *                            radio's memory
 */
uint8_t SX126xReadReg(uint16_t address);

/*!
 * \brief Write data to the buffer holding the payload in the radio
 *
 * \param [in]  offset        The offset to start writing the payload
 * \param [in]  buffer        The data to be written (the payload)
 * \param [in]  size          The number of byte to be written
 */
void SX126xWriteBuffer(uint8_t offset, uint8_t* buffer, uint8_t size);

/*!
 * \brief Read data from the buffer holding the payload in the radio
 *
 * \param [in]  offset        The offset to start reading the payload
 * \param [out] buffer        A pointer to a buffer holding the data from the radio
 * \param [in]  size          The number of byte to be read
 */
void SX126xReadBuffer(uint8_t offset, uint8_t* buffer, uint8_t size);

/*!
 * \brief Returns the status of DIOs pins
 *
 * \retval      dioStatus     A byte where each bit represents a DIO state:
 *                            [ DIO3 | DIO2 | DIO1 | BUSY ]
 */
uint8_t SX126xGetDioStatus(void);

/*!
 * \brief Returns the device type
 *
 * \retval      0: SX1261, 1: SX1262, 2: SX1268
 */
uint8_t SX126xGetDeviceType(void);

/*!
 * \brief Returns the matching frequency
 *
 * \retval      1: 868 MHz
 *              0: 915 MHz
 */
uint8_t SX126xGetFreqSelect(void);

/*!
 * \brief RF Switch power on
 */
void SX126xAntSwOn(void);

/*!
 * \brief RF Switch power off
 */
void SX126xAntSwOff(void);

void SX126x_wait_ms(uint16_t ms);
void _disable_allirq(void);
void _enable_allirq(void);

#endif
