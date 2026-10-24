/**
 * svf.c - Chamberlin state variable filter (SVF)
 * Dominik Neugebauer
 *
 * A 2-pole filter (12 dB per octave) with resonance.
 * It calculates low-pass, high-pass, band-pass and notch
 * at the same time. The mode decides which one is returned.
 *
 * Resonance makes the frequencies around the cutoff louder.
 */
#include "svf.h"
#include "synth.h"
#include <math.h>

#define PI 3.14159265359f

// The Chamberlin SVF gets unstable above about 1/6 of the sample rate.
// 48000 / 6 = 8000 Hz, so the cutoff is limited to that.
#define MIN_CUTOFF        20.0f
#define MAX_CUTOFF      (SYNTH_SAMPLE_RATE / 6.0f)

#define MIN_RESONANCE      0.5f
#define MAX_RESONANCE     10.0f


void SVF_Init(SVF *f, float cutoff_hz, float resonance)
{
    f->low  = 0.0f;                   // start with silence
    f->band = 0.0f;
    f->mode = SVF_LOWPASS;
    SVF_SetCutoff(f, cutoff_hz);      // calculate f from cutoff
    SVF_SetResonance(f, resonance);   // calculate q from resonance
}

void SVF_SetCutoff(SVF *f, float cutoff_hz)
{
    // keep the cutoff in a range where the filter is stable
    if (cutoff_hz < MIN_CUTOFF) cutoff_hz = MIN_CUTOFF;
    if (cutoff_hz > MAX_CUTOFF) cutoff_hz = MAX_CUTOFF;

    // calculate the frequency coefficient f from the cutoff frequency
    f->f = 2.0f * sinf(PI * cutoff_hz / SYNTH_SAMPLE_RATE);
}

void SVF_SetResonance(SVF *f, float resonance)
{
    // keep the resonance in a useful range
    if (resonance < MIN_RESONANCE) resonance = MIN_RESONANCE;
    if (resonance > MAX_RESONANCE) resonance = MAX_RESONANCE;

    // the filter uses damping, which is the opposite of resonance
    f->q = 1.0f / resonance;
}

void SVF_SetMode(SVF *f, SVF_Mode mode)
{
    f->mode = mode;
}

float SVF_Process(SVF *f, float in)
{
    // Chamberlin SVF: two integrators in a loop.
    // low:  follows band slowly             -> only slow changes pass
    // high: what is left of the input        -> only fast changes pass
    //       after removing low and some band
    // band: follows high slowly             -> only changes around the cutoff pass
    // The damping q feeds band back into high. Less damping (more resonance)
    //   -> the band "rings" longer at the cutoff -> the cutoff gets louder.
    f->low += f->f * f->band;
    float high = in - f->low - f->q * f->band;
    f->band += f->f * high;

    switch (f->mode) {
    case SVF_HIGHPASS: return high;
    case SVF_BANDPASS: return f->band;
    case SVF_NOTCH:    return high + f->low;
    case SVF_LOWPASS:
    default:           return f->low;
    }
}
