/**
 * cs43l22.h - CS43L22 Audio Codec Driver
 * Dominik Neugebauer
 * 
 * Control the CS43L22 audio codec via I2C. The audio data is sent via I2S.
 * Reset the codec via a GPIO pin (CubeMX label "CODEC_RESET").
 */
#ifndef CS43L22_H
#define CS43L22_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

HAL_StatusTypeDef CS43L22_Init(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef CS43L22_PowerOn(void);

HAL_StatusTypeDef CS43L22_PowerOff(void);

// in dB, range -102 ... +12
HAL_StatusTypeDef CS43L22_SetMasterVolume(int8_t db);

#endif /* CS43L22_H */
