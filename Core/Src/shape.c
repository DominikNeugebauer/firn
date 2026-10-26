/**
 * shape.c - Waveform shaping
 * Dominik Neugebauer
 *
 * Changes the shape of a waveform with one amount value.
 * 0.0 = normal waveform, 1.0 = strongest change.
 */
#include "shape.h"
#include "synth.h"
#include <math.h>

// The width moves from 0.5 (normal) down to MIN_WIDTH (strongest).
//   Only one side is used: width 0.05 and 0.95 sound the same
//   (the square is only upside down), so with both sides a full turn
//   of the poti would sound like going back to the start.
// At 0.0 the square would be silent, so stay a bit away from the edge.
#define MIN_WIDTH 0.05f

#define PI     3.14159265359f
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

// PolyBLAMP: the same idea as PolyBLEP, but for a corner instead of a jump.
//   A corner is where the wave suddenly changes its direction (the tip of
//   the triangle). A sharp corner also has overtones above the Nyquist
//   frequency, they fold back and sound harsh. PolyBLAMP rounds the
//   corner off over about 2 samples.
//   t is the distance to the corner in phase, dt the phase increment.
//   The result must be multiplied with how much the slope changes per sample.
static float poly_blamp(float t, float dt)
{
    if (t < dt) {
        // just after the corner
        t = 1.0f - t / dt;
        return t * t * t / 6.0f;
    }
    if (t > 1.0f - dt) {
        // just before the corner
        t = (t - 1.0f) / dt + 1.0f;
        return t * t * t / 6.0f;
    }
    return 0.0f;
}

void Shape_Init(Shape *s, float amount)
{
    Shape_SetAmount(s, amount);
    s->width = s->target;   // no glide at the start
}

void Shape_SetAmount(Shape *s, float amount)
{
    // keep the amount in range 0.0 ... 1.0
    if (amount < 0.0f) amount = 0.0f;
    if (amount > 1.0f) amount = 1.0f;

    // amount 0.0 -> width 0.5 (normal), amount 1.0 -> width MIN_WIDTH
    // only set the target, Shape_Process glides there slowly
    s->target = 0.5f - (0.5f - MIN_WIDTH) * amount;
}

float Shape_Process(Shape *s, float phase, float inc, uint8_t waveform)
{
    s->width += GLIDE * (s->target - s->width);
    float w = s->width;

    // Bend the phase: the middle of the wave (0.5) moves to the width.
    //   width = 0.5 -> p is the same as phase, the waveform is unchanged.
    float p = (phase < w) ? 0.5f * phase / w
                          : 0.5f + 0.5f * (phase - w) / (1.0f - w);

    // Distance to the bend point at phase = width, needed for jumps and corners there
    float t = phase - w;
    if (t < 0.0f) t += 1.0f;

    float out = 0.0f;
    float k;   // how much the slope changes at a corner (per phase, not per sample)

    switch (waveform)
        {
        case SYNTH_WAVEFORM_SINE:
            out = sinf(TWO_PI * p);
            // Bending gives the sine a small corner at phase 0 and at the width.
            //   Both corners change the slope by the same amount, 0 when not bent.
            k = PI / w - PI / (1.0f - w);
            out += k * inc * (poly_blamp(phase, inc) + poly_blamp(t, inc));
            break;
        case SYNTH_WAVEFORM_SAWTOOTH:
            out = (2.0f * p - 1.0f);
            // jump from +1 down to -1 at phase 0
            out -= poly_blep(phase, inc);
            // Bending gives the ramp a corner at the width, and one at phase 0 the other way round
            k = 1.0f / (1.0f - w) - 1.0f / w;
            out += k * inc * (poly_blamp(t, inc) - poly_blamp(phase, inc));
            break;
        case SYNTH_WAVEFORM_SQUARE:
            out = (p < 0.5f ? 1.0f : -1.0f);
            // jump up at phase 0, jump down at phase = width
            out += poly_blep(phase, inc);
            out -= poly_blep(t, inc);
            break;
        case SYNTH_WAVEFORM_TRIANGLE:
            out = (p < 0.5f ? 4.0f * p - 1.0f : 3.0f - 4.0f * p);
            // Corner at the bottom (phase 0, turns up) and at the top (width, turns down).
            //   Up slope is 2 / w, down slope is -2 / (1 - w), the corner is the difference.
            k = 2.0f / (w * (1.0f - w));
            out += k * inc * (poly_blamp(phase, inc) - poly_blamp(t, inc));
            break;

        default:
            out = sinf(TWO_PI * p);
            break;
        }

    return out;
}
