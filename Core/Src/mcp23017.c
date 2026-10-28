#include "mcp23017.h"

#define MCP23017_ADDR       0x40

/* Register */
#define IODIRA              0x00 // set direction
#define IODIRB              0x01 // set direction
#define GPIOREGA            0x12
#define GPIOREGB            0x13
#define OLATA               0x14 
#define OLATB               0x15
#define GPPUA               0x0c  // set pullup
#define GPPUB               0x0d  // set pullup

static I2C_HandleTypeDef *s_i2c = NULL;

static HAL_StatusTypeDef reg_write(uint8_t reg, uint8_t val)
{
    return HAL_I2C_Mem_Write(s_i2c, MCP23017_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 100);
}

static HAL_StatusTypeDef reg_read(uint8_t reg, uint8_t *val)
{
    return HAL_I2C_Mem_Read(s_i2c, MCP23017_ADDR, reg, I2C_MEMADD_SIZE_8BIT, val, 1, 100);
}

// MCP23017

HAL_StatusTypeDef MCP23017_Init(I2C_HandleTypeDef *hi2c) {
    int err = 0;
    uint8_t val = 0;
    s_i2c = hi2c;

    return HAL_OK;
}

HAL_StatusTypeDef MCP23017_SetGPIO(MCPGPIO gpio, uint8_t val) {
    if (s_i2c == NULL || gpio.prefix > MCPGPIOB || gpio.pin > 7)
        return HAL_ERROR;

    uint8_t reg_addr = OLATA + gpio.prefix;
    uint8_t reg = 0;

    HAL_StatusTypeDef st = reg_read(reg_addr, &reg);
    if (st != HAL_OK)
        return st;

    if (val)
        reg |= 1u << gpio.pin;
    else
        reg &= ~(1u << gpio.pin);

    return reg_write(reg_addr, reg);
}

HAL_StatusTypeDef MCP23017_SetGPIODirection(MCPGPIO gpio, uint8_t direction) {
    if (s_i2c == NULL || gpio.prefix > MCPGPIOB || gpio.pin > 7)
        return HAL_ERROR;

    uint8_t reg_addr = IODIRA + gpio.prefix;
    uint8_t reg = 0;

    HAL_StatusTypeDef st = reg_read(reg_addr, &reg);
    if (st != HAL_OK)
        return st;

    if (direction)
        reg |= 1u << gpio.pin;      // 1 = input
    else
        reg &= ~(1u << gpio.pin);   // 0 = output

    return reg_write(reg_addr, reg);
}

HAL_StatusTypeDef MCP23017_GetGPIO(MCPGPIO gpio, uint8_t* val) {
    if (s_i2c == NULL || gpio.prefix > MCPGPIOB || gpio.pin > 7)
        return HAL_ERROR;
    
    uint8_t reg_addr = GPIOREGA + gpio.prefix;
    uint8_t reg = 0;

    HAL_StatusTypeDef st = reg_read(reg_addr, &reg);
    if (st != HAL_OK)
        return st;

    *val = (reg >> gpio.pin) & 0x01;

    return st;
}
HAL_StatusTypeDef MCP23017_SetGPIOPullup(MCPGPIO gpio) {
    if (s_i2c == NULL || gpio.prefix > MCPGPIOB || gpio.pin > 7)
        return HAL_ERROR;

    uint8_t reg_addr = GPPUA + gpio.prefix;
    uint8_t reg = 0;

    HAL_StatusTypeDef st = reg_read(reg_addr, &reg);
    if (st != HAL_OK)
        return st;

    reg |= 1u << gpio.pin;

    return reg_write(reg_addr, reg);
}
