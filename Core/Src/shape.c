/**
 * shape.c - Waveform shaping
 * Dominik Neugebauer
 *
 * Changes the shape of a waveform with one width value.
 */
#include "shape.h"
#include "synth.h"
#include <math.h>

// At 0.0 or 1.0 the square would be silent, so stay a bit away from the edges
#define MIN_WIDTH 0.05f
#define MAX_WIDTH 0.95f

#define TWO_PI 6.28318530718f

// How fast the width glides to the new value.
// 0.002 per sample = about 10 ms. The poti is a bit noisy, without the
// glide the square edge would jump back and forth and give clicks.
#define GLIDE 0.002f


// PolyBLEP: rounds off a jump of the waveform over about 2 samples.
//   t is the distance to the jump in phase, dt the phase increment.
//   A hard jump can only be on a whole sample, so a small change of the
//   width moves the edge by a whole sample -> click. With PolyBLEP the edge
//   can also be between two samples, so it moves smoothly.
//   It also removes most of the aliasing (harsh high tones) of the square.
static float poly_blep(float t, float dt)
{
    if (t < dt) {
        // just after the jump
        t /= dt;
        return t + t - t * t - 1.0f;
    }
    if (t > 1.0f - dt) {
        // just before the jump
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

void Shape_Init(Shape *s, float width)
{
    Shape_SetWidth(s, width);
    s->width = s->target;   // no glide at the start
}

void Shape_SetWidth(Shape *s, float width)
{
    // keep the width in a useful range
    if (width < MIN_WIDTH) width = MIN_WIDTH;
    if (width > MAX_WIDTH) width = MAX_WIDTH;

    // only set the target, Shape_Process glides there slowly
    s->target = width;
}

float Shape_Process(Shape *s, float phase, float inc, uint8_t waveform)
{
    s->width += GLIDE * (s->target - s->width);
    float w = s->width;

    // Bend the phase: the middle of the wave (0.5) moves to the width.
    //   width = 0.5 -> p is the same as phase, the waveform is unchanged.
    float p = (phase < w) ? 0.5f * phase / w
                          : 0.5f + 0.5f * (phase - w) / (1.0f - w);

    float out = 0.0f;

    switch (waveform)
        {
        case SYNTH_WAVEFORM_SINE:
            out = sinf(TWO_PI * p);
            break;
        case SYNTH_WAVEFORM_SAWTOOTH:
            out = (2.0f * p - 1.0f);
            // jump from +1 down to -1 at phase 0
            out -= poly_blep(phase, inc);
            break;
        case SYNTH_WAVEFORM_SQUARE: {
            out = (p < 0.5f ? 1.0f : -1.0f);
            // jump up at phase 0, jump down at phase = width
            float t = phase - w;
            if (t < 0.0f) t += 1.0f;
            out += poly_blep(phase, inc);
            out -= poly_blep(t, inc);
            break;
        }
        case SYNTH_WAVEFORM_TRIANGLE:
            out = (p < 0.5f ? 4.0f * p - 1.0f : 3.0f - 4.0f * p);
            break;

        default:
            out = sinf(TWO_PI * p);
            break;
        }

    return out;
}
