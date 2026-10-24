/**
 * tapedelay.c - Tape echo (delay effect)
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
#include "tapedelay.h"
#include "synth.h"

#define MIN_TIME_S      0.010f   // 10 ms
#define MAX_FEEDBACK    0.9f     // above 1.0 the echoes would get louder forever

// How fast the delay time glides to the new value.
// 0.0005 per sample = about 40 ms. Smaller = slower glide = more tape bending.
#define GLIDE           0.0005f


void TapeDelay_Init(TapeDelay *d, int16_t *buf, uint32_t size)
{
    d->buf   = buf;
    d->size  = size;
    d->write = 0;
    for (uint32_t i = 0; i < size; i++) {
        d->buf[i] = 0;        // start with silence
    }
    d->feedback = 0.5f;
    d->mix      = 0.5f;
    TapeDelay_SetTime(d, 0.3f);
    d->time = d->target;      // no glide at the start
}

void TapeDelay_SetTime(TapeDelay *d, float seconds)
{
    // keep the time in a useful range. The longest time is limited by the
    // memory, minus 2 samples because TapeDelay_Process reads two neighbours.
    float samples = seconds * SYNTH_SAMPLE_RATE;
    float min = MIN_TIME_S * SYNTH_SAMPLE_RATE;
    float max = (float)(d->size - 2);
    if (samples < min) samples = min;
    if (samples > max) samples = max;

    // only set the target, TapeDelay_Process glides there slowly
    d->target = samples;
}

void TapeDelay_SetFeedback(TapeDelay *d, float feedback)
{
    if (feedback < 0.0f) feedback = 0.0f;
    if (feedback > MAX_FEEDBACK) feedback = MAX_FEEDBACK;
    d->feedback = feedback;
}

void TapeDelay_SetMix(TapeDelay *d, float mix)
{
    if (mix < 0.0f) mix = 0.0f;
    if (mix > 1.0f) mix = 1.0f;
    d->mix = mix;
}

float TapeDelay_Process(TapeDelay *d, float in)
{
    // Glide: move the delay time a small part towards the target.
    //   A jump in time would give a click. Gliding instead makes the read
    //   position move faster or slower than normal -> the pitch of the echo
    //   bends, like a tape that runs too fast or too slow.
    d->time += GLIDE * (d->target - d->time);

    // Read the echo: look "time" samples back into the past.
    //   The memory is a ring: when we go back before the start,
    //   we continue at the end.
    float pos = (float)d->write - d->time;
    if (pos < 0.0f) pos += (float)d->size;

    // The time is not a whole number while gliding, for example 1234.6.
    //   So we mix the two neighbour samples 1234 and 1235 (60% of the second).
    //   Without this the gliding would sound rough and crackly.
    uint32_t i0 = (uint32_t)pos;
    uint32_t i1 = i0 + 1;
    if (i1 >= d->size) i1 = 0;
    float frac = pos - (float)i0;
    float s0 = d->buf[i0] / 32767.0f;
    float s1 = d->buf[i1] / 32767.0f;
    float echo = s0 + frac * (s1 - s0);

    // Write: store the input plus a part of the echo.
    //   The echo comes back again later, a bit quieter each time -> it fades away.
    float store = in + d->feedback * echo;
    if (store >  1.0f) store =  1.0f;   // keep it in the int16 range
    if (store < -1.0f) store = -1.0f;
    d->buf[d->write] = (int16_t)(store * 32767.0f);

    d->write++;
    if (d->write >= d->size) d->write = 0;

    // Output: the original sound plus the echo
    return in + d->mix * echo;
}
