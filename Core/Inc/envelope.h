/**
 * envelope.c - ADSR envelope
 * Dominik Neugebauer
 * 
 * The envelope outputs one value from 0.0 to 1.0 for every sample.
 * This value can be used to modulate the amplitude of the oscillator.
 */
#ifndef ENVELOPE_H
#define ENVELOPE_H

typedef enum {
    ENV_IDLE = 0,
    ENV_ATTACK,
    ENV_DECAY,
    ENV_SUSTAIN,
    ENV_RELEASE
} Env_State;

void Env_SetAttack(float seconds);
void Env_SetDecay(float seconds);
void Env_SetRelease(float seconds);
void Env_SetSustain(float level);

// set gate on/off (key pressed/released)
// on = 1: key pressed, on = 0: key released
void Env_Gate(int on);

// calculate the next envelope value and return it
// this function must be called for every sample
float Env_Process(void);

// get the current state of the envelope
Env_State Env_GetState(void);

#endif /* ENVELOPE_H */