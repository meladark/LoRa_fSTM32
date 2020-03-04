/*
 * LoRa.h
 *
 *  Created on: Mar 4, 2020
 *      Author: miked
 */

#ifndef INC_LORA_H_
#define INC_LORA_H_



#endif /* INC_LORA_H_ */
void LoRa_Sleep(void);
void LoRa_STD(void);
void LoRa_Write(uint8_t Reg, uint8_t Change);
void LoRa_Read(uint8_t Reg, uint8_t* Ans);
uint8_t isTransmitting();
