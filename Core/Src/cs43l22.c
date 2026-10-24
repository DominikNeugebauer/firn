/**
 * cs43l22.c - CS43L22 Audio Codec Driver
 * Dominik Neugebauer
 * 
 * Control the CS43L22 audio codec via I2C. The audio data is sent via I2S.
 * Reset the codec via a GPIO pin (CubeMX label "CODEC_RESET").
 */
#include "cs43l22.h"
#include "main.h"

// I2C address of the CS43L22
#define CS43L22_ADDR            0x94

/* Register */
#define REG_ID                  0x01
#define REG_POWER_CTL1          0x02
#define REG_POWER_CTL2          0x04
#define REG_CLOCKING_CTL        0x05
#define REG_INTERFACE_CTL1      0x06
#define REG_MASTER_A_VOL        0x20
#define REG_MASTER_B_VOL        0x21
#define REG_HP_A_VOL            0x22
#define REG_HP_B_VOL            0x23

#define CS43L22_ID              0xE0
#define CS43L22_ID_MASK         0xF8

static I2C_HandleTypeDef *s_i2c = NULL;

static HAL_StatusTypeDef reg_write(uint8_t reg, uint8_t val)
{
    return HAL_I2C_Mem_Write(s_i2c, CS43L22_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 100);
}

static HAL_StatusTypeDef reg_read(uint8_t reg, uint8_t *val)
{
    return HAL_I2C_Mem_Read(s_i2c, CS43L22_ADDR, reg, I2C_MEMADD_SIZE_8BIT, val, 1, 100);
}

HAL_StatusTypeDef CS43L22_Init(I2C_HandleTypeDef *hi2c)
{
    int err = 0;
    uint8_t val = 0;

    s_i2c = hi2c;

    // Hardware reset via GPIO pin (CubeMX label "CODEC_RESET")
    HAL_GPIO_WritePin(CODEC_RESET_GPIO_Port, CODEC_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(CODEC_RESET_GPIO_Port, CODEC_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(5);

    // Check if the codec is present and has the correct ID
    if (reg_read(REG_ID, &val) != HAL_OK || (val & CS43L22_ID_MASK) != CS43L22_ID) {
        return HAL_ERROR;
    }

    // power off for configuration
    err |= reg_write(REG_POWER_CTL1, 0x01);

    // configure the codec registers according to the datasheet and ST BSP
    err |= reg_write(0x00, 0x99);
    err |= reg_write(0x47, 0x80);
    err |= reg_read(0x32, &val);
    err |= reg_write(0x32, val | 0x80);
    err |= reg_write(0x32, val & (uint8_t)~0x80);
    err |= reg_write(0x00, 0x00);

    // out only on headphone jack, speaker off
    err |= reg_write(REG_POWER_CTL2, 0xAF);

    // clocking: MCLK = 12.288 MHz, BCLK = 3.072 MHz, LRCLK = 48 kHz
    err |= reg_write(REG_CLOCKING_CTL, 0x81);

    // I2S format: 16-bit, I2S standard, slave mode
    err |= reg_write(REG_INTERFACE_CTL1, 0x04);

    // set volume to -24 dB
    err |= reg_write(REG_HP_A_VOL, 0x00);
    err |= reg_write(REG_HP_B_VOL, 0x00);
    err |= CS43L22_SetMasterVolume(-24);

    return err ? HAL_ERROR : HAL_OK;
}

HAL_StatusTypeDef CS43L22_PowerOn(void)
{
    return reg_write(REG_POWER_CTL1, 0x9E);
}

HAL_StatusTypeDef CS43L22_PowerOff(void)
{
    return reg_write(REG_POWER_CTL1, 0x01);
}

HAL_StatusTypeDef CS43L22_SetMasterVolume(int8_t db)
{
    if (db < -102) db = -102;
    if (db > 12)   db = 12;

    uint8_t reg = (uint8_t)(int8_t)(db * 2);

    int err = 0;
    err |= reg_write(REG_MASTER_A_VOL, reg);
    err |= reg_write(REG_MASTER_B_VOL, reg);
    return err ? HAL_ERROR : HAL_OK;
}
