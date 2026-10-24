/**
 * tapedelay.h - Tape echo (delay effect)
 * Dominik Neugebauer
 *
 * Plays the sound again a bit later, like an echo in the mountains.
 * Part of the echo is fed back, so every echo gets its own echo
 * and they slowly fade away.
 *
 * When the delay time changes, it glides slowly to the new value.
 * The echoes are then played faster or slower, so their pitch bends
 * up or down - like an old tape machine.
 */
#ifndef TAPEDELAY_H
#define TAPEDELAY_H

#include <stdint.h>

typedef struct {
    int16_t *buf;     // memory for the past samples (given by the caller)
    uint32_t size;    // number of samples in buf
    uint32_t write;   // position where the next sample is written
    float time;       // current delay time in samples (glides to target)
    float target;     // wanted delay time in samples
    float feedback;   // how much of the echo is fed back, 0.0 ... 0.9
    float mix;        // how loud the echo is, 0.0 ... 1.0
} TapeDelay;

// Prepare the effect. buf must be able to hold size samples.
void TapeDelay_Init(TapeDelay *d, int16_t *buf, uint32_t size);

// Set the delay time in seconds. It is limited by the size of buf.
void TapeDelay_SetTime(TapeDelay *d, float seconds);

// Set how long the echoes last: 0.0 = one echo, 0.9 = very long
void TapeDelay_SetFeedback(TapeDelay *d, float feedback);

// Set how loud the echo is: 0.0 = no echo, 1.0 = as loud as the input
void TapeDelay_SetMix(TapeDelay *d, float mix);

// Processes one sample and returns the sample with the echo added
float TapeDelay_Process(TapeDelay *d, float in);

#endif /* TAPEDELAY_H */
