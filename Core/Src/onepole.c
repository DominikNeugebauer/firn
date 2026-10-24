/**
 * onepole.h - One-pole high-pass filter
 * Dominik Neugebauer
 *
 * Onepole High-pass filter
 */
#include "onepole.h"
#include "synth.h"
#include <math.h>

#define PI 3.14159265359f

#define MIN_CUTOFF    20.0f
#define MAX_CUTOFF 20000.0f


void OnePole_Init(OnePole *f, float cutoff_hz)
{
    f->lp = 0.0f;                      // start with silence
    OnePole_SetCutoff(f, cutoff_hz);   // calculate a from cutoff
}

void OnePole_SetCutoff(OnePole *f, float cutoff_hz)
{
    // keep the cutoff in a useful range
    if (cutoff_hz < MIN_CUTOFF) cutoff_hz = MIN_CUTOFF;
    if (cutoff_hz > MAX_CUTOFF) cutoff_hz = MAX_CUTOFF;

    // calculate the filter coefficient a from the cutoff frequency
    f->a = 1.0f - expf(-2.0f * PI * cutoff_hz / SYNTH_SAMPLE_RATE);
}

float OnePole_Process(OnePole *f, float in)
{
    // First -> one-pole low-pass: move lp a part (a) of the way towards the input.
    //   lp only keeps the slow changes (low frequencies) of the input.
    // Second part -> high-pass: subtract the low part from the input.
    //   What is left are the fast changes (high frequencies).
    // Result: the bass and any DC offset are removed,
    //   the pitch stays the same, the sound gets thinner.
    f->lp = f->lp + f->a * (in - f->lp);
    return in - f->lp;
}
