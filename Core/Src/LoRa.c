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

const uint8_t Fifo = 0x00;
const uint8_t OpMode = 0x01;
const uint8_t FrMsb = 0x06;
const uint8_t FrMid = 0x07;
const uint8_t FrLsb = 0x08;
const uint8_t PaConfig = 0x09;
const uint8_t PaRamp = 0x0A;
const uint8_t Ocp = 0x0B;
const uint8_t Lna = 0x0C;
const uint8_t FifoAddrPtr = 0x0D;
const uint8_t FifoTxBaseAddr = 0x0E;
const uint8_t FifoRxBaseAddr = 0x0F;
const uint8_t FifoRxCurrentAddr = 0x10;
const uint8_t IrqFlagsMask = 0x11;
const uint8_t IrqFlags = 0x12;
const uint8_t RxNbBytes = 0x13;
const uint8_t RxHeaderCntValueMsb = 0x14;
const uint8_t RxHeaderCntValueLsb = 0x15;
const uint8_t RxPacketCntValueMsb = 0x16;
const uint8_t RxPacketCntValueLsb = 0x17;
const uint8_t ModemStat = 0x18;
const uint8_t PktSnrValue = 0x19;
const uint8_t PktRssiValue = 0x1A;
const uint8_t RssiValue = 0x1B;
const uint8_t HopChannel = 0x1C;
const uint8_t ModemConfig1 = 0x1D;
const uint8_t ModemConfig2 = 0x1E;
const uint8_t SymbTimeoutLsb = 0x1F;
const uint8_t PreambleMsb = 0x20;
const uint8_t PreambleLsb = 0x21;
const uint8_t PayloadLength = 0x22;
const uint8_t MaxPayloadLength = 0x23;
const uint8_t HopPeriod = 0x24;
const uint8_t FifoRxuint8_tAddr = 0x25;
const uint8_t ModemConfig3 = 0x26;
const uint8_t PpmCorrection = 0x27;
const uint8_t FeiMasb = 0x28;
const uint8_t FeiMid = 0x29;
const uint8_t FeiLsb = 0x2A;
const uint8_t RssiWideband = 0x2C;
const uint8_t DetectOptimize = 0x31;
const uint8_t InvertIQ = 0x33;
const uint8_t DetectionThreshold = 0x37;
const uint8_t SyncWord = 0x39;

uint8_t mode = 0;
uint16_t RESET_LORA = 7;

uint8_t Sub[1] = {0};


/**
 * Функция инициализации LoRa
 */
void LoRa_Init(SPI_HandleTypeDef hspi1){
	spi = hspi1;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); //Установить RESET LoRa в 0
	HAL_Delay(200);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); //Установить RESET LoRa в 0
	HAL_Delay(200);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); //Установить RESET LoRa в 0
	HAL_Delay(50);

	LoRa_Sleep();
	HAL_Delay(100);
	LoRa_Write(OpMode, 0x80);
	HAL_Delay(100);
	LoRa_Write(FifoTxBaseAddr, 0);
	LoRa_Write(FifoRxBaseAddr, 0);
	LoRa_Write(ModemConfig3, 0x04);
	setTx(17);
	LoRa_DisableCRC();
	LoRa_setSpreadingfactor(11);

	LoRa_Read(ModemConfig2, Sub);
	Sub[0] = Sub[0] & 0xFE;
	LoRa_Write(ModemConfig2, Sub[0]);

	LoRa_Write(SymbTimeoutLsb, 100);
	LoRa_setFrequency(433000000);
	LoRa_changePreamble(128);
	LoRa_Write(ModemConfig1, 114);
	SyncWordSet(0x12);
	LoRa_STD();
	HAL_Delay(100);
}

void LoRa_expMode(){
	mode = 0;
	LoRa_Read(ModemConfig1, Sub);
	Sub[0] = Sub[0] & 0xFE;
	LoRa_Write(ModemConfig1, Sub[0]);
}

void SyncWordSet(uint8_t word){
	LoRa_Write(SyncWord, word);
}

int packcount = 0;

int available(){
	LoRa_Read(RxNbBytes, Sub);
	return (Sub[0] - packcount);
}

void LoRa_changePreamble(uint16_t length){
	Sub[0] = length >> 8;
	LoRa_Write(PreambleMsb, Sub[0]);
	Sub[0] = length >> 0;
	LoRa_Write(PreambleLsb, Sub[0]);
}

uint8_t  parsePacket (uint8_t size){
	uint8_t packL = 0;
	uint8_t irqF = 0;
	LoRa_Read(IrqFlags, Sub);
	irqF = Sub[0];
	if (size > 0){
		LoRa_impMode();
		LoRa_Write(PayloadLength, size & 0xFF);
	}else {
		LoRa_expMode();
	}
	LoRa_Write(IrqFlags, irqF);
	if (((irqF & 0x40) == 0x40) && ((irqF & 0x20) == 0)){
		packcount = 0;
		if (mode == 0){
			LoRa_Read(RxNbBytes, Sub);
		} else {
			LoRa_Read(PayloadLength, Sub);
		}
		packL = Sub[0];
		LoRa_Read(FifoRxCurrentAddr, Sub);
		LoRa_Write(FifoAddrPtr, Sub[0]);
		LoRa_STD();
	}else {
		LoRa_Read(OpMode, Sub);
		if (Sub[0] != (0x80 | 0x06)){
			LoRa_Write(FifoAddrPtr, 0x00);
			LoRa_Write(OpMode, 0x80 | 0x06);
		}
	}
	return packL;
}

void recive(){
	while (1){
		uint8_t pP = parsePacket(0);
		if (pP > 0){
			char buf[pP];
			int i = 0;
			while (available() > 0){
				buf[i] = ReadPackage();
				i++;
			}
		}
	}
}
uint8_t ReadPackage(){
	if (available() <= 0) return 0;
	packcount += 1;
	LoRa_Read(Fifo, Sub);
	return Sub[0];
}

uint8_t headerPacket(uint8_t header){
	if (isTransmitting() == 1) return 0;
	LoRa_STD();
	if (header == 1) LoRa_impMode();
	else LoRa_expMode();
	LoRa_Write(FifoAddrPtr, 0);
	LoRa_Write(PayloadLength, 0);
	return 1;
}

uint8_t endPacket(){
	LoRa_Write(OpMode, 0x80 | 0x03);
	LoRa_Read(IrqFlags, Sub);
	while ((Sub[0] & 0x08) == 0){
		LoRa_Read(IrqFlags, Sub);
	}
	LoRa_Write(IrqFlags, 0x08);
	return 1;
}

void setTx(int power){
	int pa_pin = 1;
	if (pa_pin == 0){
		if (power < 0){
			power = 0;
		}else if (power > 14) power = 14;
		Sub[0] = (0x70 | power);
		LoRa_Write(PaConfig, Sub[0]);
	}else {
		if (power > 17){
			if (power > 20){
				power = 20;
			}
			power = power - 3;
			LoRa_Write(0x4d, 0x87);
			setOCP(140);
		}else {
			if (power < 2){
				power = 2;
			}
			LoRa_Write(0x4d, 0x84);
			setOCP(100);
		}
		Sub[0] = (0x80 | (power - 2));
		LoRa_Write(PaConfig, Sub[0]);
	}
}

void setOCP(uint8_t power){
	uint8_t tr = 27;
	if (power < 120){
		tr = ((power - 45) / 5);
	}else if (power <= 240) { tr = ((power + 30) / 10);}
	Sub[0] = (0x20 | (0x1F & tr));
	LoRa_Write(Ocp, Sub[0]);
}

void Write_Massage(char *buffer, uint8_t size){
	LoRa_Read(PayloadLength, Sub);
	if ((Sub[0] + size) > 255){
		size = 255 - Sub[0];
	}
	for (uint8_t i = 0; i < size; i++){
		LoRa_Write(Fifo, buffer[i]);
	}
	Sub[0] = Sub[0] + size;
	LoRa_Write(PayloadLength, Sub[0]);
}

void LoRa_impMode(){
	mode = 1;
	LoRa_Read(ModemConfig1, Sub);
	Sub[0] = Sub[0] | 0x01;
	LoRa_Write(ModemConfig1, Sub[0]);
}

void LoRa_EnableCRC(){
	LoRa_Read(ModemConfig2, Sub);
	Sub[0] = Sub[0] | 0x04;
	LoRa_Write(ModemConfig2, Sub[0]);
}

void LoRa_DisableCRC(){
	LoRa_Read(ModemConfig2, Sub);
	Sub[0] = Sub[0] & 0xfb;
	LoRa_Write(ModemConfig2, Sub[0]);
}

void LoRa_setFrequency(long frq){
	uint64_t fr = ((uint64_t)frq << 19) / 32000000;
	Sub[0] = fr >> 16;
	LoRa_Write(FrMsb, Sub[0]);
	Sub[0] = fr >> 8;
	LoRa_Write(FrMid, Sub[0]);
	Sub[0] = fr >> 0;
	LoRa_Write(FrLsb, Sub[0]);
}

void LoRa_setSpreadingfactor(uint8_t sf){
	if (sf < 6) sf = 6;
	else if (sf > 12) sf = 12;
	if (sf == 6){
		LoRa_Write(DetectOptimize, 0xC5);
		LoRa_Write(DetectionThreshold, 0x0C);
	}else {
		LoRa_Write(DetectOptimize, 0xC3);
		LoRa_Write(DetectionThreshold, 0x0A);
	}
	LoRa_Read(ModemConfig2, Sub);
	Sub[0] = (Sub[0] & 0x0F) | ((sf << 4) & 0xF0);
	LoRa_Write(ModemConfig2, Sub[0]);
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
	HAL_Delay(5);
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
	HAL_Delay(5);
	reg[0] = Reg & 0x7f; // при отладке возникла проблема, что между опусканием SS и отправкой данных слишком мало времени
	reg[1] = 0x00;
	rc = HAL_SPI_Transmit(&spi, reg, 1, HAL_MAX_DELAY);
	rc = HAL_SPI_Receive(&spi, Ans, 1, HAL_MAX_DELAY);
 	HAL_GPIO_WritePin(GPIOA, Select_chip, GPIO_PIN_SET);
}
