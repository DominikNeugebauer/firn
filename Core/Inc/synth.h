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

#endif /* SYNTH_H */
