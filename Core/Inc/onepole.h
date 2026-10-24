/**
 * onepole.c - One-pole low-pass filter
 * Dominik Neugebauer
 * 
 * A low-pass filter lets low frequencies through and makes
 * high frequencies quieter. The cutoff frequency decides where
 * "low" ends and "high" begins.
 */
#ifndef ONEPOLE_H
#define ONEPOLE_H

typedef struct {
    float a;   // coefficient for the filter, calculated from the cutoff frequency
    float out; // last output value
} OnePole;

// Prepare filter. Call this once before using the filter.
void OnePole_Init(OnePole *f, float cutoff_hz);

// Set the cutoff frequency in Hz
void OnePole_SetCutoff(OnePole *f, float cutoff_hz);

// Filters one sample and returns the filtered value
float OnePole_Process(OnePole *f, float in);

#endif /* ONEPOLE_H */