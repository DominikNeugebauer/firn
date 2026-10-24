/**
 * synth.c - Oscillator
 * Dominik Neugebauer
 *
 * The phase is running from 0.0 to 1.0 each sample.
 * The phase increment is calculated from the frequency 
 * and sample rate.
 */
#include "synth.h"
#include <math.h>

#define TWO_PI 6.28318530718f

static float s_phase = 0.0f;
static float s_inc   = 440.0f / SYNTH_SAMPLE_RATE;
static float s_amp   = 0.25f; // default is 1/4 of max
static float s_amp_target = 0.0f;
static uint8_t s_waveform = SYNTH_WAVEFORM_SINE;

void Synth_SetFrequency(float hz)
{
    s_inc = hz / SYNTH_SAMPLE_RATE;
}

void Synth_SetAmplitude(float amp)
{
    if (amp < 0.0f) amp = 0.0f;
    if (amp > 1.0f) amp = 1.0f;
    s_amp_target = amp;
}

void Synth_SetWaveform(uint8_t waveform)
{
    s_waveform = waveform;
}

void Synth_Render(int16_t *out, uint32_t frames)
{
    for (uint32_t i = 0; i < frames; i++) {
        s_amp += 0.02f * (s_amp_target - s_amp); // slow on/off ramp for anti aliasing
        
        float s = 0.0f;
        switch (s_waveform)
        {
        case SYNTH_WAVEFORM_SINE:
            s = sinf(TWO_PI * s_phase) * s_amp;
            break;
        case SYNTH_WAVEFORM_SAWTOOTH:
            s = (2.0f * s_phase - 1.0f) * s_amp;
            break;
        case SYNTH_WAVEFORM_SQUARE:
            s = (s_phase < 0.5f ? 1.0f : -1.0f) * s_amp;
            break;
        case SYNTH_WAVEFORM_TRIANGLE:
            s = (s_phase < 0.5f ? 4.0f * s_phase - 1.0f : 3.0f - 4.0f * s_phase) * s_amp;
            break;
        
        default:
            s = sinf(TWO_PI * s_phase) * s_amp;
            break;
        }

        s_phase += s_inc;
        if (s_phase >= 1.0f) s_phase -= 1.0f;

        int16_t v = (int16_t)(s * 32767.0f);
        out[2 * i]     = v;   // left
        out[2 * i + 1] = v;   // right
    }
}
