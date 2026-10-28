/**
 * waveleds.h - Waveform LEDs
 * Dominik Neugebauer
 *
 * One LED per waveform, connected to the MCP23017 GPIO expander.
 * Only the LED of the selected waveform is on.
 */
#ifndef WAVELEDS_H
#define WAVELEDS_H

#include <stdint.h>
#include "main.h"

// Setup the MCP23017 and turn all LEDs off.
// Call this once after the I2C is initialized.
HAL_StatusTypeDef WaveLeds_Init(I2C_HandleTypeDef *hi2c);

// Turn on the LED of this waveform (SYNTH_WAVEFORM_...), all others off
HAL_StatusTypeDef WaveLeds_Show(uint8_t waveform);

#endif /* WAVELEDS_H */
