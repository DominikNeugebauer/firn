/**
 * waveleds.c - Waveform LEDs
 * Dominik Neugebauer
 *
 * One LED per waveform, connected to the MCP23017 GPIO expander.
 * Only the LED of the selected waveform is on.
 */
#include "waveleds.h"
#include "mcp23017.h"

#define NUM_LEDS 4

// LED i belongs to waveform i (see SYNTH_WAVEFORM_... in synth.h)
static const MCPGPIO s_leds[NUM_LEDS] = {
    { MCPGPIOB, 0 },  // sine
    { MCPGPIOB, 1 },  // sawtooth
    { MCPGPIOB, 2 },  // square
    { MCPGPIOB, 3 },  // triangle
};

HAL_StatusTypeDef WaveLeds_Init(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef st = MCP23017_Init(hi2c);
    if (st != HAL_OK)
        return st;

    for (int i = 0; i < NUM_LEDS; i++) {
        // set the latch low first, so the LED does not flash when it becomes an output
        st = MCP23017_SetGPIO(s_leds[i], 0);
        if (st != HAL_OK)
            return st;

        st = MCP23017_SetGPIODirection(s_leds[i], 0);  // 0 = output
        if (st != HAL_OK)
            return st;
    }

    return HAL_OK;
}

HAL_StatusTypeDef WaveLeds_Show(uint8_t waveform)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        HAL_StatusTypeDef st = MCP23017_SetGPIO(s_leds[i], i == waveform);
        if (st != HAL_OK)
            return st;
    }

    return HAL_OK;
}
