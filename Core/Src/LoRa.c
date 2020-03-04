/*
 * LoRa.c
 *
 *  Created on: Mar 4, 2020
 *      Author: miked
 */
#include "main.h"
#include "LoRa.h"

/**
 * Переменная, отвечающая за ошибки SPI
 */
HAL_StatusTypeDef rc;

uint16_t Select_chip = GPIO_PIN_4;
SPI_HandleTypeDef spi;

uint8_t OpMode = 0x01;
uint8_t IrqFlags = 0x12;

uint8_t Sub[1] = {0};
/**
 * Функция инициализации LoRa
 */
void LoRa_Init(SPI_HandleTypeDef hspi1){
	spi = hspi1;
	LoRa_Sleep();
	HAL_Delay(100);
	LoRa_Write(OpMode, 0x80);
	HAL_Delay(100);
	LoRa_STD();
	HAL_Delay(100);
}

uint8_t isTransmitting(){
	LoRa_Read(OpMode, Sub);
	if ((Sub[0] & 0x03) == 0x03){
		if ((Sub[0] & 0x07) == 0x07){
			//CaD Detection
		}else{
			return 1;
		}
	}
	LoRa_Read(IrqFlags, Sub);
	if ((Sub[0] & 0x08) == 0x08){
		LoRa_Write(IrqFlags, Sub[0]);
	}
	return 0;
}


void LoRa_Sleep(){
	LoRa_Read(OpMode, Sub);
	Sub[0] = (Sub[0] | 0x80) & 0xfe;
	LoRa_Write(OpMode, Sub[0]);
}

void LoRa_STD(){
	LoRa_Read(OpMode, Sub);
	Sub[0] = (Sub[0] | 0x81);
	LoRa_Write(OpMode, Sub[0]);
}

uint8_t reg[2] = {0x00, 0x00};
uint8_t answer[2] = {0x00, 0x00};
/**
 * Функция записи данных в LoRa
 */
void LoRa_Write(uint8_t Reg, uint8_t Change){
	HAL_GPIO_WritePin(GPIOA, Select_chip, GPIO_PIN_RESET);
	reg[0] = Reg | 0x80;
	reg[1] = Change;
	rc = HAL_SPI_Transmit(&spi, reg, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOA, Select_chip, GPIO_PIN_SET);
}


/**
 * Функция чтения регистра из LoRa
 */
void LoRa_Read(uint8_t Reg, uint8_t* Ans){
	HAL_GPIO_WritePin(GPIOA, Select_chip, GPIO_PIN_RESET);
	reg[0] = Reg & 0x7f; // при отладке возникла проблема, что между опусканием SS и отправкой данных слишком мало времени
	reg[1] = 0x00;
	rc = HAL_SPI_Transmit(&spi, reg, 1, HAL_MAX_DELAY);
	rc = HAL_SPI_Receive(&spi, Ans, 1, HAL_MAX_DELAY);
 	HAL_GPIO_WritePin(GPIOA, Select_chip, GPIO_PIN_SET);
}
