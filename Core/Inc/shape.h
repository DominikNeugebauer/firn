/**
 * shape.h - Waveform shaping
 * Dominik Neugebauer
 *
 * Changes the shape of a waveform with one amount value.
 * 0.0 = normal waveform, 1.0 = strongest change.
 */
#ifndef SHAPE_H
#define SHAPE_H

#include <stdint.h>

typedef struct {
    float width;   // current width, glides to target
    float target;  // wanted width, 0.5 = unchanged waveform
} Shape;

// Prepare shaper. Call this once before using the shaper.
void Shape_Init(Shape *s, float amount);

// Set the amount in range 0.0 ... 1.0. 0.0 = normal, 1.0 = strongest
void Shape_SetAmount(Shape *s, float amount);

// Returns one sample of the shaped waveform for the given phase (0.0 ... 1.0).
// inc is the phase increment per sample (frequency / sample rate).
// waveform is one of the SYNTH_WAVEFORM_* values.
float Shape_Process(Shape *s, float phase, float inc, uint8_t waveform);

#endif /* SHAPE_H */
