/**
 * softclip.h - Soft clipper
 * Dominik Neugebauer
 *
 * Keeps a sample in the range -1.0 ... 1.0 without a hard edge.
 * Small values pass almost unchanged, big values are pressed
 * softly towards 1.0. Sounds warm instead of harsh when the
 * signal is too loud.
 */
#ifndef SOFTCLIP_H
#define SOFTCLIP_H

// Returns the sample limited softly to -1.0 ... 1.0
float SoftClip_Process(float in);

#endif /* SOFTCLIP_H */
