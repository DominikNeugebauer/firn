/**
 * synth.c - Oscillator
 * Dominik Neugebauer
 *
 * The phase is running from 0.0 to 1.0 each sample.
 * The phase increment is calculated from the frequency 
 * and sample rate.
 */
#include "synth.h"
#include "onepole.h"
#include "svf.h"
#include "tapedelay.h"
#include "shape.h"
#include "shapelfo.h"
#include <math.h>

#define TWO_PI 6.28318530718f

// Memory for the echo: 0.5 s at 48 kHz = 24000 samples (48 KB RAM)
#define TAPE_DELAY_SAMPLES 24000

static SVF tone;          // resonant low-pass, shapes the sound
static OnePole highpass;  // removes bass rumble and DC offset
static TapeDelay tape_delay;      // tape echo, plays the sound again later
static Shape shape;
static ShapeLFO shape_lfo; // makes the shape slowly stronger and back
static float s_shape = 0.0f;   // shape from Synth_SetShape, the LFO goes up from here
static int16_t tape_delay_buf[TAPE_DELAY_SAMPLES];
static float s_phase = 0.0f;
static float s_inc   = 440.0f / SYNTH_SAMPLE_RATE;
static float s_amp   = 0.25f; // default is 1/4 of max
static float s_amp_target = 0.0f;
static uint8_t s_waveform = SYNTH_WAVEFORM_SQUARE;

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

void Synth_Init(void)
{
    SVF_Init(&tone, 8000.0f, 4.0f);   // start with the low-pass fully open, medium resonance
    OnePole_Init(&highpass, 20.0f);   // only remove what is below the lowest note
    TapeDelay_Init(&tape_delay, tape_delay_buf, TAPE_DELAY_SAMPLES);   // 0.3 s, medium feedback and mix
    Shape_Init(&shape, 0.0f);         // 0.0 = normal waveform
    ShapeLFO_Init(&shape_lfo, 0.0f, 1.0f); // off until a rate is set, can reach the full shape range
}

void Synth_SetCutoff(float hz)
{
    SVF_SetCutoff(&tone, hz);
}

void Synth_SetResonance(float resonance)
{
    SVF_SetResonance(&tone, resonance);
}

void Synth_SetHighpass(float hz)
{
    OnePole_SetCutoff(&highpass, hz);
}

void Synth_SetTapeDelayTime(float seconds)
{
    TapeDelay_SetTime(&tape_delay, seconds);
}

void Synth_SetShape(float amount)
{
    // only store it, Synth_Render adds the LFO and sets the shape
    s_shape = amount;
}

void Synth_SetShapeLfoRate(float hz)
{
    ShapeLFO_SetRate(&shape_lfo, hz);
}

void Synth_Render(int16_t *out, uint32_t frames)
{
    // Move the shape with the LFO. Once per buffer (every 5 ms) is enough,
    //   the shape glides to the new value anyway, so there are no steps.
    //   LFO off -> the LFO returns 0 -> the shape is just s_shape.
    //   The LFO only makes the shape stronger, above 1.0 it stays at 1.0.
    Shape_SetAmount(&shape, s_shape + ShapeLFO_Process(&shape_lfo, frames));

    for (uint32_t i = 0; i < frames; i++) {
        s_amp += 0.02f * (s_amp_target - s_amp); // slow on/off ramp for anti aliasing
        
        float s = Shape_Process(&shape, s_phase, s_inc, s_waveform);

        // Apply amplitude
        s *= s_amp;

        // Apply the resonant low-pass filter to the sample
        s = SVF_Process(&tone, s);

        // Apply the one-pole high-pass filter to the sample
        s = OnePole_Process(&highpass, s);

        // Apply the envelope to the sample
        s *= Env_Process();

        // Apply the echo after the envelope, so the echoes go on
        // after the key is released
        s = TapeDelay_Process(&tape_delay, s);

        s_phase += s_inc;
        if (s_phase >= 1.0f) s_phase -= 1.0f;

        // Resonance can push the sample above 1.0. Without this limit the
        // int16 conversion would wrap around and give loud crackles.
        if (s >  1.0f) s =  1.0f;
        if (s < -1.0f) s = -1.0f;

        int16_t v = (int16_t)(s * 32767.0f);
        out[2 * i]     = v;   // left
        out[2 * i + 1] = v;   // right
    }
}
