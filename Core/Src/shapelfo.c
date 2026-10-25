/**
 * shapelfo.c - Shape LFO (low frequency oscillator)
 * Dominik Neugebauer
 *
 * A slow sine wave that moves the shape of the waveform away
 * from the value that is set and back. The output goes
 * from 0 to depth and back to 0.
 */
#include "shapelfo.h"
#include "synth.h"
#include <math.h>

#define TWO_PI 6.28318530718f

#define MIN_RATE  0.0f    // 0 = LFO off
#define MAX_RATE 20.0f    // above this it is not slow anymore


void ShapeLFO_Init(ShapeLFO *l, float rate_hz, float depth)
{
    l->phase = 0.0f;
    ShapeLFO_SetRate(l, rate_hz);
    ShapeLFO_SetDepth(l, depth);
}

void ShapeLFO_SetRate(ShapeLFO *l, float rate_hz)
{
    // keep the rate in a useful range
    if (rate_hz < MIN_RATE) rate_hz = MIN_RATE;
    if (rate_hz > MAX_RATE) rate_hz = MAX_RATE;

    // same as the oscillator: phase increment from frequency and sample rate
    l->inc = rate_hz / SYNTH_SAMPLE_RATE;

    // LFO off: go back to the start, the output is 0 -> the shape is not moved
    if (rate_hz == 0.0f) l->phase = 0.0f;
}

void ShapeLFO_SetDepth(ShapeLFO *l, float depth)
{
    if (depth < 0.0f) depth = 0.0f;
    l->depth = depth;
}

float ShapeLFO_Process(ShapeLFO *l, uint32_t samples)
{
    // The LFO is slow, so it does not need to run every sample.
    //   We jump forward by many samples at once, this saves CPU time.
    l->phase += l->inc * (float)samples;
    while (l->phase >= 1.0f) l->phase -= 1.0f;

    // cos goes 1 -> -1 -> 1, so 0.5 - 0.5 * cos goes 0 -> 1 -> 0.
    //   It starts at 0, so switching the LFO on gives no jump.
    return (0.5f - 0.5f * cosf(TWO_PI * l->phase)) * l->depth;
}
