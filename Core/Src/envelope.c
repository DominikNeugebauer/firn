/**
 * envelope.c - ADSR envelope
 * Dominik Neugebauer
 * 
 * The envelope outputs one value from 0.0 to 1.0 for every sample.
 * This value can be used to modulate the amplitude of the oscillator.
 */
#include "envelope.h"
#include "synth.h"

// settings in seconds
// sustain as level from 0.0 to 1.0
static float s_attack  = 0.01f;
static float s_decay   = 0.200f;
static float s_sustain = 0.6f;
static float s_release = 0.300f;
static const float min_level = 0.001f; // minimum time for attack, decay, release to avoid division by zero

static volatile int s_gate = 0;    // 1 = key pressed, 0 = key released
static int       s_last_gate = 0;  // last gate state, to detect rising/falling edges
static float     s_level = 0.0f;   // current level of the envelope, 0.0 ... 1.0
static Env_State s_state = ENV_IDLE;

void Env_SetAttack(float seconds)
{
    if(seconds < min_level) {
        seconds = min_level;
    }

    s_attack = seconds;
}

void Env_SetDecay(float seconds)
{
    if(seconds < min_level) {
        seconds = min_level;
    }

    s_decay = seconds;
}

void Env_SetRelease(float seconds)
{
    if(seconds < min_level) {
        seconds = min_level;
    }

    s_release = seconds;
}

void Env_SetSustain(float level)
{
    if(level < 0.0f) {
        level = 0.0f;
    }
    if(level > 1.0f) {
        level = 1.0f;
    }
    s_sustain = level;
}

void Env_Gate(int on)
{
    s_gate = on ? 1 : 0;
}

Env_State Env_GetState(void)
{
    return s_state;
}

float Env_Process(void)
{
    int gate = s_gate;

    if (gate && !s_last_gate) {
        // attack state on gate open
        s_state = ENV_ATTACK;
    }
    else if (!gate && s_last_gate) {
        // release state on gate close
        s_state = ENV_RELEASE;
    }
    s_last_gate = gate;


    // state machine: go one step forward in the current phase and return the level
    switch (s_state) {
        case ENV_ATTACK:
            // attack phase: go up to 1.0 in s_attack seconds
            s_level += 1.0f / (s_attack * SYNTH_SAMPLE_RATE);
            if(s_level >= 1.0f) {
                s_level = 1.0f;
                s_state = ENV_DECAY;
            }
            break;

        case ENV_DECAY:
            // decay phase: go down to the sustain level in s_decay seconds
            s_level -= (1.0f - s_sustain) / (s_decay * SYNTH_SAMPLE_RATE);
            if(s_level <= s_sustain) {
                s_level = s_sustain;
                s_state = ENV_SUSTAIN;
            }
            break;

        case ENV_SUSTAIN:
            // hold in sustain level until the gate is released
            break;

        case ENV_RELEASE:
            // release phase: go down to 0.0 in s_release seconds
            s_level -= 1.0f / (s_release * SYNTH_SAMPLE_RATE);
            if(s_level <= 0.0f) {
                s_level = 0.0f;
                s_state = ENV_IDLE;
            }
            break;

        case ENV_IDLE:
        default:
            // level stays at 0.0, nothing happens
            s_level = 0.0f;
            break;
    }

    return s_level;
}