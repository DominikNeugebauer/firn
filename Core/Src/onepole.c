/**
 * onepole.c - One-pole low-pass filter
 * Dominik Neugebauer
 * 
 * A low-pass filter lets low frequencies through and makes
 * high frequencies quieter. The cutoff frequency decides where
 * "low" ends and "high" begins.
 */
#include "onepole.h"
#include "synth.h"
#include <math.h> 

#define PI 3.14159265359f

#define MIN_CUTOFF    20.0f
#define MAX_CUTOFF 20000.0f


void OnePole_Init(OnePole *f, float cutoff_hz)
{
    f->out = 0.0f;                     // start with silence
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
    // One-pole low-pass: move the output a part (a) of the way towards the input.
    // Slow changes (low frequencies): the output has time to catch up -> they pass.
    // Fast changes (high frequencies): the input flips before the output catches up
    //   -> the output swings less -> they get quieter.
    // Result: sharp edges get rounded, high overtones are reduced,
    //   the pitch stays the same, the sound gets duller.
    f->out = f->out + f->a * (in - f->out);
    return f->out;
}