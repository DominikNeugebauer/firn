/**
 * mcp23017.h - MCP23017 GPIO Expander Driver
 * Dominik Neugebauer
 * 
 * Control the MCP23017 i2c gpio expander chip
 */
#ifndef MCP23017_H
#define MCP23017_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define MCPGPIOA 0
#define MCPGPIOB 1

typedef struct {
    char prefix;
    uint8_t pin;
} MCPGPIO;

/**
 * Inititlaizes the mcp23017
 */
HAL_StatusTypeDef MCP23017_Init(I2C_HandleTypeDef *hi2c);

/**
 * Sets the direction of a specific gpio on the expander
 * direction = 1 -> input
 * direction = 0 -> output
 */
HAL_StatusTypeDef MCP23017_SetGPIODirection(MCPGPIO gpio, uint8_t direction);

/**
 * Sets a pullup for the defined gpio of the expander
 */
HAL_StatusTypeDef MCP23017_SetGPIOPullup(MCPGPIO gpio);

/**
 * Sets the output of an gpio of the expander
 * val = 1 -> HIGH
 * val = 0 -> LOW
 */
HAL_StatusTypeDef MCP23017_SetGPIO(MCPGPIO gpio, uint8_t val);

/**
 * Returns the value measured on the gpio
 */
HAL_StatusTypeDef MCP23017_GetGPIO(MCPGPIO gpio, uint8_t* val);

#endif /* MCP23017_H */
