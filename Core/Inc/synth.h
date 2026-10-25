/**
 * synth.h - Oscillator
 * Dominik Neugebauer
 *
 * The phase is running from 0.0 to 1.0 each sample.
 * The phase increment is calculated from the frequency 
 * and sample rate.
 */
#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include "envelope.h"

#define SYNTH_SAMPLE_RATE   48000.0f

#define SYNTH_WAVEFORM_SINE      0
#define SYNTH_WAVEFORM_SAWTOOTH  1
#define SYNTH_WAVEFORM_SQUARE    2
#define SYNTH_WAVEFORM_TRIANGLE  3

// Frequency in Hz
void Synth_SetFrequency(float hz);

// Amplitude in range 0.0 ... 1.0
void Synth_SetAmplitude(float amp);

/* choose waveform: 
 * SYNTH_WAVEFORM_SINE, 
 * SYNTH_WAVEFORM_SAWTOOTH, 
 * SYNTH_WAVEFORM_SQUARE, 
 * SYNTH_WAVEFORM_TRIANGLE
 */
void Synth_SetWaveform(uint8_t waveform);

/* Fills the output buffer with the next audio samples. 
 * The buffer must be large enough to hold 2 * frames samples (stereo).
 * Will switch to mono in the future
*/
void Synth_Render(int16_t *out, uint32_t frames);

/**
 * Initialize the synthesizer. Call this once before using the synth.
 */
void Synth_Init(void);

/**
 * Set the cutoff frequency of the resonant low-pass filter (SVF) in Hz.
 * Range 20 ... 8000 Hz. The filter is applied to the output of the oscillator.
 */
void Synth_SetCutoff(float hz);

/**
 * Set the resonance of the low-pass filter (SVF).
 * 0.707 = no peak, 2 ... 10 = stronger peak at the cutoff.
 */
void Synth_SetResonance(float resonance);

/**
 * Set the cutoff frequency of the high-pass filter (one-pole) in Hz.
 * Default is 30 Hz, which only removes rumble and DC offset.
 */
void Synth_SetHighpass(float hz);

/**
 * Set the time of the tape echo in seconds (0.01 ... 0.5).
 * The time glides to the new value, so the echoes bend in pitch.
 */
void Synth_SetTapeDelayTime(float seconds);

/**
 * Set the shape of the waveform, 0.0 ... 1.0.
 * 0.0 = normal waveform, 1.0 = strongest change.
 * Square: narrower pulse. Triangle: towards a ramp.
 * Sine and sawtooth get bent. The value glides, so a noisy poti is ok.
 */
void Synth_SetShape(float amount);

/**
 * Set the speed of the shape LFO in Hz (0.0 ... 20.0).
 * The LFO makes the shape slowly stronger, starting from the value
 * of Synth_SetShape, and back. 0 = off. The LFO is off until a rate is set.
 */
void Synth_SetShapeLfoRate(float hz);

#endif /* SYNTH_H */
