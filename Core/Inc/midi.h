/**
 * midi.h - MIDI input
 * Dominik Neugebauer
 *
 * Reads MIDI bytes from the UART (31250 baud) with an interrupt,
 * one byte at a time. The parser understands note on and note off
 * on all channels. The synth is mono, the last pressed key is played.
 */
#ifndef MIDI_H
#define MIDI_H

#include <stdint.h>
#include "main.h"

// Start receiving. Call this once after the UART is initialized.
void Midi_Init(UART_HandleTypeDef *huart);

// Call this from HAL_UART_RxCpltCallback
void Midi_RxCallback(UART_HandleTypeDef *huart);

// Call this from HAL_UART_ErrorCallback, it starts receiving again
void Midi_ErrorCallback(UART_HandleTypeDef *huart);

// 1 = a key is pressed, 0 = no key pressed
int Midi_GetGate(void);

// Frequency in Hz of the last pressed key
float Midi_GetFrequency(void);

#endif /* MIDI_H */
