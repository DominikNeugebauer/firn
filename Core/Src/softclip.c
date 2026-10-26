/**
 * softclip.c - Soft clipper
 * Dominik Neugebauer
 *
 * Keeps a sample in the range -1.0 ... 1.0 without a hard edge.
 * Small values pass almost unchanged, big values are pressed
 * softly towards 1.0. Sounds warm instead of harsh when the
 * signal is too loud.
 */
#include "softclip.h"

// Above this input the output is exactly 1.0
#define LIMIT 3.0f

float SoftClip_Process(float in)
{
    // Soft clip: rounds the wave off instead of cutting it flat.
    // A hard limit makes sharp corners -> harsh aliasing.
    // Like tanh(), but faster:  0.1 -> 0.1,  1.0 -> 0.78,  >=3.0 -> 1.0
    if (in >  LIMIT) in =  LIMIT;
    if (in < -LIMIT) in = -LIMIT;

    float x2 = in * in;
    return in * (27.0f + x2) / (27.0f + 9.0f * x2);
}
