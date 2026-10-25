/**
 * shapelfo.h - Shape LFO (low frequency oscillator)
 * Dominik Neugebauer
 *
 * A slow sine wave that moves the shape of the waveform away
 * from the value that is set and back. The output goes
 * from 0 to depth and back to 0.
 */
#ifndef SHAPELFO_H
#define SHAPELFO_H

#include <stdint.h>

typedef struct {
    float phase;  // position in the sine, 0.0 ... 1.0
    float inc;    // phase increment per sample, calculated from the rate
    float depth;  // how far the shape is moved
} ShapeLFO;

// Prepare LFO. Call this once before using the LFO.
void ShapeLFO_Init(ShapeLFO *l, float rate_hz, float depth);

// Set the speed in Hz, for example 0.5 = one cycle in 2 seconds. 0 = off
void ShapeLFO_SetRate(ShapeLFO *l, float rate_hz);

// Set how far the shape is moved. 0.0 = no movement
void ShapeLFO_SetDepth(ShapeLFO *l, float depth);

// Moves the LFO forward by the given number of samples and
// returns the current value (0.0 ... depth)
float ShapeLFO_Process(ShapeLFO *l, uint32_t samples);

#endif /* SHAPELFO_H */
