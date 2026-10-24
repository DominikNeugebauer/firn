/**
 * svf.h - Chamberlin state variable filter (SVF)
 * Dominik Neugebauer
 *
 * A 2-pole filter (12 dB per octave) with resonance.
 * It calculates low-pass, high-pass, band-pass and notch
 * at the same time. The mode decides which one is returned.
 *
 * Resonance makes the frequencies around the cutoff louder.
 * This gives the typical "synth filter" sound.
 */
#ifndef SVF_H
#define SVF_H

typedef enum {
    SVF_LOWPASS = 0,  // lets low frequencies through
    SVF_HIGHPASS,     // lets high frequencies through
    SVF_BANDPASS,     // lets only frequencies around the cutoff through
    SVF_NOTCH         // removes only frequencies around the cutoff
} SVF_Mode;

typedef struct {
    float f;      // frequency coefficient, calculated from the cutoff
    float q;      // damping = 1 / resonance, calculated from the resonance
    float low;    // low-pass memory (first integrator)
    float band;   // band-pass memory (second integrator)
    SVF_Mode mode;
} SVF;

// Prepare filter. Call this once before using the filter.
void SVF_Init(SVF *f, float cutoff_hz, float resonance);

// Set the cutoff frequency in Hz
void SVF_SetCutoff(SVF *f, float cutoff_hz);

// Set the resonance. 0.5 = soft, 0.707 = no peak, 2 ... 10 = strong peak
void SVF_SetResonance(SVF *f, float resonance);

// Choose which output is returned by SVF_Process()
void SVF_SetMode(SVF *f, SVF_Mode mode);

// Filters one sample and returns the filtered value
float SVF_Process(SVF *f, float in);

#endif /* SVF_H */
